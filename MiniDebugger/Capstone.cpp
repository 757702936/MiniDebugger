#include "Capstone.h"



Capstone::Capstone()
{
}


Capstone::~Capstone()
{
}

// ���ڳ�ʼ������
void Capstone::Init()
{
	// ���öѿռ�Ļص�����
	OptMem.free = free;
	OptMem.calloc = calloc;
	OptMem.malloc = malloc;
	OptMem.realloc = realloc;
	OptMem.vsnprintf = (cs_vsnprintf_t)vsprintf_s;

	// ע��ѿռ�����麯��
	cs_option(NULL, CS_OPT_MEM, (size_t)& OptMem);

	// ��һ�����
	cs_open(CS_ARCH_X86, CS_MODE_32, &Capstone::Handle);
}
