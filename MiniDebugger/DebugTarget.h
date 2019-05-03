#pragma once
#include <Windows.h>

class DebugTarget
{
public:
	DebugTarget();
	~DebugTarget();

	// 打开文件
	bool open(const char* file);

	// 调试循环
	void DebugLoop();

	// 根据产生异常的位置打开句柄
	void OpenExceptionHandles();

	// 根据产生异常的位置关闭句柄
	void CloseExceptionHandles();

	// 分派调试事件
	DWORD DispatchDebugEvent(LPDEBUG_EVENT DbEvent);

	// 处理异常
	DWORD OnHandleException(EXCEPTION_DEBUG_INFO* ExceptionInfo);

private:
	// 调试信息
	DEBUG_EVENT m_stcDbEvent;

	// 进程句柄
	HANDLE m_hProcess;

	// 线程句柄
	HANDLE m_hThread;

	// 异常信息
	EXCEPTION_DEBUG_INFO m_stcException;
};

