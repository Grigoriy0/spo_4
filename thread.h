#pragma once
#include<Windows.h>
#include<string>


class thread_exception : public std::exception { 
public:
	thread_exception(const std::string msg) :
		exception(msg.c_str()) {
	}
};




template<typename F, typename Tuple, bool Enough, int TotalArgs, int... N>
struct call_impl
{
	void static call(F f, Tuple&& t)
	{
		call_impl<F, Tuple, TotalArgs == 1 + sizeof...(N),
			TotalArgs, N..., sizeof...(N)>::call(f, std::forward<Tuple>(t));
	}
};

template<typename F, typename Tuple, int TotalArgs, int... N>
struct call_impl<F, Tuple, true, TotalArgs, N...>
{
	void static call(F f, Tuple&& t)
	{
		f(std::get<N>(std::forward<Tuple>(t))...);
	}
};
//
//template<typename F, typename Tuple>
//void call(F f, Tuple&& t)
//{
//	typedef typename std::decay<Tuple>::type type;
//	call_impl<F, Tuple, 0 == std::tuple_size<type>::value,
//		std::tuple_size<type>::value>::call(f, std::forward<Tuple>(t));
//}

template<typename F, typename ...Types>
void call(F f, std::tuple<Types...> &&t) {
	call_impl<F, t, 0 == std::tuple_size<std::tuple<Types...>>::value, 
		std::tuple_size< std::tuple<Types...>>::value>::call(f, std::forward<std::tuple<Types...>>(t));
}




template<class retT, class ...argT>
class Thread
{
public:
	Thread(retT(*f)(argT...), argT...args) {
		_started = false;
		_f = f;
		_tu_args = std::make_tuple(args...);

		_handle = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)_Launch, this, CREATE_SUSPENDED, &_threadId);
	}

	void start() {
		if (_started)
			return;
		if (ResumeThread(_handle) == -1) {
			throw thread_exception(std::string("ResumeThread failed ") 
				+ std::to_string(GetLastError()));
		}
		_started = true;
		_threadId = GetCurrentThreadId();
	}

	void detach() {
		
	}

	void join() {
		DWORD result = WaitForSingleObject(_handle, INFINITE);
		if (result == WAIT_FAILED)
			throw thread_exception(std::string("WaitForSingleObject failed")
				+ std::to_string(GetLastError()));
	}

	
	~Thread()
	{
		CloseHandle(_handle);
		CloseHandle(_mutex);
	}
	retT(*_f)(argT...);
	std::tuple<argT...> _tu_args;
private:
	HANDLE _handle;
	HANDLE _mutex;
	DWORD _threadId;

	bool _started;
	bool _throwed;

	Thread(const Thread&);
	const Thread& operator=(const Thread&);

	static DWORD _Launch(LPVOID void_ptr){
		Thread *ptr = (Thread*)void_ptr;
		//call<retT(*)(argT...), std::tuple<argT...>>(ptr->_f, ptr->_tu_args);
		return 0;
	}

};

