#pragma once
#include "ReferenceBase.h"
#include "NameBase.h"
#include <unordered_map>
#include <Windows.h>
#include <process.h>
#include <functional>
#include "DXMacro.h"


template<typename T>
class CCriSectionObject
{
private:
	static bool m_bInit;
	static CRITICAL_SECTION CS;
public:
	CCriSectionObject()
	{
		if (nullptr == CS.DebugInfo)
		{
			InitializeCriticalSection(&CS);
			m_bInit = true;
		}
		EnterCriticalSection(&CS);
	}
	~CCriSectionObject()
	{
		LeaveCriticalSection(&CS);
	}
};

template<typename T>
typename bool CCriSectionObject<T>::m_bInit = false;

template<typename T>
typename CRITICAL_SECTION CCriSectionObject<T>::CS;


class SSDThread : public CReferenceBase, public CNameBase
{
	// 쓰레드 관리 (Static)
private:
	static std::unordered_map<std::wstring, SPTR<SSDThread>>				m_mapThread;

private:
	static SPTR<SSDThread> FindThread(const wchar_t* _Name);

public:
	static void CreateThread(const wchar_t* _Key, _beginthreadex_proc_type _WorkProc = nullptr , void* _Arg = nullptr);

	template<typename T>
	static void CreateMemberThread(const wchar_t* _Name, unsigned (T::*_Work)(void*) = nullptr, T* _Obj = nullptr, void* _Arg = nullptr)
	{
		HPTR<CThread> NewThread = FindThread(_Name);

		if (nullptr != NewThread)
		{
			TASSERT(true);
			return;
		}

		NewThread = new CThread();
		m_ThreadMap.insert(std::unordered_map<std::wstring, HPTR<CThread>>::value_type(_Name, NewThread));
		if (nullptr != _Work && nullptr != _Obj)
		{
			std::function<unsigned(void*)> pFunc = std::bind(_Work, _Obj, std::placeholders::_1);

			if (nullptr == pFunc)
			{
				return;
			}

			NewThread->m_GlobalProc = nullptr;
			NewThread->m_MemberProc = pFunc;
			NewThread->m_pArg = _Arg;
			NewThread->Work();
		}
	}


	// 쓰레드 객체 맴버 
private:
	HANDLE								m_hThread;
	unsigned int						m_ThreadID;
	void*								m_pArg;
	std::function<unsigned(void*)>		m_MemberProc;
	_beginthreadex_proc_type			m_GlobalProc;

	friend unsigned int __stdcall ThreadBaseFunc(void* _Arg);


public:
	void Work(const wchar_t* _Key, _beginthreadex_proc_type _WorkProc, void* _Arg);

	template<typename T>
	void Work(const wchar_t* _Key, T* _Obj,unsigned(T::*_Work)(void*), void* _Arg) 
	{
		SPTR<CThread> NewThread = FindThread(_Key);

		if (nullptr == NewThread)
		{
			assert(nullptr);
			return;
		}

		if (true == NewThread->IsWork())
		{
			assert(nullptr);
			return;
		}

		if (nullptr != _Work&& nullptr != _Obj) 
		{
			NewThread->m_MemberProc = std::bind(_Work, _Obj, std::placeholders::_1);

			if (nullptr == NewThread->m_MemberProc) 
			{
				assert(nullptr);
				return;
			}

			NewThread->m_GlobalProc = nullptr;
			NewThread->m_pArg = _Arg;
			NewThread->Work();
		}
	}
	void Work();

public:
	bool IsWork()
	{
		return m_hThread!= nullptr;
	}


public:
	SSDThread();
	~SSDThread();
};

