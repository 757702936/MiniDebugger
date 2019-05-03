#include "Capstone.h"



Capstone::Capstone()
{
}


Capstone::~Capstone()
{
}

// 用于初始化函数
void Capstone::Init()
{
	// 配置堆空间的回调函数
	OptMem.free = free;
	OptMem.calloc = calloc;
	OptMem.malloc = malloc;
	OptMem.realloc = realloc;
	OptMem.vsnprintf = (cs_vsnprintf_t)vsprintf_s;

	// 注册堆空间管理组函数
	cs_option(NULL, CS_OPT_MEM, (size_t)& OptMem);

	// 打开一个句柄
	cs_open(CS_ARCH_X86, CS_MODE_32, &Capstone::Handle);
}
