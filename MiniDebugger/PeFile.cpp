#include "PeFile.h"
#include <cstdio>


PeFile::PeFile()
{
}


PeFile::~PeFile()
{
}

// 初始化静态变量
char* PeFile::m_pPeBuff = NULL;
PIMAGE_DOS_HEADER PeFile::m_pDosHead = { 0 };
PIMAGE_NT_HEADERS PeFile::m_pNtHead = { 0 };

// 读取PE文件，存到一个 buff 中
void PeFile::ReadPeToBuff(const char* pFilePath)
{
	FILE* pFile;
	fopen_s(&pFile, pFilePath, "rb");
	if (!pFile)
	{
		printf("文件打开失败\n");
		return;
	}
	//获取文件大小
	fseek(pFile, 0, SEEK_END);
	int nSize = ftell(pFile);
	// 根据读取的大小申请对应大小的空间
	m_pPeBuff = new char[nSize] {};
	//读文件到内存中
	fseek(pFile, 0, SEEK_SET);
	fread(m_pPeBuff, nSize, 1, pFile);
	//关闭文件
	fclose(pFile);
}

// 判断是否是 PE 文件
bool PeFile::IsPeFile()
{
	m_pDosHead = (PIMAGE_DOS_HEADER)m_pPeBuff;
	if (m_pDosHead->e_magic != IMAGE_DOS_SIGNATURE)//0x5A4D
	{
		return false;
	}
	//NT头
	m_pNtHead = (PIMAGE_NT_HEADERS) (m_pDosHead->e_lfanew + m_pPeBuff);
	if (m_pNtHead->Signature != IMAGE_NT_SIGNATURE) //0x00004550
	{
		return false;
	}
	return true;
}

// RVA --> FOA
DWORD PeFile::RVAtoFOA(DWORD dwRVA)
{
	//区段表首地址
	PIMAGE_SECTION_HEADER pSec = IMAGE_FIRST_SECTION(m_pNtHead);
	//区段表中的个数
	DWORD dwCount = m_pNtHead->FileHeader.NumberOfSections;
	for (DWORD i = 0; i < dwCount; i++)
	{
		if (dwRVA >= pSec->VirtualAddress &&
			dwRVA < (pSec->VirtualAddress + pSec->SizeOfRawData))
		{
			return dwRVA -
				pSec->VirtualAddress + pSec->PointerToRawData;
		}
		//下一个区段
		pSec++;
	}
	return 0;
}

// 显示导出表
void PeFile::ShowExprotInfo()
{
	// 导出表位置
	PIMAGE_DATA_DIRECTORY pExportDir = &m_pNtHead->OptionalHeader.DataDirectory[0];
	// 计算导出表的文件偏移（FOA）
	DWORD dwExportFOA = RVAtoFOA(pExportDir->VirtualAddress);
	// 具体在文件中的位置
	PIMAGE_EXPORT_DIRECTORY pExport = (PIMAGE_EXPORT_DIRECTORY)(dwExportFOA + m_pPeBuff);

	// 模块名
	char* pModuleName = RVAtoFOA(pExport->Name) + m_pPeBuff;
	printf("---------模块名：%s---------\n", pModuleName);
	// 遍历导出表
	// 函数地址数量
	DWORD dwFunAddrCount = pExport->NumberOfFunctions;
	// 函数名称数量
	DWORD dwFunNameCount = pExport->NumberOfNames;
	// 函数地址表RVA
	PDWORD pFunAddr = (PDWORD)(RVAtoFOA(pExport->AddressOfFunctions) + m_pPeBuff);
	// 函数名称地址表RVA
	PDWORD pFunName = (PDWORD)(RVAtoFOA(pExport->AddressOfNames) + m_pPeBuff);
	// 序号表RVA
	PWORD pFunOrdinal = (PWORD)(RVAtoFOA(pExport->AddressOfNameOrdinals) + m_pPeBuff);
	for (DWORD i = 0; i < dwFunAddrCount; ++i)
	{
		// 如果有无效地址，直接下一个
		if (pFunAddr[i] == 0)
			continue;
		printf("函数地址：%08X ", pFunAddr[i]);
		//判断是否是符号导出（是否有函数名字）
		//遍历序号表，看是否存在此序号（地址表下标 i ）
		bool bFlag = false; // 标识是否有名字
		for (DWORD j = 0; j < dwFunNameCount; ++j)
		{
			if (i == pFunOrdinal[j])
			{
				// 存在说明有函数名称
				bFlag = true;
				DWORD dwNameAddr = pFunName[j];
				char* pName = RVAtoFOA(dwNameAddr) + m_pPeBuff;
				printf("函数名：%s ", pName);
				break;
			}
		}// for dwFunNameCount
		if (bFlag)
			printf("函数序号：%d\n", i + pExport->Base);
		else // 没有名称，直接输出 i 这个序号
			printf("函数名：[NULL] 函数序号：%d \n", i + pExport->Base);
	}// for dwFunAddrCount
}

// 显示导入表
void PeFile::ShowImportInfo()
{
	// 导入表的位置
	PIMAGE_DATA_DIRECTORY pImportDir = &m_pNtHead->OptionalHeader.DataDirectory[1];
	// 计算导入表的文件偏移（FOA）
	DWORD dwImportFOA = RVAtoFOA(pImportDir->VirtualAddress);
	//具体在文件中的位置
	PIMAGE_IMPORT_DESCRIPTOR pImport = (PIMAGE_IMPORT_DESCRIPTOR)(dwImportFOA + m_pPeBuff);

	// 遍历导入表
	while (pImport->Name)
	{
		//导入模块的名称
		printf("模块名：%s\n", RVAtoFOA(pImport->Name) + m_pPeBuff);

		//通过INT来遍历
		PIMAGE_THUNK_DATA pINT =
			(PIMAGE_THUNK_DATA)
			(RVAtoFOA(pImport->OriginalFirstThunk) + m_pPeBuff);
		while (pINT->u1.AddressOfData)
		{
			//判断到方式，如果IMAGE_THUNK_DATA最高为为1说明是序号导入
			//否则是符号导入
			if (pINT->u1.AddressOfData & 0x80000000)
			{
				//序号导入
				printf("名称：[NULL] 序号：%d\n", pINT->u1.AddressOfData & 0xFFFF);
			}
			else
			{
				PIMAGE_IMPORT_BY_NAME pName =
					(PIMAGE_IMPORT_BY_NAME)
					(RVAtoFOA(pINT->u1.AddressOfData) + m_pPeBuff);
				printf("函数名：%s 函数序号：%d\n", pName->Name, pName->Hint);
			}
			//下一个导入函数
			pINT++;
		}
		//下一个导入的dll
		pImport++;
	}
}
