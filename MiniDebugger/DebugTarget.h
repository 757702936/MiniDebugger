#pragma once
#include <Windows.h>

// ����Ŀ�꣺����ѭ�������ɵ����¼��������쳣...
class DebugTarget
{
public:
	DebugTarget();
	~DebugTarget();

	// Ĭ�Ϸ�ʽ���ļ�����
	bool open(const char* file);

	// ���ӽ��̵���
	bool OpenPid(DWORD pid);

	// ����ѭ��
	void DebugLoop();

	// ���ݲ����쳣��λ�ô򿪾��
	void OpenExceptionHandles();

	// ���ݲ����쳣��λ�ùرվ��
	void CloseExceptionHandles();

	// ���ɵ����¼�
	DWORD DispatchDebugEvent();

	// �����쳣
	DWORD OnHandleException();

private:
	// ������Ϣ
	DEBUG_EVENT m_stcDbEvent;

	// ���̾��
	HANDLE m_hProcess;

	// �߳̾��
	HANDLE m_hThread;

	// OEP
	DWORD m_OEP;

	// ϵͳ�ϵ�
	bool m_bIsSystemBP;

	// �Ƿ���Ҫ�û�����
	bool m_bNeedInput;

	// �Ĵ���״̬
	CONTEXT m_stcCT;

	// ���ӽ��̵��Է�ʽ��־
	bool m_bIsOpenPid;
};

