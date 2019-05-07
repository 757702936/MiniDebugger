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
	static void SetBreadPoint_Soft(HANDLE hProcess, DWORD address);
	// �޸�����ϵ�
	static void FixBreakPoint_Soft(HANDLE hProcess, HANDLE hThread, DWORD address);

	// ����Ӳ���ϵ�
	static void SetBreakPoint_Hard(HANDLE hThread, DWORD address, DWORD Type = 0, DWORD Len = 0);
	// �޸�Ӳ���ϵ�
	static bool FixBreakPoint_Hard(HANDLE hProcess, HANDLE hThread, DWORD address);

	// �����ڴ�ϵ�
	static void SetBreakPoint_Mem(HANDLE hProcess, DWORD address, DWORD type);
	// �޸��ڴ�ϵ�
	static bool FixBreakPoint_Mem(HANDLE hProcess, HANDLE hThread, DWORD address);
	// ��ȡ�ڴ�ִ�жϵ��쳣��ַ
	static void GetMemoryExceptionAddress(DWORD address);

private:
	// �������жϵ�
	static vector<BreakPointInfo> m_vecBP;
	// �洢�ڴ�ִ�жϵ��쳣��ַ
	static DWORD m_dwMemExceptionAddr;
	// �����޸��ڴ�����
	static DWORD m_dwOldProtect;
	// �ж��Ƿ����ڴ�ִ�жϵ�
	static bool m_bIsMeme;
};

