#include "FIFO.h"

extern "C"
{
#include "FIFOBuffer.h"
}

FIFO::FIFO(int sz, int flags):buf(FIFOBuffer_alloc(sz)),flags(flags){
}

FIFO::FIFO(const FIFO& fifo)
{
	buf = FIFOBuffer_ref(fifo.buf);
	flags = fifo.flags;
}

FIFO::~FIFO()
{
	FIFOBuffer_free(buf);
}

int FIFO::size()
{
	return buf->size;
}

int FIFO::freesize()
{
	return buf->freesize;
}
void FIFO::reset()
{
	FIFOBuffer_reset(buf);
}

const FIFO& FIFO::operator=(const FIFO& copy)
{
	FIFOBuffer_free(buf);
	buf = FIFOBuffer_ref(copy.buf);
	flags = copy.flags;
	return copy;
}

FIFO_cstr FIFO::operator[](int sz)
{
	return FIFO_cstr(*this,sz);
}

FIFO_cstr::FIFO_cstr(const FIFO& fifo,int sz):FIFO(fifo),sz(sz){
}

int FIFO_cstr::operator<<(const uint8_t* src)
{
	int writesz = sz;
	if (writesz > buf->freesize)
		if (flags & StrictWrite)
			return 0;
		else if (flags & WriteMostSz)
			writesz = buf->freesize;
	FIFOBuffer_write(buf, src, writesz);
	return writesz;
}

int FIFO_cstr::operator>>(uint8_t* dst)
{
	int readsz = sz;
	if (readsz > FIFOBuffer_existsize(buf))
		if (flags & StrictRead)
			return 0;
		else if (flags & ReadMostSz)
			readsz = FIFOBuffer_existsize(buf);
	readsz = FIFOBuffer_read(buf, dst, readsz);
	return 	readsz;
}
