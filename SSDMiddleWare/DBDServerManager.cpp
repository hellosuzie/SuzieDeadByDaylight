#include "DBDServerManager.h"
#include <IOCPServer.h>
#include "Room.h"
#include "MiddleStruct.h"



DBDServerManager::DBDServerManager()
	:CServerManager(), m_iActivcRoomCount(0)
{
	CRoom::pServerManager = this;

	for (unsigned int i = 0; i < MAXROOMCOUNT; i++)
	{
		m_vecRoom.push_back(new CRoom());
		m_vecRoom[i]->SetRoomNumber(i);
		m_vecRoom[i]->CreateGameWorld();
	}

	m_vecRoom[0]->ActiveRoom();
	m_iActivcRoomCount = 1;
}


DBDServerManager::~DBDServerManager()
{
	for (size_t i = 0; i < MAXROOMCOUNT; i++)
	{
		DELETE_POINTER(m_vecRoom[i]);
	}

	CRoom::pServerManager = nullptr;
}


void DBDServerManager::Update(const float& _DeltaTime)
{
	CIOCPServer::PreUpdate(_DeltaTime);

	while (!MainPacketQueue.Empty())
	{
		PACKET Pack;
		MainPacketQueue.Pop(&Pack);

		CServerManager::PacketPreProcessing(Pack);
		PacketProcessing(_DeltaTime, Pack);
	}

	for (size_t i = 0; i < MAXROOMCOUNT; i++)
	{
		if (true == m_vecRoom[i]->IsActive())
			m_vecRoom[i]->Update(_DeltaTime);
	}

	CIOCPServer::PostUpdate(_DeltaTime);
}

void DBDServerManager::PacketProcessing(const float& _DeltaTime, const PACKET& _Pack)
{
	switch (_Pack.Header.HeaderFlag)
	{
	case FLAG_ENTER_ROOM:
	{
		RoomEnterData Data;
		memcpy(&Data, _Pack.Packet, sizeof(RoomEnterData));
		Data.pEnterUser = _Pack.pUser;

		if (-1 == EnterRoom(Data)) 
		{
			Data.pEnterUser->SendData(nullptr, 0, FLAG_ENTER_ROOM_FAIL);
		}
	}
	default:
		break;
	}
}
void DBDServerManager::PushPacket(CServerUser* _pUser, const unsigned int& _DataLength)
{
	if (-1 == _pUser->m_RoomNumber || _pUser->m_RoomNumber >= m_iActivcRoomCount)
	{
		MainPacketQueue.AddData(_pUser->m_RecvBuffer.m_Packet, _DataLength, _pUser);
		return;
	}


	m_vecRoom[_pUser->m_RoomNumber]->PushPacket(_pUser, _DataLength);
}
void DBDServerManager::PushPacket(CServerUser* _pUser)
{
	if (-1 == _pUser->m_RoomNumber || _pUser->m_RoomNumber >= m_iActivcRoomCount)
	{
		MainPacketQueue.PushUserPacket(_pUser);
		return;
	}


	m_vecRoom[_pUser->m_RoomNumber]->PushPacket(_pUser);
}
void DBDServerManager::DisConnectUser(CServerUser* _pUser)
{
	if (nullptr == _pUser)
		return;

	ExitRoom(_pUser);
}
const int DBDServerManager::EnterRoom(const RoomEnterData& _EnterData)
{
	if (-1 == _EnterData.RoomNumber) 
	{
		int RanRoom = RandomMatching();

		if (-1 == RanRoom)
			return -1;
		
		if (false == m_vecRoom[RanRoom]->EnterRoom(_EnterData)) 
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
		if (m_vecRoom[_EnterData.RoomNumber]->IsFullRoom())
		{
			return -1;
		}
		else 
		{
			if (false == m_vecRoom[_EnterData.RoomNumber]->EnterRoom(_EnterData)) 
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
const int DBDServerManager::RandomMatching()
{
	for (unsigned int i = 0;  i < m_iActivcRoomCount; ++i)
	{
		if (false == m_vecRoom[i]->IsFullRoom())
			return i;
	}

	if (m_iActivcRoomCount <= MAXROOMCOUNT) 
	{
		++m_iActivcRoomCount;
		m_vecRoom[m_iActivcRoomCount - 1]->ActiveRoom();
		return m_iActivcRoomCount - 1;
	}

	return -1;
}
void DBDServerManager::ExitRoom(CServerUser* _pUser)
{
	if (nullptr == _pUser || _pUser->m_RoomNumber >= MAXROOMCOUNT || false == m_vecRoom[_pUser->m_RoomNumber]->IsActive())
		return;

	m_vecRoom[_pUser->m_RoomNumber]->ExitRoom(_pUser);
}