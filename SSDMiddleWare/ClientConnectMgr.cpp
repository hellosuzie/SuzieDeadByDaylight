#include <SSDCore.h>
#include "DeadByDaylightGlobal.h"
#include "ClientConnectMgr.h"
#include "MiddleStruct.h"
#include "Player.h"
#include <SSDThread.h>
#include <TimerMgr.h>


CPacketQueue			CClientConnectMgr::ClientPacketQueue;
float					CClientConnectMgr::fPacketRecvTime = 0.f;

void CClientConnectMgr::Init()
{
	ClientPacketQueue.Init();

}

unsigned int _stdcall CClientConnectMgr::PacketRecvProcess(CNetworkClient* _Arg)
{
	if (false == _Arg->IsReceivePacket())
		return 0;

	bool Check = false;

	Check = ClientPacketQueue.AddData(_Arg->m_RecvBuffer.m_Packet, _Arg->m_RecvBuffer.m_PacketSize);
	
	if (Check == false)
		return  -1;

	Check  = ClientPacketQueue.GetData(_Arg);

	if (Check == false)
		return  -1;

	return 0;
}

CClientConnectMgr::CClientConnectMgr()
	:CServerConnectMgr(), m_bCompleteConnect(false)
	, m_fPingAccTime(0.f), m_bRoomEnter(false), m_RoomDataRequestCount(0)
	, m_ClientLocalPlayer(nullptr), m_Receiver(CClientReceiver()), m_Sender(CClientSender())
	, m_LogMgr(CClientLogMgr()), m_fPing(0.f), pPingUpdate(nullptr), m_Session(CClientSession())
	, m_fMaxPing(0.f), m_SendCount(0)
	//, m_fMaxPing(0.f)
{

}


CClientConnectMgr::~CClientConnectMgr()
{
	
}

void CClientConnectMgr::SetLocalPlayer(CPlayer* _pPlayer)
{
	if (nullptr == _pPlayer)
		return;

	m_ClientLocalPlayer = _pPlayer;
}
const bool CClientConnectMgr::ExistPacket()
{
	return !ClientPacketQueue.Empty();
}
const bool CClientConnectMgr::EmptyPacketQueue()
{
	return ClientPacketQueue.Empty();
}
const bool CClientConnectMgr::GetPacket(PACKET& _Packet)
{
	return ClientPacketQueue.Pop(&_Packet);
}
void CClientConnectMgr::ClearPacketQueue()
{
	ClientPacketQueue.Clear();
}
void CClientConnectMgr::ConnectLogicServer()
{
	pLogicClient = new NetClient();

	TryServerConnect();	
}

const int CClientConnectMgr::ReceiveRoomPacket(WorldData* _BuildData)
{
	if (nullptr == _BuildData)
		return 0;

	PACKET Pack;

	while (!ClientPacketQueue.Empty())
	{
		ClientPacketQueue.Pop(&Pack);

		if (Pack.Header.HeaderFlag == FLAG_ENTER_ROOM_FAIL)
			return FLAG_ENTER_ROOM_FAIL;
		else if (Pack.Header.HeaderFlag == FLAG_ENTER_ROOM_SUCCESS) 
		{
			// 씬 빌드 데이터를 받아온다.
			unsigned int* DataSize = (unsigned int*)(Pack.Packet);
			
			memcpy_s(_BuildData, *DataSize, Pack.Packet, *DataSize);
			return FLAG_ENTER_ROOM_SUCCESS;
		}
	}

	return 0;
}

void CClientConnectMgr::ReceivePacket(const bool _Receive)
{
	pLogicClient->ReceivePacket(_Receive);
}
void CClientConnectMgr::SendPlayerState(const float& _DeltaTime)
{
	if (nullptr != m_ClientLocalPlayer && true == m_ClientLocalPlayer->IsBroadFlag()) 
	{
		PlayerData PData = m_ClientLocalPlayer->GetPlayerData();
		PData.Count = m_SendCount++;
		m_Sender.SendPacket(MakePacket(PData, FLAG_GAME_PLAYING, FLAG_DATA_LOCALPLAYER_STATE));
		m_ClientLocalPlayer->InitBroadFlag();
	}


}

void CClientConnectMgr::RequestEnterRoom(const PLAYER_ROLE_TYPE& _Type, const unsigned int& _Room /*= -1*/)
{
	RoomEnterData EnterData;
	EnterData.RoleType = _Type;
	EnterData.RoomNumber = _Room;

	m_Sender.SendPacket(MakePacket(EnterData, FLAG_ENTER_ROOM, 0));
}

