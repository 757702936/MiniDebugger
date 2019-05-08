#pragma once
#include <Windows.h>

class PeFile
{
public:
	PeFile();
	~PeFile();

	// 读取PE文件，存到一个 buff 中
	static void ReadPeToBuff(const char* pFilePath);

	// 判断是否是 PE 文件
	static bool IsPeFile();

	// RVA --> FOA
	static DWORD RVAtoFOA(DWORD dwRVA);

	// 显示导出表
	static void ShowExprotInfo();

	// 显示导入表
	static void ShowImportInfo();

private:
	// PE文件
	static char* m_pPeBuff;
	// DOS头
	static PIMAGE_DOS_HEADER m_pDosHead;
	// NT头
	static PIMAGE_NT_HEADERS m_pNtHead;
};

