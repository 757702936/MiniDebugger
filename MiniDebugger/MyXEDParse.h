#pragma once
//1. 导入头文件
#include "XEDParse/XEDParse.h"

#ifdef _WIN64
#pragma comment (lib,"XEDParse/x64/XEDParse_x64.lib")
#else
#pragma comment (lib,"XEDParse/x86/XEDParse_x86.lib")
#endif // _WIN64


class MyXEDParse
{
public:
	MyXEDParse();
	~MyXEDParse();

	// 打印 Opcode
	static void printOpcode(const unsigned char* pOpcode, int nSize);

	// 获取汇编指令，并转成 Opcode
	static void AsmToOpcode(HANDLE hProcess, DWORD address);
};

