#pragma once
#include <Windows.h>
#include <vector>
using std::vector;

// �ϵ���(������): �����������Ͷϵ�����á��޸�����ԭ
// - TF �ϵ㣺�����ϵ�(����)
// - ����ϵ㣺ʹ�� int 3 ���õĶϵ�
// - Ӳ���ϵ㣺ͨ��CPU�ṩ�ĵ��ԼĴ������õĶ�\д\ִ�жϵ�(����)��ͨ�� ���ԼĴ��� Dr0~Dr3 Dr7
// - �ڴ�ϵ㣺������ʵ�ĳһ�����ݻ��߶�ĳЩ���ݽ���д���ִ�е�ʱ�����

// �ϵ�ö��
enum BreakPointFlag
{
	bp_TF,      // TF
	bp_soft,	// �����int 3��
	bp_harde,	// Ӳ��ִ��
	bp_hardr,	// Ӳ����
	bp_hardw,	// Ӳ��д
	bp_meme,	// �ڴ�ִ��
	bp_memr,	// �ڴ��
	bp_memw     // �ڴ�д
};

struct BreakPointInfo
{
	BreakPointFlag bpFlag;
	DWORD ExceptionAddress;
	union
	{
		char oldOpcode;
	} u;
};

// �ϵ���
class BreakPoint
{
public:
	BreakPoint();
	~BreakPoint();

	// ����TF�ϵ�
	static bool SetBreakPoint_TF(HANDLE hThread);

	// ������ϵ�
	static void SetBreadPoint_Soft(HANDLE hProcess, DWORD address, bool temp);
	// �޸�����ϵ�
	static bool FixBreakPoint_Soft(HANDLE hProcess, HANDLE hThread, DWORD address);

	// ����Ӳ���ϵ�
	static void SetBreakPoint_Hard(HANDLE hThread, DWORD address, /*bool temp = false, */DWORD Type = 0, DWORD Len = 0);
	// �޸�Ӳ���ϵ�
	static bool FixBreakPoint_Hard(HANDLE hProcess, HANDLE hThread, DWORD address);

	// �����ڴ�ϵ�
	static void SetBreakPoint_Mem(HANDLE hProcess, DWORD address, bool temp, DWORD type);
	// �޸��ڴ�ϵ�
	static bool FixBreakPoint_Mem(HANDLE hProcess, HANDLE hThread, DWORD address);
	// ��ȡ�ڴ�ִ�жϵ��쳣��ַ
	static void GetMemoryExceptionAddress(DWORD address);

	// ��ȡӲ���޸����öϵ��־λ
	//static bool GetFixHardAlwaysFlag();

	// ����������öϵ�
	static void SetSoftAlways(HANDLE hProcess);
	// ����Ӳ�����öϵ�
	//static void SetHardAlways(HANDLE hThread, DWORD address);

	// ��ȡ�����ϵ�ѭ������
	static DWORD GetConditionCount();

private:
	// �������жϵ�
	static vector<BreakPointInfo> m_vecBP;
	// �洢�ڴ�ִ�жϵ��쳣��ַ
	static DWORD m_dwMemExceptionAddr;
	// �����޸��ڴ�����
	static DWORD m_dwOldProtect;
	// �ж��Ƿ����ڴ�ִ�жϵ�
	static bool m_bIsMeme;
	// ��������ϵ��־λ
	static bool m_bIsSoftAlways;
	// ����Ӳ���ϵ��־λ
	//static bool m_bIsHardAlways;
	// ����Ӳ���޸��ϵ��־λ
	//static bool m_bIsFixHardAlways;
	// �����ڴ�ϵ��־λ
	//static bool m_bIsMemAlways;
	// ��ȡ�����ϵ�
	static DWORD m_dwCtCout;
};

