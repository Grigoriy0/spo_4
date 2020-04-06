#ifdef _WIN32
#include<Windows.h>
#elif defined(__linux__) || defined(__unix__)
#include<pthread>
#endif
#include<string>
#include<iostream>
#include<tuple>



#ifndef MY_THREAD_H
#define MY_THREAD_H
template<typename F, typename Tuple, bool Enough, int TotalArgs, int... N>
struct call_impl
{
	auto static call(F f, Tuple t)
	{
		return call_impl<F, Tuple, TotalArgs == 1 + sizeof...(N),
			TotalArgs, N..., sizeof...(N)
		>::call(f, (t));
	}
};


template<typename F, typename Tuple, int TotalArgs, int... N>
struct call_impl<F, Tuple, true, TotalArgs, N...>
{
	auto static call(F f, Tuple t)
	{
		return f(std::get<N>((t))...);
	}
};



template<typename F, typename Tuple>
auto call(F f, Tuple t)
{
	typedef typename std::decay<Tuple>::type type;
	return call_impl<F, Tuple, 0 == std::tuple_size<type>::value,
		std::tuple_size<type>::value
	>::call(f, (t));
}



void _perror(std::string str) {
	str += std::to_string(GetLastError()) + " ";
	perror(str.c_str());
}

template<class retT, class ...argT>
class Thread
{
public:
	explicit Thread(retT(*f)(argT...), argT...args) {
		_started = false;
		_ends = false;
		_f = f;
		_tu_args = std::tuple<argT...>(args...);
		_handle = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)_Launch, this, CREATE_SUSPENDED, &_threadId);
		if (_handle == NULL)
			_perror("CreateThread failed");
	}

	void start() {
		if (_started)
			return;
		if (ResumeThread(_handle) == -1) {
			_perror("ResumeThread failed");
			return;
		}
		_started = true;
		_threadId = GetCurrentThreadId();
		_joinable = true;
	}

	void detach() {
		_joinable = false;
	}

	void join() {
		if (WaitForSingleObject(_handle, INFINITE) == WAIT_FAILED)
			_perror("WaitForSingleObject(_handle) failed");
		_joinable = false;
		_started = false;
	}


	void cancel() {
		if (TerminateThread(_handle, 1) == NULL)
			_perror("TerminateThread failed");
	}


	~Thread()
	{
		if (WaitForSingleObject(_handle, 0) == WAIT_OBJECT_0) {
			if (CloseHandle(_handle) == NULL)
				_perror("CloseHandle failed");
			return;
		}
		if (_joinable)
			cancel();
		if (CloseHandle(_handle) == NULL)
			_perror("CloseHandle failed");
	}

	bool isEnds() { return _ends; }

private:
	retT(*_f)(argT...);
	std::tuple<argT...> _tu_args;

	HANDLE _handle;
	DWORD _threadId;

	bool _started;
	bool _joinable;
	bool _ends;

	Thread(const Thread&);

	const Thread& operator=(const Thread&) = delete;

	static DWORD _Launch(LPVOID void_ptr){
		Thread *ptr = (Thread*)void_ptr;
		call<retT(*)(argT...), std::tuple<argT...>>(ptr->_f, ptr->_tu_args);
		ExitThread(0);
		ptr->_ends = true;
		return 0;
	}
};



#endif // MY_THREAD_H