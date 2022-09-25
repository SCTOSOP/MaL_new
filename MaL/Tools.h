#pragma once
#include <string>

typedef long long filelen;

class Log
{
public:
	Log(std::string _log, int _level, bool _showtime = true);

	const char* const what()
	{
		return log.c_str();
	}

	void print();

	static const int level_log;
	static const int level_war;
	static const int level_err;
	static const int level_suc;

protected:


private:
	const std::string log;
	const int level;
	const bool showtime;

};

bool HasFolder(std::string folderPath);

bool CreateFolder(std::string folderPath);

filelen GetFileLen(FILE* file);