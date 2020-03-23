#include<windows.h>
#include<conio.h>
#include<stdio.h>
#include"thread.h"
#include<thread>

DWORD WINAPI thread(LPVOID arg) {
	if (*(int*)arg == 555)
		ExitThread(555);
	//printf("%d", arg);
	Sleep(1000 * *(int*)arg);
	return 0;
}


HANDLE create_mutex() {
	HANDLE mutex = CreateMutex(nullptr, false, "MyMutex");
	
	
	return mutex;
}

void foo(int) {};

int main() {
		
	Thread<void, int> MyThead(false, &foo, 5);

	std::tuple<std::decay_t<int>, std::decay_t<double>> _tu_args;
	_tu_args._Myfirst._Val;

	return 0;
}