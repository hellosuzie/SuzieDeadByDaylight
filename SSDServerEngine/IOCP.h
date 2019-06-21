#pragma once
#include "SyncContainer.h"
#include "ServerObject.h"

class CIOCP : public CServerObject
{
protected:
	HANDLE								m_hIOCP;

public:
	const HANDLE GetIOCPHandle() { return m_hIOCP; }

public:
	const HANDLE CreateIOCP();
	void BindingSocket(HANDLE _BindSock, ULONG_PTR _CompletionKey = 0,const DWORD& _CurrentThreadCount = 0);
	void TerminateIOCP();

public:
	CIOCP();
	~CIOCP();
};

