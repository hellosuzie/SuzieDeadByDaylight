#include "SSDThread.h"
#include "DXTemplate.h"

std::unordered_map<std::wstring, SPTR<SSDThread>>				SSDThread::m_mapThread;

SPTR<SSDThread> SSDThread::FindThread(const wchar_t* _Name)
{
	return MAPFINDSPTR<SPTR<SSDThread>>(m_mapThread, _Name);
}
void SSDThread::CreateThread(const wchar_t* _Key, _beginthreadex_proc_type _WorkProc /*= nullptr*/, void* _Arg /*= nullptr*/)
{
	if (_WorkProc == nullptr)
		return;

	SPTR<SSDThread> NewThread = FindThread(_Key);

	if (nullptr != NewThread)
	{
		TASSERT(true);
		return;
	}

	NewThread = new SSDThread();

	NewThread->m_MemberProc = nullptr;
	NewThread->m_GlobalProc = _WorkProc;
	NewThread->m_pArg = _Arg;
	NewThread->Work();
	m_mapThread.insert(std::unordered_map<std::wstring, SPTR<SSDThread>>::value_type(_Key, NewThread));



}

SSDThread::SSDThread()
	:m_hThread(nullptr),m_MemberProc(nullptr),m_GlobalProc(nullptr),m_pArg(nullptr)
{
}


SSDThread::~SSDThread()
{
	TerminateThread(m_hThread, 0);
}

void SSDThread::Work(const wchar_t* _Key, _beginthreadex_proc_type _WorkProc, void* _Arg)
{
	SPTR<SSDThread> NewThread = FindThread(_Key);

	if (NewThread == nullptr)
	{
		assert(nullptr);
		return;
	}

	if (true == NewThread->IsWork()) 
	{
		assert(nullptr);
		return;
	}

	if (nullptr != _WorkProc)
	{
		NewThread->m_GlobalProc = _WorkProc;
		NewThread->m_pArg = _Arg;
		NewThread->Work();
	}
}
void SSDThread::Work()
{
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadBaseFunc, this, 0, &m_ThreadID);
}
unsigned int __stdcall ThreadBaseFunc(void* _Arg)
{
	if (nullptr == _Arg)
	{
		TASSERT(true);
		return 0;
	}

	SSDThread* Ptr = (SSDThread*)_Arg;

	if (nullptr != Ptr->m_GlobalProc)
	{
		Ptr->m_GlobalProc(Ptr->m_pArg);
	}
	else
	{
		Ptr->m_MemberProc(Ptr->m_pArg);
	}

	Ptr->m_hThread = nullptr;
	Ptr->m_MemberProc = nullptr;
	Ptr->m_GlobalProc = nullptr;
	Ptr->m_pArg = nullptr;
	return 0;
}
