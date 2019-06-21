#include "MiddleStruct.h"
#include "IOCPServerMgr.h"
#include "ServerCommendManager.h"




const PACKET MakePacket(void* _pData, const unsigned int& _iSize, const unsigned int& _HeaderFlag /*= 0*/, const unsigned int& _DataFlag /*= 0*/)
{
	PACKET NewPacket;

	memset(&NewPacket, 0, sizeof(PACKET));

	NewPacket.Header.Size = _iSize + sizeof(PacketHeader);
	NewPacket.Header.HeaderFlag = _HeaderFlag;
	NewPacket.Header.DataType = _DataFlag;

	if (_iSize != 0 && nullptr != _pData)
	{
		memcpy(NewPacket.Packet, _pData, _iSize);
	}

	return NewPacket;
}
const PACKET MakePacket(const unsigned int& _HeaderFlag, const unsigned int& _DataFlag) 
{
	PACKET NewPacket;

	memset(&NewPacket, 0, sizeof(PACKET));

	NewPacket.Header.Size = sizeof(PacketHeader);
	NewPacket.Header.HeaderFlag = _HeaderFlag;
	NewPacket.Header.DataType = _DataFlag;

	return NewPacket;
}

void _tagGAMEWORLDDATA::operator=(const _tagGAMEWORLDDATA& _Other)
{
	ZeroMemory(&arrPlayerData, sizeof(arrPlayerData));

	Header = _Other.Header;
	memcpy_s(&arrPlayerData, Header.DataSize, _Other.arrPlayerData, Header.DataSize);
}


///////////////////////////////////////////			Packet Processing Function			//////////////////////////////

Packet_Proc_Function						PacketProcessing::PacketProcFuntion[MAX_HEADER_FLAG] = { nullptr, };
IOCPServerMgr*								PacketProcessing::pIOCPServerManager = nullptr;

void PacketProcessing::Init(IOCPServerMgr* _pMgr)
{
	pIOCPServerManager = _pMgr;
	PacketProcFuntion[FLAG_PING] = PacketProcessing::Ping;
	PacketProcFuntion[FLAG_ENTER_ROOM] = PacketProcessing::EnterRoom;
	PacketProcFuntion[FLAG_ENTER_LOBBY_ROOM] = PacketProcessing::EnterLobbyRoom;
	PacketProcFuntion[FLAG_ENTER_TEST_ROOM] = PacketProcessing::EnterTestRoom;
	PacketProcFuntion[FLAG_SCENE_BUILD_COMPLETE] = PacketProcessing::ClientBuildComplete;
	PacketProcFuntion[FLAG_REQUEST_ROOM_LIST] = PacketProcessing::RoomList;
}


void PacketProcessing::Ping(const PACKET& _Pack)
{
	if (nullptr == _Pack.pUser)
		return;

	_Pack.pUser->ResetCriticalTime();
	pIOCPServerManager->PrintLog(LogManager::MakeLogString("Ping  Client IP : ", _Pack.pUser->GetRemoteIPAddressString()));
}

void  PacketProcessing::EnterRoom(const PACKET& _Pack)
{
	RoomEnterData Data;

	//Data = ExtractionData<RoomEnterData>(_Pack);
	memcpy(&Data, _Pack.Packet, sizeof(RoomEnterData));
	Data.pEnterUser = _Pack.pUser;

	unsigned int EnterRoomNum = pIOCPServerManager->pCommandMgr->EnterRoom(Data);
	if (-1 == EnterRoomNum)
	{
		Data.pEnterUser->SendData(nullptr, 0, FLAG_ENTER_ROOM_FAIL);
	}
	else 
	{
		LobbyEnterData LData;
		LData.EnterRData = pIOCPServerManager->pCommandMgr->m_vecCommendObj[EnterRoomNum].pRoom->GetRoomData();
		LData.LobbyID = pIOCPServerManager->pCommandMgr->m_vecCommendObj[EnterRoomNum].pRoom->GetPlayerID(_Pack.pUser);
		_Pack.pUser->SendData(&LData,sizeof(LData) ,FLAG_ENTER_ROOM_SUCCESS);		
	}
}
void _stdcall PacketProcessing::EnterLobbyRoom(const PACKET& _Pack)
{
	RoomEnterData Data;

	//memcpy(&Data, _Pack.Packet, sizeof(RoomEnterData));
	Data = ExtractionData<RoomEnterData>(_Pack);
	Data.pEnterUser = _Pack.pUser;

	unsigned int EnterRoomNum;
	if (Data.RoleType == PLAYER_KILLER) 
	{
		EnterRoomNum = pIOCPServerManager->pCommandMgr->EnterRoom(Data.pEnterUser, pIOCPServerManager->pCommandMgr->FindNoMasterRoom());
	}
	else 
	{
		EnterRoomNum = pIOCPServerManager->pCommandMgr->EnterRoom(Data.pEnterUser, Data.RoomNumber);
	}

	if (-1 == EnterRoomNum)
	{
		Data.pEnterUser->SendData(nullptr, 0, FLAG_ENTER_ROOM_FAIL);
	}
	else
	{
		LobbyEnterData LData;
		LData.EnterRData = pIOCPServerManager->pCommandMgr->m_vecCommendObj[EnterRoomNum].pRoom->GetRoomData();
		LData.LobbyID = pIOCPServerManager->pCommandMgr->m_vecCommendObj[EnterRoomNum].pRoom->GetPlayerID(_Pack.pUser);
		_Pack.pUser->SendData(&LData, sizeof(LData), FLAG_ENTER_ROOM_SUCCESS);
		pIOCPServerManager->pCommandMgr->m_vecCommendObj[EnterRoomNum].pRoom->BroadcateLobbyEnterUser(_Pack.pUser);

	}
}

void PacketProcessing::EnterTestRoom(const PACKET& _Pack)
{
	RoomEnterData Data;

	Data = ExtractionData<RoomEnterData>(_Pack);
	Data.pEnterUser = _Pack.pUser;

	if (false  == pIOCPServerManager->pCommandMgr->m_vecCommendObj[0].pRoom->EnterRoom(Data))
	{
		Data.pEnterUser->SendData(nullptr, 0, FLAG_ENTER_ROOM_FAIL);
	}
}
void PacketProcessing::ClientBuildComplete(const PACKET& _Pack)
{
	pIOCPServerManager->pCommandMgr->BuildCompleteEvent(_Pack.pUser);
}
void PacketProcessing::RoomList(const PACKET& _Pack)
{
	pIOCPServerManager->SendActiveRoomList(_Pack.pUser);
}