#include<stdio.h>
#include<vector>
#include<iostream>
#include<conio.h>

#include"mymutex.h"

#define MAX_THREADS 30


namespace vars {
	static int count = 0;

	static HANDLE time_to_end_handle; // in enabled state while user don't press 'q'
	static CRITICAL_SECTION criticalSection;
	static std::vector<void*> threads;
}

class CritSLocker {
public:
	CritSLocker(CRITICAL_SECTION *critSec) { this->critSec = critSec; while(!TryEnterCriticalSection(critSec)); }
	~CritSLocker() { LeaveCriticalSection(critSec); }
private:
	CRITICAL_SECTION * critSec;
};


void thread_function(int uniqueNum) {
	std::string message = std::to_string(uniqueNum) + " Hello from thread " + std::to_string(GetCurrentThreadId());
	while (true) {
		CritSLocker locker(&vars::criticalSection);
		if (WaitForSingleObject(vars::time_to_end_handle, 0) == WAIT_TIMEOUT) { // user said "It's time to finish"
			break;
		}
		putchar('\n');
		for (unsigned i = 0; i < message.size(); ++i){
			putchar(message[i]);
			Sleep(100);
		}
	}
}

void createThread() {
	if (vars::threads.size() != MAX_THREADS) {
		Thread<void, int> *new_thread = new Thread<void, int>(thread_function, ++vars::count);
		vars::threads.emplace_back(new_thread);
		new_thread->start();
	}
}


void removeThread() {
	if (vars::threads.empty())
		return;
	CritSLocker locker(&vars::criticalSection);
	((Thread<void, int>*)vars::threads.back())->cancel();
	delete ((Thread<void, int>*)vars::threads.back());
	vars::threads.pop_back();
	printf("Deleted\n");
	putchar('\n');
}

void foo(int, double){}

bool checkInput() {
	if (_kbhit())
	{
		char c = _getch();
		switch (c) {
		case '+':
			createThread();
			break;
		case '-':
			removeThread();
			break;
		case 'q':
			return FALSE;
		default:
			break;
		}
	}
	return TRUE;
}


int main() {

	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode = 0;
	GetConsoleMode(hStdin, &mode);
	SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));


	vars::time_to_end_handle = CreateEvent(NULL, true, true, "WorkEvent");
	if (vars::time_to_end_handle == NULL)
		print_error("Warning! threads will be closed by hard way ");
	InitializeCriticalSection(&(vars::criticalSection));
	
	while (checkInput());
	
	ResetEvent(vars::time_to_end_handle);// disable event (doesn't matter if the handle is NULL or not)
	
	for (void* ptr : vars::threads) {
		Thread<void, int> *th = (Thread<void, int>*)ptr;
		if (!th->isFinished())// if WorkEvent works, then some threads may be finished
			th->cancel();
	}
	DeleteCriticalSection(&vars::criticalSection);
	printf("\nAll threads was canceled\n");
	return 0;
}
