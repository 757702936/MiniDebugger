// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <Windows.h>

typedef BOOL (WINAPI* FP_Debug)(VOID);

DWORD g_dwIatAddr;// 目标函数在IAT中的地址， 用于恢复
FP_Debug g_fun; // 指向原API地址


BOOL WINAPI MyIsDebuggerPresent(VOID)
{
	//MessageBox(0, 0, 0, 0);
	return 0;
}

void IatHook(const char* pStrDllName, const char* pFunName)
{
	//获取当前进程的加载基址
	char* pBase = (char*)GetModuleHandle(NULL);

	//找到导入表，遍历IAT
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pBase;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + pBase);

	DWORD dwImportTableTRVA =
		pNt->OptionalHeader.DataDirectory[1].VirtualAddress;
	PIMAGE_IMPORT_DESCRIPTOR pImport =
		(PIMAGE_IMPORT_DESCRIPTOR)
		(dwImportTableTRVA + pBase);

	//遍历
	while (pImport->Name)
	{
		char* pDllname = pImport->Name + pBase;
		//_stricmp不区分大小写比较
		if (!_stricmp(pDllname, pStrDllName))
		{
			//找到对应模块
			//查找想要hook的函数
			PDWORD pIAT = (PDWORD)(pImport->FirstThunk + pBase);
			PDWORD pINT = (PDWORD)(pImport->OriginalFirstThunk + pBase);

			while (*pINT)
			{
				//符号导入
				if (!(*pINT & 0x80000000))
				{
					PIMAGE_IMPORT_BY_NAME pName =
						(PIMAGE_IMPORT_BY_NAME)
						(*pINT + pBase);

					if (!_stricmp(pName->Name, pFunName))
					{
						//保存原API地址
						//g_OldAddr = *pIAT;
						g_fun = (FP_Debug)* pIAT;
						g_dwIatAddr = (DWORD)pIAT;
						//修改成我的函数地址
						DWORD dwOld;
						VirtualProtect(pIAT, 4, PAGE_READWRITE, &dwOld);
						*pIAT = (DWORD)MyIsDebuggerPresent;
						VirtualProtect(pIAT, 4, dwOld, &dwOld);
						goto end;
					}
				}
				//下一个函数
				pIAT++;
				pINT++;
			}
		}
		//下一个模块
		pImport++;
	}
end:;
}

void UnHook()
{
	DWORD dwOld;
	VirtualProtect((LPVOID)g_dwIatAddr, 4, PAGE_READWRITE, &dwOld);
	*(DWORD*)g_dwIatAddr = (DWORD)g_fun;
	VirtualProtect((LPVOID)g_dwIatAddr, 4, dwOld, &dwOld);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
	case DLL_PROCESS_ATTACH:
		IatHook("kernel32.dll", "IsDebuggerPresent");
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
    }
    return TRUE;
}
