#pragma once
#include <cstdint>
#include <mutex>

struct FIFOBuffer;

using FIFO_privatedef = FIFOBuffer;

class FIFO;
class FIFO_cstr;

class FIFO
{
protected:
	FIFO_privatedef* buf;
	int flags;
public:
	enum Flags
	{
		WriteMostSz = 0x1,
		ReadMostSz = 0x2,
		StrictWrite = 0x4,
		StrictRead = 0x8,
	};
	FIFO(int sz,int flags = StrictWrite | StrictRead);
	FIFO(const FIFO&);
	~FIFO();
	int size();
	int freesize();
	void reset();
	const FIFO& operator=(const FIFO&);
	FIFO_cstr operator[](int sz);
};

class FIFO_cstr :public FIFO
{
public:
	int sz;
private:
	friend class FIFO;
	FIFO_cstr(const FIFO&,int sz);
	void operator=(const FIFO_cstr&);
public:
	int operator<<(const uint8_t*);
	int operator>>(uint8_t*);
};
