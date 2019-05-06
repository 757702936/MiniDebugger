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
			break;
		}
		else if (!strcmp(inputStr, "e")) // e(eb/ew/ed/ea/eu) - �޸��ڴ�
		{
			break;
		}
		//else if (!strcmp(inputStr, "u")) // �鿴�����
		//{
		//	CONTEXT ct = { 0 };
		//	ct.ContextFlags = CONTEXT_CONTROL;
		//	GetThreadContext(m_hThread, &ct);
		//	BYTE buff[512];
		//	DWORD dwRead = 0;
		//	ReadProcessMemory(m_hProcess, (LPVOID)ct.Eip, buff, 512, &dwRead);
		//	MyCapstone::DisAsm(m_hProcess, (LPVOID)ct.Eip, 10);
		//	break;
		//}
		else if (!strcmp(inputStr, "a")) // �޸ķ����
		{
			DWORD Address = 0;
			cout << "����Ҫ���õĵ�ַ: ";
			scanf_s("%x", &Address);
			MyXEDParse::AsmToOpcode(m_hProcess, Address);
			BreakPoint::SetBreadPoint_Soft(m_hProcess, Address);
			break;
		}
		else if (!strcmp(inputStr, "r")) // �鿴/�޸ļĴ���
		{
			break;
		}
		else if (!strcmp(inputStr, "k")) // �鿴ջ
		{
			break;
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

// ��ȡ�߳̾��
void User::GetThreadHandle(HANDLE hThread)
{
	m_hThread = hThread;
}
