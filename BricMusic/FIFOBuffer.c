#include "FIFOBuffer.h"
#include <stdlib.h>
#include <stdint.h>

#define FIFOBuffer_wResetsz(fifo) ((fifo)->buffer + (fifo)->size - (fifo)->writePos)
#define FIFOBuffer_rResetsz(fifo) ((fifo)->buffer + (fifo)->size - (fifo)->readPos)

FIFOBuffer* FIFOBuffer_alloc(int buffersz)
{
    FIFOBuffer* ret = malloc(sizeof(FIFOBuffer) + buffersz);
    if (!ret)
        return ret;
    ret->size = buffersz;
    ret->refcount = 1;
    FIFOBuffer_reset(ret);
    return ret;
}

FIFOBuffer* FIFOBuffer_ref(FIFOBuffer* f)
{
    f->refcount++;
    return f;
}

void FIFOBuffer_free(FIFOBuffer* f)
{
    f->refcount--;
}

void FIFOBuffer_write(FIFOBuffer* buf, const uint8_t* src, int len)
{
    if (len <= FIFOBuffer_wResetsz(buf))
        for (int i = 0; i < len; i++)
            *(buf->writePos++) = *(src++);
    else {
        int i = FIFOBuffer_wResetsz(buf);
        while (FIFOBuffer_wResetsz(buf))
            *(buf->writePos++) = *(src++);
        for (buf->writePos = buf->buffer; i < len; i++)
            *(buf->writePos++) = *(src++);
    }
    buf->freesize -= len;
}

int FIFOBuffer_read(FIFOBuffer* buf, uint8_t* dst, int maxlen)
{
    int len = maxlen > FIFOBuffer_existsize(buf) ? FIFOBuffer_existsize(buf) : maxlen;
    if (len <= FIFOBuffer_rResetsz(buf))
        for (int i = 0; i < len; i++)
            *(dst++) = *(buf->readPos++);
    else {
        int i = FIFOBuffer_rResetsz(buf);
        while (FIFOBuffer_rResetsz(buf))
            *(dst++) = *(buf->readPos++);
        for (buf->readPos = buf->buffer; i < len; i++)
            *(dst++) = *(buf->readPos++);
    }
    buf->freesize += len;
    return len;
}

void FIFOBuffer_reset(FIFOBuffer* buf)
{
    buf->readPos = buf->buffer;
    buf->writePos = buf->buffer;
    buf->freesize = buf->size;
}
