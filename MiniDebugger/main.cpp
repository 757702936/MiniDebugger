#include <iostream>
#include "DebugTarget.h"

using namespace std;

int main()
{
	DebugTarget dt;
	char file[1024] = { 0 };
	cout << "�������ļ�·����";
	cin >> file;
	dt.open(file);
	dt.DebugLoop();
	return 0;
}