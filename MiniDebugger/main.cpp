#include <iostream>
#include "DebugTarget.h"
#include "User.h"

using namespace std;

int main()
{
	User::ShowUI();
	UINT select = 0;
	DebugTarget dt;

	while (1)
	{
		cout << "1.Ĭ�Ϸ�ʽ���ԣ� 2.���ӽ��̷�ʽ����" << endl;
		cin >> select;
		switch (select)
		{
			case 1:
			{
				char file[1024] = { 0 };
				cout << "�������ļ�·����";
				cin >> file;
				dt.open(file);
				dt.DebugLoop();
				break;
			}
			case 2:
			{
				DWORD pid = 0;
				cout << "������PID��";
				cin >> pid;
				dt.OpenPid(pid);
				dt.DebugLoop();
				break;
			}
			default:
				cout << "��������ȷ�ķ�ʽ" << endl;
				break;
		}// switch
	}// while

	return 0;
}