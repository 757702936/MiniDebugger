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

	// 获取线程句柄
	static void GetThreadHandle(HANDLE hThread);

private:
	// 进程句柄
	static HANDLE m_hProcess;
	// 线程句柄
	static HANDLE m_hThread;
};

