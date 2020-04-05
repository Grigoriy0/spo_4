//#include<windows.h>
//#include<conio.h>
#include<stdio.h>
#include"mythread.h"
#include<vector>
#include<iostream>

#define MAX_THREADS

void thread_function() {

}


std::vector<void*> threads;

void createThread() {
	if (threads.size() != MAX_THREADS) {
		Thread<void> *new_thread = new Thread<void>(thread_function);
		threads.emplace_back(new_thread);
	}
}



int main() {
	


	return 0;
}
