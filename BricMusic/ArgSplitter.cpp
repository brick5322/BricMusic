#include "ArgSplitter.h"
#include <cstring>

ArgSplitter::ArgSplitter(int argc, char** argv) :
	ARGC(argc),	ARGV(argv),
	_argc(0),
	_argv(new char* [argc]),
	long_options(nullptr),
	arg_options(nullptr),
	arg_type(new Type[argc]),
	_arg_type((Type*)arg_type)
{
}

ArgSplitter::~ArgSplitter()
{
	delete[] long_options;
	delete[] arg_options;
	delete[] _argv;
	delete[] arg_type;
}

void ArgSplitter::setShortOptions(char* opts)
{
	short_options = opts;
	nb_short_options = strlen(opts);
}

void ArgSplitter::setLongOptions(size_t count, ...)
{
	if (long_options)
		delete long_options;
	nb_long_options = count;
	long_options = new const char* [count];

	std::va_list args;
	va_start(args, count);
	for (int i = 0; i < count; i++)
		long_options[i] = va_arg(args, const char*);
	va_end(args);
}

void ArgSplitter::removeLongOptions()
{
	delete long_options;
	long_options = nullptr;
	nb_long_options = 0;
}

void ArgSplitter::setOptionsWithArgs(size_t count, ...)
{
	if (arg_options)
		delete arg_options;
	nb_arg_options = count;
	arg_options = new const char* [count];

	std::va_list args;
	va_start(args, count);
	for (int i = 0; i < count; i++)
		arg_options[i] = va_arg(args, const char*);
	va_end(args);
}

void ArgSplitter::removeOptionsWithArgs()
{
	delete arg_options;
	arg_options = nullptr;
	nb_arg_options = 0;
}

int ArgSplitter::parase()
{
	_arg_type[0] = Command;
	for (int i = 1; i < ARGC; i++)
	{
		if (ARGV[i][0] == '-')
			if (ARGV[i][1] == '-')
			{
				for (int j = 0; j < nb_long_options; j++)
					if (!long_options[j])
						continue;
					else if (strcmp(long_options[j], ARGV[i] + 2) == 0)
					{
						long_options[j] = nullptr;
						_arg_type[i] = LongOption;
						goto jumpout;
					}
				for (int j = 0; j < nb_arg_options; j++)
					if (!arg_options[j])
						continue;
					else if (strcmp(arg_options[j], ARGV[i] + 2) == 0)
					{
						if (i + 1 == ARGC)
						{
							_arg_type[i] = InvalidOption;
							goto jumpout;
						}
						else if (ARGV[i + 1][0] == '-')
						{
							_arg_type[i] = InvalidOption;
							goto jumpout;
						}
						_arg_type[i] = LongOptionWithArg;
						_arg_type[i + 1] = OptionArg;
						arg_options[j] = ARGV[++i];
						goto jumpout;
					}
			}
			else
			{
				int len = strlen(ARGV[i]);
				if (len > 1)
					_arg_type[i] = ShortOptions;
				else if (len == 1)
					_arg_type[i] = ShortOption;
				else
				{
					_arg_type[i] = InvalidOption;
					goto jumpout;
				}
				for (int j = 0; j < len; j++)
				{
					for (int k = 0; k < nb_short_options; k++)
						if (ARGV[i][k] == short_options[j])
						{
							short_options[j] = 0;
							goto shortoptionget;
						}
					_arg_type[i] = InvalidOption;
					goto jumpout;
				shortoptionget:
					continue;
				}
			}
		else
		{
			_arg_type[i] = Arg;
			_argc++;
		}
	jumpout:
		continue;
	}
	_argv = new char* [_argc];
	int j = 0;
	for (int i = 0; i < ARGC; i++)
		if (_arg_type[i] == Arg)
			_argv[j++] = ARGV[i];
	return _argc;
}

const char* ArgSplitter::optArg(int i)
{
	if (i >= nb_arg_options)
		return nullptr;
	return arg_options[i];
}

bool ArgSplitter::hasLongOption(int i)
{
	if (i >= nb_long_options)
		return true;
	return !long_options[i];
}

char** ArgSplitter::argvMove()
{
	char** ret = _argv;
	_argv = 0;
	return ret;
}

int ArgSplitter::argc()
{
	return _argc;
}

char** ArgSplitter::argv()
{
	return _argv;
}

const char* ArgSplitter::operator[](int i)
{
	if (i < _argc)
		return _argv[i];
	else
		return nullptr;
}
