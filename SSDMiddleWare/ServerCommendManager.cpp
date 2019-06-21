#include "ServerCommendManager.h"
#include "ServerNetworkManager.h"
#include "ServerLogManager.h"
#include "IOCPServerMgr.h"
#include "MiddleStruct.h"
#include "Room.h"

Command_Proc_Function		CServerCommandManager::CommandProcFuntion[MAX_COMMAND_FLAG] = {nullptr,};
CPacketQueue				CServerCommandManager::LobbyPacketQueue;

unsigned int _stdcall CServerCommandManager::LobbyThreadProcess(void* _Arg)
{
	CommandObj* Obj = reinterpret_cast<CommandObj*>(_Arg);
	IOCPServerMgr* IOCPMgr = Obj->pIOCPManager;

	if (nullptr == IOCPMgr)
	{
		std::wstring ErrorText = L"Create Lobby Commend IOCP Thread Fail\n";
		OutputDebugString(ErrorText.c_str());
		return -1;
	}

	NetworkManager* NetMgr = IOCPMgr->pNetworkMgr;
	CommandManager* ComMgr = IOCPMgr->pCommandMgr;
	LogManager*  LogMgr = IOCPMgr->pLogMgr;

	HANDLE hCommandHandle = Obj->hIOCP;
	DWORD ByteSize = 0;
	ServerCommand* pCommand;
	CServerUser* pUser = nullptr;
	OVERLAPPED* POVERLAPPED = nullptr;

	while (true)
	{
		int  Result = GetQueuedCompletionStatus(hCommandHandle, &ByteSize, (PULONG_PTR)&pCommand, &POVERLAPPED, INFINITE);

		if (ENDSIGNAL == ByteSize && ENDSIGNAL == pCommand  &&  ENDSIGNAL == POVERLAPPED)
		{
			break;
		}

		if (0 == ByteSize)
		{
			continue;
		}

		CommandProcFuntion[pCommand->Flag](Obj, pCommand->DeltaTime);
		ComMgr->m_LobbyObj.CommendBuffer.Push(pCommand);
	
	}

	return 0;

}

unsigned int _stdcall CServerCommandManager::TestRoomThreadProcess(void* _Arg)
{
	CommandObj* Obj = reinterpret_cast<CommandObj*>(_Arg);
	IOCPServerMgr* IOCPMgr = Obj->pIOCPManager;

	if (nullptr == IOCPMgr)
	{
		std::wstring ErrorText = L"Create Room Commend IOCP Thread Fail\n";
		OutputDebugString(ErrorText.c_str());
		return -1;
	}

	HANDLE hCommandHandle = Obj->hIOCP;

	Obj->pRoom->CreateGameWorld();

	std::chrono::system_clock::time_point Prev = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point Cur;
	std::chrono::duration<float> Time;

	float DeltaTime = 0.f;

	while (Obj->bProgress)
	{
		Cur = std::chrono::system_clock::now();
		Time = Cur - Prev;

		Obj->pRoom->PacketProcessing(Time.count());

		if (true == Obj->pRoom->IsActive()) 
		{
			Obj->pRoom->TestRoomUpdate(Time.count());
		}

		Prev = Cur;
		Sleep(5);
	}

	return 0;
}
unsigned int _stdcall CServerCommandManager::RoomThreadProcess(void* _Arg)
{
	CommandObj* Obj = reinterpret_cast<CommandObj*>(_Arg);
	IOCPServerMgr* IOCPMgr = Obj->pIOCPManager;

	if (nullptr == IOCPMgr)
	{
		std::wstring ErrorText = L"Create Room Commend IOCP Thread Fail\n";
		OutputDebugString(ErrorText.c_str());
		return -1;
	}

	HANDLE hCommandHandle = Obj->hIOCP;

	std::chrono::system_clock::time_point Prev = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point Cur;
	std::chrono::duration<float> Time;
	float DeltaTime = 0.f;
	int Count = 0;

	while (Obj->bProgress )
	{
		Cur = std::chrono::system_clock::now();
		Time = Cur - Prev;

		Obj->pRoom->PacketProcessing(Time.count());
		Obj->pRoom->Update(Time.count());

		Prev = Cur;
		Sleep(5);
	}

	return 0;

}

void CServerCommandManager::LobbyProcessing(void* _pArg, const float& _DeltaTime)
{
	CommandObj* Obj = reinterpret_cast<CommandObj*>(_pArg);

	while (!LobbyPacketQueue.Empty()) 
	{
		PACKET Pack;
		LobbyPacketQueue.Pop(&Pack);

		if (nullptr != PacketProcessing::PacketProcFuntion[Pack.Header.HeaderFlag])
		{
			PacketProcessing::PacketProcFuntion[Pack.Header.HeaderFlag](Pack);
		}

	}
}
void CServerCommandManager::PacketProcessing(void* _pArg, const float& _DeltaTime)
{
	CommandObj* Obj = reinterpret_cast<CommandObj*>(_pArg);

	Obj->pRoom->PacketProcessing(_DeltaTime);
	//Obj->pRoom->BroadcastPacket(_DeltaTime);
}
void CServerCommandManager::GameProcessing(void* _pArg, const float& _DeltaTime)
{
	CommandObj* Obj = reinterpret_cast<CommandObj*>(_pArg);

	Obj->pRoom->GameLogicProcessing(_DeltaTime);
}

