#pragma once
#include<Windows.h>
#include<string>

class Mutex
{
public:
	Mutex(const std::string &name) {
		if (!CreateMutex(NULL, true, name.c_str())) {
			perror("CreateMutex failed");
		}
	}

	void wait() {
		if (WaitForSingleObject(_handle, INFINITE) == WAIT_FAILED) {
			perror("WaitForSingleOnject failed");
		}
	}

	void release() {
		if (!ReleaseMutex(_handle)) {
			perror("ReleaseMutex failed");
		}
	}

	~Mutex() {
		if (!CloseHandle(_handle)) {
			perror("CloseHandle failed");
		}
	}
private:
	HANDLE _handle;
};

