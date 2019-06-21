#pragma once
#include <ThreadMaker.h>
#include <IOCP.h>
#include "Room.h"
#include <SyncContainer.h>

typedef struct _tagCOMMAND 
{
	COMMAND_FLAG		Flag;
	float				DeltaTime;
	CServerUser*		pExitUser;
}ServerCommand;

typedef struct _tagCOMMANDOBJECT 
{
	bool							bProgress;
	CIOCP*							pIOCP;
	HANDLE							hIOCP;
	CRoom*							pRoom;
	class IOCPServerMgr*			pIOCPManager;
	CSyncDeque<ServerCommand>		CommendBuffer;

public:
	_tagCOMMANDOBJECT()
		: bProgress(true),pIOCP(nullptr), pRoom(nullptr), pIOCPManager(nullptr), hIOCP(NULL)
	{}
	~_tagCOMMANDOBJECT() {}
}CommandObj;

typedef void(_stdcall*   Command_Proc_Function)(void* _pArg,const float& _DeltaTime);

typedef class CServerCommandManager : public CThreadMaker
{
	static Command_Proc_Function			CommandProcFuntion[MAX_COMMAND_FLAG];
	static CPacketQueue						LobbyPacketQueue;

public:
	static unsigned int _stdcall LobbyThreadProcess(void* _Arg);
	static unsigned int _stdcall TestRoomThreadProcess(void* _Arg);
	static unsigned int _stdcall RoomThreadProcess(void* _Arg);
	static void _stdcall LobbyProcessing(void* _pArg, const float& _DeltaTime);
	static void _stdcall PacketProcessing(void* _pArg, const float& _DeltaTime);
	static void _stdcall GameProcessing(void* _pArg, const float& _DeltaTime);
	static void _stdcall ExitUser(void* _pArg, const float& _DeltaTime);

	friend class IOCPServerMgr;
	friend class CServerNetworkManager;
	friend class PacketProcessing;
	IOCPServerMgr*									m_pIOCPMgr;
	UINT											m_MaxRoomCount;

	CommandObj										m_LobbyObj;

	std::vector<CommandObj>							m_vecCommendObj;

private:
	void Init(const unsigned int& _ThreadCount);
	void Release();

private:
	const unsigned int FindNoMasterRoom();
	const int EnterRoom(const RoomEnterData& _EnterData);
	const int EnterRoom(CServerUser* _pUser,const int& _RoomNum);
	const bool  EnterTestRoom(CServerUser* _pUser);
	const int RandomMatching();
	void ExitRoom(CServerUser* _pUser);
	void BuildCompleteEvent(CServerUser* _pUser);
	void SendActiveRoomList(CServerUser* _pUser);

private:
	void RecvUserPacket(CServerUser* _pUser);
	void PacketProgress(const float& _DeltaTime);
	void GameProgress(const float& _DeltaTime);


public:
	CServerCommandManager();
	CServerCommandManager(IOCPServerMgr* _IOCPMgr);
	~CServerCommandManager();
}CommandManager;

