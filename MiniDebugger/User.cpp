#include "User.h"
#include <iostream>
#include <string>
#include "BreakPoint.h"
#include "MyCapstone.h"
#include "MyXEDParse.h"
#include "HightLight.h"
#include <psapi.h>
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")


User::User()
{
}


User::~User()
{
}

// 标志寄存器
typedef struct _EFLAGS
{
	unsigned CF : 1;  // 进位或错位
	unsigned Reserve1 : 1;
	unsigned PF : 1;  // 计算结果低位包含偶数个1时，此标志为1
	unsigned Reserve2 : 1;
	unsigned AF : 1;  // 辅助进位标志，当位3处有进位或借位时该标志为1
	unsigned Reserve3 : 1;
	unsigned ZF : 1;  // 计算结果为0时，此标志为1
	unsigned SF : 1;  // 符号标志，计算结果为负时该标志为1
	unsigned TF : 1;  // * 陷阱标志，此标志为1时，CPU每次仅会执行1条指令
	unsigned IF : 1;  // 中断标志，为0时禁止响应（屏蔽中断），为1时恢复
	unsigned DF : 1;  // 方向标志
	unsigned OF : 1;  // 溢出标志，计算结果超出机器表达范围时为1，否则为0
	unsigned IOPL : 2;  // 用于标明当前任务的I/O特权级
	unsigned NT : 1;  // 任务嵌套标志
	unsigned Reserve4 : 1;
	unsigned RF : 1;  // 调试异常相应控制标志位，为1禁止响应指令断点异常
	unsigned VM : 1;  // 为1时启用虚拟8086模式
	unsigned AC : 1;  // 内存对齐检查标志
	unsigned VIF : 1;  // 虚拟中断标志
	unsigned VIP : 1;  // 虚拟中断标志
	unsigned ID : 1;  // CPUID检查标志
	unsigned Reserve5 : 10;
}EFLAGS, * PEFLAGS;

// 初始化静态成员
HANDLE User::m_hProcess = 0;
HANDLE User::m_hThread = 0;
void* User::m_pAddress = 0;
HANDLE User::m_hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
DWORD User::m_dwMemExcAddress = 0;

