#include "BreakPoint.h"
#include "User.h"
#include <iostream>
using namespace std;

// DR7寄存器结构体
typedef struct _DBG_REG7 {
	unsigned L0 : 1; unsigned G0 : 1;
	unsigned L1 : 1; unsigned G1 : 1;
	unsigned L2 : 1; unsigned G2 : 1;
	unsigned L3 : 1; unsigned G3 : 1;
	unsigned LE : 1; unsigned GE : 1;
	unsigned : 6;// 保留的无效空间
	unsigned RW0 : 2; unsigned LEN0 : 2;
	unsigned RW1 : 2; unsigned LEN1 : 2;
	unsigned RW2 : 2; unsigned LEN2 : 2;
	unsigned RW3 : 2; unsigned LEN3 : 2;
} R7, * PR7;


BreakPoint::BreakPoint()
{
}


BreakPoint::~BreakPoint()
{
}

// 初始化静态变量
vector<BreakPointInfo> BreakPoint::m_vecBP;
DWORD BreakPoint::m_dwMemExceptionAddr = 0;
DWORD BreakPoint::m_dwOldProtect = 0;
bool BreakPoint::m_bIsMeme = false;
bool BreakPoint::m_bIsSoftAlways = true;
//bool BreakPoint::m_bIsHardAlways = false;
//bool BreakPoint::m_bIsMemAlways = true;
//bool BreakPoint::m_bIsFixHardAlways = false;

// 设置TF断点
bool BreakPoint::SetBreakPoint_TF(HANDLE hThread)
{
	CONTEXT ct = { 0 };
	ct.ContextFlags = CONTEXT_CONTROL; // 指定想要获取的寄存器
	// 获取线程环境
	bool getResult = GetThreadContext(hThread, &ct);
	if (!getResult)
	{
		cout << "TF断点：获取线程环境失败" << endl;
		return false;
	}
	// 把 TF 标志位改为 1， TF标志位在 Flags 寄存器的第 8 位，CPU会自动重置 TF 位
	ct.EFlags |= 0x100;

	// 将修改后的寄存器状态设置到目标线程
	bool setResult = SetThreadContext(hThread, &ct);
	if (!setResult)
	{
		cout << "TF断点：设置线程环境失败" << endl;
		return false;
	}
	return true;
}

// 设置软断点
void BreakPoint::SetBreadPoint_Soft(HANDLE hProcess, DWORD address, bool temp)
{
	// 软件断点的原理就是修改目标代码中的【第一个字节】为
	// 0xCC，修复的时候，因为 int 3 触发的是一个陷阱类异
	// 常，所以指向的是下一条指令的位置，那么需要对 eip 执
	// 行减法操作，还原指令

	// 是否设置为永久断点
	m_bIsSoftAlways = temp;

	SIZE_T byte = 0;
	DWORD oldProtect = 0;
	// 保存断点信息结构体
	BreakPointInfo stcBP_Soft = { bp_soft, address };
	// 修改内存保护属性
	VirtualProtectEx(hProcess, (LPVOID)address, 1,
		PAGE_EXECUTE_READWRITE, &oldProtect);
	// 获取到原有数据保存到结构体
	ReadProcessMemory(hProcess, (LPVOID)address, &stcBP_Soft.u.oldOpcode,
		1, &byte);
	// 将 0xCC 写入到目标位置
	WriteProcessMemory(hProcess, (LPVOID)address, "\xCC", 1, &byte);
	// 还原内存保护属性
	VirtualProtectEx(hProcess, (LPVOID)address, 1, oldProtect, &oldProtect);
	// 保存断点
	m_vecBP.push_back(stcBP_Soft);
}

