#pragma once
#include <Windows.h>
#include <vector>
using namespace std;

// 模块信息结构体
typedef struct _MYMODULEINFO
{
	char name[MAX_PATH];
	DWORD startAddress;
	DWORD nSize;
} MYMODULEINFO;


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

	// 打印寄存器数据
	static void PrintReg(SIZE_T reg, WORD color);

	// 打印标志寄存器数据
	static void PrintEflag(DWORD flag, WORD color);

	// 查看寄存器状态
	static void ShowRegisterInfo(const CONTEXT& ct);
	
	// 查看寄存器的值
	static void SearchRegisterInfo();

	// 修改寄存器的值
	static void ModifyRegisterInfo(const char* regBuff, DWORD data);

	// 查看模块信息
	static void ShowMyModuleInfo();

private:
	// 进程句柄
	static HANDLE m_hProcess;
	// 线程句柄
	static HANDLE m_hThread;
	// 异常地址
	static void* m_pAddress;
	// 控制台输出句柄
	static HANDLE m_hStdOut;
};

