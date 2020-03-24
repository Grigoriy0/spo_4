#include<windows.h>
#include<conio.h>
#include<stdio.h>
#include"thread.h"
#include<thread>
#include<iostream>

DWORD WINAPI thread(LPVOID arg) {
	if (*(int*)arg == 555)
		ExitThread(555);
	//printf("%d", arg);
	Sleep(1000 * *(int*)arg);
	return 0;
}









void foo(int num, double) {
	printf("%d", num);

}

int main() {
		
	//Thread<void, int> MyThead(&foo, 5);
	for (int i = 0; i < 100; ++i)
		printf("%d\n", 0);
	system("pause");
	
	call<void(*)(int, double), std::tuple<int, double>>(foo, std::make_tuple<int, double>(5, 7.8));
	
	return 0;
}