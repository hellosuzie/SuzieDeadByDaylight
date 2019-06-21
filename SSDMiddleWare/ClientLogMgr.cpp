#include "ClientLogMgr.h"


std::string				CClientLogMgr::strLog;

unsigned int _stdcall CClientLogMgr::ClientLogThreadProcess(void* _Arg)
{
	CClientLogMgr* pLogMgr = reinterpret_cast<CClientLogMgr*>(_Arg);

	if (nullptr == pLogMgr)
	{
		std::wstring ErrorText = L"Create Log IOCP Thread Fail\n";
		OutputDebugString(ErrorText.c_str());
		return -1;
	}

	AllocConsole();
	freopen_s(&pLogMgr->m_pConOut, "CONOUT$", "wt", stdout);


	HANDLE hLogIOCP = pLogMgr->m_LogIOCPHandle;
	DWORD ByteSize = 0;
	LogData* pLogData;
	char	LogBuffer[MAXLOGSTRING];
	CServerUser* pUser = nullptr;
	OVERLAPPED* POVERLAPPED = nullptr;

	while (true)
	{
		int  Result = GetQueuedCompletionStatus(hLogIOCP, &ByteSize, (PULONG_PTR)&pLogData, &POVERLAPPED, INFINITE);

		if (ENDSIGNAL == ByteSize && ENDSIGNAL == pLogData  &&  ENDSIGNAL == POVERLAPPED)
		{
			break;
		}

		CopyMemory(LogBuffer, pLogData->StrBuffer, pLogData->StrSize);

		if (0 == ByteSize)
		{
			continue;
		}

		if (pLogData->Flag & LOG_PRINT_CONSOL)
		{
			printf("%s\n", LogBuffer);
		}


	}

	if (nullptr != pLogMgr->m_pConOut)
		FreeConsole();

	return 0;

}



CClientLogMgr::CClientLogMgr()
	:m_pConOut(nullptr), m_LogCS(CServerCS())
{
}


CClientLogMgr::~CClientLogMgr()
{
}


void CClientLogMgr::Init()
{
	m_LogIOCPHandle = m_LogIOCP.CreateIOCP();
	MakeThread(ClientLogThreadProcess, this);


}
void CClientLogMgr::Release()
{
	m_LogIOCP.TerminateIOCP();
	CThreadMaker::Release();
}

void CClientLogMgr::PrintLog(const std::string& _Log, const LOG_FLAG& _Flag /*= LOG_PRINT_CONSOL*/)
{
	LocalCS Lock(&m_LogCS);
	static LogData Log;
	Log.Flag = _Flag;
	Log.StrSize = (_Log.size() + 1) * sizeof(char);
	strcpy_s(Log.StrBuffer, Log.StrSize, _Log.c_str());
	PostQueuedCompletionStatus(m_LogIOCPHandle, sizeof(Log), (ULONG_PTR)&Log, ENDSIGNAL);
}