void CServerCommandManager::ExitUser(void* _pArg, const float& _DeltaTime)
{

}
CServerCommandManager::CServerCommandManager()
	: m_LobbyObj(CommandObj())
{
	
}
CServerCommandManager::CServerCommandManager(IOCPServerMgr* _IOCPMgr)
	: m_pIOCPMgr(_IOCPMgr), m_LobbyObj(CommandObj())
{
}


CServerCommandManager::~CServerCommandManager()
{
}

void CServerCommandManager::Init(const unsigned int& _ThreadCount)
{
	CommandProcFuntion[COMMAND_LOBBY_PACKET_PROCESSING] = LobbyProcessing;
	CommandProcFuntion[COMMAND_PACKET_PROCESSING] = PacketProcessing;
	CommandProcFuntion[COMMAND_GAME_PROCESSING] = GameProcessing;
	CommandProcFuntion[COMMAND_EXIT_USER_PROCESSING] = ExitUser;

	LobbyPacketQueue.Init();
	m_LobbyObj.pIOCP = new CIOCP();
	m_LobbyObj.pIOCPManager = m_pIOCPMgr;

	m_LobbyObj.pIOCP->CreateIOCP();
	m_LobbyObj.hIOCP = m_LobbyObj.pIOCP->GetIOCPHandle();

	MakeThread(LobbyThreadProcess, &m_LobbyObj);
	//MakeThread(LobbyThreadProcess, &m_LobbyObj, THREAD_PRIORITY_HIGHEST);

	m_MaxRoomCount = _ThreadCount;
	m_vecCommendObj.resize(m_MaxRoomCount);



	for (UINT i = 0; i < m_MaxRoomCount; i++)
	{
		m_vecCommendObj[i].pIOCP = new CIOCP();
		m_vecCommendObj[i].pRoom = new CRoom();
		m_vecCommendObj[i].pIOCPManager = m_pIOCPMgr;

		m_vecCommendObj[i].pIOCP->CreateIOCP();
		m_vecCommendObj[i].hIOCP = m_vecCommendObj[i].pIOCP->GetIOCPHandle();
		m_vecCommendObj[i].pRoom->SetRoomNumber(i);
		m_vecCommendObj[i].pRoom->CreateGameWorld();
		m_vecCommendObj[i].pRoom->UnActiveRoom();
		if (i == 0) 
		{
			m_vecCommendObj[i].pRoom->SetTestRoom();
			MakeThread(TestRoomThreadProcess, &m_vecCommendObj[i]);
		}
		else 
		{
			MakeThread(RoomThreadProcess, &m_vecCommendObj[i]);
		}
		//MakeThread(RoomThreadProcess, &m_vecCommendObj[i], THREAD_PRIORITY_HIGHEST);

	}


	IOCPServerMgr::InitComplete();
}
void CServerCommandManager::Release()
{
	size_t ThreadCount = m_vecCommendObj.size();

	for (size_t i = 0; i < ThreadCount; i++)
	{
		//m_vecCommendObj[i].bProgress = false;
		PostQueuedCompletionStatus(m_vecCommendObj[i].pIOCP->GetIOCPHandle(), ENDSIGNAL, ENDSIGNAL, ENDSIGNAL);
	}

	CThreadMaker::Release();

	for (size_t i = 0; i < ThreadCount; i++)
	{
		DELETE_POINTER(m_vecCommendObj[i].pIOCP);
		DELETE_POINTER(m_vecCommendObj[i].pRoom);
		m_vecCommendObj[i].pIOCPManager = nullptr;
	}

	m_vecCommendObj.clear();

}

const unsigned int CServerCommandManager::FindNoMasterRoom()
{
	for (size_t i = 1; i < m_vecCommendObj.size(); i++)
	{
		if (false == m_vecCommendObj[i].pRoom->IsActive()) 
		{
			return (unsigned int)i;
		}
	}

	return -1;
}
const int CServerCommandManager::EnterRoom(const RoomEnterData& _EnterData)
{
	if (-1 == _EnterData.RoomNumber)
	{
		int RanRoom = RandomMatching();

		if (-1 == RanRoom)
			return -1;

		CRoom* Room = m_vecCommendObj[RanRoom].pRoom;

		if (false == Room->EnterRoom(_EnterData))
		{
			return -1;
		}
		else
		{
			return	RanRoom;
		}
	}
	else
	{
		CRoom* Room = m_vecCommendObj[_EnterData.RoomNumber].pRoom;

		if (Room->IsFullRoom())
		{
			return -1;
		}
		else
		{
			if (false == Room->EnterRoom(_EnterData))
			{
				return -1;
			}
			else
			{
				return _EnterData.RoomNumber;
			}
		}
	}

	return -1;
}