// 获取用户输入
DWORD User::GetUserInput()
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
			cout << "是否设置为永久断点：是(1)，否(0)" << endl;
			BOOL bTemp = true;
			scanf_s("%d", &bTemp);
			BreakPoint::SetBreadPoint_Soft(m_hProcess, Address, bTemp);
			continue;
		}
		else if (!strcmp(inputStr, "bme")) // 设置内存执行断点
		{
			cout << "输入要设置的地址: ";
			scanf_s("%x", &m_dwMemExcAddress);
			BreakPoint::SetBreakPoint_Mem(m_hProcess, m_dwMemExcAddress, false, 8);
			continue;
		}
		else if (!strcmp(inputStr, "bmr")) // 设置内存读断点
		{
			//DWORD Address = 0;
			cout << "输入要设置的地址: ";
			scanf_s("%x", &m_dwMemExcAddress);
			BreakPoint::SetBreakPoint_Mem(m_hProcess, m_dwMemExcAddress, false, 0);
			continue;
		}
		else if (!strcmp(inputStr, "bmw")) // 设置内存写断点
		{
			DWORD Address = 0;
			cout << "输入要设置的地址: ";
			scanf_s("%x", &Address);
			BreakPoint::SetBreakPoint_Mem(m_hProcess, Address, false, 1);
			continue;
		}
		else if (!strcmp(inputStr, "bhe")) // 硬件执行断点
		{
			DWORD Address = 0;
			cout << "输入要设置的地址: ";
			scanf_s("%x", &Address);
			/*cout << "是否设置为永久断点：是(1)，否(0)" << endl;
			BOOL bTemp = true;
			scanf_s("%d", &bTemp);*/
			BreakPoint::SetBreakPoint_Hard(m_hThread, Address/*, bTemp*/);
			continue;
		}
		else if (!strcmp(inputStr, "bhr")) // 硬件读断点
		{
			DWORD Address = 0;
			cout << "输入要设置的地址: ";
			scanf_s("%x", &Address);
			BreakPoint::SetBreakPoint_Hard(m_hThread, Address, /*false, */3, 3);
			continue;
		}
		else if (!strcmp(inputStr, "bhw")) // 硬件写断点
		{
			DWORD Address = 0;
			cout << "输入要设置的地址: ";
			scanf_s("%x", &Address);
			BreakPoint::SetBreakPoint_Hard(m_hThread, Address, 1, 3);
			continue;
		}
		else if (!strcmp(inputStr, "bct")) // 条件断点
		{
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
			CONTEXT ct = { CONTEXT_ALL };
			BYTE c[2] = { 0 };
			DWORD	dwRead = 0;

			GetThreadContext(m_hThread, &ct);
			ReadProcessMemory(m_hProcess, m_pAddress, c, 2, &dwRead);
			/**
			* call 的机器码有:
			* 0xe8 : 5byte,
			* 0x9a : 7byte,
			* 0xff :
			*	 0x10ff ~ 0x1dff
			* rep 前缀的指令也可以步过
			*/
			if (c[0] == 0xe8/*call*/
				|| c[0] == 0xf3/*rep*/
				|| c[0] == 0x9a/*call*/
				|| (c[0] == 0xff && 0x10 <= c[1] && c[1] <= 0x1d)/*call*/
				)
			{
				DWORD len = MyCapstone::GetOpcodeLen(m_hProcess, m_pAddress, 10);
				ct.Eip += len;
				BreakPoint::SetBreadPoint_Soft(m_hProcess, ct.Eip, 0);
			}
			else
			{
				BreakPoint::SetBreakPoint_TF(m_hThread);
			}
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
		else if (!strcmp(inputStr, "hk")) // 反反调试
		{
			
			continue;
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
		else if (!strcmp(inputStr, "rr")) // 查看寄存器
		{
			SearchRegisterInfo();
			continue;
		}
		else if (!strcmp(inputStr, "re")) // 修改寄存器
		{
			char regBuff[10] = { 0 };
			DWORD data = 0;
			cout << "输入要修改的寄存器: ";
			scanf_s("%s", regBuff, 10);
			cout << "输入要修改的值: ";
			scanf_s("%X", &data);
			ModifyRegisterInfo(regBuff, data);
			continue;
		}
		else if (!strcmp(inputStr, "k")) // 查看栈
		{
			SearchStackInfo();
			continue;
		}
		else if (!strcmp(inputStr, "m")) // 查看模块
		{
			ShowMyModuleInfo();
			continue;
		}
		else if (!strcmp(inputStr, "h")) // 查看帮助
		{
			system("cls");
			ShowHelpManual();
			continue;
		}
		else
		{
			cout << "输入的指令错误！" << endl;
		}
	} // while
	return DBG_CONTINUE;
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
	cout << "\tbme - 设置内存执行断点" << endl;
	cout << "\tbmr - 设置内存读断点" << endl;
	cout << "\tbmw - 设置内存写断点" << endl;
	cout << "\tbhe - 硬件执行断点" << endl;
	cout << "\tbhr - 硬件读断点" << endl;
	cout << "\tbhw - 硬件写断点" << endl;
	cout << "\tbct - 条件断点" << endl;
	cout << "\tbl - 查看断点列表" << endl;
	cout << "\tbc - 删除指定断点" << endl;
	cout << "<2.运行控制>" << endl;
	cout << "\tg - 运行" << endl;
	cout << "\tt - 单步步入" << endl;
	cout << "\tp - 单步步过" << endl;
	cout << "\tgr - 运行到返回" << endl;
	cout << "\tq - 退出调试" << endl;
	cout << "\thk - 反反调试" << endl;
	cout << "<3.信息查看>" << endl;
	cout << "\td(db/dw/dd/da/du) - 查看内存" << endl;
	cout << "\te(eb/ew/ed/ea/eu) - 修改内存" << endl;
	cout << "\tu - 查看反汇编" << endl;
	cout << "\ta - 修改反汇编" << endl;
	cout << "\trr - 查看寄存器" << endl;
	cout << "\tre - 修改寄存器" << endl;
	cout << "\tk - 查看栈" << endl;
	cout << "\tm - 查看模块" << endl;
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
	//char *buff = tmpBuff;
	char* buff = new char[80];
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

// 打印寄存器数据
void User::PrintReg(SIZE_T reg, WORD color)
{
	SetConsoleTextAttribute(m_hStdOut, color);
	printf(" %08X", reg);
	SetConsoleTextAttribute(m_hStdOut, F_WHITE);
	printf(" |");
}

// 打印标志寄存器数据
void User::PrintEflag(DWORD flag, WORD color)
{
	SetConsoleTextAttribute(m_hStdOut, color);
	printf("%3d ", flag);
	SetConsoleTextAttribute(m_hStdOut, F_WHITE);
	printf("|");
}

// 查看寄存器状态
void User::ShowRegisterInfo(const CONTEXT& ct)
{
	static bool bFirst = true;
	static CONTEXT preCT = { 0 };
	if (bFirst)
	{
		bFirst = false;
		preCT = ct;
	}

	printf("------------------------------------------------------------------\n");
	printf("    eax   |    ecx   |    edx   |    ebx   |    esi   |    edi   |\n");
	// EAX
	if (preCT.Eax != ct.Eax)
		PrintReg(ct.Eax, F_H_RED);
	else
		PrintReg(ct.Eax, F_WHITE);
	// ECX
	if (preCT.Ecx != ct.Ecx)
		PrintReg(ct.Ecx, F_H_RED);
	else
		PrintReg(ct.Ecx, F_WHITE);
	// EDX
	if (preCT.Edx != ct.Edx)
		PrintReg(ct.Edx, F_H_RED);
	else
		PrintReg(ct.Edx, F_WHITE);
	// EBX
	if (preCT.Ebx != ct.Ebx)
		PrintReg(ct.Ebx, F_H_RED);
	else
		PrintReg(ct.Ebx, F_WHITE);
	// ESI
	if (preCT.Esi != ct.Esi)
		PrintReg(ct.Esi, F_H_RED);
	else
		PrintReg(ct.Esi, F_WHITE);
	// EDI
	if (preCT.Edi != ct.Edi)
		PrintReg(ct.Edi, F_H_RED);
	else
		PrintReg(ct.Edi, F_WHITE);
	printf("\n");

	// 打印栈顶、栈底、标志位寄存器
	printf("    esp   |    ebp   ||||| CF | PF | AF | ZF | SF | TF | DF | OF |\n");
	PEFLAGS pEflags = (PEFLAGS)& ct.EFlags;
	PEFLAGS pPreEflags = (PEFLAGS)& preCT.EFlags;
	// ESP
	if (preCT.Esp != ct.Esp)
		PrintReg(ct.Esp, F_H_RED);
	else
		PrintReg(ct.Esp, F_WHITE);
	// EBP
	if (preCT.Ebp != ct.Ebp)
		PrintReg(ct.Ebp, F_H_RED);
	else
		PrintReg(ct.Ebp, F_WHITE);

	printf("||||");

	// CF
	if (pPreEflags->CF != pEflags->CF)
		PrintEflag(pEflags->CF, F_H_RED);
	else
		PrintEflag(pEflags->CF, F_WHITE);
	// PF
	if (pPreEflags->PF != pEflags->PF)
		PrintEflag(pEflags->PF, F_H_RED);
	else
		PrintEflag(pEflags->PF, F_WHITE);
	// AF
	if (pPreEflags->AF != pEflags->AF)
		PrintEflag(pEflags->AF, F_H_RED);
	else
		PrintEflag(pEflags->AF, F_WHITE);
	// ZF
	if (pPreEflags->ZF != pEflags->ZF)
		PrintEflag(pEflags->ZF, F_H_RED);
	else
		PrintEflag(pEflags->ZF, F_WHITE);
	// SF
	if (pPreEflags->SF != pEflags->SF)
		PrintEflag(pEflags->SF, F_H_RED);
	else
		PrintEflag(pEflags->SF, F_WHITE);
	// TF
	if (pPreEflags->TF != pEflags->TF)
		PrintEflag(pEflags->TF, F_H_RED);
	else
		PrintEflag(pEflags->TF, F_WHITE);
	// DF
	if (pPreEflags->DF != pEflags->DF)
		PrintEflag(pEflags->DF, F_H_RED);
	else
		PrintEflag(pEflags->DF, F_WHITE);
	// OF
	if (pPreEflags->OF != pEflags->OF)
		PrintEflag(pEflags->OF, F_H_RED);
	else
		PrintEflag(pEflags->OF, F_WHITE);
	printf("\n");

	printf("--> eip  %08X\n", ct.Eip);
	preCT = ct;
	printf("------------------------------------------------------------------\n");
}

// 查看寄存器的值
void User::SearchRegisterInfo()
{
	CONTEXT ct = { CONTEXT_ALL };
	GetThreadContext(m_hThread, &ct);

	printf("------------------------------------------------------------------\n");
	printf("    eax   |    ecx   |    edx   |    ebx   |    esi   |    edi   |\n");
	// EAX
	PrintReg(ct.Eax, F_WHITE);
	PrintReg(ct.Ecx, F_WHITE);
	PrintReg(ct.Edx, F_WHITE);
	PrintReg(ct.Ebx, F_WHITE);
	PrintReg(ct.Esi, F_WHITE);
	PrintReg(ct.Edi, F_WHITE);
	printf("\n");

	// 打印栈顶、栈底、标志位寄存器
	printf("    esp   |    ebp   ||||| CF | PF | AF | ZF | SF | TF | DF | OF |\n");
	PEFLAGS pEflags = (PEFLAGS)& ct.EFlags;
	PrintReg(ct.Esp, F_WHITE);
	PrintReg(ct.Ebp, F_WHITE);

	printf("||||");

	PrintEflag(pEflags->CF, F_WHITE);
	PrintEflag(pEflags->PF, F_WHITE);
	PrintEflag(pEflags->AF, F_WHITE);
	PrintEflag(pEflags->ZF, F_WHITE);
	PrintEflag(pEflags->SF, F_WHITE);
	PrintEflag(pEflags->TF, F_WHITE);
	PrintEflag(pEflags->DF, F_WHITE);
	PrintEflag(pEflags->OF, F_WHITE);
	printf("\n");

	printf("--> eip  %08X\n", ct.Eip);

}

// 修改寄存器的值
void User::ModifyRegisterInfo(const char* regBuff, DWORD data)
{
	CONTEXT ct = { CONTEXT_ALL };
	GetThreadContext(m_hThread, &ct);

	// EAX
	if (!strcmp(regBuff, "eax"))
		ct.Eax = data;
	// ECX
	else if (!strcmp(regBuff, "ecx"))
		ct.Ecx = data;
	// EDX
	else if (!strcmp(regBuff, "edx"))
		ct.Edx = data;
	// EBX
	else if (!strcmp(regBuff, "ebx"))
		ct.Ebx = data;
	// ESI
	else if (!strcmp(regBuff, "esi"))
		ct.Esi = data;
	// EDI
	else if (!strcmp(regBuff, "edi"))
		ct.Edi = data;
	// ESP
	else if (!strcmp(regBuff, "esp"))
		ct.Esp = data;
	// EBP
	else if (!strcmp(regBuff, "ebp"))
		ct.Ebp = data;
	
	SetThreadContext(m_hThread, &ct);
}

// 查看模块信息
void User::ShowMyModuleInfo()
{
	// 自定义模块结构体，存到向量中
	MYMODULEINFO stcModule = { 0 };
	vector<MYMODULEINFO> vecModuleInfo;

	// 枚举进程模块
	DWORD dwNeed = 0;
	// 第一次获取模块个数
	EnumProcessModulesEx(m_hProcess, nullptr, 0, &dwNeed, LIST_MODULES_ALL);
	DWORD dwModuleCount = dwNeed / sizeof(HMODULE); // dwNeed 是所有模块的大小
	// 根据获取的个数申请缓存
	HMODULE* pModule = new HMODULE[dwModuleCount];
	// 第二次获取模块信息到 pModule
	EnumProcessModulesEx(m_hProcess, pModule, dwNeed, &dwNeed, LIST_MODULES_ALL);
	// 存储路径
	char path[MAX_PATH] = { 0 };
	// 存储模块信息结构体（系统）
	MODULEINFO mi = { 0 };
	// 循环获取模块详细信息
	for (size_t i = 0; i < dwModuleCount; ++i)
	{
		// 获取模块路径
		GetModuleFileNameExA(m_hProcess, pModule[i], path, MAX_PATH);
		GetModuleInformation(m_hProcess, pModule[i], &mi, sizeof(MODULEINFO));
		LPSTR tmp = PathFindFileNameA(path);
		memcpy(stcModule.name, tmp, MAX_PATH); // 名字
		stcModule.startAddress = (DWORD)mi.lpBaseOfDll; // 加载基址
		stcModule.nSize = mi.SizeOfImage; // 大小
		vecModuleInfo.push_back(stcModule);
	}
	// 显示信息
	printf("+------------------+----------+----------------------------------------------------+\n");
	printf("|     加载基址     + 模块大小 |                    模块名                          |\n");
	printf("+------------------+----------+----------------------------------------------------+\n");
	for (size_t i = 0; i < vecModuleInfo.size(); ++i)
	{
		printf("| %08X | %08X | %-50s |\n",
			vecModuleInfo[i].startAddress,
			vecModuleInfo[i].nSize,
			vecModuleInfo[i].name);
	}
	printf("+------------------+----------+----------------------------------------------------+\n");
	delete[] pModule;
}

// 返回用户输入的 内存执行断点地址
DWORD User::ReturnInputAddress()
{
	return m_dwMemExcAddress;
}