// 修复软件断点
void BreakPoint::FixBreakPoint_Soft(HANDLE hProcess, HANDLE hThread, DWORD address)
{
	// 判断是否需要修复
	for (size_t i = 0; i < m_vecBP.size(); ++i)
	{
		// 判断断点类型和地址是否匹配
		if (m_vecBP[i].bpFlag == bp_soft
			&& m_vecBP[i].ExceptionAddress == address)
		{
			CONTEXT ct = { CONTEXT_CONTROL };
			// 获取线程环境
			GetThreadContext(hThread, &ct);
			// 因为 eip 指向下一条，所以 -1
			ct.Eip -= 1;
			// 设置线程环境
			SetThreadContext(hThread, &ct);

			// 将原有数据写到目标位置
			DWORD oldProtect = 0, byte = 0;
			VirtualProtectEx(hProcess, (LPVOID)address, 1,
				PAGE_EXECUTE_READWRITE, &oldProtect);
			WriteProcessMemory(hProcess, (LPVOID)address,
				&m_vecBP[i].u.oldOpcode, 1, &byte);
			VirtualProtectEx(hProcess, (LPVOID)address, 1,
				oldProtect, &oldProtect);

			// 3. 这个断点是不是永久断点,需不需要被删除
			//  - 需要删除就 erase() 
			//  - 不需要删除就设置一个是否有效的标志位
			if (!m_bIsSoftAlways)
			{
				m_vecBP.erase(m_vecBP.begin() + i);
			}
			break;
		}
	} // for

	// 永久断点，设置一个TF
	if (m_bIsSoftAlways)
	{
		SetBreakPoint_TF(hThread);
	}
}

// 设置硬件断点
void BreakPoint::SetBreakPoint_Hard(HANDLE hThread, DWORD address, /*bool temp, */DWORD Type, DWORD Len)
{
	// 如果类型设置位0，那么长度必须为0
	// 支持硬件断点的寄存器有 6 个，其中有 4 个用于保存地址
	// 硬件断点最多可以设置 4 个，再多就失败了

	// 是否设置为永久断点
	//m_bIsHardAlways = temp;

	// 获取调试寄存器
	CONTEXT ct = { CONTEXT_DEBUG_REGISTERS };
	GetThreadContext(hThread, &ct);

	// 获取 Dr7 结构体并解析
	PR7 Dr7 = (PR7)& ct.Dr7;
	
	if (0 == Len) // 执行断点地址默认
	{
	}
	else if (1 == Len) // 读断点，2字节的对齐粒度
	{
		address = address - address % 2;
	}
	else if (3 == Len) // 写断点，4字节的对齐粒度
	{
		address = address - address % 4;
	}

	// 保存断点信息结构体
	BreakPointInfo stcBP_Hard = { bp_harde, address };

	// 通过 Dr7 中的L(n) 知道当前的调试寄存器是否被使用
	if (Dr7->L0 == FALSE)
	{
		// 设置硬件断点是否有效
		Dr7->L0 = TRUE;
		// 设置断点类型
		Dr7->RW0 = Type;
		// 设置断点地址的对齐长度
		Dr7->LEN0 = Len;
		// 设置断点地址
		ct.Dr0 = address;
	}
	else if (Dr7->L1 == FALSE)
	{
		Dr7->L1 = TRUE;
		Dr7->RW1 = Type;
		Dr7->LEN1 = Len;
		ct.Dr1 = address;
	}
	else if (Dr7->L2 == FALSE)
	{
		Dr7->L2 = TRUE;
		Dr7->RW2 = Type;
		Dr7->LEN2 = Len;
		ct.Dr2 = address;
	}
	else if (Dr7->L3 == FALSE)
	{
		Dr7->L3 = TRUE;
		Dr7->RW3 = Type;
		Dr7->LEN3 = Len;
		ct.Dr3 = address;
	}
	else
	{
		return;
	}

	// 将修改更新到线程
	SetThreadContext(hThread, &ct);
	// 添加到断点向量
	m_vecBP.push_back(stcBP_Hard);
}

