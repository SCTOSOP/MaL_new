#include "Tools.h"
#include "Linker.h"
#include "Translate.h"
#include <iostream>
#include <vector>
#include <time.h>
using namespace std;

static int IsDebug = 0;

int main(int arglen, char** arg)
{
	// E:\MaL
	Log("MaL v0.1", 0x000000, false).print();

	string linkpath = "";
	if (arglen > 1)
	{
		linkpath = string(arg[1]);
	}
	for (int i = 2; i < arglen; i++)
	{
		if (string(arg[i]) == "--debug")
		{
			IsDebug = 1;
		}
	}
	if (linkpath == "")
	{
		cout << "LinkPath:";
		cin >> linkpath;
	}

	if (IsDebug == 1)
	{
		Log("!!DEBUGING!!", Log::level_err, false).print();
	}

	Linker linker(linkpath.c_str(), IsDebug);
	// 保存已打开的文件内容，并作为链接和编译的共用内容，最终将直接被写入编译后文件
	vector<string>* codes;
	try
	{
		clock_t starttime = clock();
		codes = (vector<string>*)linker.link();

		printf_s("Timer : %ld ms\n", clock() - starttime);
	}
	catch (const int e)
	{
		Log("Linker中发生错误，返回参考值：" + to_string(e) + " 。", Log::level_err).print();
		return 0;
	}

	Builder builder(linkpath.c_str(), codes, IsDebug);
	try
	{
		clock_t starttime = clock();
		codes = (vector<string>*)builder.build();

		printf_s("Timer : %ld ms\n", clock() - starttime);
	}
	catch (const int e)
	{
		Log("Builder中发生错误，返回参考值：" + to_string(e) + " 。", Log::level_err).print();
		return 0;
	}

	return 0;
}