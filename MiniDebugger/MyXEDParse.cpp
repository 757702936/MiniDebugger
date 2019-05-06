#include "MyXEDParse.h"
#include <cstdio>


MyXEDParse::MyXEDParse()
{
}


MyXEDParse::~MyXEDParse()
{
}

// 打印 Opcode
void MyXEDParse::printOpcode(const unsigned char* pOpcode, int nSize)
{
	for (int i = 0; i < nSize; ++i)
	{
		printf("%02X ", pOpcode[i]);
	}
}

// 获取汇编指令，并转成 OpCode
void MyXEDParse::AsmToOpcode(HANDLE hProcess, DWORD address)
{
	SIZE_T byte = 0;
	DWORD oldProtect = 0;

	XEDPARSE xed = { 0 };

	// 接受生成opcode的的初始地址
	xed.cip = address;
	getchar();

	// 接收指令
	printf("指令：");
	gets_s(xed.instr, XEDPARSE_MAXBUFSIZE);

	// xed.cip, 汇编带有跳转偏移的指令时,需要配置这个字段
	if (XEDPARSE_OK != XEDParseAssemble(&xed))
	{
		printf("指令错误：%s\n", xed.error);
		//continue;
		return;
	}
	// 打印汇编指令所生成的opcode
	//printf("%08X : ", (unsigned int)xed.cip);
	//printOpcode(xed.dest, xed.dest_size);
	//printf("\n");

	// 修改内存保护属性
	VirtualProtectEx(hProcess, (LPVOID)address, xed.dest_size,
		PAGE_EXECUTE_READWRITE, &oldProtect);
	WriteProcessMemory(hProcess, (LPVOID)address, xed.dest, xed.dest_size, &byte);
	// 还原内存保护属性
	VirtualProtectEx(hProcess, (LPVOID)address, xed.dest_size, oldProtect, &oldProtect);

	// 将地址增加到下一条指令的首地址
	//xed.cip += xed.dest_size;
}

