#pragma once
#include<Windows.h>
#include<string>

class thread_exception : public std::exception { 
public:
	thread_exception(const std::string msg) :
		exception(msg.c_str()) {
	}
};


template<class retT, class ...argT>
class Thread
{
public:
	static retT(*_f)(argT...);
	static std::tuple<argT...> _tu_args;
	static DWORD _win_func(LPVOID){
		//std::invoke(_f, (argT)_tu_args);
		_f(_tu_args._Myfirst._Val);
		return 0;
	}

	Thread(bool startNow, retT(*f)(argT...), argT...args) {
		_started = startNow;
		_f = f;
		_tu_args = std::tuple<argT...>(args...);

		//class _Runner : public Runner {
		//	//static retT(*_f)(argT...);
		//	//static std::tuple<argT...> _tu_args;
		//	_Runner(retT(*f)(argT...), argT...args) :Runner(){
		//		_f = f;
		//		_tu_args = std::tuple(std::move(args)...);
		//	}
		//};
		
		_handle = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)_win_func, nullptr, (startNow) ? 0 : CREATE_SUSPENDED, &_threadId);
	}

	void start() {
		if (_started)
			return;
		if (ResumeThread(_handle) == -1) {
			throw thread_exception(std::string("ResumeThread failed ") 
				+ std::to_string(GetLastError()));
		}
		_started = true;
	}

	
	HANDLE _handle;
	HANDLE _mutex;
	DWORD _threadId;

	bool _started;
	bool _throwed;
	
	/*
	static struct Runner {
		static DWORD _win_func(LPVOID arg);
	};*/
	/*retT(*_user_func)(argT...);
	argT*... _user_args;
	*/
	~Thread()
	{
		CloseHandle(_handle);
		CloseHandle(_mutex);
	}
};

