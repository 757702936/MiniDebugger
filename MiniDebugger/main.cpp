#include <iostream>
#include "DebugTarget.h"

using namespace std;

int main()
{
	DebugTarget dt;
	char file[1024] = { 0 };
	cout << "请输入文件路径：";
	cin >> file;
	dt.open(file);
	dt.DebugLoop();
	return 0;
}