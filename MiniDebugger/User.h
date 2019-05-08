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

// 保存 DLL 信息
struct PluginInfo
{
	char name[20];
	HMODULE module; // 模块句柄
};

// 获取DLL名字
struct Info
{
	char name[20];
};

// 插件版本
typedef int(*pPluginVer)(Info& info);
// 执行插件
typedef void(*ExecPlugin)();

// 用户交互：显示界面，获取用户输入，设置断点...
class User
{
public:
	User();
	~User();

	// 获取用户输入
	static DWORD GetUserInput();

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

	// 返回用户输入的 内存执行断点地址
	static DWORD ReturnInputAddress();

	// 获取条件断点循环次数
	static DWORD GetConditionCount();

	// 初始化插件
	static void InitPlugins();

	// 执行插件
	static void ExecutePlugins();

private:
	// 进程句柄
	static HANDLE m_hProcess;
	// 线程句柄
	static HANDLE m_hThread;
	// 异常地址
	static void* m_pAddress;
	// 控制台输出句柄
	static HANDLE m_hStdOut;
	// 存储用户输入的 内存执行断点地址
	static DWORD m_dwMemExcAddress;
	// 条件断点循环次数
	static DWORD m_dwCount;
	// 存储插件信息
	static vector<PluginInfo> m_vecPlugins;
};

