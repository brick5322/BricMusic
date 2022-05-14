#pragma once

#include <mutex>
#include <functional>
extern "C"
{
#include "SimpleString.h"
}
using FIFO_privatedef = struct FIFOBuffer;

class FIFO_cstr
{
private:
	FIFO_cstr(std::mutex& mtx, FIFO_privatedef* fifo_buffer,int sz);
	FIFO_cstr operator=(const FIFO_cstr&);
	std::mutex& mtx;
	FIFO_privatedef* fifo_buffer;
	int sz;
public:
	~FIFO_cstr();
	friend class FIFO;
	int operator<<(const unsigned char* str);
	int operator>>(unsigned char* buf);
};

class FIFO
{
public:
	using writeErr_Callback = std::function<bool(int& freesize, const unsigned char*, int)>;
	using readErr_Callback = std::function<bool(int& freesize, int)>;
private:
	std::mutex mtx;
	FIFO_privatedef* fifo_buffer;
	writeErr_Callback func_writeErr;
	readErr_Callback func_readErr;
public:
	FIFO(writeErr_Callback func_writeErr, readErr_Callback func_readErr, int maxsize);
	FIFO(int maxsize);
	~FIFO();
	int getFreesize();
	int size();
	void reset();
	FIFO& operator<<(SimpleString& str);
	FIFO& operator>>(SimpleBuffer& buf);
	const FIFO& operator=(FIFO&& copy);
	FIFO_cstr operator[](int sz);
};

