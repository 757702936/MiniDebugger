#pragma once
#include <Windows.h>


// 用户交互：显示界面，获取用户输入，设置断点...
class User
{
public:
	User();
	~User();

	// 获取用户输入
	static void GetUserInput();

	// 显示用户界面
	static void ShowUI();

	// 显示帮助信息
	static void ShowHelpManual();

	// 获取进程句柄
	static void GetProcessHandle(HANDLE hProcess);

	// 获取异常地址
	static void GetExceptionAddress(void* address);

	// 获取线程句柄
	static void GetThreadHandle(HANDLE hThread);

	// 查看内存数据
	static void SearchMemoryInfo(DWORD address);

	// 修改内存数据
	static void ModifyMemoryInfo(DWORD address, char* buff, size_t nSize);

	// 查看栈信息
	static void SearchStackInfo();

private:
	// 进程句柄
	static HANDLE m_hProcess;
	// 线程句柄
	static HANDLE m_hThread;
	// 异常地址
	static void* m_pAddress;
};

