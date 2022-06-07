#pragma once
#include <cstdarg>

class ArgSplitter
{
public:
	ArgSplitter(int argc, char** argv);
	~ArgSplitter();
	void setShortOptions(char* opts);
	void setLongOptions(size_t count, ...);
	void removeLongOptions();
	void setOptionsWithArgs(size_t count, ...);
	void removeOptionsWithArgs();
	int parase();
	const char* optArg(int i);
	bool hasLongOption(int i);
	char** argvMove();
	int argc();
	char** argv();
	const char* operator[](int i);
	enum Type { Command, ShortOption, ShortOptions, LongOption, LongOptionWithArg, OptionArg, Arg, InvalidOption };
	const Type* arg_type;

private:
	int ARGC;
	char** ARGV;

	Type* _arg_type;
	int _argc;
	char** _argv;

	int nb_short_options;
	char* short_options;

	int nb_long_options;
	const char** long_options;

	int nb_arg_options;
	const char** arg_options;

};
