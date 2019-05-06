#include "User.h"
#include <iostream>
#include <string>
#include "BreakPoint.h"
#include "MyCapstone.h"
#include "MyXEDParse.h"

using namespace std;

User::User()
{
}


User::~User()
{
}

// 初始化静态成员
HANDLE User::m_hProcess = 0;
HANDLE User::m_hThread = 0;
void* User::m_pAddress = 0;

// 获取用户输入
void User::GetUserInput()
{
	cout << "请输入指令：";
	char inputStr[30] = { 0 };
	while (cin >> inputStr)
	{
		if (!strcmp(inputStr, "bp")) // 设置软件断点
		{
			DWORD Address = 0;
			cout << "输入要设置的地址: ";
			scanf_s("%x", &Address);
			BreakPoint::SetBreadPoint_Soft(m_hProcess, Address);
			break;
		}
		else if (!strcmp(inputStr, "bm")) // 设置内存断点
		{
			break;
		}
		else if (!strcmp(inputStr, "bhe")) // 硬件执行断点
		{
			DWORD Address = 0;
			cout << "输入要设置的地址: ";
			scanf_s("%x", &Address);
			BreakPoint::SetBreakPoint_Hard(m_hThread, Address);
			break;
		}
		else if (!strcmp(inputStr, "bhr")) // 硬件读断点
		{
			DWORD Address = 0;
			cout << "输入要设置的地址: ";
			scanf_s("%x", &Address);
			BreakPoint::SetBreakPoint_Hard(m_hThread, Address, 3, 3);
			break;
		}
		else if (!strcmp(inputStr, "bhw")) // 硬件写断点
		{
			DWORD Address = 0;
			cout << "输入要设置的地址: ";
			scanf_s("%x", &Address);
			BreakPoint::SetBreakPoint_Hard(m_hThread, Address, 1, 3);
			break;
		}
		else if (!strcmp(inputStr, "bl")) // 查看断点列表
		{
			break;
		}
		else if (!strcmp(inputStr, "bc")) // 删除指定断点
		{
			break;
		}
		else if (!strcmp(inputStr, "g")) // 运行
		{
			break;
		}
		else if (!strcmp(inputStr, "t")) // 单步步入
		{
			BreakPoint::SetBreakPoint_TF(m_hThread);
			break;
		}
		else if (!strcmp(inputStr, "p")) // 单步步过
		{
			break;
		}
		else if (!strcmp(inputStr, "gr")) // 运行到返回
		{
			break;
		}
		else if (!strcmp(inputStr, "q")) // 退出调试
		{
			ExitProcess(0);
		}
		else if (!strcmp(inputStr, "d")) // d(db/dw/dd/da/du) - 查看内存
		{
			DWORD Address = 0;
			cout << "输入要设置的地址: ";
			scanf_s("%x", &Address);
			SearchMemoryInfo(Address);
			continue;
		}
		else if (!strcmp(inputStr, "e")) // e(eb/ew/ed/ea/eu) - 修改内存
		{
			DWORD Address = 0;
			char buff[10] = { 0 };
			size_t nSize = 0;
			cout << "输入要设置的地址: ";
			scanf_s("%x", &Address);
			//cout << endl;
			cout << "输入修改长度：";
			scanf_s("%d", &nSize);
			cout << "输入修改内容：";
			scanf_s("%s", buff, 10);
			ModifyMemoryInfo(Address, buff, nSize);
			continue;
		}
		else if (!strcmp(inputStr, "u")) // 查看反汇编
		{
			MyCapstone::DisAsm(m_hProcess, m_pAddress, 10);
			continue;
		}
		else if (!strcmp(inputStr, "a")) // 修改反汇编
		{
			DWORD Address = 0;
			cout << "输入要设置的地址: ";
			scanf_s("%x", &Address);
			MyXEDParse::AsmToOpcode(m_hProcess, Address);
			continue;
		}
		else if (!strcmp(inputStr, "r")) // 查看/修改寄存器
		{
			break;
		}
		else if (!strcmp(inputStr, "k")) // 查看栈
		{
			SearchStackInfo();
			continue;
		}
		else if (!strcmp(inputStr, "h")) // 查看帮助
		{
			system("cls");
			ShowHelpManual();
			break;
		}
		else
		{
			cout << "输入的指令错误！" << endl;
		}
	} // while
}

// 显示用户界面
void User::ShowUI()
{
	cout << "**************************************************" << endl;
	cout << "*                     调试器                     *" << endl;
	cout << "*                输入 h 查看帮助                 *" << endl;
	cout << "**************************************************" << endl;
}