// 修复硬件断点
bool BreakPoint::FixBreakPoint_Hard(HANDLE hProcess, HANDLE hThread, DWORD address)
{
	// 修复的过程中，首先要知道是什么断点
	for (size_t i = 0; i < m_vecBP.size(); ++i)
	{
		// 判断断点类型和地址是否匹配
		if (m_vecBP[i].bpFlag == bp_harde
			&& m_vecBP[i].ExceptionAddress == address)
		{
			// 获取调试寄存器
			CONTEXT ct = { CONTEXT_DEBUG_REGISTERS };
			GetThreadContext(hThread, &ct);

			// 获取 Dr7 寄存器
			PR7 Dr7 = (PR7)& ct.Dr7;
			// 根据 Dr6 的低 4 位知道是谁被触发了
			int index = ct.Dr6 & 0xF;
			// 将触发的断点设置成无效的
			switch (index)
			{
				case 1: Dr7->L0 = 0; break;
				case 3: Dr7->L1 = 0; break;
				case 4: Dr7->L2 = 0; break;
				case 8: Dr7->L3 = 0; break;
			}
			// 将修改更新到线程
			SetThreadContext(hThread, &ct);

			m_vecBP.erase(m_vecBP.begin() + i);
			break;
		}
	} // for

	// 用于再次设置内存执行断点
	if (m_bIsMeme)
	{
		m_bIsMeme = false;
		VirtualProtectEx(hProcess, (LPVOID)User::ReturnInputAddress(), 1,
			PAGE_NOACCESS, &m_dwOldProtect);
		return false;
	}
	
	// 用于再次设置 软件断点，以实现永久软件断点功能
	if (m_bIsSoftAlways)
	{
		SetSoftAlways(hProcess);
		return false;
	}

	return true;
}

// 设置内存断点
void BreakPoint::SetBreakPoint_Mem(HANDLE hProcess, DWORD address, bool temp, DWORD type)
{
	// 是否设置为永久断点

	BreakPointInfo stcBP_Mem;

	// 读取
	if (0 == type)
	{
		stcBP_Mem = { bp_memr, address };
		VirtualProtectEx(hProcess, (LPVOID)address, 1,
			PAGE_NOACCESS, &m_dwOldProtect);
	}
	// 写入
	else if(1 == type)
	{
		stcBP_Mem = { bp_memw, address };
		VirtualProtectEx(hProcess, (LPVOID)address, 1,
			PAGE_READONLY, &m_dwOldProtect);
	}
	// 执行
	else if (8 == type)
	{
		stcBP_Mem = { bp_meme, address };
		VirtualProtectEx(hProcess, (LPVOID)address, 1,
			PAGE_NOACCESS, &m_dwOldProtect);
	}

	// 保存断点
	m_vecBP.push_back(stcBP_Mem);
}

// 修复内存断点
bool BreakPoint::FixBreakPoint_Mem(HANDLE hProcess, HANDLE hThread, DWORD address)
{
	for (size_t i = 0; i < m_vecBP.size(); ++i)
	{
		// 判断断点类型和地址是否匹配
		// 读取
		if (m_vecBP[i].bpFlag == bp_memr
			&& m_vecBP[i].ExceptionAddress == address)
		{
			// 还原内存保护属性
			VirtualProtectEx(hProcess, (LPVOID)address, 1,
				m_dwOldProtect, &m_dwOldProtect);
			m_vecBP.erase(m_vecBP.begin() + i);
		}
		// 写入
		else if (m_vecBP[i].bpFlag == bp_memw
			&& m_vecBP[i].ExceptionAddress == address)
		{
			// 还原内存保护属性
			VirtualProtectEx(hProcess, (LPVOID)address, 1,
				m_dwOldProtect, &m_dwOldProtect);
			m_vecBP.erase(m_vecBP.begin() + i);
		}
		// 执行
		else if (m_vecBP[i].bpFlag == bp_meme
			&& m_vecBP[i].ExceptionAddress == m_dwMemExceptionAddr)
		{
			// 还原内存保护属性
			VirtualProtectEx(hProcess, (LPVOID)address, 1,
				m_dwOldProtect, &m_dwOldProtect);
			m_vecBP.erase(m_vecBP.begin() + i);
		}
		// 否则设置 TF 断点，直到异常地址 与 执行断点匹配
		else
		{
			// 还原内存保护属性
			VirtualProtectEx(hProcess, (LPVOID)address, 1,
				m_dwOldProtect, &m_dwOldProtect);
			SetBreakPoint_TF(hThread);
			m_bIsMeme = true;
			return false;
		}
	}
	return true;
}

