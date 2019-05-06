#include "MyCapstone.h"

// 初始化静态变量
csh MyCapstone::Handle = { 0 };
cs_opt_mem MyCapstone::OptMem = { 0 };

// 用于初始化函数
void MyCapstone::Init()
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
	cs_open(CS_ARCH_X86, CS_MODE_32, &MyCapstone::Handle);
}

// 反汇编指定条数的语句
void MyCapstone::DisAsm(HANDLE Handle, LPVOID Addr, DWORD Count)
{
	// 用来读取指令位置内存的缓冲区信息
	cs_insn* ins = nullptr;
	PCHAR buff = new CHAR[Count * 16]{ 0 };

	// 读取指定长度的内存空间
	DWORD dwWrite = 0;
	ReadProcessMemory(Handle, (LPVOID)Addr, buff, Count * 16, &dwWrite);
	int count = cs_disasm(MyCapstone::Handle, (uint8_t*)buff, Count * 16, (uint64_t)Addr, 0, &ins);
	for (DWORD i = 0; i < Count; ++i)
	{
		printf("%08X\t", (UINT)ins[i].address);
		for (uint16_t j = 0; j < 16; ++j)
		{
			if (j < ins[i].size)
				printf("%02X", ins[i].bytes[j]);
			else
				printf("  ");
		}
		// 输出对应的反汇编
		printf("\t%s %s\n", ins[i].mnemonic, ins[i].op_str);
	}
	printf("\n");
	// 释放动态分配的空间
	delete[] buff;
	cs_free(ins, count);
}

// 获取指令长度
UINT MyCapstone::GetOpcodeLen(HANDLE Handle, LPVOID Addr, DWORD Count)
{
	// 用来读取指令位置内存的缓冲区信息
	cs_insn* ins = nullptr;
	PCHAR buff = new CHAR[Count * 16]{ 0 };

	// 读取指定长度的内存空间
	DWORD dwWrite = 0;
	ReadProcessMemory(Handle, (LPVOID)Addr, buff, Count * 16, &dwWrite);
	cs_disasm(MyCapstone::Handle, (uint8_t*)buff, Count * 16, (uint64_t)Addr, 0, &ins);

	return ins->size;
}
