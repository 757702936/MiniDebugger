#include <Windows.h>
#include <cstdio>
#include <TlHelp32.h>

struct Info
{
	char name[20];
};

// �жϰ汾
extern "C" __declspec(dllexport) int getinfo(Info& info)
{
	strcpy_s(info.name, 20, "myplugin");
	return 1;
}

// ��ʾ
extern "C" __declspec(dllexport) void show()
{
	// ����һ�����̿���
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		printf("���̿��ճ���\n");
		return;
	}
	// �洢�������ݵĽṹ��
	PROCESSENTRY32 procInfo = { 0 };
	// ��ʼ����ȡ���̵Ĵ�С�����룩
	procInfo.dwSize = sizeof(PROCESSENTRY32);
	// ��������	
	if (Process32First(hProcessSnap, &procInfo))
	{
		do
		{			
			printf("��PID����%d\t", procInfo.th32ProcessID);// ����ID			
			printf("������������%s\n", procInfo.szExeFile);    // ������
		} while (Process32Next(hProcessSnap, &procInfo));
	}
	// �رտ���
	CloseHandle(hProcessSnap);
}