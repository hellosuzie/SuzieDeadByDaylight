#pragma once
#include "SyncContainer.h"
#include <vector>
#include <thread>

class CThreadMaker : public CServerObject
{

protected :
	std::vector<std::thread>				m_vecThread;
	std::vector<HANDLE>						m_vecThreadHandle;



public:
	template<typename FunctionType>
	void MakeThread(FunctionType _ThreadFunc)
	{
		m_vecThread.push_back(std::thread(_ThreadFunc));
		m_vecThreadHandle.push_back(m_vecThread.back()native_handle());
	}

	template<typename FunctionType>
	void MakeThread(FunctionType _ThreadFunc,void* _Arg)
	{
		m_vecThread.push_back(std::thread(_ThreadFunc, _Arg));
		m_vecThreadHandle.push_back(m_vecThread.back().native_handle());

	}
	template<typename FunctionType>
	void MakeThread(FunctionType _ThreadFunc, void* _Arg, const int& _Priority)
	{
		m_vecThread.push_back(std::thread(_ThreadFunc, _Arg));
		m_vecThreadHandle.push_back(m_vecThread.back().native_handle());
		BOOL Check = SetThreadPriority(m_vecThreadHandle.back(), _Priority);
		if (FALSE == Check) 
		{
			TASSERT(true);
		}
	}

public:
	virtual void Release();

public:
	CThreadMaker();
	~CThreadMaker();
};

