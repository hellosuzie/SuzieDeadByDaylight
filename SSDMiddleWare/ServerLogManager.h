#pragma once
#include <ThreadMaker.h>
#include <IOCP.h>
#include "MiddleMacro.h"
#include "MiddleStruct.h"
#include <DXHeader.h>


typedef class ServerLogManager : public CThreadMaker
{
private:
	friend class IOCPServerMgr;
	static FILE*									pConOut;
	static unsigned int _stdcall LogThreadProcess(void* _Arg);
	
public:
	template<typename ...Rest>
	static const std::string MakeLogString(Rest ..._Arg) 
	{
		std::string ReturnString;
		ReturnString = MakeString(_Arg...);

		return ReturnString;
	}


private:
	CServerCS								m_LogCS;
	IOCPServerMgr*							m_pIOCPMgr;
	CIOCP									m_LogIOCP;
	HANDLE									m_LogIOCPHandle;
	CSyncDeque<LogData>						m_deLogData;



private:
	//template<typename ...Rest>
	//static void MakeString(const char* _Str , Rest ..._Arg)
	//{
	//	strLog += _Str;
	//	MakeString(_Arg...);
	//}

	//template<typename ...Rest>
	//static void MakeString(const int& _StrNum, Rest ..._Arg)
	//{
	//	strLog += std::to_string(_StrNum);
	//	MakeString(_Arg...);
	//}

	//template<typename ...Rest>
	//static void MakeString(const unsigned __int64& _StrNum, Rest ..._Arg)
	//{
	//	strLog += std::to_string(_StrNum);
	//	MakeString(_Arg...);
	//}

	//template<typename ...Rest>
	//static void MakeString(const DWORD& _StrNum, Rest ..._Arg)
	//{
	//	strLog += std::to_string(_StrNum);
	//	MakeString(_Arg...);
	//}

	//template<typename ...Rest>
	//static void MakeString(const float& _StrNum, Rest ..._Arg)
	//{
	//	float PrintNum = roundf(_StrNum * 100.f);
	//	strLog += std::to_string(PrintNum * 0.01f);
	//	MakeString(_Arg...);
	//}

	//template<typename ...Rest>
	//static void MakeString(const Vec3& _StrVec, Rest ..._Arg)
	//{
	//	strLog += "( ";
	//
	//	for (size_t i = 0; i < 3; i++)
	//	{
	//		MakeString(_StrVec.x);
	//		
	//		if (i <= 2) 
	//		{
	//			strLog += ", ";
	//		}
	//	}
	//	strLog += " )";
	//	MakeString(_Arg...);
	//}

//private:
//	static void MakeString(const char*  _Str)
//	{
//		strLog += _Str;
//	}
//
//	static void MakeString(const int& _StrNum)
//	{
//		strLog += std::to_string(_StrNum);
//	}
//	
//	static void MakeString(const unsigned __int64& _StrNum)
//	{
//		strLog += std::to_string(_StrNum);
//	}
//
//	static void MakeString(const DWORD& _StrNum)
//	{
//		strLog += std::to_string(_StrNum);
//	}
//
//	static 	void MakeString(const float& _StrNum)
//	{
//		float PrintNum = roundf(_StrNum * 100.f);
//		strLog += std::to_string(PrintNum * 0.01f);
//		return;
//	}
//
	//static void MakeString(const Vec3& _StrVec)
	//{
	//	strLog += "( ";

	//	for (size_t i = 0; i < 3; i++)
	//	{
	//		MakeString(_StrVec.x);

	//		if (i <= 2)
	//		{
	//			strLog += ", ";
	//		}
	//	}
	//	strLog += " )";
	//}


	template<typename ...Rest>
	static std::string MakeString(const char* _Str, Rest ..._Arg)
	{
		return _Str + MakeString(_Arg...);
	}

	template<typename ...Rest>
	static std::string MakeString(const int& _StrNum, Rest ..._Arg)
	{
		return std::to_string(_StrNum) + MakeString(_Arg...);
	}

	template<typename ...Rest>
	static std::string MakeString(const unsigned __int64& _StrNum, Rest ..._Arg)
	{
		return std::to_string(_StrNum) + MakeString(_Arg...);
	}

	template<typename ...Rest>
	static std::string MakeString(const DWORD& _StrNum, Rest ..._Arg)
	{
		return std::to_string(_StrNum) + MakeString(_Arg...);
	}

	template<typename ...Rest>
	static std::string MakeString(const float& _StrNum, Rest ..._Arg)
	{
		float PrintNum = roundf(_StrNum * 1000.f);
		return std::to_string(PrintNum * 0.001f) + MakeString(_Arg...);
	}

	template<typename ...Rest>
	static std::string MakeString(const Vec3& _StrVec, Rest ..._Arg)
	{
		std::string Temp = "";

		Temp += "( ";

		for (size_t i = 0; i < 3; i++)
		{
			Temp += MakeString(_StrVec.x);

			if (i <= 2)
			{
				Temp += ", ";
			}
		}
		Temp += " )";
		return Temp + MakeString(_Arg...);
	}

private:
	static std::string MakeString(const char*  _Str)
	{
		return _Str;
	}

	static std::string MakeString(const int& _StrNum)
	{
		return std::to_string(_StrNum);
	}

	static std::string MakeString(const unsigned __int64& _StrNum)
	{
		return std::to_string(_StrNum);
	}

	static std::string MakeString(const DWORD& _StrNum)
	{
		return std::to_string(_StrNum);
	}

	static 	std::string MakeString(const float& _StrNum)
	{
		float PrintNum = roundf(_StrNum * 1000.f);
		return std::to_string(PrintNum * 0.001f);
	}

	static std::string MakeString(const Vec3& _StrVec)
	{
		std::string Temp = "";
		Temp += "( ";

		for (size_t i = 0; i < 3; i++)
		{
			Temp += MakeString(_StrVec.x);

			if (i <= 2)
			{
				Temp += ", ";
			}
		}

		Temp += " )";
		return Temp;
	}

private:
	void Init();
	void Release() override;

public:
	void PrintLog(const std::string& _Log, const LOG_FLAG& _Flag);



public:
	ServerLogManager();
	ServerLogManager(IOCPServerMgr* _pIOCPMgr);
	~ServerLogManager();
}LogManager;

