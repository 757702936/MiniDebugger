#pragma once
#include <Windows.h>
#include <vector>
using namespace std;

// ģ����Ϣ�ṹ��
typedef struct _MYMODULEINFO
{
	char name[MAX_PATH];
	DWORD startAddress;
	DWORD nSize;
} MYMODULEINFO;


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

	// ��ӡ�Ĵ�������
	static void PrintReg(SIZE_T reg, WORD color);

	// ��ӡ��־�Ĵ�������
	static void PrintEflag(DWORD flag, WORD color);

	// �鿴�Ĵ���״̬
	static void ShowRegisterInfo(const CONTEXT& ct);
	
	// �鿴�Ĵ�����ֵ
	static void SearchRegisterInfo();

	// �޸ļĴ�����ֵ
	static void ModifyRegisterInfo(const char* regBuff, DWORD data);

	// �鿴ģ����Ϣ
	static void ShowMyModuleInfo();

private:
	// ���̾��
	static HANDLE m_hProcess;
	// �߳̾��
	static HANDLE m_hThread;
	// �쳣��ַ
	static void* m_pAddress;
	// ����̨������
	static HANDLE m_hStdOut;
};

