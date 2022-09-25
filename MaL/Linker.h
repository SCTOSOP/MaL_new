#pragma once
#include <string>

class Linker
{
public:
	Linker(const char* _path, int _isdebug = 0);

	void* link();

protected:


private:
	const std::string path;
	const int isdebug;

};

