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

// ��ʼ����̬��Ա
HANDLE User::m_hProcess = 0;
HANDLE User::m_hThread = 0;
void* User::m_pAddress = 0;

// ��ȡ�û�����
void User::GetUserInput()
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
			BreakPoint::SetBreadPoint_Soft(m_hProcess, Address);
			break;
		}
		else if (!strcmp(inputStr, "bm")) // �����ڴ�ϵ�
		{
			break;
		}
		else if (!strcmp(inputStr, "bhe")) // Ӳ��ִ�жϵ�
		{
			DWORD Address = 0;
			cout << "����Ҫ���õĵ�ַ: ";
			scanf_s("%x", &Address);
			BreakPoint::SetBreakPoint_Hard(m_hThread, Address);
			break;
		}
		else if (!strcmp(inputStr, "bhr")) // Ӳ�����ϵ�
		{
			DWORD Address = 0;
			cout << "����Ҫ���õĵ�ַ: ";
			scanf_s("%x", &Address);
			BreakPoint::SetBreakPoint_Hard(m_hThread, Address, 3, 3);
			break;
		}
		else if (!strcmp(inputStr, "bhw")) // Ӳ��д�ϵ�
		{
			DWORD Address = 0;
			cout << "����Ҫ���õĵ�ַ: ";
			scanf_s("%x", &Address);
			BreakPoint::SetBreakPoint_Hard(m_hThread, Address, 1, 3);
			break;
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
			//cout << endl;
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
		else if (!strcmp(inputStr, "r")) // �鿴/�޸ļĴ���
		{
			break;
		}
		else if (!strcmp(inputStr, "k")) // �鿴ջ
		{
			SearchStackInfo();
			continue;
		}
		else if (!strcmp(inputStr, "h")) // �鿴����
		{
			system("cls");
			ShowHelpManual();
			break;
		}
		else
		{
			cout << "�����ָ�����" << endl;
		}
	} // while
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
	cout << "\tbm - �����ڴ�ϵ�" << endl;
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
	cout << "\tr - �鿴/�޸ļĴ���" << endl;
	cout << "\tk - �鿴ջ" << endl;
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
	char *buff = tmpBuff;

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
