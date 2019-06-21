#pragma once
#include <Script.h>
#include <unordered_map>
#include "MiddleStruct.h"

#define MAXHOOK 4
#define MAXGENCIRCLE 2

enum RULE_MODE 
{
	SERVER_RULE,
	CLIENT_RULE,
	MAX_RULE_MODE,
};

enum MAINSCENE_UI
{
	MAINUI_GENERATOR_HEALINGKIT,
	MAINUI_GENERATOR_COUNT,
	MAINUI_LIFECOUNT,
	MAINUI_PERK,
	MAINUI_PLAYER1_STATE,
	MAINUI_PLAYER2_STATE,
	MAINUI_PLAYER3_STATE,
	MAINUI_PLAYER4_STATE,
	MAINUI_GAUGE_FRAME,
	MAINUI_CUR_GAUGE,
	MAINUI_ACTION_GUIDE,
	MAINSCENE_UI,
};

enum EVENTCAMERA 
{
	EVENT_CAMERA1,
	EVENT_CAMERA2,
	MAX_EVENT_CAMERA,
};
class CMagicCircle;
class CPlayer;
class CTerrain;
class CDoor;
class CDBDRule : public CScript
{
private:
	friend class CRoom;
private:
	CCamera*																	m_pEventCamera[MAX_EVENT_CAMERA];
	EVENTCAMERA																	m_CurEventCam;

	RULE_MODE																	m_eRuleMode;
	CPlayer*																	m_pLocalPlayer;
	CTerrain*																	m_pSceneTerrain;
	std::unordered_map<unsigned __int64, CPlayer*>								m_mapPlayer;
	std::unordered_map<unsigned __int64, CPlayer*>::iterator					m_mapPlayerIter;
	std::unordered_map<unsigned __int64, CPlayer*>::iterator					m_mapPlayerEndIter;
	std::list<CPlayer*>															m_listPlayer;
	std::list<class CHook*>														m_listHook;
	std::vector<CMagicCircle*>													m_vecMagicCircle;

private:
	UINT																		m_CircleCount;

private:
	SPTR<class CUIBase>															m_SceneUI[MAINSCENE_UI];
	bool																		m_bEscapeEvent;
	float																		m_fAccEventTime;

	unsigned int																m_GrantSurvivorID;


private:
	CDoor*																		m_EscapeDoor;
	bool																		m_bGameEnd;
	float																		m_AccEndTime;

public:
	const bool Init()override;
	const bool Init(const RULE_MODE& _Mode);
	void PreUpdate(const float& _DeltaTime)override;
	void Update(const float& _DeltaTime)override;

public:
	const bool UpdatePlayer(const PlayerData& _RecvData);
	void UpdateGernerator(const CircleData& _RecvData);
	void UpdateTrap(const CircleData& _RecvData);
	void UpdatePlayer(const PlayerSignal& _Signal, const int& _DataFlag);

public:
	void InitializeSceneProcessing(WorldData* _InitData);
	void InitializeSceneProcessing_Test(WorldData* _InitData);
	void RoomDataProcessing(const struct _tagPACKET& _Pack);
	void SyncroWorldProcessing(const struct _tagPACKET& _Pack);
	void ExitPlayerProcessing(const _tagPACKET& _Pack);
	void RemotePlayerProcessing(const _tagPACKET& _Pack);
	void RulePacketProcessing(const _tagPACKET& _Pack);
	void CollsionPacketProcessing(const _tagPACKET& _Pack);

public:
	void SetTerrain(CTerrain* _pTerrain);

public:
	CHook* PopHook() 
	{
		if (true == m_listHook.empty())
			return nullptr;

		CHook* Front =  m_listHook.front();
		m_listHook.pop_front();
		return Front;
	}
	void PushHook(CHook* _pHook)
	{
		std::list<CHook*>::iterator Start = m_listHook.begin();
		std::list<CHook*>::iterator End = m_listHook.end();
		for (; Start != End; ++Start)
		{
			if (*Start == _pHook)
			{
				return;
			}
		}

		m_listHook.push_back(_pHook);
	}

private:
	CPlayer* CreateServerPlayer(const RoomEnterData& _EnterData);
	CPlayer* CreatePlayer(const PlayerData& _Data, const bool& _Local = false,const bool& _Test = false);
	void InsertPlayer(CPlayer* _pPlayer);
	void DeletePlayer(CPlayer* _pPlayer);

public:
	CMagicCircle* CreateServerMagicCircle(class CActor* _Actor,const int& _Type,const Vec3& _Pos, const float _Rot);
	CMagicCircle* CreateMagicCircle(class CActor* _Actor,const MagicCircleData& _RecvData);
	void InitWorldObj();


public:
	void CreateSceneUI();

public:
	void DebugRender() override;

public:
	void ChangeFocusingObjEvent_Survivor(CPlayer* _pLocalPlayer);
	void ChangeFocusingObjEvent_Killer(CPlayer* _pLocalPlayer);

public:
	void MainGaugeOn();
	void MainGaugeOff();
	void SetMainGauge(const float& _Gauge);

public:
	void GeneratorCompleteEvent(const unsigned int& _ID);
	void AllGeneratorCompleteEvent();

public:
	void PushEventCamera(CCamera* _Cam1, CCamera* _Cam2);
	void UpdateLiftUI(CPlayer* _LocalPlayer);
	void UpdateSurvivorUI(CPlayer* _SurvivorPlayer);

public:
	void CreateEscapeDoor();
	void TriggerGameEnd();

public:
	CDBDRule();
	~CDBDRule();
};

