#pragma once
#include <unordered_map>
#include <string>

/*命令区*/
// 指令：声明
static constexpr unsigned char LPDefine					= 0x01;
// 0【指令】 1【Define类型】 2-9【内存指针】 10【类型】 11-18【被赋值内容】
static constexpr unsigned long long LPDefine_size		= 19;
// 指令：构建函数
static constexpr unsigned char LPBuildFunc				= 0x01;
// 0【指令】 
static constexpr unsigned long long LPBuildFunc_size	= 0x01;

/*Define类型*/
// Define变量
static constexpr unsigned char LPDefine_v	= 0x00;
// Define常量
static constexpr unsigned char LPDefine_c	= 0x01;

/*类型区*/
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

