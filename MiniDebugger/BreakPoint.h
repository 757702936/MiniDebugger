#pragma once
#include <Windows.h>
#include <vector>
using std::vector;

// �ϵ���(������): �����������Ͷϵ�����á��޸�����ԭ
// - TF �ϵ㣺�����ϵ�(����)
// - ����ϵ㣺ʹ�� int 3 ���õĶϵ�
// - Ӳ���ϵ㣺ͨ��CPU�ṩ�ĵ��ԼĴ������õĶ�\д\ִ�жϵ�(����)��ͨ�� ���ԼĴ��� Dr0~Dr3 Dr7
// - �ڴ�ϵ㣺������ʵ�ĳһ�����ݻ��߶�ĳЩ���ݽ���д���ִ�е�ʱ�����

enum BreakPointFlag
{
	bp_TF,
	bp_soft,
	bp_harde,
	bp_hardr,
	bp_hardw,
	bp_mem
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
	static void FixBreakPoint_Hard(HANDLE hThread, DWORD address);

	// �����ڴ�ϵ�
	//static void SetBreakPoint_Mem(HANDLE hThread);
	// �޸��ڴ�ϵ�

private:
	// �������жϵ�
	static vector<BreakPointInfo> m_vecBP;
};

