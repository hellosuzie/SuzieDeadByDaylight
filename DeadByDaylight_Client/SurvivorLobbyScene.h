#pragma once
#include <Scene.h>
#include <DeadByDaylightGlobal.h>

class CUIBase;
class CSurvivorLobbyScene : public CSceneScript
{
	static Vec3													SurvivorLobbyPos[4];
	static Vec3													SurvivorLobbyUIPos[4];
	static Vec3													SurvivorLobbyKillerUIPos;

private:
	std::wstring												TreeTypeName[TREETYPEMAX];
	bool														m_bReady;
	SPTR<CUIBase>												m_ReadyUI;

	SPTR<CUIBase>												m_KillerReadyUI;
	unsigned __int64											m_KillerID;
	unsigned int												m_UserCount;
	
	LobbyPlayer													m_LocalSurvivor;
	std::queue<LobbyPlayer>										m_quSurvivor;
	std::list<LobbyPlayer>										m_listRemoteSurvivor;

	SPTR<CUIBase>												m_RoomNumUI[MAX_ROOM_NUMBER_UI];
	SPTR<CUIBase>												m_RoomUserUI[MAX_USER_COUNT_UI];
	//SPTR<CUIBase>												m_RoomButton[MAX_ROOM_BUTTON_UI];

public:
	void WakeUp()override;
	const bool Loading() override;
public:
	void PreUpdate(const float& _DeltaTime) override;
	void PostUpdate(const float& _DeltaTime) override;
	void LastUpdate(const float& _DeltaTime) override;
	void DebugRender()override;

public:
	void ReadyEnter();
	void ReadyStay();
	void ReadyExit();


public:
	void EnterUser(const unsigned __int64& _ID);
	void ReadyUser(const unsigned __int64& _ID);
	void ReadyCancleUser(const unsigned __int64& _ID);
	void ExitUser(const unsigned __int64& _ID);
	void BuildGameWorld(const WorldData& _Data);

public:
	void InitializeLobby();

public:
	CSurvivorLobbyScene(CScene* _PairScene);
	~CSurvivorLobbyScene();
};

