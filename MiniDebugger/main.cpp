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
		cout << "1.默认方式调试， 2.附加进程方式调试" << endl;
		cin >> select;
		switch (select)
		{
			case 1:
			{
				char file[1024] = { 0 };
				cout << "请输入文件路径：";
				cin >> file;
				dt.open(file);
				dt.DebugLoop();
				break;
			}
			case 2:
			{
				DWORD pid = 0;
				cout << "请输入PID：";
				cin >> pid;
				dt.OpenPid(pid);
				dt.DebugLoop();
				break;
			}
			default:
				cout << "请输入正确的方式" << endl;
				break;
		}// switch
	}// while

	return 0;
}