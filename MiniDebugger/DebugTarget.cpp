#include "DebugTarget.h"
#include "Capstone.h"
#include <iostream>
using namespace std;


DebugTarget::DebugTarget()
{
	// ������Ϣ
	m_stcDbEvent = { 0 };
	// ���̾��
	m_hProcess = 0;
	// �߳̾��
	m_hThread = 0;
	// �쳣��Ϣ
	m_stcException = { 0 };
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

	// �رվ��
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	// ��ʼ�����������
	Capstone::Init();

	return true;
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
		
		// ���ɵ����¼�
		result = DispatchDebugEvent(&m_stcDbEvent);

		// ���ݲ����쳣��λ�ùرվ��
		CloseExceptionHandles();

		// �ظ������¼��Ĵ�����
		ContinueDebugEvent(m_stcDbEvent.dwProcessId,
			m_stcDbEvent.dwThreadId, result);
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
DWORD DebugTarget::DispatchDebugEvent(LPDEBUG_EVENT DbEvent)
{
	// ���ڱ��������Ϣ�Ĵ�����
	DWORD result = DBG_CONTINUE;

	switch (DbEvent->dwDebugEventCode)
	{
		// ���������¼�
		case CREATE_PROCESS_DEBUG_EVENT:
		{
			break;
		}
		// �����쳣��Ϣ�¼�
		case EXCEPTION_DEBUG_EVENT:
		{
			result = OnHandleException(&m_stcDbEvent.u.Exception);
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
DWORD DebugTarget::OnHandleException(EXCEPTION_DEBUG_INFO* ExceptionInfo)
{
	// �쳣����
	DWORD ExceptionCode = ExceptionInfo->ExceptionRecord.ExceptionCode;
	// �쳣��ַ
	LPVOID ExceptionAddress = ExceptionInfo->ExceptionRecord.ExceptionAddress;

	switch (ExceptionCode)
	{
		// ����ϵ�
		case EXCEPTION_BREAKPOINT:
		{
			break;
		}
		// Ӳ���ϵ�
		case EXCEPTION_SINGLE_STEP:
		{
			break;
		}
		// �ڴ�ϵ�
		case EXCEPTION_ACCESS_VIOLATION:
		{
			break;
		}
	}

	return DBG_CONTINUE;
}
