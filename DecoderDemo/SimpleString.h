#pragma once


typedef struct SimpleString
{
	int sz;
	char buffer[];
}SimpleString,SimpleBuffer;

SimpleString* SimpleString_alloc(size_t sz);

void SimpleString_free(SimpleString* str);