#pragma once
#include <Windows.h>

class PeFile
{
public:
	PeFile();
	~PeFile();

	// ��ȡPE�ļ����浽һ�� buff ��
	static void ReadPeToBuff(const char* pFilePath);

	// �ж��Ƿ��� PE �ļ�
	static bool IsPeFile();

	// RVA --> FOA
	static DWORD RVAtoFOA(DWORD dwRVA);

	// ��ʾ������
	static void ShowExprotInfo();

	// ��ʾ�����
	static void ShowImportInfo();

private:
	// PE�ļ�
	static char* m_pPeBuff;
	// DOSͷ
	static PIMAGE_DOS_HEADER m_pDosHead;
	// NTͷ
	static PIMAGE_NT_HEADERS m_pNtHead;
};

