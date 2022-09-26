#include "LParts.h"
using namespace std;

void LPTS_init()
{
	LPTS.insert(pair("lld", LPlld));
	LPTS.insert(pair("ulld", LPulld));
	LPTS.insert(pair("dbl", LPdouble));
	LPTS.insert(pair("str", LPstring));
	LPTS.insert(pair("char", LPchar));
	LPTS.insert(pair("uchar", LPuchar));
	LPTS.insert(pair("void", LPvoid));
}
