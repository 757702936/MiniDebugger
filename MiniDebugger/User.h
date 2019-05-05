#pragma once
#include <Windows.h>


// �û���������ʾ���棬��ȡ�û����룬���öϵ�...
class User
{
public:
	User();
	~User();

	// ��ȡ�û�����
	static void GetUserInput();

	// ��ʾ�û�����
	static void ShowUI();

	// ��ʾ������Ϣ
	static void ShowHelpManual();

	// ��ȡ���̾��
	static void GetProcessHandle(HANDLE hProcess);

	// ��ȡ�߳̾��
	static void GetThreadHandle(HANDLE hThread);

private:
	// ���̾��
	static HANDLE m_hProcess;
	// �߳̾��
	static HANDLE m_hThread;
};

