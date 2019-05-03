#include "DebugTarget.h"
#include "Capstone.h"
#include <iostream>
using namespace std;


DebugTarget::DebugTarget()
{
	// 调试信息
	m_stcDbEvent = { 0 };
	// 进程句柄
	m_hProcess = 0;
	// 线程句柄
	m_hThread = 0;
	// 异常信息
	m_stcException = { 0 };
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

	// 关闭句柄
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	// 初始化反汇编引擎
	Capstone::Init();

	return true;
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
		
		// 分派调试事件
		result = DispatchDebugEvent(&m_stcDbEvent);

		// 根据产生异常的位置关闭句柄
		CloseExceptionHandles();

		// 回复调试事件的处理结果
		ContinueDebugEvent(m_stcDbEvent.dwProcessId,
			m_stcDbEvent.dwThreadId, result);
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
DWORD DebugTarget::DispatchDebugEvent(LPDEBUG_EVENT DbEvent)
{
	// 用于保存调试信息的处理结果
	DWORD result = DBG_CONTINUE;

	switch (DbEvent->dwDebugEventCode)
	{
		// 创建进程事件
		case CREATE_PROCESS_DEBUG_EVENT:
		{
			break;
		}
		// 产生异常信息事件
		case EXCEPTION_DEBUG_EVENT:
		{
			result = OnHandleException(&m_stcDbEvent.u.Exception);
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
DWORD DebugTarget::OnHandleException(EXCEPTION_DEBUG_INFO* ExceptionInfo)
{
	// 异常类型
	DWORD ExceptionCode = ExceptionInfo->ExceptionRecord.ExceptionCode;
	// 异常地址
	LPVOID ExceptionAddress = ExceptionInfo->ExceptionRecord.ExceptionAddress;

	switch (ExceptionCode)
	{
		// 软件断点
		case EXCEPTION_BREAKPOINT:
		{
			break;
		}
		// 硬件断点
		case EXCEPTION_SINGLE_STEP:
		{
			break;
		}
		// 内存断点
		case EXCEPTION_ACCESS_VIOLATION:
		{
			break;
		}
	}

	return DBG_CONTINUE;
}
