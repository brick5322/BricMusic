#include "FIFO.h"
extern "C"
{
#include "FIFOprivate.h"
}

FIFO::FIFO(writeErr_Callback func_writeErr, readErr_Callback func_readErr, int maxsize):func_writeErr(func_writeErr), func_readErr(func_readErr),fifo_buffer(FIFOBuffer_alloc(maxsize))
{
	if (!fifo_buffer)
		throw "Alloc Error";
}

FIFO::FIFO(int maxsize):fifo_buffer(FIFOBuffer_alloc(maxsize))
{
}

FIFO::~FIFO()
{
	FIFOBuffer_free(fifo_buffer);
}

int FIFO::getFreesize()
{
	return fifo_buffer->freesize;
}

FIFO& FIFO::operator<<(SimpleString& str) {
	int remained_sz = str.sz;
	unsigned char* buffer = (unsigned char*)str.buffer;
	do {
		mtx.lock();
		int writein = FIFOBuffer_writein(fifo_buffer, buffer, remained_sz);
		remained_sz -= writein;
		buffer += writein;
		mtx.unlock();
	}while (func_writeErr(fifo_buffer->freesize, buffer, remained_sz));

	return *this;
}

FIFO& FIFO::operator>>(SimpleBuffer& buf) {
	unsigned char* buffer = (unsigned char*)buf.buffer;
	int copied_sz = 0;
	do {
		mtx.lock();
		copied_sz += FIFOBuffer_readout(fifo_buffer, buffer, buf.sz-copied_sz);
		mtx.unlock();
	}while (func_readErr(fifo_buffer->freesize, buf.sz - copied_sz));
	return *this;
}

FIFO_cstr FIFO::operator[](int sz)
{
	mtx.lock();
	return FIFO_cstr(mtx,fifo_buffer,sz);
}

FIFO_cstr::FIFO_cstr(std::mutex& mtx, FIFO_privatedef* fifo_buffer, int sz):mtx(mtx),fifo_buffer(fifo_buffer),sz(sz)
{
}

FIFO_cstr::~FIFO_cstr()
{
	mtx.unlock();
}

int FIFO_cstr::operator<<(const unsigned char* str)
{
	return FIFOBuffer_writein(fifo_buffer, str, sz);
}

int FIFO_cstr::operator>>(unsigned char* buf)
{
	return FIFOBuffer_readout(fifo_buffer, buf, sz);
}


