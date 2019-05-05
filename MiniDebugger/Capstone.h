#pragma once
#include <Windows.h>
#include "Capstone/include/capstone.h"
#pragma comment(lib,"capstone/capstone.lib")
#pragma comment(linker, "/NODEFAULTLIB:\"libcmtd.lib\"")

// 工具类
// 反汇编引擎
class Capstone
{
public:// 设置为默认构造函数
	Capstone() = default;
	~Capstone() = default;

	// 用于初始化函数
	static void Init();
	// 用于执行反汇编的函数
	static void DisAsm(HANDLE Handle, LPVOID Addr, DWORD Count);

private:
	// 用于初始化和内存管理的句柄
	static csh Handle;
	static cs_opt_mem OptMem;
};
