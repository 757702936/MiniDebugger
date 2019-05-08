#include "DebugTarget.h"
#include "MyCapstone.h"
#include "User.h"
#include "BreakPoint.h"
#include <iostream>
using namespace std;

// HookDll 路径
#define HOOKDLLPATH "D:\\Codes\\VS\\Project\\MiniDebugger\\Debug\\HookDll.dll"


DebugTarget::DebugTarget()
{
	// 调试信息
	m_stcDbEvent = { 0 };
	// 进程句柄
	m_hProcess = 0;
	// 线程句柄
	m_hThread = 0;
	// 系统断点
	m_bIsSystemBP = true;
	// 是否需要用户输入
	m_bNeedInput = true;
	// OEP
	m_OEP = 0;
	// 寄存器状态
	m_stcCT = { 0 };
	// 附加进程调试方式标志
	m_bIsOpenPid = false;

	m_dwPid = 0;

	m_hHookProcess = 0;

	m_dwCtCout = 0;
}


DebugTarget::~DebugTarget()
{
}

// 打开文件
bool DebugTarget::open(const char* file)
{
	// 创建进程时需要使用的结构体
	STARTUPINFOA si = { sizeof(STARTUPINFOA) };
	PROCESS_INFORMATION pi = { 0 };

	// 以调试方式 创建一个进程
	BOOL isSuccess = CreateProcessA(file, NULL, NULL, NULL, FALSE,
		DEBUG_ONLY_THIS_PROCESS | CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	if (!isSuccess)
	{
		cout << "创建调试目标进程失败！" << endl;
		return false;
	}

	m_dwPid = pi.dwProcessId;
	m_hHookProcess = pi.hProcess;
	// 注入DLL
	//InjectDll();

	// 关闭句柄
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	// 初始化反汇编引擎
	MyCapstone::Init();

	return true;
}

// 附加进程调试
bool DebugTarget::OpenPid(DWORD pid)
{
	HANDLE hToken;
	HANDLE hProcess = GetCurrentProcess();  // 获取当前进程句柄

	// 打开当前进程的Token，就是一个权限令牌，第二个参数可以用TOKEN_ALL_ACCESS
	if (OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		TOKEN_PRIVILEGES tkp;
		if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid))
		{
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

			//通知系统修改进程权限
			BOOL bREt = AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, 0);
		}
		CloseHandle(hToken);
	}

	// 初始化反汇编引擎
	MyCapstone::Init();

	m_bIsOpenPid = true;

	return DebugActiveProcess(pid);
}

// 调试循环
void DebugTarget::DebugLoop()
{
	// 用于保存调试信息的处理结果
	DWORD result = DBG_CONTINUE;
	while (true)
	{
		// 等待调试事件
		WaitForDebugEvent(&m_stcDbEvent, INFINITE);

		// 根据产生异常的位置打开句柄
		OpenExceptionHandles();

		// 用户类获取句柄
		User::GetProcessHandle(m_hProcess);
		User::GetThreadHandle(m_hThread);
		User::GetExceptionAddress(m_stcDbEvent.u.Exception.ExceptionRecord.ExceptionAddress);
		
		// 分派调试事件
		result = DispatchDebugEvent();

		// 回复调试事件的处理结果
		ContinueDebugEvent(m_stcDbEvent.dwProcessId,
			m_stcDbEvent.dwThreadId, result);

		// 根据产生异常的位置关闭句柄
		CloseExceptionHandles();
	}
}

// 根据产生异常的位置打开句柄
void DebugTarget::OpenExceptionHandles()
{
	m_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_stcDbEvent.dwProcessId);
	m_hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, m_stcDbEvent.dwThreadId);
}

// 根据产生异常的位置关闭句柄
void DebugTarget::CloseExceptionHandles()
{
	CloseHandle(m_hThread);
	CloseHandle(m_hProcess);
}

