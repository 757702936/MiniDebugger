#pragma once
#include <Windows.h>

// 调试目标：调试循环，分派调试事件，处理异常...
class DebugTarget
{
public:
	DebugTarget();
	~DebugTarget();

	// 默认方式打开文件调试
	bool open(const char* file);

	// 附加进程调试
	bool OpenPid(DWORD pid);

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

	// 注入DLL
	void InjectDll();

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

	// 附加进程调试方式标志
	bool m_bIsOpenPid;

	// 创建进程PID
	DWORD m_dwPid;

	// 存储 Hook 句柄
	HANDLE m_hHookProcess;

	// 获取条件断点
	DWORD m_dwCtCout;
};

