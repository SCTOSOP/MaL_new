#pragma once
#include <unordered_map>
#include <string>

/*������*/
// ָ�����
static constexpr unsigned char LPDefine					= 0x01;
// 0��ָ� 1��Define���͡� 2-9���ڴ�ָ�롿 10�����͡� 11-18������ֵ���ݡ�
static constexpr unsigned long long LPDefine_size		= 19;
// ָ���������
static constexpr unsigned char LPBuildFunc				= 0x01;
// 0��ָ� 
static constexpr unsigned long long LPBuildFunc_size	= 0x01;

/*Define����*/
// Define����
static constexpr unsigned char LPDefine_v	= 0x00;
// Define����
static constexpr unsigned char LPDefine_c	= 0x01;

/*������*/
static std::unordered_map<std::string, unsigned char> LPTS;
static constexpr unsigned char LPlld		= 0x00;
static constexpr unsigned char LPulld		= 0x01;
static constexpr unsigned char LPdouble		= 0x02;
static constexpr unsigned char LPstring		= 0x03;
static constexpr unsigned char LPchar		= 0x04;
static constexpr unsigned char LPuchar		= 0x05;
static constexpr unsigned char LPvoid		= 0x06;

void LPTS_init();
//static constexpr unsigned char TLPvector	= 0x06;
//static constexpr unsigned char TLPmap		= 0x07;

