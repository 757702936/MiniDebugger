#pragma once
#include <Windows.h>
#include "Capstone/include/Capstone.h"
#pragma comment(lib,"Capstone/Capstone.lib")
#pragma comment(linker, "/NODEFAULTLIB:\"libcmtd.lib\"")

// 工具类
// 反汇编引擎
class MyCapstone
{
public:// 设置为默认构造函数
	MyCapstone() = default;
	~MyCapstone() = default;

	// 用于初始化函数
	static void Init();
	// 用于执行反汇编的函数
	static void DisAsm(HANDLE Handle, LPVOID Addr, DWORD Count);

private:
	// 用于初始化和内存管理的句柄
	static csh Handle;
	static cs_opt_mem OptMem;
};