// 获取内存执行断点异常地址
void BreakPoint::GetMemoryExceptionAddress(DWORD address)
{
	m_dwMemExceptionAddr = address;
}

// 获取硬件修复永久断点标志位
//bool BreakPoint::GetFixHardAlwaysFlag()
//{
//	return m_bIsFixHardAlways;
//}

// 设置软件永久断点
void BreakPoint::SetSoftAlways(HANDLE hProcess)
{
	SIZE_T byte = 0;
	DWORD oldProtect = 0;
	
	for (size_t i = 0; i < m_vecBP.size(); ++i)
	{
		// 判断断点类型和地址是否匹配
		// 读取
		if (m_vecBP[i].bpFlag == bp_soft
			/*&& m_vecBP[i].ExceptionAddress == address*/)
		{
			// 修改内存保护属性
			VirtualProtectEx(hProcess, (LPVOID)m_vecBP[i].ExceptionAddress, 1,
				PAGE_EXECUTE_READWRITE, &oldProtect);
			// 获取到原有数据保存到结构体
			ReadProcessMemory(hProcess, (LPVOID)m_vecBP[i].ExceptionAddress, &m_vecBP[i].u.oldOpcode,
				1, &byte);
			// 将 0xCC 写入到目标位置
			WriteProcessMemory(hProcess, (LPVOID)m_vecBP[i].ExceptionAddress, "\xCC", 1, &byte);
			// 还原内存保护属性
			VirtualProtectEx(hProcess, (LPVOID)m_vecBP[i].ExceptionAddress, 1, oldProtect, &oldProtect);
		}
	}
}

// 设置硬件永久断点
//void BreakPoint::SetHardAlways(HANDLE hThread, DWORD address)
//{
//	// 获取调试寄存器
//	CONTEXT ct = { CONTEXT_DEBUG_REGISTERS };
//	GetThreadContext(hThread, &ct);
//
//	// 获取 Dr7 结构体并解析
//	PR7 Dr7 = (PR7)& ct.Dr7;
//
//	for (size_t i = 0; i < m_vecBP.size(); ++i)
//	{
//		if (m_vecBP[i].bpFlag == bp_harde
//			&& m_vecBP[i].ExceptionAddress == address)
//		{
//			// 通过 Dr7 中的L(n) 知道当前的调试寄存器是否被使用
//			if (Dr7->L0 == FALSE)
//			{
//				// 设置硬件断点是否有效
//				Dr7->L0 = TRUE;
//				// 设置断点类型
//				Dr7->RW0 = 0;
//				// 设置断点地址的对齐长度
//				Dr7->LEN0 = 0;
//				// 设置断点地址
//				ct.Dr0 = m_vecBP[i].ExceptionAddress;
//			}
//			else if (Dr7->L1 == FALSE)
//			{
//				Dr7->L1 = TRUE;
//				Dr7->RW1 = 0;
//				Dr7->LEN1 = 0;
//				ct.Dr1 = m_vecBP[i].ExceptionAddress;
//			}
//			else if (Dr7->L2 == FALSE)
//			{
//				Dr7->L2 = TRUE;
//				Dr7->RW2 = 0;
//				Dr7->LEN2 = 0;
//				ct.Dr2 = m_vecBP[i].ExceptionAddress;
//			}
//			else if (Dr7->L3 == FALSE)
//			{
//				Dr7->L3 = TRUE;
//				Dr7->RW3 = 0;
//				Dr7->LEN3 = 0;
//				ct.Dr3 = m_vecBP[i].ExceptionAddress;
//			}
//			else
//			{
//				return;
//			}
//		}// if
//	}// for
//
//	// 将修改更新到线程
//	SetThreadContext(hThread, &ct);
//}
