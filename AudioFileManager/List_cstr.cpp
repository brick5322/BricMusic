#include "List_cstr.h"
#include <cstring>

extern"C"
{
#include "List.h"
#include "Node.h"
}

List_cstr::List_cstr():basic_list(BasicList_alloc(2))
{
	if (!basic_list)
		throw 1;
}

List_cstr::~List_cstr()
{
	BasicList_free(basic_list);
}

bool List_cstr::add(const char* src, Flag flag)
{
	size_t len = strlen(src) + 1;
	Node* add = nullptr;
	switch (flag)
	{
	case copy:
		add = BasicList_add(basic_list, copy, len);
		if (!add)
			return true;
		strcpy(Node_getArray(char,add), src);
		break;
	case move:
		add = BasicList_add(basic_list, move, sizeof(char*));
		if (!add)
			return true;
		Node_getData(const char*, add) = src;
		break;
	default:
		break;
	}
	return false;
}

bool List_cstr::insert(const char* src, uint32_t pos, Flag flag)
{
	size_t len = strlen(src) + 1;
	Node* ins = nullptr;
	switch (flag)
	{
	case copy:
		ins = BasicList_insert(basic_list,pos, copy, len);
		if (!ins)
			return true;
		strcpy(Node_getArray(char, ins), src);
		break;
	case move:
		ins = BasicList_insert(basic_list,pos, move, sizeof(char*));
		if (!ins)
			return true;
		Node_getData(const char*, ins) = src;
		break;
	default:
		break;
	}
	return false;
}

void List_cstr::remove(uint32_t pos)
{
	BasicList_delete(basic_list, pos);
}

const char* List_cstr::operator[](uint32_t pos)
{
	Node* current = BasicList_get(basic_list,pos);
	switch (current->datainf.type.typeID)
	{
	case copy:
		return Node_getArray(char, current);
	case move:
		return Node_getData(char*,current);
	default:
		break;
	}
	return nullptr;
}

bool List_cstr::update(const char* src, uint32_t pos, Flag flag)
{
	size_t len = strlen(src) + 1;
	Node* upd = nullptr;
	switch (flag)
	{
	case copy:
		upd = BasicList_update(basic_list, pos, copy, len);
		if (!upd)
			return true;
		strcpy(Node_getArray(char, upd), src);
		break;
	case move:
		upd = BasicList_update(basic_list, pos, move, sizeof(char*));
		if (!upd)
			return true;
		Node_getData(const char*, upd) = src;
		break;
	default:
		break;
	}
	return false;
}
