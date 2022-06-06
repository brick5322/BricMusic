#pragma once
#include <stdint.h>

#define FIFOBuffer_existsize(f) ((f)->size-(f)->freesize)

typedef struct FIFOBuffer
{
	int refcount;
	uint8_t* readPos;
	uint8_t* writePos;
	int size;
	int freesize;
	uint8_t buffer[];
}FIFOBuffer;

FIFOBuffer* FIFOBuffer_alloc(int buffersz);

FIFOBuffer* FIFOBuffer_ref(FIFOBuffer* f);

void FIFOBuffer_free(FIFOBuffer* f);

void FIFOBuffer_write(FIFOBuffer* buf, const uint8_t* src, int len);

int FIFOBuffer_read(FIFOBuffer* buf, uint8_t* dst, int maxlen);

void FIFOBuffer_reset(FIFOBuffer* buf);
