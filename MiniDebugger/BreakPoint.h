#pragma once
#include <Windows.h>
#include <vector>
using std::vector;

// 断点类(工具类): 管理所有类型断点的设置、修复、还原
// - TF 断点：单步断点(步入)
// - 软件断点：使用 int 3 设置的断点
// - 硬件断点：通过CPU提供的调试寄存器设置的读\写\执行断点(单步)，通过 调试寄存器 Dr0~Dr3 Dr7
// - 内存断点：程序访问到某一块数据或者对某些数据进行写入或执行的时候断下

enum BreakPointFlag
{
	bp_TF,
	bp_soft,
	bp_harde,
	bp_hardr,
	bp_hardw,
	bp_mem
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
	static void SetBreadPoint_Soft(HANDLE hProcess, DWORD address);
	// 修复软件断点
	static void FixBreakPoint_Soft(HANDLE hProcess, HANDLE hThread, DWORD address);

	// 设置硬件断点
	static void SetBreakPoint_Hard(HANDLE hThread, DWORD address, DWORD Type = 0, DWORD Len = 0);
	// 修复硬件断点
	static void FixBreakPoint_Hard(HANDLE hThread, DWORD address);

	// 设置内存断点
	//static void SetBreakPoint_Mem(HANDLE hThread);
	// 修复内存断点

private:
	// 保存所有断点
	static vector<BreakPointInfo> m_vecBP;
};

