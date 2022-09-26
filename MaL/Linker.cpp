#include "Linker.h"
#include "Tools.h"
#include <Windows.h>
#include <unordered_set>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>
using namespace std;

struct file_info
{
	size_t start_line;
	size_t end_line;
	char filename[MAX_PATH];

};

Linker::Linker(const char* _path, int _isdebug)
	:path(_path), isdebug(_isdebug)
{
	Log("[Linker] Linker Init", Log::level_log).print();

}

void* Linker::link()
{
	Log("[Linker] Start Link", Log::level_log).print();

	string linkpath = path + "\\Link";
	// 创建Linker文件夹
	if (HasFolder(linkpath))
	{
		Log("[Linker] Existing folder \"Link\"", Log::level_war).print();
	}
	else
	{
		if (CreateFolder(linkpath))
		{
			Log("[Linker] Create folder \"Link\"", Log::level_log).print();
		}
		else
		{
			Log("[Linker] Cannot create folder \"Link\"", Log::level_err).print();
			throw - 1;
		}
	}
	
	// 创建link文件
	string linkfilepath = linkpath + "\\link.mallink";
	FILE* flink = NULL; fopen_s(&flink, linkfilepath.c_str(), "w+");
	if (flink == NULL)
	{
		Log("[Linker] Cannot create file \"Link\\link.mallink\"", Log::level_err).print();
		throw - 1;
	}
	else
	{
		Log("[Linker] Create file \"Link\\link.mallink\"", Log::level_log).print();
	}

	string mainfilepath = path + "\\main.mal";
	FILE* fmain = NULL; fopen_s(&fmain, mainfilepath.c_str(), "r");
	// 读取main文件
	if (fmain == NULL)
	{
		Log("[Linker] No file \"main.mal\"", Log::level_err).print();
		throw - 1;
	}
	// 保存已打开的文件内容，并作为链接和编译的共用内容，最终将直接被写入编译后文件
	vector<string> *codes = new vector<string>();

	// 保存main.mal的FILE*与内容
	filelen fmainl = GetFileLen(fmain);
	char* tmain = new char[fmainl];
	fread_s(tmain, fmainl, 1, fmainl, fmain);
	// 分割main.mal内容并直接存入codes
	boost::split(*codes, tmain, boost::is_any_of("\n"), boost::token_compress_on);
	delete[] tmain;
	codes->pop_back();

	// 保存已打开的文件FILE*
	unordered_set<string> files_opened;

	size_t times = 0;
	auto item = codes->begin();
	while (item != codes->end())
	{
		times++;
		if (times == 1000)
		{
			printf_s("|");
			times = 0;
		}

		// 处理空格和tab
		string tmp = "";
		auto sitem = (*item).begin();
		bool isinmarks = false;
		while (sitem!=(*item).end())
		{
			if (*sitem == '"')
			{
				// 排除\"
				if (sitem != (*item).begin())
				{
					if (*(sitem - 1) != '\\')
					{
						isinmarks = !isinmarks;
					}
				}
				tmp += *sitem;
			}
			// 查找空格和tab
			else if (*sitem == ' ' || *sitem == '	')
			{
				if (isinmarks)
				{
					tmp += *sitem;
				}
			}
			else
			{
				tmp += *sitem;
			}
			sitem++;
		}

		// 处理注释
		if (tmp[0] == '/')
		{
			if (boost::regex_match(tmp, boost::regex("//\\S*")))
			{
				item = codes->erase(item);
				continue;
			}
		}

		// 正则表达式匹配#include<path>
		boost::cmatch regexincludefilename;
		if (tmp[0] == '#')
		{
			if (boost::regex_match(tmp.c_str(), regexincludefilename, boost::regex("#include<(\\S+)>")))
			{
				if (files_opened.count(regexincludefilename[1]) != 0)
				{
					item = codes->erase(item);
					continue;
				}
				files_opened.insert(regexincludefilename[1]);

				string newfilepath = path + "\\" + regexincludefilename[1];
				Log("[Linker] Try to open file \"" + regexincludefilename[1] + "\"", Log::level_log).print();
				FILE* newf = NULL;
				fopen_s(&newf, newfilepath.c_str(), "r");
				if (newf == NULL)
				{
					Log("[Linker] Fail to open file \"" + regexincludefilename[1] + "\"", Log::level_err).print();
					throw - 1;
				}
				filelen newfilelen = GetFileLen(newf);
				char* tnewf = new char[newfilelen];
				fread_s(tnewf, newfilelen, 1, newfilelen, newf);
				vector<string> newfilevector;
				boost::split(newfilevector, tnewf, boost::is_any_of("\n"), boost::token_compress_on);
				delete[] tnewf;
				newfilevector.pop_back();

				item = codes->erase(item);

				auto newfvi = newfilevector.end() - 1;
				while (true)
				{
					item = codes->insert(item, *newfvi);
					if (newfvi == newfilevector.begin())
					{
						break;
					}
					newfvi--;
				}
				continue;
			}
		}
		
		*item = tmp;
		item++;
	}

	// TODO Linker
	char separatorchar = '\n';
	fwrite(&separatorchar, 1, 1, flink);
	for (auto& i : *codes)
	{
		fwrite(i.c_str(), 1, i.size() + 1, flink);
		_fseeki64(flink, -1, SEEK_CUR);
		fwrite(&separatorchar, 1, 1, flink);
	}
	fclose(flink);

	Log("[Linker] Finish!", Log::level_suc).print();
	return codes;
}
