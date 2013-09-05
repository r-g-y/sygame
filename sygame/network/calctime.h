#pragma once
#include "sys.h"
/**
 * 统计运行时间 当大于 100ms 时出日志
 */
class CalcTime{
public:
	CalcTime(DWORD tapTime,const char* info,const char *file,DWORD line)
	{
		this->tapTime = tapTime;
		this->startTime = sys::sTime::getNowTime();
		this->info = info;
		this->file = file;
		this->line = line;
	}
	~CalcTime()
	{
		sys::stTimeValue nowTime =  sys::sTime::getNowTime();
		if (startTime + tapTime < nowTime)
		{
			sys::stTimeValue time = sys::sTime::getNowTime() - startTime;
			printf("%s 超时%u ms %us at %s:%u\n",info.c_str(),time.msecond,time.second,file.c_str(),line);
		}
	}
private:
	sys::stTimeValue startTime;
	DWORD tapTime;	
	std::string info;
	std::string  file;
	DWORD line;
};

#define CALCTIME(tap,info) CalcTime calcTime(tap,info,__FILE__,__LINE__) 