// 显示帮助信息
void User::ShowHelpManual()
{
	cout << "\n<1.断点>" << endl;
	cout << "\tbp - 设置软件断点" << endl;
	cout << "\tbm - 设置内存断点" << endl;
	cout << "\tbhe - 硬件执行断点" << endl;
	cout << "\tbhr - 硬件读断点" << endl;
	cout << "\tbhw - 硬件写断点" << endl;
	cout << "\tbl - 查看断点列表" << endl;
	cout << "\tbc - 删除指定断点" << endl;
	cout << "<2.运行控制>" << endl;
	cout << "\tg - 运行" << endl;
	cout << "\tt - 单步步入" << endl;
	cout << "\tp - 单步步过" << endl;
	cout << "\tgr - 运行到返回" << endl;
	cout << "\tq - 退出调试" << endl;
	cout << "<3.信息查看>" << endl;
	cout << "\td(db/dw/dd/da/du) - 查看内存" << endl;
	cout << "\te(eb/ew/ed/ea/eu) - 修改内存" << endl;
	cout << "\tu - 查看反汇编" << endl;
	cout << "\ta - 修改反汇编" << endl;
	cout << "\tr - 查看/修改寄存器" << endl;
	cout << "\tk - 查看栈" << endl;
	cout << "\th - 查看帮助\n" << endl;
}

// 获取进程句柄
void User::GetProcessHandle(HANDLE hProcess)
{
	m_hProcess = hProcess;	
}

// 获取异常地址
void User::GetExceptionAddress(void* address)
{
	m_pAddress = address;
}

// 获取线程句柄
void User::GetThreadHandle(HANDLE hThread)
{
	m_hThread = hThread;
}

// 查看内存数据
void User::SearchMemoryInfo(DWORD address)
{
	SIZE_T byte = 0;
	DWORD oldProtect = 0;
	unsigned char ch = 0;
	char tmpBuff[80] = { 0 };
	char *buff = tmpBuff;

	// 修改内存保护属性
	VirtualProtectEx(m_hProcess, (LPVOID)address, 80,
		PAGE_EXECUTE_READWRITE, &oldProtect);
	// 获取到原有数据保存到结构体
	ReadProcessMemory(m_hProcess, (LPVOID)address, buff, 80, &byte);
	// 还原内存保护属性
	VirtualProtectEx(m_hProcess, (LPVOID)address, 80, oldProtect, &oldProtect);
	// 打印数据
	printf("--------+--------------------------------------------------+------------------+\n");
	printf("        |  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  |                  |\n");
	printf("--------+--------------------------------------------------+------------------+\n");

	// 打印 5 行
	for (size_t i = 0; i < 5; ++i)
	{
		// 打印地址
		printf("%08X| ", address);
		address += 16;
		for (size_t i = 0; i < 16; ++i)
		{
			ch = buff[i];
			printf("%02X ", ch);
		}
		printf(" | ");
		// 打印地址里的数据
		for (int i = 0; i < 16; ++i)
		{
			printf("%c ", buff[i] < 33 || buff[i] > 126 ? '.' : buff[i]);
		}
		printf(" |\n");
		buff += 16;
	}
	printf("--------+--------------------------------------------------+------------------+\n");
}

// 修改内存数据
void User::ModifyMemoryInfo(DWORD address, char* buff, size_t nSize)
{
	SIZE_T byte = 0;
	DWORD oldProtect = 0;

	// 修改内存保护属性
	VirtualProtectEx(m_hProcess, (LPVOID)address, nSize,
		PAGE_EXECUTE_READWRITE, &oldProtect);
	// 将 要修改的数据 写入到目标位置
	WriteProcessMemory(m_hProcess, (LPVOID)address, buff, nSize, &byte);
	// 还原内存保护属性
	VirtualProtectEx(m_hProcess, (LPVOID)address, nSize, oldProtect, &oldProtect);
}

// 查看栈信息
void User::SearchStackInfo()
{
	CONTEXT ct = { 0 };
	ct.ContextFlags = CONTEXT_CONTROL;
	GetThreadContext(m_hThread, &ct);
	SIZE_T buff[512] = { 0 };
	DWORD dwRead = 0;
	ReadProcessMemory(m_hProcess, (LPVOID)ct.Esp, buff, 512, &dwRead);
	SIZE_T* p = (SIZE_T*)buff;
	SIZE_T address = (SIZE_T)ct.Esp;
	for (size_t i = 0; i < 10; ++i)
	{
		printf("%08X | %08X\n", address, *p);
		++p;
		address += sizeof(SIZE_T);
	}
}
