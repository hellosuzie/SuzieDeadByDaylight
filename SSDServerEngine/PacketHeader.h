#pragma once
#define PACKET_SIZE 1024
#define PACKET_PAYLOAD 1024 
#define PACKET_DOUDLESIZE 2048
#define STREAM_SIZE 8192

#define EOR 0xdededede

enum HEADER_FLAG
{
	FLAG_PING  = 1,
	FLAG_PONG,
	FLAG_CONNECT,
	FLAG_DISCONNECT,
	FLAG_ENTER_ROOM,
	FLAG_ENTER_ROOM_RANDOM,
	FLAG_ENTER_TEST_ROOM = FLAG_ENTER_ROOM_RANDOM,	//  접속 절차가 없는 테스트용 룸으로 입장
	FLAG_ENTER_LOBBY_ROOM,							//  접속 절차 통한 룸 입장
	FLAG_ENTER_ROOM_SUCCESS,						//	처음 게임 월드(서버 씬에 배치된 순간)에 접속했을때의 플레그 값
	FLAG_ENTER_ROOM_FAIL,							//  게임 월드 배치에 실패한경우
	FLAG_LOBBY_WAITING,								//  게임 로비에서 대기 중일때
	FLAG_GAME_PLAYING,								//  기존에 게임 월드에서 플레이 중일때
	FLAG_SCENE_BUILD_COMPLETE,						//  클라이언트가 서버로부터 패킷을 받을 수 잇는 상태가 되었음을 알리는 플레그 값
	FLAG_REQUEST_ROOM_DATA,							//  룸에 접속한 유저들이 서버로 데이터를 요구하는 플레그 값
	FLAG_GAME_EXIT_USER,							//  게임에서 다른 유저가 나간경우
	FLAG_REQUEST_ROOM_LIST,							//  현재 활성화 중인 방정보를 요청
	FLAG_PLAYING_RULE_SIGNAL,						//  인게임 중의 각 클라이언트의 Rule 객체에게 전송하는 정보
	MAX_HEADER_FLAG,
	FLAG_LOGICDATA =128,
	FLAG_DATA_ROOM_WORLD,
	FLAG_DATA_SYNCRO_WORLD,							// 강제로 플레이어들의 기하 상태를 동기화 시키는 데이터
	FLAG_DATA_ENTER_SURVIVOR,
	FLAG_DATA_ENTER_KILLER,
	FLAG_DATA_TRANSLATE,
	FLAG_DATA_COLLISION,
	FLAG_DATA_LOCALPLAYER_STATE,
	FLAG_DATA_BROADCATE_REMOTE,
	FLAG_DATA_ENTER_TEST,
	FLAG_DATA_ROOMENTER_USER,
	FLAG_DATA_READY,
	FLAG_DATA_READY_CANCLE,
	FLAG_DATA_ROOM_EXIT,
	FLAG_DATA_BUILD_GAMEWORLD,
	FLAG_DATA_WORLD_OBJECT,
	FLAG_DATA_WORLD_BUILD_COMPLETE,
	FLAG_DATA_RESCUEPLAYER,
	FLAG_DATA_HEALINGPLAYER,
	FLAG_DATA_SELFHEALING,
	FLAG_DATA_SUMMONSPLAYER,
	FLAG_DATA_HOOKPLAYER,
	FLAG_DATA_DISSOLVEPLAYER,
	FLAG_DATA_GANERATOR,
	FLAG_DATA_TRAP,
	FLAG_DATA_GANERATOR_COMPLETE,
	FLAG_DATA_HEALING_COMPLETE,
	FLAG_DATA_SELFHEALING_COMPLETE,
	FLAG_DATA_ACCHOOK,
	FLAG_DATA_RESCUED_COMPETE,
	FLAG_DATA_DEAD,
	FLAG_DATA_DEAD_OUT,
	FLAG_DATA_ESCAPE,
	FLAG_DATA_GAME_END,
	MAX_DATA_TYPE_FLAG
};


typedef struct _tagPACKETHEADER
{
	DWORD					Size;
	WORD					HeaderFlag;
	WORD					DataType;
}PacketHeader,PHeader,Header;


typedef struct _tagPACKET
{
	PacketHeader			Header;
	char					Packet[PACKET_SIZE];
	class CServerUser*		pUser;


	void operator=(const _tagPACKET& _Other) 
	{
		Header.Size = _Other.Header.Size;
		Header.HeaderFlag = _Other.Header.HeaderFlag;
		Header.DataType = _Other.Header.DataType;

		CopyMemory(Packet, _Other.Packet, sizeof(Packet));
	}
}PACKET;



template<typename DATA>
const DATA ExtractionData(const PACKET& _Pack)
{
	DATA ReturnData;

	CopyMemory(&ReturnData, _Pack.Packet, sizeof(DATA));
	return ReturnData;
}

// 가변 길이 패킷용 데이터 추출 함수
template<typename DATA>
const DATA ExtractionData(const PACKET& _Pack, const size_t& _Size)
{
	DATA ReturnData;

	CopyMemory(&ReturnData, _Pack.Packet, _Size);
	return ReturnData;
}


