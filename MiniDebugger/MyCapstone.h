#pragma once
#include <Windows.h>
#include "Capstone/include/Capstone.h"
#pragma comment(lib,"Capstone/Capstone.lib")
#pragma comment(linker, "/NODEFAULTLIB:\"libcmtd.lib\"")

// ������
// ���������
class MyCapstone
{
public:// ����ΪĬ�Ϲ��캯��
	MyCapstone() = default;
	~MyCapstone() = default;

	// ���ڳ�ʼ������
	static void Init();
	// ����ִ�з����ĺ���
	static void DisAsm(HANDLE Handle, LPVOID Addr, DWORD Count);

private:
	// ���ڳ�ʼ�����ڴ����ľ��
	static csh Handle;
	static cs_opt_mem OptMem;
};
