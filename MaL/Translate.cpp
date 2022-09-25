#include "Translate.h"
#include "Tools.h"
#include "LParts.h"
#include <vector>
#include <unordered_set>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>
using namespace std;

static const std::string log_notuserresponsibility("这可能不是您代码的问题，请考虑：链接文件是否被篡改，此问题一般不会存在！");

Builder::Builder(const char* _path, void* __codes, int _isdebug)
	:path(_path), isdebug(_isdebug), _codes(__codes)
{

}

void* Builder::build()
{
	vector<string>* codes = (vector<string>*)_codes;

	string buildpath = path + "\\Build";
	// 创建Builder文件夹
	if (HasFolder(buildpath))
	{
		Log("[Builder] Existing folder \"Build\"", Log::level_war).print();
	}
	else
	{
		if (CreateFolder(buildpath))
		{
			Log("[Builder] Create folder \"Build\"", Log::level_log).print();
		}
		else
		{
			Log("[Builder] Cannot create folder \"Build\"", Log::level_err).print();
			throw - 1;
		}
	}

	// 创建build文件
	string buildfilepath = buildpath + "\\build.malbuild";
	FILE* fbuild = NULL; fopen_s(&fbuild, buildfilepath.c_str(), "w+");
	if (fbuild == NULL)
	{
		Log("[Builder] Cannot create file \"Build\\link.malbuild\"", Log::level_err).print();
		throw - 1;
	}
	else
	{
		Log("[Builder] Create file \"Build\\build.malbuild\"", Log::level_log).print();
	}
	
	// 记录层级关系
	vector<string>* pathinfile = new vector<string>();

	auto item = codes->begin();
	while (item != codes->end())
	{
		boost::cmatch regexcmatch;
		if ((*item)[0] == 'n')
		{
			// namespace
			if (boost::regex_match((*item).c_str(), regexcmatch, boost::regex("namespace:(\\S+)")))
			{
				// 判断是否是文末
				if (item + 1 == codes->end())
				{
					Log("[Builder] 意外的文末：\"" + regexcmatch[1] + "\"后没有内容！", Log::level_err).print();
					throw - 1;
				}
				// 判断是否有"{"
				if ((*(item + 1))[0] != '{')
				{
					Log("[Builder] \"" + *item + "\"后没有预期的\"{\"，" + log_notuserresponsibility, Log::level_err).print();
					throw - 1;
				}

				item += 2; // 跳过 "{"
				if (pathinfile->size() != 0)
				{
					// 如果有上一级，则加上上一级
					pathinfile->push_back((*pathinfile)[pathinfile->size() - 1] + regexcmatch[1]);
				}
				else
				{
					pathinfile->push_back(regexcmatch[1]);
				}
				continue;
			}
		}

		item++;
	}

	delete pathinfile;
	std::fclose(fbuild);

	Log("[Builder] Finish!", Log::level_suc).print();
	return codes;
}

