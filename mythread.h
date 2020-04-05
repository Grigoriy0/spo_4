#pragma once
#include<Windows.h>
#include<string>
#include<iostream>
#include<tuple>

template<typename F, typename Tuple, bool Enough, int TotalArgs, int... N>
struct call_impl
{
	auto static call(F f, Tuple&& t)
	{
		return call_impl<F, Tuple, TotalArgs == 1 + sizeof...(N),
			TotalArgs, N..., sizeof...(N)
		>::call(f, std::forward<Tuple>(t));
	}
};


template<typename F, typename Tuple, int TotalArgs, int... N>
struct call_impl<F, Tuple, true, TotalArgs, N...>
{
	auto static call(F f, Tuple&& t)
	{
		return f(std::get<N>(std::forward<Tuple>(t))...);
	}
};



template<typename F, typename Tuple>
auto call(F f, Tuple&& t)
{
	typedef typename std::decay<Tuple>::type type;
	return call_impl<F, Tuple, 0 == std::tuple_size<type>::value,
		std::tuple_size<type>::value
	>::call(f, std::forward<Tuple>(t));
}





template<class retT, class ...argT>
class Thread
{
public:
	explicit Thread(retT(*f)(argT...), argT...args) {
		_started = false;
		_f = f;
		_tu_args = std::make_tuple<argT...>(args...);
		_handle = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)_Launch, this, CREATE_SUSPENDED, &_threadId);
		if (_handle == INVALID_HANDLE_VALUE)
			perror("CreateThread failed");
	}

	void start() {
		if (_started)
			return;
		if (ResumeThread(_handle) == -1) {
			perror("ResumeThread failed");
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
		DWORD result = WaitForSingleObject(_handle, INFINITE);
		if (result == WAIT_FAILED)
			perror("WaitForSingleObject(_handle) failed");
		_joinable = false;
		_started = false;
	}


	void cancel() {
		bool result = TerminateThread(_handle, 1);
		if (!result)
			perror("TerminateThread failed");
	}


	~Thread()
	{
		if (WaitForSingleObject(_handle, 0) == WAIT_OBJECT_0) {
			CloseHandle(_handle);
			return;
		}
		if (_joinable)
			cancel();
		CloseHandle(_handle);
	}
private:
	retT(*_f)(argT...);
	std::tuple<argT...> _tu_args;

	HANDLE _handle;
	DWORD _threadId;

	bool _started;
	bool _joinable;

	Thread(const Thread&);

	const Thread& operator=(const Thread&) = delete;

	static DWORD _Launch(LPVOID void_ptr){
		Thread *ptr = (Thread*)void_ptr;
		call<retT(*)(argT...), std::tuple<argT...>>(ptr->_f, ptr->_tu_args);
		ExitThread(0);
		return 0;
	}
};

