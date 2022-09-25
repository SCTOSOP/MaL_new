#include "Tools.h"
#include <direct.h>
#include <io.h>
#include <windows.h>
#include <stdio.h>
#include <time.h>

const int Log::level_log = 0x00000A;
const int Log::level_war = 0x0000A0;
const int Log::level_err = 0x000A00;
const int Log::level_suc = 0x00A000;

Log::Log(std::string _log, int _level, bool _showtime)
	:log(_log), level(_level), showtime(_showtime)
{

}

void Log::print()
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	switch (level)
	{
	case level_log:
		SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		break;
	case level_war:
		SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
		break;
	case level_err:
		SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | FOREGROUND_RED);
		break;
	case level_suc:
		SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
		break;
	default:
		break;
	}
	if (showtime)
	{
		struct tm stime;
		time_t now = time(0);
		localtime_s(&stime, &now);
		char* tmp = new char[64];
		strftime(tmp, 64, "%Y-%m-%d %H:%M:%S", &stime);
		printf_s("[%s] ", tmp);
		delete[] tmp;
	}
	printf_s("%s\n", log.c_str());
	SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

bool HasFolder(std::string folderPath)
{
	return (_access(folderPath.c_str(), 0) == 0);
}

bool CreateFolder(std::string folderPath)
{
	return (_mkdir(folderPath.c_str()) == 0);
}

filelen GetFileLen(FILE* file)
{
	filelen tmp = _ftelli64(file);
	_fseeki64(file, 0, SEEK_END);
	filelen re = _ftelli64(file);
	_fseeki64(file, tmp, SEEK_SET);

	return re;
}
