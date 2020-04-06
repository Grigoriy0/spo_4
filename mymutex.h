#include<string>

#include"mythread.h"



#ifndef MY_MUTEX_H
#define MY_MUTEX_H
class Mutex
{
public:
	Mutex(const std::string &name) {
		if ((_handle = CreateMutex(NULL, true, name.c_str())) == NULL ) {
			_perror("CreateMutex failed");
		}
	}

	void wait() {
		if (WaitForSingleObject(_handle, INFINITE) == WAIT_FAILED) {
			_perror("WaitForSingleOnject failed");
		}
	}

	void release() {
		if (ReleaseMutex(_handle) == NULL) {
			_perror("ReleaseMutex failed");
		}
	}

	~Mutex() {
		if (CloseHandle(_handle) == NULL) {
			_perror("CloseHandle failed");
		}
	}
private:
	HANDLE _handle;
};



class MutexLocker {
public:
	MutexLocker(Mutex*mu) { _mu = mu; _mu->wait(); }
	~MutexLocker() { _mu->release(); }
private:
	Mutex*_mu;
};

#endif // MY_MUTEX_H