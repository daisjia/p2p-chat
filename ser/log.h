#ifndef _LOG_H_
#define _LOG_H_

#include<string>
#include <cstdio>
#include<iostream>

void LOGE(string str)
{
	std::cout << "======= ERROR ======= : " << str << std::endl;
}

void LOGD(string str)
{
	std::cout << "======= LOGE ======= : " << str << std::endl;
}

#endif // !_LOG_H