const int CServerCommandManager::EnterRoom(CServerUser* _pUser, const int& _RoomNum)
{
	if (-1 == _RoomNum)
	{
		int RanRoom = RandomMatching();

		if (-1 == RanRoom)
			return -1;

		CRoom* Room = m_vecCommendObj[RanRoom].pRoom;

		if (false == Room->EnterRoom(_pUser))
		{
			return -1;
		}
		else
		{
			return	RanRoom;
		}
	}
	else
	{
		CRoom* Room = m_vecCommendObj[_RoomNum].pRoom;

		if (Room->IsFullRoom())
		{
			return -1;
		}
		else
		{
			if (false == Room->EnterRoom(_pUser))
			{
				return -1;
			}
			else
			{
				return _RoomNum;
			}
		}
	}

	return -1;
}

const bool CServerCommandManager::EnterTestRoom(CServerUser* _pUser)
{
	CRoom* Room = m_vecCommendObj[0].pRoom;

	if (Room->IsFullRoom())
	{
		return false;
	}
	
	return Room->EnterRoom(_pUser);
}
const int CServerCommandManager::RandomMatching()
{
	
	for (unsigned int i = 1; i < m_MaxRoomCount; ++i)
	{
		if (false == m_vecCommendObj[i].pRoom->IsFullRoom())
			return i;
	}

	return -1;
}
void CServerCommandManager::ExitRoom(CServerUser* _pUser)
{
	if (_pUser->m_RoomNumber < m_MaxRoomCount) 
	{
		PACKET Pack;
		Pack.Header.HeaderFlag = FLAG_DISCONNECT;
		Pack.pUser = _pUser;
		m_vecCommendObj[_pUser->m_RoomNumber].pRoom->PushPacket(Pack);
	}

}
void CServerCommandManager::BuildCompleteEvent(CServerUser* _pUser)
{
	//m_vecCommendObj[_pUser->m_RoomNumber].pRoom->BuildCompleteEvent(_pUser);
}
void CServerCommandManager::SendActiveRoomList(CServerUser* _pUser)
{
	RoomList Data;
	
	for (unsigned int i = 1; i < m_MaxRoomCount; i++)
	{
		if (false == m_vecCommendObj[i].pRoom->IsActive())
			continue;

		Data.ArrRoomData[Data.Header.ActiveRoomCount].RoomNumber = m_vecCommendObj[i].pRoom->GetRoomNumber();
		Data.ArrRoomData[Data.Header.ActiveRoomCount].RoomUserCount = m_vecCommendObj[i].pRoom->GetUserCount();
		Data.ArrRoomData[Data.Header.ActiveRoomCount].bGamePlaying = m_vecCommendObj[i].pRoom->IsGamePlaying();
		++Data.Header.ActiveRoomCount;
	}

	Data.Header.DataSize = sizeof(ActiveRoomHeader) + sizeof(ActiveRoomData)* Data.Header.ActiveRoomCount;
	_pUser->SendData(&Data, Data.Header.DataSize, FLAG_REQUEST_ROOM_LIST, 0);
}
void CServerCommandManager::RecvUserPacket(CServerUser* _pUser)
{
	if (_pUser->m_RoomNumber >= m_MaxRoomCount)
	{
		LobbyPacketQueue.PushUserPacket(_pUser);
	}
	else 
	{
		m_vecCommendObj[_pUser->m_RoomNumber].pRoom->PushPacket(_pUser);
	}


}


void CServerCommandManager::PacketProgress(const float& _DeltaTime)
{
	ServerCommand* Command =  m_LobbyObj.CommendBuffer.Pop();

	if (nullptr == Command)
	{
		TASSERT(true);
		return;
	}

	Command->Flag  = COMMAND_LOBBY_PACKET_PROCESSING;
	Command->DeltaTime = _DeltaTime;
	

	PostQueuedCompletionStatus(m_LobbyObj.hIOCP, sizeof(ServerCommand), (ULONG_PTR)Command, NULL);

	//size_t Count = m_vecCommendObj.size();
	//for (size_t i = 0; i < Count; i++)
	//{
	//	Command = m_vecCommendObj[i].CommendBuffer.Pop();
	//	if (nullptr == Command) 
	//	{
	//		TASSERT(true);
	//		continue;
	//	}
	//	Command->Flag = COMMAND_PACKET_PROCESSING;
	//	Command->DeltaTime = _DeltaTime;
	//	PostQueuedCompletionStatus(m_vecCommendObj[i].hIOCP, sizeof(ServerCommand), (ULONG_PTR)Command, NULL);
	//}
}
void CServerCommandManager::GameProgress(const float& _DeltaTime)
{
	size_t Count = m_vecCommendObj.size();


	ServerCommand* Command = nullptr;

	for (size_t i = 0; i < Count; i++)
	{
		Command = m_vecCommendObj[i].CommendBuffer.Pop();
		if (nullptr == Command) 
		{
			TASSERT(true);
			continue;
		}

		Command->Flag = COMMAND_GAME_PROCESSING;
		Command->DeltaTime = _DeltaTime;
		PostQueuedCompletionStatus(m_vecCommendObj[i].hIOCP, sizeof(ServerCommand), (ULONG_PTR)Command, NULL);
	}
}