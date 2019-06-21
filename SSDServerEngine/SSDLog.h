#pragma once
#include <Windows.h>


class SSDLog
{
public:
	static BOOL WriteLog(LPTSTR _data, ...);

private:
	SSDLog() {}
	virtual ~SSDLog() = 0;
};

