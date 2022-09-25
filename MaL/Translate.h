#pragma once
#include <string>

class Builder
{
public:
	Builder(const char* _path, void* __codes, int _isdebug = 0);

	void* build();

protected:


private:
	const std::string path;
	const int isdebug;
	const void* _codes;
};

