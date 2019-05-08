#include "DebugTarget.h"
#include "MyCapstone.h"
#include "User.h"
#include "BreakPoint.h"
#include <iostream>
using namespace std;

// HookDll ·��
#define HOOKDLLPATH "D:\\Codes\\VS\\Project\\MiniDebugger\\Debug\\HookDll.dll"


DebugTarget::DebugTarget()
{
	// ������Ϣ
	m_stcDbEvent = { 0 };
	// ���̾��
	m_hProcess = 0;
	// �߳̾��
	m_hThread = 0;
	// ϵͳ�ϵ�
	m_bIsSystemBP = true;
	// �Ƿ���Ҫ�û�����
	m_bNeedInput = true;
	// OEP
	m_OEP = 0;
	// �Ĵ���״̬
	m_stcCT = { 0 };
	// ���ӽ��̵��Է�ʽ��־
	m_bIsOpenPid = false;

	m_dwPid = 0;

	m_hHookProcess = 0;

	m_dwCtCout = 0;
}


DebugTarget::~DebugTarget()
{
}

// ���ļ�
bool DebugTarget::open(const char* file)
{
	// ��������ʱ��Ҫʹ�õĽṹ��
	STARTUPINFOA si = { sizeof(STARTUPINFOA) };
	PROCESS_INFORMATION pi = { 0 };

	// �Ե��Է�ʽ ����һ������
	BOOL isSuccess = CreateProcessA(file, NULL, NULL, NULL, FALSE,
		DEBUG_ONLY_THIS_PROCESS | CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	if (!isSuccess)
	{
		cout << "��������Ŀ�����ʧ�ܣ�" << endl;
		return false;
	}

	m_dwPid = pi.dwProcessId;
	m_hHookProcess = pi.hProcess;
	// ע��DLL
	InjectDll();

	// �رվ��
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	// ��ʼ�����������
	MyCapstone::Init();

	return true;
}

// ���ӽ��̵���
bool DebugTarget::OpenPid(DWORD pid)
{
	HANDLE hToken;
	HANDLE hProcess = GetCurrentProcess();  // ��ȡ��ǰ���̾��

	// �򿪵�ǰ���̵�Token������һ��Ȩ�����ƣ��ڶ�������������TOKEN_ALL_ACCESS
	if (OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		TOKEN_PRIVILEGES tkp;
		if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid))
		{
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

			//֪ͨϵͳ�޸Ľ���Ȩ��
			BOOL bREt = AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, 0);
		}
		CloseHandle(hToken);
	}

	// ��ʼ�����������
	MyCapstone::Init();

	m_bIsOpenPid = true;

	return DebugActiveProcess(pid);
}

// ����ѭ��
void DebugTarget::DebugLoop()
{
	// ���ڱ��������Ϣ�Ĵ�����
	DWORD result = DBG_CONTINUE;
	while (true)
	{
		// �ȴ������¼�
		WaitForDebugEvent(&m_stcDbEvent, INFINITE);

		// ���ݲ����쳣��λ�ô򿪾��
		OpenExceptionHandles();

		// �û����ȡ���
		User::GetProcessHandle(m_hProcess);
		User::GetThreadHandle(m_hThread);
		User::GetExceptionAddress(m_stcDbEvent.u.Exception.ExceptionRecord.ExceptionAddress);
		
		// ���ɵ����¼�
		result = DispatchDebugEvent();

		// �ظ������¼��Ĵ�����
		ContinueDebugEvent(m_stcDbEvent.dwProcessId,
			m_stcDbEvent.dwThreadId, result);

		// ���ݲ����쳣��λ�ùرվ��
		CloseExceptionHandles();
	}
}

// ���ݲ����쳣��λ�ô򿪾��
void DebugTarget::OpenExceptionHandles()
{
	m_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_stcDbEvent.dwProcessId);
	m_hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, m_stcDbEvent.dwThreadId);
}

// ���ݲ����쳣��λ�ùرվ��
void DebugTarget::CloseExceptionHandles()
{
	CloseHandle(m_hThread);
	CloseHandle(m_hProcess);
}

// ���ɵ����¼�
DWORD DebugTarget::DispatchDebugEvent()
{
	// ���ڱ��������Ϣ�Ĵ�����
	DWORD result = DBG_CONTINUE;

	switch (m_stcDbEvent.dwDebugEventCode)
	{
		// ���������¼�
		case CREATE_PROCESS_DEBUG_EVENT:
		{
			m_OEP = (DWORD)m_stcDbEvent.u.CreateProcessInfo.lpStartAddress;
			result = DBG_CONTINUE;
			break;
		}
		// �����쳣��Ϣ�¼�
		case EXCEPTION_DEBUG_EVENT:
		{
			result = OnHandleException();
			break;
		}
		// �������Ҳ�����Ѵ���
		default:
			result = DBG_CONTINUE;
			break;
	}

	return result;
}

