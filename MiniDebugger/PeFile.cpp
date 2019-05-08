#include "PeFile.h"
#include <cstdio>


PeFile::PeFile()
{
}


PeFile::~PeFile()
{
}

// ��ʼ����̬����
char* PeFile::m_pPeBuff = NULL;
PIMAGE_DOS_HEADER PeFile::m_pDosHead = { 0 };
PIMAGE_NT_HEADERS PeFile::m_pNtHead = { 0 };

// ��ȡPE�ļ����浽һ�� buff ��
void PeFile::ReadPeToBuff(const char* pFilePath)
{
	FILE* pFile;
	fopen_s(&pFile, pFilePath, "rb");
	if (!pFile)
	{
		printf("�ļ���ʧ��\n");
		return;
	}
	//��ȡ�ļ���С
	fseek(pFile, 0, SEEK_END);
	int nSize = ftell(pFile);
	// ���ݶ�ȡ�Ĵ�С�����Ӧ��С�Ŀռ�
	m_pPeBuff = new char[nSize] {};
	//���ļ����ڴ���
	fseek(pFile, 0, SEEK_SET);
	fread(m_pPeBuff, nSize, 1, pFile);
	//�ر��ļ�
	fclose(pFile);
}

// �ж��Ƿ��� PE �ļ�
bool PeFile::IsPeFile()
{
	m_pDosHead = (PIMAGE_DOS_HEADER)m_pPeBuff;
	if (m_pDosHead->e_magic != IMAGE_DOS_SIGNATURE)//0x5A4D
	{
		return false;
	}
	//NTͷ
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
	//���α��׵�ַ
	PIMAGE_SECTION_HEADER pSec = IMAGE_FIRST_SECTION(m_pNtHead);
	//���α��еĸ���
	DWORD dwCount = m_pNtHead->FileHeader.NumberOfSections;
	for (DWORD i = 0; i < dwCount; i++)
	{
		if (dwRVA >= pSec->VirtualAddress &&
			dwRVA < (pSec->VirtualAddress + pSec->SizeOfRawData))
		{
			return dwRVA -
				pSec->VirtualAddress + pSec->PointerToRawData;
		}
		//��һ������
		pSec++;
	}
	return 0;
}

// ��ʾ������
void PeFile::ShowExprotInfo()
{
	// ������λ��
	PIMAGE_DATA_DIRECTORY pExportDir = &m_pNtHead->OptionalHeader.DataDirectory[0];
	// ���㵼������ļ�ƫ�ƣ�FOA��
	DWORD dwExportFOA = RVAtoFOA(pExportDir->VirtualAddress);
	// �������ļ��е�λ��
	PIMAGE_EXPORT_DIRECTORY pExport = (PIMAGE_EXPORT_DIRECTORY)(dwExportFOA + m_pPeBuff);

	// ģ����
	char* pModuleName = RVAtoFOA(pExport->Name) + m_pPeBuff;
	printf("---------ģ������%s---------\n", pModuleName);
	// ����������
	// ������ַ����
	DWORD dwFunAddrCount = pExport->NumberOfFunctions;
	// ������������
	DWORD dwFunNameCount = pExport->NumberOfNames;
	// ������ַ��RVA
	PDWORD pFunAddr = (PDWORD)(RVAtoFOA(pExport->AddressOfFunctions) + m_pPeBuff);
	// �������Ƶ�ַ��RVA
	PDWORD pFunName = (PDWORD)(RVAtoFOA(pExport->AddressOfNames) + m_pPeBuff);
	// ��ű�RVA
	PWORD pFunOrdinal = (PWORD)(RVAtoFOA(pExport->AddressOfNameOrdinals) + m_pPeBuff);
	for (DWORD i = 0; i < dwFunAddrCount; ++i)
	{
		// �������Ч��ַ��ֱ����һ��
		if (pFunAddr[i] == 0)
			continue;
		printf("������ַ��%08X ", pFunAddr[i]);
		//�ж��Ƿ��Ƿ��ŵ������Ƿ��к������֣�
		//������ű����Ƿ���ڴ���ţ���ַ���±� i ��
		bool bFlag = false; // ��ʶ�Ƿ�������
		for (DWORD j = 0; j < dwFunNameCount; ++j)
		{
			if (i == pFunOrdinal[j])
			{
				// ����˵���к�������
				bFlag = true;
				DWORD dwNameAddr = pFunName[j];
				char* pName = RVAtoFOA(dwNameAddr) + m_pPeBuff;
				printf("��������%s ", pName);
				break;
			}
		}// for dwFunNameCount
		if (bFlag)
			printf("������ţ�%d\n", i + pExport->Base);
		else // û�����ƣ�ֱ����� i ������
			printf("��������[NULL] ������ţ�%d \n", i + pExport->Base);
	}// for dwFunAddrCount
}

// ��ʾ�����
void PeFile::ShowImportInfo()
{
	// ������λ��
	PIMAGE_DATA_DIRECTORY pImportDir = &m_pNtHead->OptionalHeader.DataDirectory[1];
	// ���㵼�����ļ�ƫ�ƣ�FOA��
	DWORD dwImportFOA = RVAtoFOA(pImportDir->VirtualAddress);
	//�������ļ��е�λ��
	PIMAGE_IMPORT_DESCRIPTOR pImport = (PIMAGE_IMPORT_DESCRIPTOR)(dwImportFOA + m_pPeBuff);

	// ���������
	while (pImport->Name)
	{
		//����ģ�������
		printf("ģ������%s\n", RVAtoFOA(pImport->Name) + m_pPeBuff);

		//ͨ��INT������
		PIMAGE_THUNK_DATA pINT =
			(PIMAGE_THUNK_DATA)
			(RVAtoFOA(pImport->OriginalFirstThunk) + m_pPeBuff);
		while (pINT->u1.AddressOfData)
		{
			//�жϵ���ʽ�����IMAGE_THUNK_DATA���ΪΪ1˵������ŵ���
			//�����Ƿ��ŵ���
			if (pINT->u1.AddressOfData & 0x80000000)
			{
				//��ŵ���
				printf("���ƣ�[NULL] ��ţ�%d\n", pINT->u1.AddressOfData & 0xFFFF);
			}
			else
			{
				PIMAGE_IMPORT_BY_NAME pName =
					(PIMAGE_IMPORT_BY_NAME)
					(RVAtoFOA(pINT->u1.AddressOfData) + m_pPeBuff);
				printf("��������%s ������ţ�%d\n", pName->Name, pName->Hint);
			}
			//��һ�����뺯��
			pINT++;
		}
		//��һ�������dll
		pImport++;
	}
}
