#pragma once
#include <Scene.h>
#include <DeadByDaylightGlobal.h>



class CKillerLobbyScene : public CSceneScript
{
private:
	static Vec3													KillerLobbySurvivorPosition[4];
	static Vec3													KillerLobbySurvivorUIPosition[4];

private:
	bool														m_bReady;
	int															m_UserCount;

	LobbyPlayer													m_Killer;
	std::queue<LobbyPlayer>										m_quSurvivor;
	std::list<LobbyPlayer>										m_listRemoteSurvivor;

	
	SPTR<CUIBase>												m_RoomNumUI[MAX_ROOM_NUMBER_UI];
	SPTR<CUIBase>												m_RoomUserUI[MAX_USER_COUNT_UI];
	SPTR<CUIBase>												m_RoomButton[MAX_ROOM_BUTTON_UI];

private:
	std::wstring TreeTypeName[TREETYPEMAX];

public:
	void WakeUp()override;
	const bool Loading() override;

	void DebugRender()override;
public:
	void PreUpdate(const float& _DeltaTime) override;
	void PostUpdate(const float& _DeltaTime) override;
	void LastUpdate(const float& _DeltaTime) override;

public:
	void ReadyEnter();
	void ReadyStay();
	void ReadyExit();

	void BackEnter();
	void BackStay();
	void BackExit();


public:
	void EnterUser(const unsigned __int64& _ID);
	void ReadyUser(const unsigned __int64& _ID);
	void ReadyCancleUser(const unsigned __int64& _ID);
	void ExitUser(const unsigned __int64& _ID);
	void BuildGameWorld(const WorldData& _Data);

public:
	void InitializeLobby();

public:
	CKillerLobbyScene(CScene* _PairScene);
	~CKillerLobbyScene();
};

