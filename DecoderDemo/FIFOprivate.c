#include "FIFOprivate.h"
#include <stdio.h>

struct FIFOBuffer* FIFOBuffer_alloc(int sz)
{
	struct FIFOBuffer* ret = malloc(sizeof(struct FIFOBuffer) + sz);
	if (!ret)
		return ret;
	*(int*)&ret->size = sz;
	ret->freesize = sz;
	ret->readpos = ret->buffer;
	ret->writepos = ret->buffer;
	return ret;
}

void FIFOBuffer_free(struct FIFOBuffer* f)
{
	free(f);
}

inline int FIFOBuffer_sz_before_writter_reset(struct FIFOBuffer* fifo) {
	return fifo->buffer + fifo->size - fifo->writepos;
}

inline int FIFOBuffer_sz_before_reader_reset(struct FIFOBuffer* fifo) {
	return  fifo->buffer + fifo->size - fifo->readpos;
}

inline int FIFOBuffer_current_size(struct FIFOBuffer* fifo) {
	return fifo->size - fifo->freesize;
}



int FIFOBuffer_writein(struct FIFOBuffer* fifo_buffer,const unsigned char* src, int sz)
{
	int writein = min(sz, fifo_buffer->freesize);
	if (fifo_buffer->freesize >= sz) {
		fifo_buffer->freesize -= writein;
		writein = min(FIFOBuffer_sz_before_writter_reset(fifo_buffer), writein);
		for (int i = 0; i < writein; i++)
			*(fifo_buffer->writepos++) = *(src++);
		fifo_buffer->writepos = fifo_buffer->buffer;
		for (int i = 0; i < sz - writein; i++)
			*(fifo_buffer->writepos++) = *(src++);
	}
	else {
		writein = min(FIFOBuffer_sz_before_writter_reset(fifo_buffer), fifo_buffer->freesize);
		for (int i = 0; i < writein; i++)
			*(fifo_buffer->writepos++) = *(src++);
		fifo_buffer->writepos = fifo_buffer->buffer;
		for (int i = 0; i < fifo_buffer->freesize - writein; i++)
			*(fifo_buffer->writepos++) = *(src++);
		fifo_buffer->freesize = 0;
	}
	printf("WRITE:%d,free:%d\n", writein, fifo_buffer->freesize);
	return writein;
}

int FIFOBuffer_readout(struct FIFOBuffer* fifo_buffer, unsigned char* dst, int sz)
{
	int readout = min(sz, FIFOBuffer_current_size(fifo_buffer));
	 if (fifo_buffer->size-fifo_buffer->freesize>= sz) {
		fifo_buffer->freesize += readout;
		readout = min(FIFOBuffer_sz_before_reader_reset(fifo_buffer), readout);
		for (int i = 0; i < readout; i++)
			*(dst++) = *(fifo_buffer->readpos++);
		fifo_buffer->readpos = fifo_buffer->buffer;
		for (int i = 0; i < sz - readout; i++)
			*(dst++) = *(fifo_buffer->readpos++);
	}
	else {
		int len = min(FIFOBuffer_sz_before_reader_reset(fifo_buffer), FIFOBuffer_current_size(fifo_buffer));
		for (int i = 0; i < len; i++)
			*(dst++) = *(fifo_buffer->readpos++);
		fifo_buffer->readpos = fifo_buffer->buffer;
		for (int i = 0; i < readout - len; i++)
			*(dst++) = *(fifo_buffer->readpos++);
		fifo_buffer->freesize = fifo_buffer->size;
	}
	 printf("READ :%d,free:%d\n", readout, fifo_buffer->freesize);

	return readout;
}
