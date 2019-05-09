#pragma once
#include <Windows.h>
#include <vector>
using std::vector;

// 断点类(工具类): 管理所有类型断点的设置、修复、还原
// - TF 断点：单步断点(步入)
// - 软件断点：使用 int 3 设置的断点
// - 硬件断点：通过CPU提供的调试寄存器设置的读\写\执行断点(单步)，通过 调试寄存器 Dr0~Dr3 Dr7
// - 内存断点：程序访问到某一块数据或者对某些数据进行写入或执行的时候断下

// 断点枚举
enum BreakPointFlag
{
	bp_TF,      // TF
	bp_soft,	// 软件（int 3）
	bp_harde,	// 硬件执行
	bp_hardr,	// 硬件读
	bp_hardw,	// 硬件写
	bp_meme,	// 内存执行
	bp_memr,	// 内存读
	bp_memw     // 内存写
};

struct BreakPointInfo
{
	BreakPointFlag bpFlag;
	DWORD ExceptionAddress;
	union
	{
		char oldOpcode;
	} u;
};

// 断点类
class BreakPoint
{
public:
	BreakPoint();
	~BreakPoint();

	// 设置TF断点
	static bool SetBreakPoint_TF(HANDLE hThread);

	// 设置软断点
	static void SetBreadPoint_Soft(HANDLE hProcess, DWORD address, bool temp);
	// 修复软件断点
	static bool FixBreakPoint_Soft(HANDLE hProcess, HANDLE hThread, DWORD address);

	// 设置硬件断点
	static void SetBreakPoint_Hard(HANDLE hThread, DWORD address, /*bool temp = false, */DWORD Type = 0, DWORD Len = 0);
	// 修复硬件断点
	static bool FixBreakPoint_Hard(HANDLE hProcess, HANDLE hThread, DWORD address);

	// 设置内存断点
	static void SetBreakPoint_Mem(HANDLE hProcess, DWORD address, bool temp, DWORD type);
	// 修复内存断点
	static bool FixBreakPoint_Mem(HANDLE hProcess, HANDLE hThread, DWORD address);
	// 获取内存执行断点异常地址
	static void GetMemoryExceptionAddress(DWORD address);

	// 获取硬件修复永久断点标志位
	//static bool GetFixHardAlwaysFlag();

	// 设置软件永久断点
	static void SetSoftAlways(HANDLE hProcess);
	// 设置硬件永久断点
	//static void SetHardAlways(HANDLE hThread, DWORD address);

	// 获取条件断点循环次数
	static DWORD GetConditionCount();

	// 获取调试器交给被调试程序处理标志位状态
	static bool GetNoHandle();

private:
	// 保存所有断点
	static vector<BreakPointInfo> m_vecBP;
	// 存储内存执行断点异常地址
	static DWORD m_dwMemExceptionAddr;
	// 保存修改内存属性
	static DWORD m_dwOldProtect;
	// 判断是否是内存执行断点
	static bool m_bIsMeme;
	// 永久软件断点标志位
	static bool m_bIsSoftAlways;
	// 永久硬件断点标志位
	//static bool m_bIsHardAlways;
	// 永久硬件修复断点标志位
	//static bool m_bIsFixHardAlways;
	// 永久内存断点标志位
	//static bool m_bIsMemAlways;
	// 获取条件断点
	static DWORD m_dwCtCout;
	// 调试器交给被调试程序处理标志位
	static bool m_bNoHandle;
};

