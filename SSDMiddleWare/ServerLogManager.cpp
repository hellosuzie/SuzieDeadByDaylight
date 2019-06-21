#include "ServerLogManager.h"
#include "ServerCommendManager.h"
#include "ServerNetworkManager.h"
#include "IOCPServerMgr.h"


FILE*					ServerLogManager::pConOut = nullptr;


unsigned int _stdcall ServerLogManager::LogThreadProcess(void* _Arg)
{
	AllocConsole();

	freopen_s(&pConOut, "CONOUT$", "wt", stdout);


	IOCPServerMgr* IOCPMgr = (IOCPServerMgr*)_Arg;

	if (nullptr == IOCPMgr)
	{
		std::wstring ErrorText = L"Create Log IOCP Thread Fail\n";
		OutputDebugString(ErrorText.c_str());
		return -1;
	}

	NetworkManager* NetMgr = IOCPMgr->pNetworkMgr;
	CommandManager* ComMgr = IOCPMgr->pCommandMgr;
	LogManager* LogMgr = IOCPMgr->pLogMgr;

	HANDLE hLogIOCP = LogMgr->m_LogIOCPHandle;
	DWORD ByteSize = 0;
	LogData* pLogData;
	CServerUser* pUser = nullptr;
	OVERLAPPED* POVERLAPPED = nullptr;

	while (true)
	{
		int  Result = GetQueuedCompletionStatus(hLogIOCP, &ByteSize, (PULONG_PTR)&pLogData, &POVERLAPPED, INFINITE);

		if (ENDSIGNAL == ByteSize && ENDSIGNAL == pLogData  &&  ENDSIGNAL == POVERLAPPED)
		{
			break;
		}

		if (0 == ByteSize)
		{
			continue;
		}

		if (pLogData->Flag & LOG_PRINT_CONSOL) 
		{
			printf("%s\n", pLogData->StrBuffer);
		}
		
		LogMgr->m_deLogData.Push(pLogData);
	}

	if (nullptr != pConOut)
		FreeConsole();

	return 0;

}



ServerLogManager::ServerLogManager()
	: m_pIOCPMgr(nullptr), m_LogIOCP(CIOCP()),m_LogIOCPHandle(NULL)
	, m_LogCS(CServerCS())
{
}

ServerLogManager::ServerLogManager(IOCPServerMgr* _pIOCPMgr)
	: m_pIOCPMgr(_pIOCPMgr), m_LogIOCP(CIOCP()), m_LogIOCPHandle(NULL)
	, m_LogCS(CServerCS())
{

}
ServerLogManager::~ServerLogManager()
{
}


void ServerLogManager::Init() 
{
	m_LogIOCPHandle =  m_LogIOCP.CreateIOCP();
	MakeThread(LogThreadProcess, m_pIOCPMgr);
	//MakeThread(LogThreadProcess, m_pIOCPMgr, THREAD_PRIORITY_HIGHEST);
	IOCPServerMgr::InitComplete();
}
void ServerLogManager::Release()
{
	PostQueuedCompletionStatus(m_LogIOCPHandle, ENDSIGNAL, ENDSIGNAL, ENDSIGNAL);

	CThreadMaker::Release();
}

void ServerLogManager::PrintLog(const std::string& _Log, const LOG_FLAG& _Flag)
{
	LogData* Data =	m_deLogData.Pop();

	if (nullptr == Data) 
	{
		TASSERT(true);
		return;
	}

	Data->Flag = _Flag;
	Data->StrSize = (_Log.size() + 1 )* sizeof(char);
	strcpy_s(Data->StrBuffer, Data->StrSize ,_Log.c_str());
	PostQueuedCompletionStatus(m_LogIOCPHandle, sizeof(LogData*), (ULONG_PTR)Data, ENDSIGNAL);
}