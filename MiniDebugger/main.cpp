#include <iostream>
#include "DebugTarget.h"
#include "User.h"

using namespace std;

int main()
{
	User::ShowUI();

	DebugTarget dt;
	char file[1024] = { 0 };
	cout << "请输入文件路径：";
	cin >> file;
	dt.open(file);
	
	dt.DebugLoop();

	return 0;
}