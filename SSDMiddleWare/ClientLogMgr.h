#pragma once
#include <ThreadMaker.h>
#include "ServerLogManager.h"


class CClientLogMgr : public CThreadMaker
{
private:
	friend class CClientConnectMgr;
	static unsigned int _stdcall ClientLogThreadProcess(void* _Arg);

public:
	static std::string								strLog;
	template<typename ...Rest>
	static const std::string MakeLogString(Rest ..._Arg)
	{
		std::string ReturnString;

		CServerCS Lock;
		Lock.Enter();
		strLog.clear();
		MakeString(_Arg...);
		ReturnString = strLog;
		Lock.Leave();
		return ReturnString;
	}


private:
	CServerCS								m_LogCS;
	CIOCP									m_LogIOCP;
	HANDLE									m_LogIOCPHandle;
	FILE*									m_pConOut;




private:
	template<typename ...Rest>
	static void MakeString(const char* _Str, Rest ..._Arg)
	{
		strLog += _Str;
		MakeString(_Arg...);
	}

	template<typename ...Rest>
	static void MakeString(const int& _StrNum, Rest ..._Arg)
	{
		strLog += std::to_string(_StrNum);
		MakeString(_Arg...);
	}

	template<typename ...Rest>
	static void MakeString(const unsigned __int64& _StrNum, Rest ..._Arg)
	{
		strLog += std::to_string(_StrNum);
		MakeString(_Arg...);
	}

	template<typename ...Rest>
	static void MakeString(const DWORD& _StrNum, Rest ..._Arg)
	{
		strLog += std::to_string(_StrNum);
		MakeString(_Arg...);
	}

	template<typename ...Rest>
	static void MakeString(const float& _StrNum, Rest ..._Arg)
	{
		float PrintNum = roundf(_StrNum * 1000.f);
		strLog += std::to_string(PrintNum * 0.001f);
		MakeString(_Arg...);
	}

	template<typename ...Rest>
	static void MakeString(const Vec3& _StrVec, Rest ..._Arg)
	{
		strLog += "( ";

		for (size_t i = 0; i < 3; i++)
		{
			MakeString(_StrVec.x);

			if (i <= 2)
			{
				strLog += ", ";
			}
		}
		strLog += " )";
		MakeString(_Arg...);
	}

private:
	static void MakeString(const char*  _Str)
	{
		strLog += _Str;
	}

	static void MakeString(const int& _StrNum)
	{
		strLog += std::to_string(_StrNum);
	}

	static void MakeString(const unsigned __int64& _StrNum)
	{
		strLog += std::to_string(_StrNum);
	}

	static void MakeString(const DWORD& _StrNum)
	{
		strLog += std::to_string(_StrNum);
	}

	static 	void MakeString(const float& _StrNum)
	{
		float PrintNum = roundf(_StrNum * 1000.f);
		strLog += std::to_string(PrintNum * 0.001f);
		return;
	}

	static void MakeString(const Vec3& _StrVec)
	{
		strLog += "( ";

		for (size_t i = 0; i < 3; i++)
		{
			MakeString(_StrVec.x);

			if (i <= 2)
			{
				strLog += ", ";
			}
		}
		strLog += " )";
	}

private:
	void Init();
	void Release() override;

public:
	void PrintLog(const std::string& _Log, const LOG_FLAG& _Flag = LOG_PRINT_CONSOL);



public:
	CClientLogMgr();
	~CClientLogMgr();
};

