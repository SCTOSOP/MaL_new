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
static const std::string logbuilder("[Builder] ");

// LogErr������������޷�ʶ��Ĵ�����
void LogCommonErr(vector<string>::iterator item)
{
	Log(logbuilder + "�������޷�ʶ��Ĵ����У�" + (*item), Log::level_err).print();
}

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
	pathinfile->push_back(":");

	// ����MaL�������
	map<void*, size_t>* asmcodes = new map<void*, size_t>();

	// �ܼ��ڴ�
	size_t memsiz = 0;
	// ����������ͳ��
	unordered_set<string>* varandcons = new unordered_set<string>();
	// ����ͳ��
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
			// ��������
			if (boost::regex_match((*item).c_str(), regexcmatch, boost::regex("v:(\\S+):(\\S+)=(\\S+);")))
			{
				char* tmp = new char[LPDefine_size];
				// ��������asmcodes
				asmcodes->insert(pair(tmp, LPDefine_size));

				// ��ֵָ��
				tmp[0] = LPDefine;
				// ��ֵDefine���ͣ�������
				tmp[1] = LPDefine_v;

				// δ֪���ʹ���
				if (LPTS.count(regexcmatch[1].str()) == 0)
				{
					Log(logbuilder + "δ֪�����ͣ�" + *item, Log::level_err).print();
					throw - 1;
				}
				// ��ֵ����
				tmp[10] = LPTS[regexcmatch[1].str()];

				// ����γɱ�����
				string vcname = (*pathinfile)[pathinfile->size() - 1] + regexcmatch[2].str();

				// �����ظ�����
				if (varandcons->count(vcname) != 0)
				{
					Log(logbuilder + "�����ظ���" + *item, Log::level_err).print();
					throw - 1;
				}
				// ��������������
				varandcons->insert(vcname);

				// ��ֵ����λ�ã���λ�ò������Ѷ�Ӧ�������������������ѣ�
				*(size_t*)(tmp + 2) = memsiz;

				if (regexcmatch[1].str() == "str")
				{
					// д��������
					asmcodesstring->push_back(regexcmatch[3].str().substr(1, regexcmatch[3].str().size() - 2));
					// �������������
					*(size_t*)(tmp + 11) = asmcodesstring->size() - 1;

					memsiz += 8;
				}
				else if (regexcmatch[1].str() == "lld")
				{
					// ��ֵ����
					*(size_t*)(tmp + 11) = atoll(regexcmatch[3].str().c_str());

					memsiz += 8;
				}
				else if (regexcmatch[1].str() == "ulld")
				{
					// ��ֵ����
					std::stringstream ssteam(regexcmatch[1].str());
					ssteam >> *(size_t*)(tmp + 11);

					memsiz += 8;
				}
				else if (regexcmatch[1].str() == "double")
				{
					// ��ֵ����
					*(size_t*)(tmp + 11) = atof(regexcmatch[3].str().c_str());

					memsiz += 8;
				}
				else if (regexcmatch[1].str() == "char")
				{
					// ��ֵ����
					*(size_t*)(tmp + 11) = regexcmatch[3].str()[1];

					memsiz += 1;
				}
				else if (regexcmatch[1] == "uchar")
				{
					// ��ֵ����
					*(size_t*)(tmp + 11) = regexcmatch[3].str()[1];

					memsiz += 1;
				}

				hassolved = true;
			}

			break;

		case 'c':
			// ��������
			if (boost::regex_match((*item).c_str(), regexcmatch, boost::regex("c:(\\S+):(\\S+)=(\\S+);")))
			{
				char* tmp = new char[LPDefine_size];
				// ��������asmcodes
				asmcodes->insert(pair(tmp, LPDefine_size));

				// ��ֵָ��
				tmp[0] = LPDefine;
				// ��ֵDefine���ͣ�������
				tmp[1] = LPDefine_c;

				// δ֪���ʹ���
				if (LPTS.count(regexcmatch[1].str()) == 0)
				{
					Log(logbuilder + "δ֪�����ͣ�" + *item, Log::level_err).print();
					throw - 1;
				}
				// ��ֵ����
				tmp[10] = LPTS[regexcmatch[1].str()];

				// ����γɱ�����
				string vcname = (*pathinfile)[pathinfile->size() - 1] + regexcmatch[2].str();

				// �����ظ�����
				if (varandcons->count(vcname) != 0)
				{
					Log(logbuilder + "�����ظ���" + *item, Log::level_err).print();
					throw - 1;
				}
				// ��������������
				varandcons->insert(vcname);

				// ��ֵ����λ�ã���λ�ò������Ѷ�Ӧ�������������������ѣ�
				*(size_t*)(tmp + 2) = memsiz;

				if (regexcmatch[1].str() == "str")
				{
					// д��������
					asmcodesstring->push_back(regexcmatch[3].str().substr(1, regexcmatch[3].str().size() - 2));
					// �������������
					*(size_t*)(tmp + 11) = asmcodesstring->size() - 1;

					memsiz += 8;
				}
				else if (regexcmatch[1].str() == "lld")
				{
					// ��ֵ����
					*(size_t*)(tmp + 11) = atoll(regexcmatch[3].str().c_str());

					memsiz += 8;
				}
				else if (regexcmatch[1].str() == "ulld")
				{
					// ��ֵ����
					std::stringstream ssteam(regexcmatch[1].str());
					ssteam >> *(size_t*)(tmp + 11);

					memsiz += 8;
				}
				else if (regexcmatch[1].str() == "double")
				{
					// ��ֵ����
					*(size_t*)(tmp + 11) = atof(regexcmatch[3].str().c_str());

					memsiz += 8;
				}
				else if (regexcmatch[1].str() == "char")
				{
					// ��ֵ����
					*(size_t*)(tmp + 11) = regexcmatch[3].str()[1];

					memsiz += 1;
				}
				else if (regexcmatch[1] == "uchar")
				{
					// ��ֵ����
					*(size_t*)(tmp + 11) = regexcmatch[3].str()[1];

					memsiz += 1;
				}

				hassolved = true;
			}

			break;

		case 'f':
			// ��������
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

	// д���ļ�

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

	// �ļ��ṹ��
	// size_t : ������
	// size_t : ռ���ڴ��ܴ�С
	// ������
	// �ַ���
	//

	Log("[Builder] Finish!", Log::level_suc).print();
	return codes;
}

