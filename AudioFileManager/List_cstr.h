#pragma once
#include <cstdint>

using List = struct BasicList;

enum Flag{copy = 1,move = 2};

class List_cstr
{
public:
	List_cstr();
	~List_cstr();
	bool add(const char* src, Flag flag = copy);
	bool insert(const char* src,uint32_t pos, Flag flag = copy);
	void remove(uint32_t pos);
	const char* operator[](uint32_t pos);
	bool update(const char* src,uint32_t pos, Flag flag = copy);
private:
	List* basic_list;
};

