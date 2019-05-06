#include "BreakPoint.h"
#include <iostream>
using namespace std;

// DR7�Ĵ����ṹ��
typedef struct _DBG_REG7 {
	unsigned L0 : 1; unsigned G0 : 1;
	unsigned L1 : 1; unsigned G1 : 1;
	unsigned L2 : 1; unsigned G2 : 1;
	unsigned L3 : 1; unsigned G3 : 1;
	unsigned LE : 1; unsigned GE : 1;
	unsigned : 6;// ��������Ч�ռ�
	unsigned RW0 : 2; unsigned LEN0 : 2;
	unsigned RW1 : 2; unsigned LEN1 : 2;
	unsigned RW2 : 2; unsigned LEN2 : 2;
	unsigned RW3 : 2; unsigned LEN3 : 2;
} R7, * PR7;


BreakPoint::BreakPoint()
{
}


BreakPoint::~BreakPoint()
{
}

// ��ʼ�� ����
vector<BreakPointInfo> BreakPoint::m_vecBP;

// ����TF�ϵ�
bool BreakPoint::SetBreakPoint_TF(HANDLE hThread)
{
	CONTEXT ct = { 0 };
	ct.ContextFlags = CONTEXT_CONTROL; // ָ����Ҫ��ȡ�ļĴ���
	// ��ȡ�̻߳���
	bool getResult = GetThreadContext(hThread, &ct);
	if (!getResult)
	{
		cout << "TF�ϵ㣺��ȡ�̻߳���ʧ��" << endl;
		return false;
	}
	// �� TF ��־λ��Ϊ 1�� TF��־λ�� Flags �Ĵ����ĵ� 8 λ��CPU���Զ����� TF λ
	ct.EFlags |= 0x100;
	// ���޸ĺ�ļĴ���״̬���õ�Ŀ���߳�
	bool setResult = SetThreadContext(hThread, &ct);
	if (!setResult)
	{
		cout << "TF�ϵ㣺�����̻߳���ʧ��" << endl;
		return false;
	}

	return true;
}

// ������ϵ�
void BreakPoint::SetBreadPoint_Soft(HANDLE hProcess, DWORD address)
{
	// ����ϵ��ԭ������޸�Ŀ������еġ���һ���ֽڡ�Ϊ
	// 0xCC���޸���ʱ����Ϊ int 3 ��������һ����������
	// ��������ָ�������һ��ָ���λ�ã���ô��Ҫ�� eip ִ
	// �м�����������ԭָ��

	SIZE_T byte = 0;
	DWORD oldProtect = 0;
	// ����ϵ���Ϣ�ṹ��
	BreakPointInfo stcBP_Soft = { bp_soft, address };
	// �޸��ڴ汣������
	VirtualProtectEx(hProcess, (LPVOID)address, 1,
		PAGE_EXECUTE_READWRITE, &oldProtect);
	// ��ȡ��ԭ�����ݱ��浽�ṹ��
	ReadProcessMemory(hProcess, (LPVOID)address, &stcBP_Soft.u.oldOpcode,
		1, &byte);
	// �� 0xCC д�뵽Ŀ��λ��
	WriteProcessMemory(hProcess, (LPVOID)address, "\xCC", 1, &byte);
	// ��ԭ�ڴ汣������
	VirtualProtectEx(hProcess, (LPVOID)address, 1, oldProtect, &oldProtect);
	// ����ϵ�
	m_vecBP.push_back(stcBP_Soft);
}

// �޸�����ϵ�
void BreakPoint::FixBreakPoint_Soft(HANDLE hProcess, HANDLE hThread, DWORD address)
{
	// �ж��Ƿ���Ҫ�޸�
	for (size_t i = 0; i < m_vecBP.size(); ++i)
	{
		// �ж϶ϵ����ͺ͵�ַ�Ƿ�ƥ��
		if (m_vecBP[i].bpFlag == bp_soft
			&& m_vecBP[i].ExceptionAddress == address)
		{
			CONTEXT ct = { CONTEXT_CONTROL };
			// ��ȡ�̻߳���
			GetThreadContext(hThread, &ct);
			// ��Ϊ eip ָ����һ�������� -1
			ct.Eip -= 1;
			// �����̻߳���
			SetThreadContext(hThread, &ct);

			// ��ԭ������д��Ŀ��λ��
			DWORD oldProtect = 0, byte = 0;
			VirtualProtectEx(hProcess, (LPVOID)address, 1,
				PAGE_EXECUTE_READWRITE, &oldProtect);
			WriteProcessMemory(hProcess, (LPVOID)address,
				&m_vecBP[i].u.oldOpcode, 1, &byte);
			VirtualProtectEx(hProcess, (LPVOID)address, 1,
				oldProtect, &oldProtect);

			// 3. ����ϵ��ǲ������öϵ�,�費��Ҫ��ɾ��
			//  - ��Ҫɾ���� erase() 
			//  - ����Ҫɾ��������һ���Ƿ���Ч�ı�־λ
			m_vecBP.erase(m_vecBP.begin() + i);
			break;
		}
	} // for
}

