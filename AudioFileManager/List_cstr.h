#pragma once

using List = struct BasicList;

enum Flag{copy,move};

class List_cstr
{
public:
	List_cstr();
	~List_cstr();
	bool add(const char* src, Flag flag = copy);
	bool insert(const char* src,int pos, Flag flag = copy);
	bool remove(int pos);
	const char* operator[](int pos);
	bool update(const char* src,int pos, Flag flag = copy);
private:
	List* basic_list;
};

