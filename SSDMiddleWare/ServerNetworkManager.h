#pragma once
#include <ThreadMaker.h>

typedef class CServerNetworkManager : public CThreadMaker
{
private:
	friend class IOCPServerMgr;
	IOCPServerMgr*								m_pIOCPMgr;

public:
	static unsigned int _stdcall NetworkThreadProcess(void* _Arg);

private:
	class CIOCP*								m_NetworkIOCP;
	HANDLE										m_hNetIOCP;


private:
	void Init(const unsigned int& _ThreadCount);
	void Release() override;



public:
	CServerNetworkManager();
	CServerNetworkManager(IOCPServerMgr* _IOCPMgr);
	~CServerNetworkManager();
}NetworkManager;

