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

	// ��ȡ�쳣��ַ
	static void GetExceptionAddress(void* address);

	// ��ȡ�߳̾��
	static void GetThreadHandle(HANDLE hThread);

	// �鿴�ڴ�����
	static void SearchMemoryInfo(DWORD address);

	// �޸��ڴ�����
	static void ModifyMemoryInfo(DWORD address, char* buff, size_t nSize);

	// �鿴ջ��Ϣ
	static void SearchStackInfo();

private:
	// ���̾��
	static HANDLE m_hProcess;
	// �߳̾��
	static HANDLE m_hThread;
	// �쳣��ַ
	static void* m_pAddress;
};

