#pragma once
#include <Windows.h>

// 调试目标：调试循环，分派调试事件，处理异常...
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
	DWORD DispatchDebugEvent();

	// 处理异常
	DWORD OnHandleException();

private:
	// 调试信息
	DEBUG_EVENT m_stcDbEvent;

	// 进程句柄
	HANDLE m_hProcess;

	// 线程句柄
	HANDLE m_hThread;

	// OEP
	DWORD m_OEP;

	// 系统断点
	bool m_bIsSystemBP;

	// 是否需要用户输入
	bool m_bNeedInput;

	// 寄存器状态
	CONTEXT m_stcCT;
};

