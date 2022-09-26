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
static const std::string logbuilder("[Builder] ");

// LogErr输出：编译器无法识别的代码行
void LogCommonErr(vector<string>::iterator item)
{
	Log(logbuilder + "编译器无法识别的代码行：" + (*item), Log::level_err).print();
}

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
	pathinfile->push_back(":");

	// 最终MaL类汇编代码
	map<void*, size_t>* asmcodes = new map<void*, size_t>();

	// 总计内存
	size_t memsiz = 0;
	// 变量、常量统计
	unordered_set<string>* varandcons = new unordered_set<string>();
	// 函数统计
	unordered_set<string>* funcs = new unordered_set<string>();

	vector<string>* asmcodesstring = new vector<string>();

	auto item = codes->begin();
	while (item != codes->end())
	{
		boost::cmatch regexcmatch;
		bool hassolved = false;
		switch ((*item)[0])
		{
		case 'v':
			// 声明变量
			if (boost::regex_match((*item).c_str(), regexcmatch, boost::regex("v:(\\S+):(\\S+)=(\\S+);")))
			{
				char* tmp = new char[LPDefine_size];
				// 加入最终asmcodes
				asmcodes->insert(pair(tmp, LPDefine_size));

				// 赋值指令
				tmp[0] = LPDefine;
				// 赋值Define类型（变量）
				tmp[1] = LPDefine_v;

				// 未知类型处理
				if (LPTS.count(regexcmatch[1].str()) == 0)
				{
					Log(logbuilder + "未知的类型：" + *item, Log::level_err).print();
					throw - 1;
				}
				// 赋值类型
				tmp[10] = LPTS[regexcmatch[1].str()];

				// 组合形成变量名
				string vcname = (*pathinfile)[pathinfile->size() - 1] + regexcmatch[2].str();

				// 量名重复处理
				if (varandcons->count(vcname) != 0)
				{
					Log(logbuilder + "量名重复：" + *item, Log::level_err).print();
					throw - 1;
				}
				// 将变量加入量堆
				varandcons->insert(vcname);

				// 赋值变量位置（此位置不与量堆对应，解释器不依赖于量堆）
				*(size_t*)(tmp + 2) = memsiz;

				if (regexcmatch[1].str() == "str")
				{
					// 写入数据区
					asmcodesstring->push_back(regexcmatch[3].str().substr(1, regexcmatch[3].str().size() - 2));
					// 保存数据区编号
					*(size_t*)(tmp + 11) = asmcodesstring->size() - 1;

					memsiz += 8;
				}
				else if (regexcmatch[1].str() == "lld")
				{
					// 赋值变量
					*(size_t*)(tmp + 11) = atoll(regexcmatch[3].str().c_str());

					memsiz += 8;
				}
				else if (regexcmatch[1].str() == "ulld")
				{
					// 赋值变量
					std::stringstream ssteam(regexcmatch[1].str());
					ssteam >> *(size_t*)(tmp + 11);

					memsiz += 8;
				}
				else if (regexcmatch[1].str() == "double")
				{
					// 赋值变量
					*(size_t*)(tmp + 11) = atof(regexcmatch[3].str().c_str());

					memsiz += 8;
				}
				else if (regexcmatch[1].str() == "char")
				{
					// 赋值变量
					*(size_t*)(tmp + 11) = regexcmatch[3].str()[1];

					memsiz += 1;
				}
				else if (regexcmatch[1] == "uchar")
				{
					// 赋值变量
					*(size_t*)(tmp + 11) = regexcmatch[3].str()[1];

					memsiz += 1;
				}

				hassolved = true;
			}

			break;

		case 'c':
			// 声明常量
			if (boost::regex_match((*item).c_str(), regexcmatch, boost::regex("c:(\\S+):(\\S+)=(\\S+);")))
			{
				char* tmp = new char[LPDefine_size];
				// 加入最终asmcodes
				asmcodes->insert(pair(tmp, LPDefine_size));

				// 赋值指令
				tmp[0] = LPDefine;
				// 赋值Define类型（常量）
				tmp[1] = LPDefine_c;

				// 未知类型处理
				if (LPTS.count(regexcmatch[1].str()) == 0)
				{
					Log(logbuilder + "未知的类型：" + *item, Log::level_err).print();
					throw - 1;
				}
				// 赋值类型
				tmp[10] = LPTS[regexcmatch[1].str()];

				// 组合形成变量名
				string vcname = (*pathinfile)[pathinfile->size() - 1] + regexcmatch[2].str();

				// 量名重复处理
				if (varandcons->count(vcname) != 0)
				{
					Log(logbuilder + "量名重复：" + *item, Log::level_err).print();
					throw - 1;
				}
				// 将变量加入量堆
				varandcons->insert(vcname);

				// 赋值变量位置（此位置不与量堆对应，解释器不依赖于量堆）
				*(size_t*)(tmp + 2) = memsiz;

				if (regexcmatch[1].str() == "str")
				{
					// 写入数据区
					asmcodesstring->push_back(regexcmatch[3].str().substr(1, regexcmatch[3].str().size() - 2));
					// 保存数据区编号
					*(size_t*)(tmp + 11) = asmcodesstring->size() - 1;

					memsiz += 8;
				}
				else if (regexcmatch[1].str() == "lld")
				{
					// 赋值变量
					*(size_t*)(tmp + 11) = atoll(regexcmatch[3].str().c_str());

					memsiz += 8;
				}
				else if (regexcmatch[1].str() == "ulld")
				{
					// 赋值变量
					std::stringstream ssteam(regexcmatch[1].str());
					ssteam >> *(size_t*)(tmp + 11);

					memsiz += 8;
				}
				else if (regexcmatch[1].str() == "double")
				{
					// 赋值变量
					*(size_t*)(tmp + 11) = atof(regexcmatch[3].str().c_str());

					memsiz += 8;
				}
				else if (regexcmatch[1].str() == "char")
				{
					// 赋值变量
					*(size_t*)(tmp + 11) = regexcmatch[3].str()[1];

					memsiz += 1;
				}
				else if (regexcmatch[1] == "uchar")
				{
					// 赋值变量
					*(size_t*)(tmp + 11) = regexcmatch[3].str()[1];

					memsiz += 1;
				}

				hassolved = true;
			}

			break;

		case 'f':
			// 构建函数
			if (boost::regex_match((*item).c_str(), regexcmatch, boost::regex("f:(\\S+):(\\S+)")))
			{

			}

			break;

		}

		if (hassolved)
		{
			item++;
			continue;
		}

		if (boost::regex_match((*item).c_str(), regexcmatch, boost::regex("(\\S+)=(\\S+)")))
		{

		}
		else
		{
			LogCommonErr(item);
			throw - 1;
		}

		item++;
	}

	delete pathinfile;
	delete varandcons;
	delete funcs;

	// 写入文件

	size_t codesize = asmcodes->size();
	std::fwrite(&codesize, 1, sizeof(size_t), fbuild);

	std::fwrite(&memsiz, 1, sizeof(memsiz), fbuild);

	for (auto& i : *asmcodes)
	{
		std::fwrite(i.first, 1, i.second, fbuild);
		delete[] i.first;
	}
	delete asmcodes;
	delete asmcodesstring;
	std::fclose(fbuild);

	// 文件结构：
	// size_t : 代码量
	// size_t : 占用内存总大小
	// 代码区
	// 字符区
	//

	Log("[Builder] Finish!", Log::level_suc).print();
	return codes;
}

