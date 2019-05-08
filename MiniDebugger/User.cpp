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

// ��־�Ĵ���
typedef struct _EFLAGS
{
	unsigned CF : 1;  // ��λ���λ
	unsigned Reserve1 : 1;
	unsigned PF : 1;  // ��������λ����ż����1ʱ���˱�־Ϊ1
	unsigned Reserve2 : 1;
	unsigned AF : 1;  // ������λ��־����λ3���н�λ���λʱ�ñ�־Ϊ1
	unsigned Reserve3 : 1;
	unsigned ZF : 1;  // ������Ϊ0ʱ���˱�־Ϊ1
	unsigned SF : 1;  // ���ű�־��������Ϊ��ʱ�ñ�־Ϊ1
	unsigned TF : 1;  // * �����־���˱�־Ϊ1ʱ��CPUÿ�ν���ִ��1��ָ��
	unsigned IF : 1;  // �жϱ�־��Ϊ0ʱ��ֹ��Ӧ�������жϣ���Ϊ1ʱ�ָ�
	unsigned DF : 1;  // �����־
	unsigned OF : 1;  // �����־������������������ﷶΧʱΪ1������Ϊ0
	unsigned IOPL : 2;  // ���ڱ�����ǰ�����I/O��Ȩ��
	unsigned NT : 1;  // ����Ƕ�ױ�־
	unsigned Reserve4 : 1;
	unsigned RF : 1;  // �����쳣��Ӧ���Ʊ�־λ��Ϊ1��ֹ��Ӧָ��ϵ��쳣
	unsigned VM : 1;  // Ϊ1ʱ��������8086ģʽ
	unsigned AC : 1;  // �ڴ�������־
	unsigned VIF : 1;  // �����жϱ�־
	unsigned VIP : 1;  // �����жϱ�־
	unsigned ID : 1;  // CPUID����־
	unsigned Reserve5 : 10;
}EFLAGS, * PEFLAGS;

// ��ʼ����̬��Ա
HANDLE User::m_hProcess = 0;
HANDLE User::m_hThread = 0;
void* User::m_pAddress = 0;
HANDLE User::m_hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
DWORD User::m_dwMemExcAddress = 0;
DWORD User::m_dwCount = 0;


