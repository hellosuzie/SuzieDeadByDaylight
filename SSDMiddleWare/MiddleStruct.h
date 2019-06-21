#pragma once
#include <SSDServerCore.h>
#include <DXHeader.h>
#include "MiddleEnum.h"
#include "MiddleMacro.h"


typedef struct _tagLOGDATA
{
	LOG_FLAG		Flag;
	char			StrBuffer[MAXLOGSTRING];
	size_t			StrSize;

public:
	~_tagLOGDATA() {}
}LogData;

typedef struct _tagROOMENTERDATA 
{
	unsigned int			RoomNumber;
	PLAYER_ROLE_TYPE		RoleType;
	CServerUser*			pEnterUser;


	_tagROOMENTERDATA() 
		:RoomNumber(-1),RoleType(MAX_PLAYER_ROLE_TYPE), pEnterUser(nullptr)
	{}
	~_tagROOMENTERDATA() 
	{}
}RoomEnterData;

typedef struct _tagACTIVEROOMHEADER
{
	unsigned int				DataSize;
	unsigned int				ActiveRoomCount;
}ActiveRoomHeader;

typedef struct _tagACTIVEROOMDATA
{
	unsigned int			RoomNumber;
	unsigned int			RoomUserCount;
	bool					bGamePlaying;
	unsigned __int64		LobbyUserID[MAXROOMUSER];
}ActiveRoomData;



typedef struct _tagACTIVEROOMLIST
{
	ActiveRoomHeader			Header;
	ActiveRoomData				ArrRoomData[MAXACCESSROOMCOUNT];

	_tagACTIVEROOMLIST()
	{
		ZeroMemory(&Header, sizeof(Header));
		ZeroMemory(ArrRoomData, sizeof(ArrRoomData));
	}
	~_tagACTIVEROOMLIST()
	{}
}RoomList;


typedef struct _tagLOBBYENTERDATA
{
	ActiveRoomData			EnterRData;
	unsigned __int64		LobbyID;

}LobbyEnterData;



typedef struct _tagPLAYERINITDATA
{
	bool					ServerPlayer;

}PlayerInitData;


typedef struct _tagPLAYERDATA 
{
	unsigned __int64		PlayerID;
	int						PlayerRole;
	int						PlayerState;
	Vec3					Pos;
	Vec3					Rot;
	unsigned int			Count;

public:
	_tagPLAYERDATA() 
		:PlayerID(0),Pos(Vec3()), Rot(Vec3()),PlayerRole(0), PlayerState(0), Count(0)
	{}
	~_tagPLAYERDATA(){}
}PlayerData;


typedef struct _tagGAMEWORLDHEADER 
{
	unsigned int				DataSize;
	unsigned int				CurPlayerCount;
	// 클라이언트들의 자신들의 로컬 플레이어 데이터를 알기 위한 해더값
	unsigned __int64			LocalPlayerID;


	_tagGAMEWORLDHEADER() : DataSize(0) , CurPlayerCount(0) , LocalPlayerID(0){}
	~_tagGAMEWORLDHEADER() {}
}WorldDataHeader;


typedef struct _tagGAMEWORLDDATA 
{
	WorldDataHeader				Header;
	PlayerData					arrPlayerData[MAXROOMUSER];

public:
	void operator=(const _tagGAMEWORLDDATA& _Other);

	_tagGAMEWORLDDATA()
		: Header(WorldDataHeader())
	{}
	~_tagGAMEWORLDDATA() {}


}WorldData;

typedef struct _tagMAGICCIRCLEDATA 
{
	int						Type;
	int						ID;
	Vec3					InitPos;
	float					InitRotY;

	_tagMAGICCIRCLEDATA() 
		:Type(-1), ID(-1),InitPos(Vec3()),InitRotY(0.f)
	{}
	~_tagMAGICCIRCLEDATA(){}
}MagicCircleData;

typedef struct _tagWORLDOBJECTHEADER
{
	unsigned int				DataSize;
	unsigned int				ObjCount;
	bool						LastData;

	_tagWORLDOBJECTHEADER() 
		 : DataSize(0),ObjCount(0),LastData(false)
	{}
	~_tagWORLDOBJECTHEADER() {}
} WorldObjHeader;

typedef struct _tagWORLDOBJECTDATA
{
	WorldObjHeader					Header;
	MagicCircleData					arrCircleData[MAXMAGICCIRCLE];

} WorldObjData;

typedef struct _tagLOBBYPLAYER 
{
	class CDissolveSkeletalRenderer*					LobbyRen;
	unsigned __int64									LobbyID;
	UINT												Idx;
	class  CUIBase*										ReadySignUI;

	_tagLOBBYPLAYER() 
		:LobbyRen(nullptr),LobbyID(0), Idx(0), ReadySignUI(nullptr)
	{}
	~_tagLOBBYPLAYER() {}
}LobbyPlayer;

// 로컬 플레이어들의 키보드 & 마우스 입력에따른 신호
typedef struct _tagTERMINALSIGNAL 
{
	unsigned __int64									PlayerID;
	unsigned __int64									TargetID;
}TerminalSignal;


typedef struct _tagCIRCLRDATA 
{
	unsigned __int64									PlayerID;
	unsigned int										CircleID;
	union 
	{
		float											Repair;
		unsigned int									CircleCount;
		int												Flag;
	}Value;
	//float												Repair;
}CircleData;

typedef struct _tagPLAYERSIGNAL
{
	unsigned __int64									PlayerID;
	unsigned __int64									TargetID;
	union 
	{
		float			DeltaHeal;
		float			Heal;
		float			Summons;
		float			Hook;
		unsigned int	Life;
		unsigned int	Death;

	}Value;
	
}PlayerSignal;

class IOCPServerMgr;
typedef void  (_stdcall*   Packet_Proc_Function  )(const PACKET& _Pack);

class PacketProcessing 
{
public:
	static IOCPServerMgr*								pIOCPServerManager;
	static Packet_Proc_Function							PacketProcFuntion[MAX_HEADER_FLAG];

public:
	static void Init(IOCPServerMgr* _pMgr);

	
public:
	static void _stdcall Ping(const PACKET& _Pack);
	static void _stdcall EnterRoom(const PACKET& _Pack);
	static void _stdcall EnterLobbyRoom(const PACKET& _Pack);
	static void _stdcall EnterTestRoom(const PACKET& _Pack);
	static void _stdcall ClientBuildComplete(const PACKET& _Pack);
	static void _stdcall RoomList(const PACKET& _Pack);
	


private:
	PacketProcessing() {};
	virtual ~PacketProcessing() = 0;
};


template<typename DATA>
const _tagPACKET MakePacket(const DATA& _Data, const unsigned int& _HeaderFlag, const unsigned int& _DataFlag)
{
	_tagPACKET NewPacket;
	ZeroMemory(&NewPacket, sizeof(_tagPACKET));

	NewPacket.Header.Size = sizeof(DATA) + sizeof(PacketHeader);
	NewPacket.Header.HeaderFlag = _HeaderFlag;
	NewPacket.Header.DataType = _DataFlag;

	CopyMemory(NewPacket.Packet, &_Data, sizeof(DATA));

	return NewPacket;
}

const PACKET MakePacket(void* _pData, const unsigned int& _iSize, const unsigned int& _HeaderFlag , const unsigned int& _DataFlag );
const PACKET MakePacket( const unsigned int& _HeaderFlag, const unsigned int& _DataFlag);



