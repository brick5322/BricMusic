#pragma once

#include <stdlib.h>

struct FIFOBuffer
{
	const int size;
	int freesize;
	char* readpos;
	char* writepos;
	char buffer[];
};

struct FIFOBuffer* FIFOBuffer_alloc(int sz);

void FIFOBuffer_free(struct FIFOBuffer* f);

int FIFOBuffer_writein(struct FIFOBuffer* f,const unsigned char* src, int sz);

int FIFOBuffer_readout(struct FIFOBuffer* f, unsigned char* dst, int sz);