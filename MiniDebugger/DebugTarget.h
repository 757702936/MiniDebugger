#pragma once
#include <Windows.h>

class DebugTarget
{
public:
	DebugTarget();
	~DebugTarget();

	// ���ļ�
	bool open(const char* file);

	// ����ѭ��
	void DebugLoop();

	// ���ݲ����쳣��λ�ô򿪾��
	void OpenExceptionHandles();

	// ���ݲ����쳣��λ�ùرվ��
	void CloseExceptionHandles();

	// ���ɵ����¼�
	DWORD DispatchDebugEvent(LPDEBUG_EVENT DbEvent);

	// �����쳣
	DWORD OnHandleException(EXCEPTION_DEBUG_INFO* ExceptionInfo);

private:
	// ������Ϣ
	DEBUG_EVENT m_stcDbEvent;

	// ���̾��
	HANDLE m_hProcess;

	// �߳̾��
	HANDLE m_hThread;

	// �쳣��Ϣ
	EXCEPTION_DEBUG_INFO m_stcException;
};