void CClientConnectMgr::RequestEnterLobbyRoom(const PLAYER_ROLE_TYPE& _Type, const unsigned int& _Room /*= -1*/)
{
	RoomEnterData EnterData;
	EnterData.RoleType = _Type;
	EnterData.RoomNumber = _Room;

	m_Sender.SendPacket(MakePacket(EnterData, FLAG_ENTER_LOBBY_ROOM, 0));
}



void CClientConnectMgr::RequestEnterTestRoom()
{
	RoomEnterData EnterData;
	EnterData.RoomNumber = 0;
	EnterData.RoleType = PLAYER_SURVIVOR;

	m_Sender.SendPacket(MakePacket(EnterData, FLAG_ENTER_TEST_ROOM, 0));
}
void CClientConnectMgr::NotifyBuildComplete()
{
	m_Sender.SendPacket(MakePacket(FLAG_SCENE_BUILD_COMPLETE, 0));
}
void CClientConnectMgr::RequestRoomData()
{
	m_Sender.SendPacket(MakePacket(FLAG_REQUEST_ROOM_DATA,0));
}
void CClientConnectMgr::RequestActiveRoomList()
{
	m_Sender.SendPacket(MakePacket(FLAG_REQUEST_ROOM_LIST, 0));
}
void CClientConnectMgr::NotifyReadyGame() 
{
	m_Sender.SendPacket(MakePacket(FLAG_LOBBY_WAITING, FLAG_DATA_READY));
}
void CClientConnectMgr::NotifyCancleReady() 
{
	m_Sender.SendPacket(MakePacket(FLAG_LOBBY_WAITING, FLAG_DATA_READY_CANCLE));
}
void CClientConnectMgr::NotifyLobbyExit() 
{
	m_Sender.SendPacket(MakePacket(FLAG_LOBBY_WAITING, FLAG_DATA_ROOM_EXIT));
}
void CClientConnectMgr::NotifyWorldBuildComplete()
{
	m_Sender.SendPacket(MakePacket(FLAG_LOBBY_WAITING, FLAG_DATA_WORLD_BUILD_COMPLETE));
}
void CClientConnectMgr::RequestSceneObject()
{
	m_Sender.SendPacket(MakePacket(FLAG_LOBBY_WAITING, FLAG_DATA_WORLD_OBJECT));
}
void CClientConnectMgr::TryServerConnect()
{
	if (false == pLogicClient->IsConnect()) 
	{
		if (true == pLogicClient->Connect(LOGICPORT, SERVER_IP))
		{
			//pLogicClient->PacketProgressStart(CClientConnectMgr::PacketRecvProcess);
			m_LogMgr.Init();
			m_Receiver.Init();
			m_Sender.Init();
			
			m_LogMgr.PrintLog(CClientLogMgr::MakeLogString("Wait Complete Connect"));
			m_Sender.SendPacket(MakePacket(FLAG_CONNECT , 0));
			pLogicClient->RecvPacket();
		}
	}
	else 
	{
		m_LogMgr.PrintLog(CClientLogMgr::MakeLogString("Try Complete Server Connect"));
		m_Sender.SendPacket(MakePacket(FLAG_CONNECT,0));
	}

}

void CClientConnectMgr::CheckConnectServer(const float& _DeltaTime)
{
	if (true == m_bCompleteConnect)
		return;

	if (m_fPingAccTime >= TRYCONNECTTIME)
	{
		TryServerConnect();
		m_fPingAccTime = 0.f;
	}

	PACKET Pack;
	ZeroMemory(&Pack, sizeof(Pack));

	while (!ClientPacketQueue.Empty()) 
	{
		ClientPacketQueue.Pop(&Pack);

		if (Pack.Header.HeaderFlag == FLAG_CONNECT)
		{
			m_bCompleteConnect = true;
			ClientPacketQueue.Clear();
			m_fPingAccTime = 0.f;
			return;
		}
	}

	m_fPingAccTime += _DeltaTime;
}


void CClientConnectMgr::DisConnectLogicServer()
{
	m_Receiver.Release();
	m_Sender.Release();
	m_Session.Release();
	m_LogMgr.Release();

	CServerConnectMgr::DisConnectLogicServer();
}

void CClientConnectMgr::OperationSessionThread()
{
	m_Session.Init();
}
void CClientConnectMgr::SendPingEvent()
{
	m_Session.SendPingEvent();
}
void CClientConnectMgr::RecvPongEvent()
{
	m_Session.RecvPongEvent();
}
void  CClientConnectMgr::RecordPing(const float& _Ping)
{
	m_fPing = _Ping;
	m_fMaxPing = (m_fMaxPing < m_fPing) ? m_fPing : m_fMaxPing;
}