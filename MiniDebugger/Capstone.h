#pragma once
#include <Windows.h>
#include "Capstone/include/capstone.h"
#pragma comment(lib,"capstone/capstone.lib")
#pragma comment(linker, "/NODEFAULTLIB:\"libcmtd.lib\"")


class Capstone
{
public:// 设置为默认构造函数
	Capstone() = default;
	~Capstone() = default;

	// 用于初始化函数
	static void Init();

private:
	// 用于初始化和内存管理的句柄
	static csh Handle;
	static cs_opt_mem OptMem;
};
