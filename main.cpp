#include<stdio.h>
#include<vector>
#include<iostream>
#include<conio.h>

#include"mymutex.h"

#define MAX_THREADS 30

namespace vars {
	static int count = 0;

	static HANDLE time_to_end_handle; // in enabled state while user don't press 'q'
	static Mutex mutex = Mutex("PrintDataMutex");
	static std::vector<void*> threads;
}


void thread_function(int uniqueNum) {
	std::string message = std::to_string(uniqueNum) + " Hello from thread " + std::to_string(GetCurrentThreadId());
	while (true) {
		MutexLocker locker(&vars::mutex);
		if (WaitForSingleObject(vars::time_to_end_handle, 0) == WAIT_TIMEOUT) // user said "It's time to finish"
			break;
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
	if (vars::threads.size() == 1)
		vars::mutex.release();
}


void removeThread() {
	if (vars::threads.empty())return;
	((Thread<void, int>*)vars::threads[0])->cancel();
	vars::threads.erase(vars::threads.begin());
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
	
	vars::time_to_end_handle = CreateEvent(NULL, true, true, "WorkEvent");
	if (vars::time_to_end_handle == NULL)
		print_error("Warning! threads will be closed by hard way ");
	
	while (checkInput());
	
	ResetEvent(vars::time_to_end_handle);// disable event (doesn't matter if the handle is NULL or not)
	if (vars::time_to_end_handle != NULL) // if WorkEvent works
		vars::mutex.wait();
	for (void* ptr : vars::threads) {
		Thread<void, int> *th = (Thread<void, int>*)ptr;
		if (!th->isFinished())// if WorkEvent works, then some threads may be finished
			th->cancel();
	}
	printf("\nAll threads was canceled\n");
	return 0;
}
