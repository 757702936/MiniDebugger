#include <Windows.h>
#include <cstdio>
#include <TlHelp32.h>

struct Info
{
	char name[20];
};

// 判断版本
extern "C" __declspec(dllexport) int getinfo(Info& info)
{
	strcpy_s(info.name, 20, "myplugin");
	return 1;
}

// 显示
extern "C" __declspec(dllexport) void show()
{
	// 创建一个进程快照
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		printf("进程快照出错\n");
		return;
	}
	// 存储进程内容的结构体
	PROCESSENTRY32 procInfo = { 0 };
	// 初始化获取进程的大小（必须）
	procInfo.dwSize = sizeof(PROCESSENTRY32);
	// 遍历进程	
	if (Process32First(hProcessSnap, &procInfo))
	{
		do
		{			
			printf("【PID】：%d\t", procInfo.th32ProcessID);// 进程ID			
			printf("【进程名】：%s\n", procInfo.szExeFile);    // 进程名
		} while (Process32Next(hProcessSnap, &procInfo));
	}
	// 关闭快照
	CloseHandle(hProcessSnap);
}