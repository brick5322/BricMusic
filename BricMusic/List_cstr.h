#pragma once
#include "pubdefs.h"

class List_cstr
{
public:
	List_cstr(Flag flag = copy);
	~List_cstr();
	bool add(const char* src);
	bool insert(const char* src,uint32_t pos);
	void remove(uint32_t pos);
	const char* operator[](uint32_t pos);
	bool update(const char* src,uint32_t pos);
	size_t length();
	List_cstr& operator()(Flag flag);
	List_cstr& operator<<(const char*);
	List_cstr& operator>>(const char*&);
private:
	List* basic_list;
	Flag flag;
};

