#include "SimpleString.h"
#include <stdlib.h>

SimpleString* SimpleString_alloc(size_t sz)
{
	SimpleString* ret;
	ret = malloc(sz + sizeof(int)*2);
	if (!ret)
		return ret;
	ret->sz = sz;
	return ret;
}

void SimpleString_free(SimpleString* str)
{
	free(str);
}