// 分派调试事件
DWORD DebugTarget::DispatchDebugEvent()
{
	// 用于保存调试信息的处理结果
	DWORD result = DBG_CONTINUE;

	switch (m_stcDbEvent.dwDebugEventCode)
	{
		// 创建进程事件
		case CREATE_PROCESS_DEBUG_EVENT:
		{
			m_OEP = (DWORD)m_stcDbEvent.u.CreateProcessInfo.lpStartAddress;
			result = DBG_CONTINUE;
			break;
		}
		// 产生异常信息事件
		case EXCEPTION_DEBUG_EVENT:
		{
			result = OnHandleException();
			break;
		}
		// 其余情况也返回已处理
		default:
			result = DBG_CONTINUE;
			break;
	}

	return result;
}

// 处理异常
DWORD DebugTarget::OnHandleException()
{
	DWORD dwOldProtect = 0;
	// 异常类型
	DWORD ExceptionCode = m_stcDbEvent.u.Exception.ExceptionRecord.ExceptionCode;
	// 异常地址
	DWORD ExceptionAddress = (DWORD)m_stcDbEvent.u.Exception.ExceptionRecord.ExceptionAddress;

	DWORD MemoryExceptionAddress = m_stcDbEvent.u.Exception.ExceptionRecord.ExceptionInformation[1];
	BreakPoint::GetMemoryExceptionAddress(MemoryExceptionAddress);

	bool bCtCout = true;

	switch (ExceptionCode)
	{
		// 修复软件断点
		case EXCEPTION_BREAKPOINT:
		{
			// 判断是不是系统断点
			if (m_bIsSystemBP)
			{
				// 在 OEP 位置设置一个软件断点
				BreakPoint::SetBreadPoint_Soft(m_hProcess, m_OEP, 0);

				// 下一次就不是系统断点了
				m_bIsSystemBP = false;

				// 这个位置不接收用户输入，第一次只显示
				if (m_bIsOpenPid)
					m_bNeedInput = true;
				else
					m_bNeedInput = false;
				break;
			}
			// 修复当前设置的软件断点
			bCtCout = BreakPoint::FixBreakPoint_Soft(m_hProcess, m_hThread, ExceptionAddress);
			break;
		}
		// 修复硬件断点
		case EXCEPTION_SINGLE_STEP:
		{
			bool bTemp = BreakPoint::FixBreakPoint_Hard(m_hProcess, m_hThread, ExceptionAddress);
			m_bNeedInput = bTemp;
			break;
		}
		// 修复内存断点
		case EXCEPTION_ACCESS_VIOLATION:
		{
			bool bTemp = BreakPoint::FixBreakPoint_Mem(m_hProcess, m_hThread, MemoryExceptionAddress);
			m_bNeedInput = bTemp;
			break;
		}
	}

	// 如果需要断下并接收输入
	if (m_bNeedInput)
	{
		// 获取条件断点次数
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

 //注入DLL
void DebugTarget::InjectDll()
{
#if 1
	// 从目标进程申请一块内存（大小是DLL路径的长度）
	LPVOID lpBuff = VirtualAllocEx(m_hHookProcess, NULL,
		1, // //由于是按粒度（4096字节）分配内存，写1也是相当于分配4K
		MEM_COMMIT,
		PAGE_READWRITE);

	// 将 DLL 路径写入到目标进程中
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
	// 创建远程线程
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
		printf("进程打开失败\n");
		return;
	}
	//2.从目标进程中申请一块内存（大小是DLL路径的长度）
	LPVOID lpBuf = VirtualAllocEx(hProcess,
		NULL,
		1, //由于是按粒度（4096字节）分配内存，写1也是一样的
		MEM_COMMIT,
		PAGE_READWRITE);
	//3.将dll路径写入到目标进程中
	DWORD dwWrite;
	WriteProcessMemory(hProcess,
		lpBuf, "D:\\Codes\\VS\\Project\\MiniDebugger\\Debug\\HookDll.dll",
		strlen("D:\\Codes\\VS\\Project\\MiniDebugger\\Debug\\HookDll.dll"),
		&dwWrite);
	//4.创建远程线程
	HANDLE hThread = CreateRemoteThread(hProcess,
		NULL, NULL,
		(LPTHREAD_START_ROUTINE)LoadLibraryA,
		lpBuf, 0, 0);
#endif
}
