#include "MyXEDParse.h"
#include <cstdio>


MyXEDParse::MyXEDParse()
{
}


MyXEDParse::~MyXEDParse()
{
}

// ��ӡ Opcode
void MyXEDParse::printOpcode(const unsigned char* pOpcode, int nSize)
{
	for (int i = 0; i < nSize; ++i)
	{
		printf("%02X ", pOpcode[i]);
	}
}

// ��ȡ���ָ���ת�� OpCode
void MyXEDParse::AsmToOpcode(HANDLE hProcess, DWORD address)
{
	SIZE_T byte = 0;
	DWORD oldProtect = 0;

	XEDPARSE xed = { 0 };

	// ��������opcode�ĵĳ�ʼ��ַ
	xed.cip = address;
	getchar();

	// ����ָ��
	printf("ָ�");
	gets_s(xed.instr, XEDPARSE_MAXBUFSIZE);

	// xed.cip, ��������תƫ�Ƶ�ָ��ʱ,��Ҫ��������ֶ�
	if (XEDPARSE_OK != XEDParseAssemble(&xed))
	{
		printf("ָ�����%s\n", xed.error);
		//continue;
		return;
	}
	xed.instr;

	// ��ӡ���ָ�������ɵ�opcode
	printf("%08X : ", (unsigned int)xed.cip);
	printOpcode(xed.dest, xed.dest_size);
	printf("\n");

	// �޸��ڴ汣������
	VirtualProtectEx(hProcess, (LPVOID)address, xed.dest_size,
		PAGE_EXECUTE_READWRITE, &oldProtect);
	// ��ȡ��ԭ�����ݱ��浽�ṹ��
	// ReadProcessMemory(hProcess, (LPVOID)address, &stcBP_Soft.u.oldOpcode, 1, &byte);
	// �� 0xCC д�뵽Ŀ��λ��
	WriteProcessMemory(hProcess, (LPVOID)address, xed.dest, xed.dest_size, &byte);
	// ��ԭ�ڴ汣������
	VirtualProtectEx(hProcess, (LPVOID)address, xed.dest_size, oldProtect, &oldProtect);

	// ����ַ���ӵ���һ��ָ����׵�ַ
	xed.cip += xed.dest_size;
}