// �����쳣
DWORD DebugTarget::OnHandleException()
{
	DWORD dwOldProtect = 0;
	// �쳣����
	DWORD ExceptionCode = m_stcDbEvent.u.Exception.ExceptionRecord.ExceptionCode;
	// �쳣��ַ
	DWORD ExceptionAddress = (DWORD)m_stcDbEvent.u.Exception.ExceptionRecord.ExceptionAddress;

	DWORD MemoryExceptionAddress = m_stcDbEvent.u.Exception.ExceptionRecord.ExceptionInformation[1];
	BreakPoint::GetMemoryExceptionAddress(MemoryExceptionAddress);

	bool bCtCout = true;

	switch (ExceptionCode)
	{
		// �޸�����ϵ�
		case EXCEPTION_BREAKPOINT:
		{
			// �ж��ǲ���ϵͳ�ϵ�
			if (m_bIsSystemBP)
			{
				// �� OEP λ������һ������ϵ�
				BreakPoint::SetBreadPoint_Soft(m_hProcess, m_OEP, 0);

				// ��һ�ξͲ���ϵͳ�ϵ���
				m_bIsSystemBP = false;

				// ���λ�ò������û����룬��һ��ֻ��ʾ
				if (m_bIsOpenPid)
					m_bNeedInput = true;
				else
					m_bNeedInput = false;
				break;
			}
			// �޸���ǰ���õ�����ϵ�
			bCtCout = BreakPoint::FixBreakPoint_Soft(m_hProcess, m_hThread, ExceptionAddress);
			break;
		}
		// �޸�Ӳ���ϵ�
		case EXCEPTION_SINGLE_STEP:
		{
			bool bTemp = BreakPoint::FixBreakPoint_Hard(m_hProcess, m_hThread, ExceptionAddress);
			m_bNeedInput = bTemp;
			break;
		}
		// �޸��ڴ�ϵ�
		case EXCEPTION_ACCESS_VIOLATION:
		{
			bool bTemp = BreakPoint::FixBreakPoint_Mem(m_hProcess, m_hThread, MemoryExceptionAddress);
			m_bNeedInput = bTemp;
			break;
		}
	}

	// �����Ҫ���²���������
	if (m_bNeedInput)
	{
		// ��ȡ�����ϵ����
		if (bCtCout == false)
		{
			static int CountFlag = 1;
			if (CountFlag == 1)
			{
				m_dwCtCout = BreakPoint::GetConditionCount();
				CountFlag = 2;
			}
		}
		if (m_dwCtCout > 0)
		{
			m_dwCtCout--;
			return DBG_CONTINUE;
		}
		else
		{
			system("cls");
			m_stcCT.ContextFlags = CONTEXT_ALL;
			GetThreadContext(m_hThread, &m_stcCT);
			User::ShowRegisterInfo(m_stcCT);
			MyCapstone::DisAsm(m_hProcess, (LPVOID)ExceptionAddress, 10);
			User::GetUserInput();
		}
	}

	m_bNeedInput = true;

	return DBG_CONTINUE;
}

 //ע��DLL
void DebugTarget::InjectDll()
{
#if 1
	// ��Ŀ���������һ���ڴ棨��С��DLL·���ĳ��ȣ�
	LPVOID lpBuff = VirtualAllocEx(m_hHookProcess, NULL,
		1, // //�����ǰ����ȣ�4096�ֽڣ������ڴ棬д1Ҳ���൱�ڷ���4K
		MEM_COMMIT,
		PAGE_READWRITE);

	// �� DLL ·��д�뵽Ŀ�������
	DWORD dwWrite = 0;

	DWORD dwRet = WriteProcessMemory(m_hHookProcess, lpBuff,
		//D:\Codes\VS\Project\MiniDebugger\Debug
		"D:\\Codes\\VS\\Project\\MiniDebugger\\Debug\\HookDll.dll",
		strlen("D:\\Codes\\VS\\Project\\MiniDebugger\\Debug\\HookDll.dll") + 1,
		&dwWrite);
	if (dwRet == 0)
	{
		return ;
	}
	// ����Զ���߳�
	HANDLE hThread = CreateRemoteThread(m_hHookProcess, NULL, NULL,
		(LPTHREAD_START_ROUTINE)LoadLibraryA,
		lpBuff, 0, 0);
	if (hThread == INVALID_HANDLE_VALUE)
	{
		return ;
	}
#else
	HANDLE hProcess = OpenProcess(
		PROCESS_ALL_ACCESS,
		FALSE, m_dwPid);
	if (!hProcess)
	{
		printf("���̴�ʧ��\n");
		return;
	}
	//2.��Ŀ�����������һ���ڴ棨��С��DLL·���ĳ��ȣ�
	LPVOID lpBuf = VirtualAllocEx(hProcess,
		NULL,
		1, //�����ǰ����ȣ�4096�ֽڣ������ڴ棬д1Ҳ��һ����
		MEM_COMMIT,
		PAGE_READWRITE);
	//3.��dll·��д�뵽Ŀ�������
	DWORD dwWrite;
	WriteProcessMemory(hProcess,
		lpBuf, "D:\\Codes\\VS\\Project\\MiniDebugger\\Debug\\HookDll.dll",
		strlen("D:\\Codes\\VS\\Project\\MiniDebugger\\Debug\\HookDll.dll"),
		&dwWrite);
	//4.����Զ���߳�
	HANDLE hThread = CreateRemoteThread(hProcess,
		NULL, NULL,
		(LPTHREAD_START_ROUTINE)LoadLibraryA,
		lpBuf, 0, 0);
#endif
}
