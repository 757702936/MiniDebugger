#pragma once
#include <Windows.h>
#include "Capstone/include/capstone.h"
#pragma comment(lib,"capstone/capstone.lib")
#pragma comment(linker, "/NODEFAULTLIB:\"libcmtd.lib\"")

// ������
// ���������
class Capstone
{
public:// ����ΪĬ�Ϲ��캯��
	Capstone() = default;
	~Capstone() = default;

	// ���ڳ�ʼ������
	static void Init();
	// ����ִ�з����ĺ���
	static void DisAsm(HANDLE Handle, LPVOID Addr, DWORD Count);

private:
	// ���ڳ�ʼ�����ڴ����ľ��
	static csh Handle;
	static cs_opt_mem OptMem;
};