// ����Ӳ���ϵ�
void BreakPoint::SetBreakPoint_Hard(HANDLE hThread, DWORD address, DWORD Type, DWORD Len)
{
	// �����������λ0����ô���ȱ���Ϊ0
	// ֧��Ӳ���ϵ�ļĴ����� 6 ���������� 4 �����ڱ����ַ
	// Ӳ���ϵ����������� 4 �����ٶ��ʧ����

	// ��ȡ���ԼĴ���
	CONTEXT ct = { CONTEXT_DEBUG_REGISTERS };
	GetThreadContext(hThread, &ct);

	// ��ȡ Dr7 �ṹ�岢����
	PR7 Dr7 = (PR7)& ct.Dr7;
	
	if (0 == Len) // ִ�жϵ��ַĬ��
	{
	}
	else if (1 == Len) // ���ϵ㣬2�ֽڵĶ�������
	{
		address = address - address % 2;
	}
	else if (3 == Len) // д�ϵ㣬4�ֽڵĶ�������
	{
		address = address - address % 4;
	}

	// ����ϵ���Ϣ�ṹ��
	BreakPointInfo stcBP_Hard = { bp_harde, address };

	// ͨ�� Dr7 �е�L(n) ֪����ǰ�ĵ��ԼĴ����Ƿ�ʹ��
	if (Dr7->L0 == FALSE)
	{
		// ����Ӳ���ϵ��Ƿ���Ч
		Dr7->L0 = TRUE;
		// ���öϵ�����
		Dr7->RW0 = Type;
		// ���öϵ��ַ�Ķ��볤��
		Dr7->LEN0 = Len;
		// ���öϵ��ַ
		ct.Dr0 = address;
	}
	else if (Dr7->L1 == FALSE)
	{
		Dr7->L1 = TRUE;
		Dr7->RW1 = Type;
		Dr7->LEN1 = Len;
		ct.Dr1 = address;
	}
	else if (Dr7->L2 == FALSE)
	{
		Dr7->L2 = TRUE;
		Dr7->RW2 = Type;
		Dr7->LEN2 = Len;
		ct.Dr2 = address;
	}
	else if (Dr7->L3 == FALSE)
	{
		Dr7->L3 = TRUE;
		Dr7->RW3 = Type;
		Dr7->LEN3 = Len;
		ct.Dr3 = address;
	}
	else
	{
		return;
	}

	// ���޸ĸ��µ��߳�
	SetThreadContext(hThread, &ct);
	// ��ӵ��ϵ�����
	m_vecBP.push_back(stcBP_Hard);
}

// �޸�Ӳ���ϵ�
void BreakPoint::FixBreakPoint_Hard(HANDLE hThread, DWORD address)
{
	// �޸��Ĺ����У�����Ҫ֪����ʲô�ϵ�
	for (size_t i = 0; i < m_vecBP.size(); ++i)
	{
		// �ж϶ϵ����ͺ͵�ַ�Ƿ�ƥ��
		// ִ��
		if (m_vecBP[i].bpFlag == bp_harde
			&& m_vecBP[i].ExceptionAddress == address)
		{
			// ��ȡ���ԼĴ���
			CONTEXT ct = { CONTEXT_DEBUG_REGISTERS };
			GetThreadContext(hThread, &ct);

			// ��ȡ Dr7 �Ĵ���
			PR7 Dr7 = (PR7)& ct.Dr7;
			// ���� Dr6 �ĵ� 4 λ֪����˭��������
			int index = ct.Dr6 & 0xF;
			// �������Ķϵ����ó���Ч��
			switch (index)
			{
				case 1: Dr7->L0 = 0; break;
				case 3: Dr7->L1 = 0; break;
				case 4: Dr7->L2 = 0; break;
				case 8: Dr7->L3 = 0; break;
			}
			// ���޸ĸ��µ��߳�
			SetThreadContext(hThread, &ct);

			m_vecBP.erase(m_vecBP.begin() + i);
			break;
		}
	} // for
}
