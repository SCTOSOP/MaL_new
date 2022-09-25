#include "Translate.h"
#include "Tools.h"
#include "LParts.h"
#include <vector>
#include <unordered_set>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>
using namespace std;

static const std::string log_notuserresponsibility("����ܲ�������������⣬�뿼�ǣ������ļ��Ƿ񱻴۸ģ�������һ�㲻����ڣ�");

Builder::Builder(const char* _path, void* __codes, int _isdebug)
	:path(_path), isdebug(_isdebug), _codes(__codes)
{

}

void* Builder::build()
{
	vector<string>* codes = (vector<string>*)_codes;

	string buildpath = path + "\\Build";
	// ����Builder�ļ���
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

	// ����build�ļ�
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
	
	// ��¼�㼶��ϵ
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
				// �ж��Ƿ�����ĩ
				if (item + 1 == codes->end())
				{
					Log("[Builder] �������ĩ��\"" + regexcmatch[1] + "\"��û�����ݣ�", Log::level_err).print();
					throw - 1;
				}
				// �ж��Ƿ���"{"
				if ((*(item + 1))[0] != '{')
				{
					Log("[Builder] \"" + *item + "\"��û��Ԥ�ڵ�\"{\"��" + log_notuserresponsibility, Log::level_err).print();
					throw - 1;
				}

				item += 2; // ���� "{"
				if (pathinfile->size() != 0)
				{
					// �������һ�����������һ��
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