// ��ȡ�û�����
DWORD User::GetUserInput()
{
	cout << "������ָ�";
	char inputStr[30] = { 0 };
	while (cin >> inputStr)
	{
		if (!strcmp(inputStr, "bp")) // ��������ϵ�
		{
			DWORD Address = 0;
			cout << "����Ҫ���õĵ�ַ: ";
			scanf_s("%x", &Address);
			cout << "�Ƿ�����Ϊ���öϵ㣺��(1)����(0)" << endl;
			BOOL bTemp = true;
			scanf_s("%d", &bTemp);
			if (1 == bTemp)
			{
				cout << "����Ҫѭ���Ĵ�����";
				scanf_s("%d", &m_dwCount);
			}
			BreakPoint::SetBreadPoint_Soft(m_hProcess, Address, bTemp);
			continue;
		}
		else if (!strcmp(inputStr, "bme")) // �����ڴ�ִ�жϵ�
		{
			cout << "����Ҫ���õĵ�ַ: ";
			scanf_s("%x", &m_dwMemExcAddress);
			BreakPoint::SetBreakPoint_Mem(m_hProcess, m_dwMemExcAddress, false, 8);
			continue;
		}
		else if (!strcmp(inputStr, "bmr")) // �����ڴ���ϵ�
		{
			//DWORD Address = 0;
			cout << "����Ҫ���õĵ�ַ: ";
			scanf_s("%x", &m_dwMemExcAddress);
			BreakPoint::SetBreakPoint_Mem(m_hProcess, m_dwMemExcAddress, false, 0);
			continue;
		}
		else if (!strcmp(inputStr, "bmw")) // �����ڴ�д�ϵ�
		{
			DWORD Address = 0;
			cout << "����Ҫ���õĵ�ַ: ";
			scanf_s("%x", &Address);
			BreakPoint::SetBreakPoint_Mem(m_hProcess, Address, false, 1);
			continue;
		}
		else if (!strcmp(inputStr, "bhe")) // Ӳ��ִ�жϵ�
		{
			DWORD Address = 0;
			cout << "����Ҫ���õĵ�ַ: ";
			scanf_s("%x", &Address);
			/*cout << "�Ƿ�����Ϊ���öϵ㣺��(1)����(0)" << endl;
			BOOL bTemp = true;
			scanf_s("%d", &bTemp);*/
			BreakPoint::SetBreakPoint_Hard(m_hThread, Address/*, bTemp*/);
			continue;
		}
		else if (!strcmp(inputStr, "bhr")) // Ӳ�����ϵ�
		{
			DWORD Address = 0;
			cout << "����Ҫ���õĵ�ַ: ";
			scanf_s("%x", &Address);
			BreakPoint::SetBreakPoint_Hard(m_hThread, Address, /*false, */3, 3);
			continue;
		}
		else if (!strcmp(inputStr, "bhw")) // Ӳ��д�ϵ�
		{
			DWORD Address = 0;
			cout << "����Ҫ���õĵ�ַ: ";
			scanf_s("%x", &Address);
			BreakPoint::SetBreakPoint_Hard(m_hThread, Address, 1, 3);
			continue;
		}
		else if (!strcmp(inputStr, "bl")) // �鿴�ϵ��б�
		{
			break;
		}
		else if (!strcmp(inputStr, "bc")) // ɾ��ָ���ϵ�
		{
			break;
		}
		else if (!strcmp(inputStr, "g")) // ����
		{
			break;
		}
		else if (!strcmp(inputStr, "t")) // ��������
		{
			BreakPoint::SetBreakPoint_TF(m_hThread);
			break;
		}
		else if (!strcmp(inputStr, "p")) // ��������
		{
			CONTEXT ct = { CONTEXT_ALL };
			BYTE c[2] = { 0 };
			DWORD	dwRead = 0;

			GetThreadContext(m_hThread, &ct);
			ReadProcessMemory(m_hProcess, m_pAddress, c, 2, &dwRead);
			/**
			* call �Ļ�������:
			* 0xe8 : 5byte,
			* 0x9a : 7byte,
			* 0xff :
			*	 0x10ff ~ 0x1dff
			* rep ǰ׺��ָ��Ҳ���Բ���
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
		else if (!strcmp(inputStr, "gr")) // ���е�����
		{
			break;
		}
		else if (!strcmp(inputStr, "q")) // �˳�����
		{
			ExitProcess(0);
		}
		else if (!strcmp(inputStr, "d")) // d(db/dw/dd/da/du) - �鿴�ڴ�
		{
			DWORD Address = 0;
			cout << "����Ҫ���õĵ�ַ: ";
			scanf_s("%x", &Address);
			SearchMemoryInfo(Address);
			continue;
		}
		else if (!strcmp(inputStr, "e")) // e(eb/ew/ed/ea/eu) - �޸��ڴ�
		{
			DWORD Address = 0;
			char buff[10] = { 0 };
			size_t nSize = 0;
			cout << "����Ҫ���õĵ�ַ: ";
			scanf_s("%x", &Address);
			cout << "�����޸ĳ��ȣ�";
			scanf_s("%d", &nSize);
			cout << "�����޸����ݣ�";
			scanf_s("%s", buff, 10);
			ModifyMemoryInfo(Address, buff, nSize);
			continue;
		}
		else if (!strcmp(inputStr, "u")) // �鿴�����
		{
			MyCapstone::DisAsm(m_hProcess, m_pAddress, 10);
			continue;
		}
		else if (!strcmp(inputStr, "a")) // �޸ķ����
		{
			DWORD Address = 0;
			cout << "����Ҫ���õĵ�ַ: ";
			scanf_s("%x", &Address);
			MyXEDParse::AsmToOpcode(m_hProcess, Address);
			continue;
		}
		else if (!strcmp(inputStr, "rr")) // �鿴�Ĵ���
		{
			SearchRegisterInfo();
			continue;
		}
		else if (!strcmp(inputStr, "re")) // �޸ļĴ���
		{
			char regBuff[10] = { 0 };
			DWORD data = 0;
			cout << "����Ҫ�޸ĵļĴ���: ";
			scanf_s("%s", regBuff, 10);
			cout << "����Ҫ�޸ĵ�ֵ: ";
			scanf_s("%X", &data);
			ModifyRegisterInfo(regBuff, data);
			continue;
		}
		else if (!strcmp(inputStr, "k")) // �鿴ջ
		{
			SearchStackInfo();
			continue;
		}
		else if (!strcmp(inputStr, "m")) // �鿴ģ��
		{
			ShowMyModuleInfo();
			continue;
		}
		else if (!strcmp(inputStr, "h")) // �鿴����
		{
			system("cls");
			ShowHelpManual();
			continue;
		}
		else
		{
			cout << "�����ָ�����" << endl;
		}
	} // while
	return DBG_CONTINUE;
}

// ��ʾ�û�����
void User::ShowUI()
{
	cout << "**************************************************" << endl;
	cout << "*                     ������                     *" << endl;
	cout << "*                ���� h �鿴����                 *" << endl;
	cout << "**************************************************" << endl;
}

// ��ʾ������Ϣ
void User::ShowHelpManual()
{
	cout << "\n<1.�ϵ�>" << endl;
	cout << "\tbp - ��������ϵ�" << endl;
	cout << "\tbme - �����ڴ�ִ�жϵ�" << endl;
	cout << "\tbmr - �����ڴ���ϵ�" << endl;
	cout << "\tbmw - �����ڴ�д�ϵ�" << endl;
	cout << "\tbhe - Ӳ��ִ�жϵ�" << endl;
	cout << "\tbhr - Ӳ�����ϵ�" << endl;
	cout << "\tbhw - Ӳ��д�ϵ�" << endl;
	cout << "\tbl - �鿴�ϵ��б�" << endl;
	cout << "\tbc - ɾ��ָ���ϵ�" << endl;
	cout << "<2.���п���>" << endl;
	cout << "\tg - ����" << endl;
	cout << "\tt - ��������" << endl;
	cout << "\tp - ��������" << endl;
	cout << "\tgr - ���е�����" << endl;
	cout << "\tq - �˳�����" << endl;
	cout << "<3.��Ϣ�鿴>" << endl;
	cout << "\td(db/dw/dd/da/du) - �鿴�ڴ�" << endl;
	cout << "\te(eb/ew/ed/ea/eu) - �޸��ڴ�" << endl;
	cout << "\tu - �鿴�����" << endl;
	cout << "\ta - �޸ķ����" << endl;
	cout << "\trr - �鿴�Ĵ���" << endl;
	cout << "\tre - �޸ļĴ���" << endl;
	cout << "\tk - �鿴ջ" << endl;
	cout << "\tm - �鿴ģ��" << endl;
	cout << "\th - �鿴����\n" << endl;
}

// ��ȡ���̾��
void User::GetProcessHandle(HANDLE hProcess)
{
	m_hProcess = hProcess;	
}

// ��ȡ�쳣��ַ
void User::GetExceptionAddress(void* address)
{
	m_pAddress = address;
}

// ��ȡ�߳̾��
void User::GetThreadHandle(HANDLE hThread)
{
	m_hThread = hThread;
}

// �鿴�ڴ�����
void User::SearchMemoryInfo(DWORD address)
{
	SIZE_T byte = 0;
	DWORD oldProtect = 0;
	unsigned char ch = 0;
	char tmpBuff[80] = { 0 };
	//char *buff = tmpBuff;
	char* buff = new char[80];
	// �޸��ڴ汣������
	VirtualProtectEx(m_hProcess, (LPVOID)address, 80,
		PAGE_EXECUTE_READWRITE, &oldProtect);
	// ��ȡ��ԭ�����ݱ��浽�ṹ��
	ReadProcessMemory(m_hProcess, (LPVOID)address, buff, 80, &byte);
	// ��ԭ�ڴ汣������
	VirtualProtectEx(m_hProcess, (LPVOID)address, 80, oldProtect, &oldProtect);
	// ��ӡ����
	printf("--------+--------------------------------------------------+------------------+\n");
	printf("        |  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  |                  |\n");
	printf("--------+--------------------------------------------------+------------------+\n");

	// ��ӡ 5 ��
	for (size_t i = 0; i < 5; ++i)
	{
		// ��ӡ��ַ
		printf("%08X| ", address);
		address += 16;
		for (size_t i = 0; i < 16; ++i)
		{
			ch = buff[i];
			printf("%02X ", ch);
		}
		printf(" | ");
		// ��ӡ��ַ�������
		for (int i = 0; i < 16; ++i)
		{
			printf("%c ", buff[i] < 33 || buff[i] > 126 ? '.' : buff[i]);
		}
		printf(" |\n");
		buff += 16;
	}
	printf("--------+--------------------------------------------------+------------------+\n");
}

// �޸��ڴ�����
void User::ModifyMemoryInfo(DWORD address, char* buff, size_t nSize)
{
	SIZE_T byte = 0;
	DWORD oldProtect = 0;

	// �޸��ڴ汣������
	VirtualProtectEx(m_hProcess, (LPVOID)address, nSize,
		PAGE_EXECUTE_READWRITE, &oldProtect);
	// �� Ҫ�޸ĵ����� д�뵽Ŀ��λ��
	WriteProcessMemory(m_hProcess, (LPVOID)address, buff, nSize, &byte);
	// ��ԭ�ڴ汣������
	VirtualProtectEx(m_hProcess, (LPVOID)address, nSize, oldProtect, &oldProtect);
}

// �鿴ջ��Ϣ
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

// ��ӡ�Ĵ�������
void User::PrintReg(SIZE_T reg, WORD color)
{
	SetConsoleTextAttribute(m_hStdOut, color);
	printf(" %08X", reg);
	SetConsoleTextAttribute(m_hStdOut, F_WHITE);
	printf(" |");
}

// ��ӡ��־�Ĵ�������
void User::PrintEflag(DWORD flag, WORD color)
{
	SetConsoleTextAttribute(m_hStdOut, color);
	printf("%3d ", flag);
	SetConsoleTextAttribute(m_hStdOut, F_WHITE);
	printf("|");
}

// �鿴�Ĵ���״̬
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

	// ��ӡջ����ջ�ס���־λ�Ĵ���
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

// �鿴�Ĵ�����ֵ
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

	// ��ӡջ����ջ�ס���־λ�Ĵ���
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

// �޸ļĴ�����ֵ
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

// �鿴ģ����Ϣ
void User::ShowMyModuleInfo()
{
	// �Զ���ģ��ṹ�壬�浽������
	MYMODULEINFO stcModule = { 0 };
	vector<MYMODULEINFO> vecModuleInfo;

	// ö�ٽ���ģ��
	DWORD dwNeed = 0;
	// ��һ�λ�ȡģ�����
	EnumProcessModulesEx(m_hProcess, nullptr, 0, &dwNeed, LIST_MODULES_ALL);
	DWORD dwModuleCount = dwNeed / sizeof(HMODULE); // dwNeed ������ģ��Ĵ�С
	// ���ݻ�ȡ�ĸ������뻺��
	HMODULE* pModule = new HMODULE[dwModuleCount];
	// �ڶ��λ�ȡģ����Ϣ�� pModule
	EnumProcessModulesEx(m_hProcess, pModule, dwNeed, &dwNeed, LIST_MODULES_ALL);
	// �洢·��
	char path[MAX_PATH] = { 0 };
	// �洢ģ����Ϣ�ṹ�壨ϵͳ��
	MODULEINFO mi = { 0 };
	// ѭ����ȡģ����ϸ��Ϣ
	for (size_t i = 0; i < dwModuleCount; ++i)
	{
		// ��ȡģ��·��
		GetModuleFileNameExA(m_hProcess, pModule[i], path, MAX_PATH);
		GetModuleInformation(m_hProcess, pModule[i], &mi, sizeof(MODULEINFO));
		LPSTR tmp = PathFindFileNameA(path);
		memcpy(stcModule.name, tmp, MAX_PATH); // ����
		stcModule.startAddress = (DWORD)mi.lpBaseOfDll; // ���ػ�ַ
		stcModule.nSize = mi.SizeOfImage; // ��С
		vecModuleInfo.push_back(stcModule);
	}
	// ��ʾ��Ϣ
	printf("+------------------+----------+----------------------------------------------------+\n");
	printf("|     ���ػ�ַ     + ģ���С |                    ģ����                          |\n");
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

// �����û������ �ڴ�ִ�жϵ��ַ
DWORD User::ReturnInputAddress()
{
	return m_dwMemExcAddress;
}

// ��ȡ�����ϵ�ѭ������
DWORD User::GetConditionCount()
{
	return m_dwCount;
}
