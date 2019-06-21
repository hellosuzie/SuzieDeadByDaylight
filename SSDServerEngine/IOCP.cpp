#include "IOCP.h"



CIOCP::CIOCP()
	:m_hIOCP(NULL)
{
}


CIOCP::~CIOCP()
{
	if (NULL != m_hIOCP)
	{
		CloseHandle(m_hIOCP);
	}
}


const HANDLE CIOCP::CreateIOCP() 
{
	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
	return m_hIOCP;
}
void CIOCP::BindingSocket(HANDLE _BindSock, ULONG_PTR _CompletionKey /*= 0*/, const DWORD& _CurrentThreadCount /*= 0*/)
{
	CreateIoCompletionPort(_BindSock, m_hIOCP, _CompletionKey, _CurrentThreadCount);
}
void CIOCP::TerminateIOCP()
{
	PostQueuedCompletionStatus(m_hIOCP, ENDSIGNAL, ENDSIGNAL, ENDSIGNAL);
}