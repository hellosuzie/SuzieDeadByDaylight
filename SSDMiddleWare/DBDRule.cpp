#include "DBDRule.h"
#include "DeadByDaylightGlobal.h"
#include <DXFont.h>
#include "Hook.h"
#include <Actor.h>
#include "MiddleStruct.h"
#include "Player.h"
#include <Scene.h>
#include "ClientConnectMgr.h"
#include <Terrain.h>
#include "PlayerFSM.h"
#include "KillerFSM.h"
#include <TransForm.h>
#include <SSDRandom.h>
#include "MagicCircle.h"
#include <SphereCollider.h>
#include "Door.h"
#include <SoundPlayer.h>
#include <Sound.h>
#include <ParticleRenderer.h>



CDBDRule::CDBDRule()
	:m_pLocalPlayer(nullptr), m_pSceneTerrain(nullptr), m_eRuleMode(MAX_RULE_MODE)
	, m_CircleCount(0), m_bEscapeEvent(false), m_GrantSurvivorID(MAINUI_PLAYER1_STATE)
	, m_EscapeDoor(nullptr), m_bGameEnd(false), m_AccEndTime(0.f)
{
	ZeroMemory(m_SceneUI, sizeof(m_SceneUI));
	ZeroMemory(m_pEventCamera, sizeof(m_pEventCamera));
}


CDBDRule::~CDBDRule()
{
}

const bool CDBDRule::Init()
{
	CHook* NewHook = nullptr;

	for (size_t i = 0; i < MAXHOOK; i++)
	{
		NewHook = CreateActor(UPDATE_LAYER_NORMAL, L"HookActor")->AddCom<CHook>();
		NewHook->ActorOff();
		NewHook->m_pRule = this;
		m_listHook.push_back(NewHook);
	}

	
	return CScript::Init();
}

const bool CDBDRule::Init(const RULE_MODE& _Mode)
{
	m_eRuleMode = _Mode;

	if (m_eRuleMode == SERVER_RULE) 
	{

	}
	else 
	{
		CHook* NewHook = nullptr;

		for (size_t i = 0; i < MAXHOOK; i++)
		{
			NewHook = CreateActor(UPDATE_LAYER_NORMAL, L"HookActor")->AddCom<CHook>();
			NewHook->ActorOff();
			NewHook->m_pRule = this;
			m_listHook.push_back(NewHook);
		}

		return CScript::Init();
	}

	return true;
}
void CDBDRule::PreUpdate(const float& _DeltaTime)
{
	if (true == m_bGameEnd) 
	{
		if (m_AccEndTime >= 4.f) 
		{
			CScene::ChangeScene(L"EndingScene");
			Off();
			return;
		}

		m_AccEndTime += _DeltaTime;
	}
}
void CDBDRule::Update(const float& _DeltaTime)
{
	if (m_bEscapeEvent == true) 
	{
		if (m_fAccEventTime >= 7.f) 
		{
			m_bEscapeEvent = false;
			m_fAccEventTime = 0.f;

			SCENE->ChangeMainCamera(m_pLocalPlayer->GetPlayerCamera());
			m_pLocalPlayer->GetPlayerCamera()->SetCameraBluring(2.5f, m_pEventCamera[m_CurEventCam], 3.f);
			m_EscapeDoor->OpenEscapeDoor();
		}

		m_fAccEventTime += _DeltaTime;
	}
}
const bool CDBDRule::UpdatePlayer(const PlayerData& _RecvData)
{
	m_mapPlayerIter = m_mapPlayer.find(_RecvData.PlayerID);

	if (m_mapPlayerEndIter == m_mapPlayerIter)
		return false;
	
	return m_mapPlayerIter->second->UpdatePlayer(_RecvData);
}

void CDBDRule::UpdatePlayer(const PlayerSignal& _Signal, const int& _DataFlag)
{
	m_mapPlayerIter = m_mapPlayer.find(_Signal.TargetID);

	if (m_mapPlayerEndIter == m_mapPlayerIter)
		return;

	switch (_DataFlag) 
	{
	case FLAG_DATA_HEALINGPLAYER:
	{
		PlayerSignal Signal;
		Signal.PlayerID = _Signal.PlayerID;
		Signal.TargetID = _Signal.TargetID;
		m_mapPlayerIter->second->m_HealingCount += _Signal.Value.DeltaHeal;
	
		if (m_mapPlayerIter->second->m_HealingCount >= MAXHEALINGCOUNT && m_mapPlayerIter->second->m_LifeCount < MAXSURVIVORLIFE)
		{
			m_mapPlayerIter->second->m_LifeCount += 1;
			m_mapPlayerIter->second->m_HealingCount = 0.f;
			Signal.Value.Life = m_mapPlayerIter->second->m_LifeCount;

			for (auto P : m_mapPlayer)
			{
				P.second->m_pPairUser->SendData(&Signal, sizeof(Signal), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_HEALING_COMPLETE);
			}
		}
		else 
		{
			Signal.Value.Heal = m_mapPlayerIter->second->m_HealingCount;

			for (auto P : m_mapPlayer)
			{
				P.second->m_pPairUser->SendData(&Signal, sizeof(Signal), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_HEALINGPLAYER);
			}
		}

	
	}
		break; 
	case FLAG_DATA_RESCUEPLAYER:
	{
		PlayerSignal Signal;
		Signal = _Signal;

		for (auto P : m_mapPlayer)
		{
			P.second->m_pPairUser->SendData(&Signal, sizeof(Signal), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_RESCUEPLAYER);
		}

	}
	break;
	case FLAG_DATA_SUMMONSPLAYER:
	{
		PlayerSignal Signal;
		Signal.PlayerID = _Signal.PlayerID;
		Signal.TargetID = _Signal.TargetID;
		m_mapPlayerIter = m_mapPlayer.find(Signal.PlayerID);

		if (m_mapPlayerIter == m_mapPlayerEndIter)
			return;

		if (m_mapPlayerIter->second->GetPlayerRole() == PLAYER_KILLER)
		{
			m_mapPlayerIter->second->m_pPlayerFSM->ChangePlayerState(PLAYER_SUMMONS);

			for (auto P : m_mapPlayer)
			{
				P.second->m_pPairUser->SendData(&Signal, sizeof(Signal), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_SUMMONSPLAYER);
			}
		}	
	}
	break;
	case FLAG_DATA_DISSOLVEPLAYER:
	{
		PlayerSignal Signal;
		Signal.PlayerID = _Signal.PlayerID;
		Signal.TargetID = _Signal.TargetID;
		m_mapPlayerIter = m_mapPlayer.find(Signal.TargetID);

		if (m_mapPlayerIter != m_mapPlayerEndIter && true == m_mapPlayerIter->second->IsGroggy()) 
		{
			for (auto P : m_mapPlayer)
			{
				P.second->m_pPairUser->SendData(&Signal, sizeof(Signal), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_DISSOLVEPLAYER);
			}
		}

	}
	break;
	case FLAG_DATA_HOOKPLAYER:
	{
		PlayerSignal Signal;
		Signal.PlayerID = _Signal.PlayerID;
		Signal.TargetID = _Signal.TargetID;
		m_mapPlayerIter = m_mapPlayer.find(Signal.TargetID);

		if (m_mapPlayerIter != m_mapPlayerEndIter && m_mapPlayerIter->second->m_LifeCount == 0)
		{
			m_mapPlayerIter->second->AddDeathCount();
			Signal.Value.Death = m_mapPlayerIter->second->m_DeathCount;

			for (auto P : m_mapPlayer)
			{
				P.second->m_pPairUser->SendData(&Signal, sizeof(Signal), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_HOOKPLAYER);
			}
		}

	}
	break;
	case FLAG_DATA_ACCHOOK:
	{
		PlayerSignal Signal;
		Signal.PlayerID = _Signal.PlayerID;
		Signal.TargetID = _Signal.TargetID;
		m_mapPlayerIter = m_mapPlayer.find(Signal.PlayerID);

		if (m_mapPlayerIter != m_mapPlayerEndIter)
		{
			m_mapPlayerIter->second->m_HookCount += _Signal.Value.Hook;
			
			if (m_mapPlayerIter->second->m_HookCount >= MAXHOOKCOUNT) 
			{
				m_mapPlayerIter->second->m_HookCount = MAXHOOKCOUNT;
				Signal.Value.Hook = MAXHOOKCOUNT;

				m_mapPlayerIter->second->m_pPlayerFSM->ChangePlayerState(PLAYER_DEAD);

				for (auto P : m_mapPlayer)
				{
					P.second->m_pPairUser->SendData(&Signal, sizeof(Signal), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_DEAD);
				}
			}
			else 
			{
				Signal.Value.Hook = m_mapPlayerIter->second->m_HookCount;

				for (auto P : m_mapPlayer)
				{
					P.second->m_pPairUser->SendData(&Signal, sizeof(Signal), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_ACCHOOK);
				}
			}

			
		}

	}
	break;
	case FLAG_DATA_RESCUED_COMPETE: 
	{
		PlayerSignal Signal;
		Signal.PlayerID = _Signal.PlayerID;
		Signal.TargetID = _Signal.TargetID;
		m_mapPlayerIter = m_mapPlayer.find(Signal.PlayerID);

		m_mapPlayerIter->second->m_LifeCount = 1;
		m_mapPlayerIter->second->m_pPlayerFSM->ChangePlayerState(PLAYER_IDLE);

		for (auto P : m_mapPlayer) 
		{
			P.second->m_pPairUser->SendData(&Signal, sizeof(Signal), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_RESCUED_COMPETE);
		}
	}
		break;
	case FLAG_DATA_DEAD_OUT:
	{
		PlayerSignal Signal;
		Signal.PlayerID = _Signal.PlayerID;
		Signal.TargetID = _Signal.TargetID;
		m_mapPlayerIter = m_mapPlayer.find(Signal.PlayerID);

		if (m_mapPlayerIter == m_mapPlayerEndIter)
			return;

		m_mapPlayerIter->second->m_bGamePlayEnd = true;
		int Count = 0;
		for (auto P : m_mapPlayer)
		{
			if (P.second->m_bGamePlayEnd == false)
			{
				++Count;
			}

			P.second->m_pPairUser->SendData(&Signal, sizeof(Signal), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_DEAD_OUT);
		}

		if (Count <= 1)
		{
			for (auto P : m_mapPlayer)
			{
				if (false == P.second->m_bGamePlayEnd)
				{
					P.second->m_pPairUser->SendData(&Signal, sizeof(Signal), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_GAME_END);
				}
			}
		}

	}
	break;
	case FLAG_DATA_ESCAPE:
	{
		PlayerSignal Signal;
		Signal.PlayerID = _Signal.PlayerID;
		Signal.TargetID = _Signal.TargetID;
		m_mapPlayerIter = m_mapPlayer.find(Signal.PlayerID);

		if (m_mapPlayerIter == m_mapPlayerEndIter)
			return;

		m_mapPlayerIter->second->m_bGamePlayEnd = true;
		int Count = 0;
		for (auto P : m_mapPlayer)
		{
			if (P.second->m_bGamePlayEnd == false)
			{
				++Count;
			}

			P.second->m_pPairUser->SendData(&Signal, sizeof(Signal), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_ESCAPE);
		}

		if (Count <= 1) 
		{
			for (auto P : m_mapPlayer)
			{
				if (false == P.second->m_bGamePlayEnd) 
				{
					P.second->m_pPairUser->SendData(&Signal, sizeof(Signal), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_GAME_END);
				}
			}
		}

	}
	break;
	}
}
void CDBDRule::UpdateGernerator(const CircleData& _RecvData) 
{
	if (m_vecMagicCircle.size() <= _RecvData.CircleID) 
	{
		return;
	}

	if (MC_GREEN == m_vecMagicCircle[_RecvData.CircleID]->GetMagicCircleColor()
		&& false == m_vecMagicCircle[_RecvData.CircleID]->CheckRepairComplete())
	{
		m_vecMagicCircle[_RecvData.CircleID]->RepairGenerator(_RecvData.Value.Repair);

		CircleData Data;
		Data.PlayerID = _RecvData.PlayerID;
		Data.CircleID = _RecvData.CircleID;
		Data.Value.Repair = m_vecMagicCircle[_RecvData.CircleID]->GetGeneratorCount();

		if (true == m_vecMagicCircle[_RecvData.CircleID]->IsMaximumGeneratorCount()) 
		{
			++m_CircleCount;
			Data.Value.CircleCount = m_CircleCount;
			m_vecMagicCircle[_RecvData.CircleID]->RepairCompleteEvent();
			for (auto P : m_mapPlayer)
			{
				P.second->m_pPairUser->SendData((char*)&Data, sizeof(Data), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_GANERATOR_COMPLETE);
			}
		}
		else 
		{
			for (auto P : m_mapPlayer)
			{
				P.second->m_pPairUser->SendData((char*)&Data, sizeof(Data), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_GANERATOR);
			}
		}
	}
}
void CDBDRule::UpdateTrap(const CircleData& _RecvData) 
{
	if (m_vecMagicCircle.size() <= _RecvData.CircleID)
	{
		return;
	}

	if (MC_RED == m_vecMagicCircle[_RecvData.CircleID]->GetMagicCircleColor())
		return;

	m_mapPlayerIter = m_mapPlayer.find(_RecvData.PlayerID);
	
	if (m_mapPlayerIter == m_mapPlayerEndIter)
		return;

	if (m_mapPlayerIter->second->GetPlayerRole() == PLAYER_SURVIVOR && true == m_vecMagicCircle[_RecvData.CircleID]->CheckTrapAvailable())
	{
		m_vecMagicCircle[_RecvData.CircleID]->ActiveTrap();

		CircleData Data;
		Data.PlayerID = _RecvData.PlayerID;
		Data.CircleID = _RecvData.CircleID;
		Data.Value.Flag = TRAP_ACTIVE;
		for (auto P : m_mapPlayer)
		{
			P.second->m_pPairUser->SendData((char*)&Data, sizeof(Data), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_TRAP);
		}
	}
	else if(m_mapPlayerIter->second->GetPlayerRole() == PLAYER_KILLER && true == m_vecMagicCircle[_RecvData.CircleID]->CheckTrapActive())
	{
		m_vecMagicCircle[_RecvData.CircleID]->StopTrap();

		CircleData Data;
		Data.PlayerID = _RecvData.PlayerID;
		Data.CircleID = _RecvData.CircleID;
		Data.Value.Flag = TRAP_STOP;

		for (auto P : m_mapPlayer)
		{
			P.second->m_pPairUser->SendData((char*)&Data, sizeof(Data), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_TRAP);
		}
	}

	if (MC_RED == m_vecMagicCircle[_RecvData.CircleID]->GetMagicCircleColor()
		&& true == m_vecMagicCircle[_RecvData.CircleID]->CheckTrapActive())
	{
		m_vecMagicCircle[_RecvData.CircleID]->ActiveTrap();

		CircleData Data;
		Data.PlayerID = _RecvData.PlayerID;
		Data.CircleID = _RecvData.CircleID;

		for (auto P : m_mapPlayer)
		{
			P.second->m_pPairUser->SendData((char*)&Data, sizeof(Data), FLAG_PLAYING_RULE_SIGNAL, FLAG_DATA_TRAP);
		}
		
	}
}

void CDBDRule::InitializeSceneProcessing(WorldData* _InitData)
{
	for (unsigned int i = 0; i < _InitData->Header.CurPlayerCount; ++i)
	{
		if (_InitData->Header.LocalPlayerID == _InitData->arrPlayerData[i].PlayerID)
		{
			CreatePlayer(_InitData->arrPlayerData[i], true);
		}
		else
		{
			CreatePlayer(_InitData->arrPlayerData[i]);
		}
	}
}
void CDBDRule::InitializeSceneProcessing_Test(WorldData* _InitData)
{
	for (unsigned int i = 0; i < _InitData->Header.CurPlayerCount; ++i)
	{
		if (_InitData->Header.LocalPlayerID == _InitData->arrPlayerData[i].PlayerID)
		{
			CreatePlayer(_InitData->arrPlayerData[i], true, false);
		}
		else
		{
			CreatePlayer(_InitData->arrPlayerData[i], false, false);
		}
	}
}
void CDBDRule::RoomDataProcessing(const _tagPACKET& _Pack)
{
	WorldData UpdateData;

	unsigned int* DataSize = (unsigned int*)(_Pack.Packet);

	UpdateData = ExtractionData<WorldData>(_Pack, *DataSize);

	for (unsigned int i = 0; i < UpdateData.Header.CurPlayerCount; ++i)
	{
		// 로컬 플레이어의 데이터는 여기서 갱신하지 않는다.
		if (UpdateData.Header.LocalPlayerID == UpdateData.arrPlayerData[i].PlayerID)
			continue;

		m_mapPlayerIter = m_mapPlayer.find(UpdateData.arrPlayerData[i].PlayerID);

		// 맵안에 없는 플레이어가 생긴경우 새로 접속한 플레이어로 간주 플레이어를 만든다.
		if (m_mapPlayerIter == m_mapPlayerEndIter)
		{
			CreatePlayer(UpdateData.arrPlayerData[i]);
			continue;
		}

		m_mapPlayerIter->second->UpdatePlayer(UpdateData.arrPlayerData[i]);
	}

}
void CDBDRule::SyncroWorldProcessing(const _tagPACKET& _Pack)
{
	WorldData UpdateData;

	unsigned int* DataSize = (unsigned int*)(_Pack.Packet);

	UpdateData = ExtractionData<WorldData>(_Pack, *DataSize);
	unsigned int MaxCount = DXMath::Clamp(UpdateData.Header.CurPlayerCount, (unsigned int)0, (unsigned int)MAXROOMUSER);

	for (unsigned int i = 0; i < MaxCount; ++i)
	{

		m_mapPlayerIter = m_mapPlayer.find(UpdateData.arrPlayerData[i].PlayerID);

		// 맵안에 없는 플레이어가 생긴경우 새로 접속한 플레이어로 간주 플레이어를 만든다.
		if (m_mapPlayerIter == m_mapPlayerEndIter)
		{
			CreatePlayer(UpdateData.arrPlayerData[i]);
			continue;
		}

		if (UpdateData.arrPlayerData[i].PlayerID == m_pLocalPlayer->GetPlayerID()) 
		{
			m_pLocalPlayer->SyncroPlayer(UpdateData.arrPlayerData[i]);
		}
		else 
		{
			m_mapPlayerIter = m_mapPlayer.find(UpdateData.arrPlayerData[i].PlayerID);
			//m_mapPlayerIter->second->UpdatePlayer(UpdateData.arrPlayerData[i]);
			m_mapPlayerIter->second->SyncroPlayer(UpdateData.arrPlayerData[i]);
		}

	}
}

void CDBDRule::ExitPlayerProcessing(const _tagPACKET& _Pack)
{
	unsigned __int64 ExitPalyerID = ExtractionData<unsigned __int64>(_Pack);

	m_mapPlayerIter = m_mapPlayer.find(ExitPalyerID);

	if (m_mapPlayerIter != m_mapPlayerEndIter)
	{
		m_mapPlayerIter->second->ACTOR->Death();
		m_mapPlayer.erase(m_mapPlayerIter);
		m_mapPlayerEndIter = m_mapPlayer.end();
	}
}
void CDBDRule::RemotePlayerProcessing(const _tagPACKET& _Pack)
{
	WorldData UpdateData;

	unsigned int* DataSize = (unsigned int*)(_Pack.Packet);

	UpdateData = ExtractionData<WorldData>(_Pack, *DataSize);

	unsigned int MaxCount = DXMath::Clamp(UpdateData.Header.CurPlayerCount, (unsigned int)0, (unsigned int)MAXROOMUSER);

	for (unsigned int i = 0; i < MaxCount; ++i)
	{
		if (m_pLocalPlayer->GetPlayerID() == UpdateData.arrPlayerData[i].PlayerID)
			continue;

		m_mapPlayerIter = m_mapPlayer.find(UpdateData.arrPlayerData[i].PlayerID);

		// 맵안에 없는 플레이어가 생긴경우 새로 접속한 플레이어로 간주 플레이어를 만든다.
		if (m_mapPlayerIter == m_mapPlayerEndIter)
		{
			continue;
		}

		m_mapPlayerIter->second->UpdatePlayer(UpdateData.arrPlayerData[i]);
	}
}

;
void CDBDRule::RulePacketProcessing(const _tagPACKET& _Pack)
{
	switch (_Pack.Header.DataType)
	{
	case FLAG_DATA_GANERATOR:
	{
		CircleData Data = ExtractionData<CircleData>(_Pack);
		m_vecMagicCircle[Data.CircleID]->SetGeneratorCount(Data.Value.Repair);
	}
		break;
	case FLAG_DATA_TRAP: 
	{
		CircleData Data = ExtractionData<CircleData>(_Pack);

		if (Data.Value.Flag == TRAP_ACTIVE)
		{
			m_vecMagicCircle[Data.CircleID]->ActiveTrap();
		}
		else if (Data.Value.Flag == TRAP_STOP)
		{
			m_vecMagicCircle[Data.CircleID]->StopTrap();
		}
		else if (Data.Value.Flag == TRAP_RECOVERY) 
		{
			m_vecMagicCircle[Data.CircleID]->RecoveryCompleteTrap();
		}
	}
		break;
	case FLAG_DATA_GANERATOR_COMPLETE: 
	{
		CircleData Data = ExtractionData<CircleData>(_Pack);
		m_vecMagicCircle[Data.CircleID]->RepairCompleteEvent();
		m_CircleCount = Data.Value.CircleCount;

		switch (m_CircleCount)
		{
		case 1:
			m_SceneUI[MAINUI_GENERATOR_COUNT]->SetUITexture(L"Number1");
			break;
		case 2:
			m_SceneUI[MAINUI_GENERATOR_COUNT]->SetUITexture(L"Number0");
			break;
		default:
			break;
		}

		m_mapPlayerIter = m_mapPlayer.find(Data.PlayerID);

		if (m_mapPlayerEndIter != m_mapPlayerIter) 
		{
			m_mapPlayerIter->second->m_pPlayerFSM->ChangePlayerState(PLAYER_IDLE);
		}

		if (m_CircleCount == MAXGENCIRCLE)
		{
			AllGeneratorCompleteEvent();
		}

	}
		break;
	case FLAG_DATA_HEALING_COMPLETE:
	case FLAG_DATA_HEALINGPLAYER: 
	case FLAG_DATA_SUMMONSPLAYER:
	case FLAG_DATA_RESCUEPLAYER:
	case FLAG_DATA_SELFHEALING:
	case FLAG_DATA_DISSOLVEPLAYER:
	case FLAG_DATA_HOOKPLAYER:
	case FLAG_DATA_ACCHOOK:
	case FLAG_DATA_DEAD:
	case FLAG_DATA_DEAD_OUT:
	case FLAG_DATA_RESCUED_COMPETE:
	case FLAG_DATA_ESCAPE:
	case FLAG_DATA_GAME_END:
	{
		PlayerSignal S = ExtractionData<PlayerSignal>(_Pack);
		m_mapPlayerIter = m_mapPlayer.find(S.TargetID);
		
		if (m_mapPlayerIter == m_mapPlayerEndIter)
		{
			return;
		}

		switch (_Pack.Header.DataType)
		{
		case FLAG_DATA_HEALINGPLAYER: 
		{
			m_mapPlayerIter->second->m_HealingCount = S.Value.Heal;
		}
			break;
		case FLAG_DATA_HEALING_COMPLETE:
		{
			m_mapPlayerIter->second->m_HealingCount = 0.f;
			m_mapPlayerIter->second->m_LifeCount = (UINT)S.Value.Life;

			if (m_mapPlayerIter->second == m_pLocalPlayer) 
			{
				switch (m_mapPlayerIter->second->m_LifeCount)
				{
				case 0: 
					m_SceneUI[MAINUI_LIFECOUNT]->SetUITexture(L"Number0");
					break;
				case 1:
					m_SceneUI[MAINUI_LIFECOUNT]->SetUITexture(L"Number1");
					break;
				case 2:
					m_SceneUI[MAINUI_LIFECOUNT]->SetUITexture(L"Number2");
					break;
				case 3:
					m_SceneUI[MAINUI_LIFECOUNT]->SetUITexture(L"Number3");
					break;
				}
			}

			switch (m_mapPlayerIter->second->m_LifeCount)
			{
			case 0:
				m_SceneUI[m_mapPlayerIter->second->m_SurvivorID]->SetUITexture(L"HitTwo");
				break;
			case 1:
				m_SceneUI[m_mapPlayerIter->second->m_SurvivorID]->SetUITexture(L"HitTwo");
				break;
			case 2:
				m_SceneUI[m_mapPlayerIter->second->m_SurvivorID]->SetUITexture(L"HitOne");
				break;
			case 3:
				m_SceneUI[m_mapPlayerIter->second->m_SurvivorID]->SetUITexture(L"Idle");
				break;
			}

			m_mapPlayerIter->second->EndHealingEvent();
			m_mapPlayerIter = m_mapPlayer.find(S.PlayerID);
			if(m_mapPlayerEndIter != m_mapPlayerIter)
				m_mapPlayerIter->second->EndHealingEvent();
		}
		break;
		case FLAG_DATA_SUMMONSPLAYER: 
		{
			m_mapPlayerIter = m_mapPlayer.find(S.PlayerID);
			
			if (m_mapPlayerIter == m_mapPlayerEndIter)
				return;

			m_mapPlayerIter->second->m_pPlayerFSM->ChangePlayerState(PLAYER_SUMMONS);
		}
			break;
		case FLAG_DATA_DISSOLVEPLAYER:
		{
			m_mapPlayerIter = m_mapPlayer.find(S.TargetID);

			if (m_mapPlayerIter == m_mapPlayerEndIter)
				return;

			m_mapPlayerIter->second->m_pPlayerFSM->DissolveEvent();
		}
		break;
		case FLAG_DATA_HOOKPLAYER:
		{
			m_mapPlayerIter = m_mapPlayer.find(S.TargetID);

			if (m_mapPlayerIter == m_mapPlayerEndIter)
				return;

			m_SceneUI[m_mapPlayerIter->second->m_SurvivorID]->SetUITexture(L"Hook");
			m_mapPlayerIter->second->SetDeathCount(S.Value.Death);
			m_mapPlayerIter->second->m_pPlayerFSM->ChangePlayerState(PLAYER_HOOK);
		}
			break;
		case FLAG_DATA_RESCUEPLAYER:
		{
			m_mapPlayerIter = m_mapPlayer.find(S.PlayerID);

			if (m_mapPlayerIter != m_mapPlayerEndIter)
			{
				m_mapPlayerIter->second->m_pPlayerFSM->ChangePlayerState(PLAYER_RESCUE);
			}

			m_mapPlayerIter = m_mapPlayer.find(S.TargetID);

			if (m_mapPlayerIter != m_mapPlayerEndIter)
			{
				m_mapPlayerIter->second->m_pPlayerFSM->ChangePlayerState(PLAYER_RESCUED);
			}

		}
			break;
		case FLAG_DATA_ACCHOOK:
		{
			m_mapPlayerIter = m_mapPlayer.find(S.PlayerID);
		
			if (m_mapPlayerIter == m_mapPlayerEndIter) 
			{
				return;
			}

			m_mapPlayerIter->second->SetHookCount(S.Value.Hook);

		}
			break;
		case FLAG_DATA_DEAD: 
		{
			m_mapPlayerIter = m_mapPlayer.find(S.PlayerID);

			if (m_mapPlayerIter == m_mapPlayerEndIter)
			{
				return;
			}

			m_SceneUI[m_mapPlayerIter->second->m_SurvivorID]->SetUITexture(L"Dead");
			m_mapPlayerIter->second->m_pPlayerFSM->ChangePlayerState(PLAYER_DEAD);
		}
			break;
		case FLAG_DATA_RESCUED_COMPETE:
		{
			m_mapPlayerIter = m_mapPlayer.find(S.PlayerID);

			if (m_mapPlayerIter == m_mapPlayerEndIter)
			{
				return;
			}

			if (m_mapPlayerIter->second->m_eNetType == LOCAL_PLAYER) 
			{
				m_SceneUI[MAINUI_LIFECOUNT]->SetUITexture(L"Number1");
			}

			m_SceneUI[m_mapPlayerIter->second->m_SurvivorID]->SetUITexture(L"HitTwo");
			m_mapPlayerIter->second->m_pPlayerFSM->RescuedEvent();

		}
		break;
		case FLAG_DATA_DEAD_OUT:
		{
			m_mapPlayerIter = m_mapPlayer.find(S.PlayerID);

			if (m_mapPlayerIter == m_mapPlayerEndIter)
			{
				return;
			}

			if (m_mapPlayerIter->second->m_eNetType == LOCAL_PLAYER)
			{
				TriggerGameEnd();
				DBD::bGameWin = false;
				return;
			}

		}
		break;
		case FLAG_DATA_ESCAPE:
		{
			m_mapPlayerIter = m_mapPlayer.find(S.PlayerID);

			if (m_mapPlayerIter == m_mapPlayerEndIter)
			{
				return;
			}

			m_SceneUI[m_mapPlayerIter->second->m_SurvivorID]->SetUITexture(L"Exit");

			if (m_mapPlayerIter->second->m_eNetType == LOCAL_PLAYER)
			{
				TriggerGameEnd();
				DBD::bGameWin = true;
			}
			

		}
		break;
		case FLAG_DATA_GAME_END:
		{
			if (nullptr == m_pLocalPlayer)
			{
				TriggerGameEnd();
				DBD::bGameWin = false;
			}

			if (m_pLocalPlayer->GetPlayerRole() == PLAYER_KILLER) 
			{
				TriggerGameEnd();
				DBD::bGameWin = false;
				return;
			}
			else 
			{
				TriggerGameEnd();
				DBD::bGameWin = true;
				return;
			}
		}
		break;
		default:
			break;
		}
	}
	default:
		break;
	}
}
void CDBDRule::CollsionPacketProcessing(const _tagPACKET& _Pack)
{
	WorldData UpdateData;

	unsigned int* DataSize = (unsigned int*)(_Pack.Packet);

	UpdateData = ExtractionData<WorldData>(_Pack, *DataSize);

	unsigned int MaxCount =  DXMath::Clamp(UpdateData.Header.CurPlayerCount, (unsigned int)0, (unsigned int)m_mapPlayer.size());

	for (unsigned int i = 0; i < MaxCount; i++)
	{
		m_mapPlayerIter = m_mapPlayer.find(UpdateData.arrPlayerData[i].PlayerID);

		if (m_mapPlayerIter == m_mapPlayerEndIter)
			continue;

		m_mapPlayerIter->second->m_pPlayerFSM->ChangePlayerState((PLAYER_STATE)UpdateData.arrPlayerData[i].PlayerState);

	}

}
void CDBDRule::SetTerrain(CTerrain* _pTerrain)
{
	if (nullptr == _pTerrain)
		return;


	m_pSceneTerrain = _pTerrain;
}

CPlayer* CDBDRule::CreateServerPlayer(const RoomEnterData& _EnterData)
{
	CPlayer* NewPlayer = nullptr;
	NewPlayer = CreateActor(UPDATE_LAYER_NORMAL, L"PlayerActor")->AddCom<CPlayer>(PlayerInitData{ true });

	NewPlayer->m_bServerFlag = true;
	NewPlayer->m_bEnter = true;
	NewPlayer->m_bBroadSync = false;
	NewPlayer->m_pPairUser = _EnterData.pEnterUser;
	NewPlayer->SetPlayerID(_EnterData.pEnterUser);
	NewPlayer->SetPlayerRole(_EnterData.RoleType);
	NewPlayer->SetRemotePlayer();
	NewPlayer->MountPlayerFSM<CPlayerFSM>();

	if (_EnterData.RoleType == PLAYER_SURVIVOR) 
	{
		NewPlayer->m_pPlayerFSM->InitializeSurvivorCollider(false);
	}
	else 
	{
		NewPlayer->m_pPlayerFSM->InitializeKillerCollider(false);
	}
		

	NewPlayer->SetRule(this);

	InsertPlayer(NewPlayer);
	return NewPlayer;
}
CPlayer* CDBDRule::CreatePlayer(const PlayerData& _Data, const bool& _Local /*= false*/, const bool& _Test /*= false*/)
{
	CPlayer* NewPlayer = nullptr;

	switch (_Data.PlayerRole)
	{
	case PLAYER_SURVIVOR:
	{
		NewPlayer = CreateActor(UPDATE_LAYER_NORMAL, L"PlayerActor")->AddCom<CPlayer>(PlayerInitData{ false });
		CSurvivorFSM* Fsm = NewPlayer->MountPlayerFSM<CSurvivorFSM>();
		NewPlayer->m_SurvivorID = m_GrantSurvivorID++;

		m_SceneUI[NewPlayer->m_SurvivorID]->On();

		// 로컬 플레이어 생성
		if (true == _Local)
		{
			NewPlayer->SetLocalPlayer();
			m_pLocalPlayer = NewPlayer;
			GETSINGLE(CClientConnectMgr).SetLocalPlayer(NewPlayer);
		}
		else
		{
			NewPlayer->SetRemotePlayer();
		}
	}

	break;
	case PLAYER_KILLER:
	{
		NewPlayer = CreateActor(UPDATE_LAYER_NORMAL, L"PlayerActor")->AddCom<CPlayer>(PlayerInitData{ false });
		CKillerFSM* Fsm = NewPlayer->MountPlayerFSM<CKillerFSM>();
		
		// 로컬 플레이어 생성
		if (true == _Local)
		{
			NewPlayer->SetLocalPlayer();
			m_pLocalPlayer = NewPlayer;
			GETSINGLE(CClientConnectMgr).SetLocalPlayer(NewPlayer);
			m_SceneUI[MAINUI_LIFECOUNT]->Off();
		}
		else
		{
			NewPlayer->SetRemotePlayer();
		}
	}
	break;
	default:
		break;
	}


	if (nullptr == NewPlayer)
		return nullptr;

	NewPlayer->SetRule(this);
	NewPlayer->InitializePlayer(_Data);
	m_pSceneTerrain->PushNavigator(NewPlayer->GetPlayerNavigator());
	
	//if (m_eRuleMode == SERVER_RULE)
	//{
	//	m_pSceneTerrain->PushNavigator(NewPlayer->GetPlayerNavigator());
	//	m_pSceneTerrain->InitNavigatorPosition();
	//}

	InsertPlayer(NewPlayer);
	return NewPlayer;
}


CMagicCircle* CDBDRule::CreateServerMagicCircle(CActor* _Actor, const int& _Type, const Vec3& _Pos, const float _Rot)
{
	if (nullptr == _Actor)
		return nullptr;

	_Actor->TRANS->SetPos(_Pos);
	_Actor->TRANS->SetRotate(Vec3(0.f, _Rot, 0.f));

	CMagicCircle* NewCircle = _Actor->AddCom<CMagicCircle>(MCData{ (MAGICCIRCLE_COLOR)_Type ,true });
	NewCircle->SetCircleID((int)m_vecMagicCircle.size());
	NewCircle->m_bServerFlag = true;
	m_vecMagicCircle.push_back(NewCircle);

	return NewCircle;
}
CMagicCircle* CDBDRule::CreateMagicCircle(class CActor* _Actor, const MagicCircleData& _RecvData)
{
	if (nullptr == _Actor)
		return nullptr;
	 
	CMagicCircle* NewCircle =  _Actor->AddCom<CMagicCircle>((MAGICCIRCLE_COLOR)_RecvData.Type);
	NewCircle->m_bServerFlag = false;
	_Actor->TRANS->SetPos(_RecvData.InitPos);
	_Actor->TRANS->SetRotate(Vec3(0.f, _RecvData.InitRotY, 0.f));
	NewCircle->SetCircleID(_RecvData.ID);
	m_vecMagicCircle.push_back(NewCircle);

	return NewCircle;
}
void CDBDRule::InitWorldObj()
{
	for (size_t i = 0; i < m_vecMagicCircle.size(); i++)
	{
		if (MC_GREEN == m_vecMagicCircle[i]->GetMagicCircleColor()) 
		{
			m_vecMagicCircle[i]->m_eGeneratorState = GENERATOR_IDLE;
			m_vecMagicCircle[i]->m_GeneratorCount = 0.f;
			m_vecMagicCircle[i]->m_fAccTime = 0.f;
		}
		else 
		{
			m_vecMagicCircle[i]->m_eBoardState = BOARD_IDLE;
			m_vecMagicCircle[i]->m_KillerAttackCol->Off() ;
			m_vecMagicCircle[i]->m_fAccTime = 0.f;
		}

	}
}
void CDBDRule::InsertPlayer(CPlayer* _pPlayer)
{
	if (nullptr == _pPlayer)
		return;

	m_mapPlayer.insert(std::unordered_map<unsigned __int64, SPTR<CPlayer>>::value_type(_pPlayer->GetPlayerID(), _pPlayer));
	m_mapPlayerEndIter = m_mapPlayer.end();
}
void CDBDRule::DeletePlayer(CPlayer* _pPlayer)
{
	if (nullptr == _pPlayer)
		return;

	m_mapPlayerIter = m_mapPlayer.find(_pPlayer->GetPlayerID());

	if (m_mapPlayerIter != m_mapPlayerEndIter)
	{
		m_mapPlayerIter->second->ACTOR->Death();
		m_mapPlayer.erase(m_mapPlayerIter);
		m_mapPlayerEndIter = m_mapPlayer.end();
	}
	
}

void CDBDRule::DebugRender()
{
	if (false == CGameObject::IsDebug())
		return;

	wchar_t	DebugBuff[MAXSTRING];

	memset(DebugBuff, 0, sizeof(wchar_t)*MAXSTRING);
	swprintf_s(DebugBuff, L"Current Connected User : %d", (int)m_mapPlayer.size());
	DBD::pPrettyFont->DrawString(DebugBuff, 18.f, Vec2(0.f, 20.f), Vec4::Pink);


	Vec3 Pos = m_pLocalPlayer->GetWorldPos();
	swprintf_s(DebugBuff, L"Local Player ID : %llu , Player Pos : (%.2f , %.2f , %.2f)", m_pLocalPlayer->GetPlayerID(), Pos.x, Pos.y, Pos.z);
	DBD::pPrettyFont->DrawString(DebugBuff, 18.f, Vec2(0.f, 40.f), Vec4::Pink);

	m_mapPlayerIter = m_mapPlayer.begin();
	m_mapPlayerEndIter = m_mapPlayer.end();

	for (size_t i = 0; m_mapPlayerIter != m_mapPlayerEndIter; ++m_mapPlayerIter)
	{
		if (m_mapPlayerIter->second == m_pLocalPlayer)
			continue;

		Vec3 Pos = m_mapPlayerIter->second->GetWorldPos();
		swprintf_s(DebugBuff, L"Remote Player ID : %llu , Player Pos : (%.2f , %.2f , %.2f)", m_mapPlayerIter->second->GetPlayerID(), Pos.x, Pos.y, Pos.z);
		DBD::pPrettyFont->DrawString(DebugBuff, 18.f, Vec2(0.f, 60.f + (float)i * 20.f), Vec4::Pink);
		++i;
	}

	swprintf_s(DebugBuff, L"Wait Hook Count  :  %d", (int)m_listHook.size());
	DBD::pPrettyFont->DrawString(DebugBuff, 20.f, Vec2(4.f, 300.f), Vec4::Pink);
}

void CDBDRule::CreateSceneUI()
{
	SPTR<CCamera> UICam = SCENE->GetUICamera();

	// 발전기 UI 

	m_SceneUI[MAINUI_GENERATOR_COUNT] = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Number2" });
	m_SceneUI[MAINUI_GENERATOR_COUNT]->SetSubSize(Vec3(50.f, 69.f, 0.f));
	m_SceneUI[MAINUI_GENERATOR_COUNT]->SetSubPivot(Vec3(STATEX - 190.f, STATEY + 45.f, 100.f));

	m_SceneUI[MAINUI_GENERATOR_HEALINGKIT] = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"PlayerState" });
	m_SceneUI[MAINUI_GENERATOR_HEALINGKIT]->SetSubSize(Vec3(428.f, 307.f, 0.f));
	m_SceneUI[MAINUI_GENERATOR_HEALINGKIT]->SetSubPivot(Vec3(STATEX, STATEY, 100.f));

	m_SceneUI[MAINUI_LIFECOUNT] = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Number3" });
	m_SceneUI[MAINUI_LIFECOUNT]->SetSubSize(Vec3(50.f, 69.f, 0.f));
	m_SceneUI[MAINUI_LIFECOUNT]->SetSubPivot(Vec3(STATEX + 90.f, STATEY + 45.f, 100.f));

	// Perk UI
	m_SceneUI[MAINUI_PERK] = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Perk" });
	m_SceneUI[MAINUI_PERK]->SetSubSize(Vec3(307.f, 307.f, 0.f));
	m_SceneUI[MAINUI_PERK]->SetSubPivot(Vec3(650.f, -330.f, 100.f));


	//플레이어
	m_SceneUI[MAINUI_PLAYER1_STATE]= UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Idle" });
	m_SceneUI[MAINUI_PLAYER1_STATE]->SetSubSize(Vec3(100.f, 100.f, 0.f));
	m_SceneUI[MAINUI_PLAYER1_STATE]->SetSubPivot(Vec3(STATEX - 180.f, STATEY - 48.f, 100.f));
	m_SceneUI[MAINUI_PLAYER1_STATE]->Off();

	m_SceneUI[MAINUI_PLAYER2_STATE] = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Idle" });
	m_SceneUI[MAINUI_PLAYER2_STATE]->SetSubSize(Vec3(100.f, 100.f, 0.f));
	m_SceneUI[MAINUI_PLAYER2_STATE]->SetSubPivot(Vec3(STATEX - 180.f + 100.f, STATEY - 48.f, 100.f));
	m_SceneUI[MAINUI_PLAYER2_STATE]->Off();

	m_SceneUI[MAINUI_PLAYER3_STATE] = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Idle" });
	m_SceneUI[MAINUI_PLAYER3_STATE]->SetSubSize(Vec3(100.f, 100.f, 0.f));
	m_SceneUI[MAINUI_PLAYER3_STATE]->SetSubPivot(Vec3(STATEX - 180.f + 200.f, STATEY - 48.f, 100.f));
	m_SceneUI[MAINUI_PLAYER3_STATE]->Off();

	m_SceneUI[MAINUI_PLAYER4_STATE] = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Idle" });
	m_SceneUI[MAINUI_PLAYER4_STATE]->SetSubSize(Vec3(100.f, 100.f, 0.f));
	m_SceneUI[MAINUI_PLAYER4_STATE]->SetSubPivot(Vec3(STATEX - 180.f + 300.f, STATEY - 48.f, 100.f));
	m_SceneUI[MAINUI_PLAYER4_STATE]->Off();


	m_SceneUI[MAINUI_CUR_GAUGE] = UICam->ACTOR->AddUICom(UI_DATA{ UL_GAUGE, UICam.PTR, UCT_BASE, L"GaugeFull" });
	m_SceneUI[MAINUI_CUR_GAUGE]->SetMaxGauge(Vec3(341.f, 25.6f, 0.f));
	m_SceneUI[MAINUI_CUR_GAUGE]->SetMinGauge(Vec3(0.f, 25.6f, 0.f));
	m_SceneUI[MAINUI_CUR_GAUGE]->SetGaugeStartPos(Vec3(-182.f, -330.f, 100.f));
	m_SceneUI[MAINUI_CUR_GAUGE]->SetUIGauge(0.f);
	m_SceneUI[MAINUI_CUR_GAUGE]->Off();

	m_SceneUI[MAINUI_GAUGE_FRAME] = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Gauge" });
	m_SceneUI[MAINUI_GAUGE_FRAME]->SetSubSize(Vec3(400.f, 25.6f, 0.f));
	m_SceneUI[MAINUI_GAUGE_FRAME]->SetSubPivot(Vec3(0.f, -330.f, 100.f));
	m_SceneUI[MAINUI_GAUGE_FRAME]->Off();

	m_SceneUI[MAINUI_ACTION_GUIDE] = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"RunMagic" });
	m_SceneUI[MAINUI_ACTION_GUIDE]->SetSubSize(Vec3(200.f, 38.f, 0.f));
	m_SceneUI[MAINUI_ACTION_GUIDE]->SetSubPivot(Vec3(0.f, -280.f, 100.f));
	m_SceneUI[MAINUI_ACTION_GUIDE]->Off();
}

void CDBDRule::ChangeFocusingObjEvent_Survivor(CPlayer* _pLocalPlayer)
{
	if (nullptr == _pLocalPlayer)
		return;

	switch (_pLocalPlayer->m_eFocusing)
	{
	case FOCUSING_NONE:
	{
		if (_pLocalPlayer->m_LifeCount > 1)
		{
			m_SceneUI[MAINUI_ACTION_GUIDE]->Off();
			m_SceneUI[MAINUI_GAUGE_FRAME]->Off();
			m_SceneUI[MAINUI_CUR_GAUGE]->Off();
			return;
		}
		else
		{
			m_SceneUI[MAINUI_ACTION_GUIDE]->SetUITexture(L"SelfHeal");
			m_SceneUI[MAINUI_GAUGE_FRAME]->On();
			m_SceneUI[MAINUI_CUR_GAUGE]->On();
		}
	}
	break;
	case FOCUSING_HEALING_PLAYER: 
	{
		m_SceneUI[MAINUI_ACTION_GUIDE]->SetUITexture(L"Heal");
		m_SceneUI[MAINUI_GAUGE_FRAME]->On();
		m_SceneUI[MAINUI_CUR_GAUGE]->On();
	}
	break;
	case FOCUSING_RESCUE_PLAYER:
	{
		m_SceneUI[MAINUI_ACTION_GUIDE]->SetUITexture(L"Rescue");
		m_SceneUI[MAINUI_GAUGE_FRAME]->On();
		m_SceneUI[MAINUI_CUR_GAUGE]->On();
	}
	break;
	case FOCUSING_GENERATOR:
		m_SceneUI[MAINUI_ACTION_GUIDE]->SetUITexture(L"RunMagic");
		m_SceneUI[MAINUI_GAUGE_FRAME]->On();
		m_SceneUI[MAINUI_CUR_GAUGE]->On();
		break;
	case FOCUSING_TRAP:
		m_SceneUI[MAINUI_ACTION_GUIDE]->SetUITexture(L"StartMagic");
		m_SceneUI[MAINUI_GAUGE_FRAME]->Off();
		m_SceneUI[MAINUI_CUR_GAUGE]->Off();
		break;
	default:
		break;
	}

	m_SceneUI[MAINUI_ACTION_GUIDE]->On();
}
void CDBDRule::ChangeFocusingObjEvent_Killer(CPlayer* _pLocalPlayer)
{
	if (nullptr == _pLocalPlayer)
		return;

	switch (_pLocalPlayer->m_eFocusing)
	{
	case FOCUSING_NONE:
	{
		m_SceneUI[MAINUI_ACTION_GUIDE]->Off();
		m_SceneUI[MAINUI_GAUGE_FRAME]->Off();
		m_SceneUI[MAINUI_CUR_GAUGE]->Off();
		return;
	}
	break;
	case FOCUSING_SUMMONS_PLAYER:
	{
		if (nullptr == m_SceneUI[MAINUI_ACTION_GUIDE])
			break;
		m_SceneUI[MAINUI_ACTION_GUIDE]->SetUITexture(L"SummonsHook");
		m_SceneUI[MAINUI_GAUGE_FRAME]->Off();
		m_SceneUI[MAINUI_CUR_GAUGE]->Off();
	}
	break;
	case FOCUSING_GENERATOR:
		m_SceneUI[MAINUI_ACTION_GUIDE]->SetUITexture(L"DestroyMagic");
		m_SceneUI[MAINUI_GAUGE_FRAME]->On();
		m_SceneUI[MAINUI_CUR_GAUGE]->On();
		break;
	case FOCUSING_TRAP:
		m_SceneUI[MAINUI_ACTION_GUIDE]->SetUITexture(L"DestroyMagic");
		m_SceneUI[MAINUI_GAUGE_FRAME]->Off();
		m_SceneUI[MAINUI_CUR_GAUGE]->Off();
		break;
	default:
		break;
	}

	if(nullptr != m_SceneUI[MAINUI_ACTION_GUIDE])
		m_SceneUI[MAINUI_ACTION_GUIDE]->On();
}
void CDBDRule::MainGaugeOn() 
{
	m_SceneUI[MAINUI_CUR_GAUGE]->On();
	m_SceneUI[MAINUI_GAUGE_FRAME]->On();
}
void CDBDRule::MainGaugeOff() 
{
	m_SceneUI[MAINUI_CUR_GAUGE]->Off();
	m_SceneUI[MAINUI_GAUGE_FRAME]->Off();
}
void CDBDRule::SetMainGauge(const float& _Gauge)
{
	m_SceneUI[MAINUI_CUR_GAUGE]->SetUIGauge(_Gauge);
}


void CDBDRule::GeneratorCompleteEvent(const unsigned int& _ID) 
{
	if (_ID >= m_vecMagicCircle.size())
		return;

	if (m_CircleCount >= MAXMAGICCIRCLE)
		return;

	if (MC_GREEN == m_vecMagicCircle[_ID]->GetMagicCircleColor()
		|| false == m_vecMagicCircle[_ID]->CheckRepairComplete())
	{
		m_vecMagicCircle[_ID]->RepairCompleteEvent();
		++m_CircleCount;

		switch (m_CircleCount)
		{
		case 1:
			m_SceneUI[MAINUI_GENERATOR_COUNT]->SetUITexture(L"Number1");
			break;
		case 2:
			m_SceneUI[MAINUI_GENERATOR_COUNT]->SetUITexture(L"Number0");
			break;
		default:
			break;
		}

		CSoundPlayer::GlobalPlaySound(L"GeneratorOn");
		if (m_CircleCount == MAXMAGICCIRCLE) 
		{
			// 모든 클라이언트의 탈출 이벤트를 작동 시킨다.
			AllGeneratorCompleteEvent();

			
		}
	}
}
void CDBDRule::AllGeneratorCompleteEvent() 
{
	m_bEscapeEvent = true;

	Vec3 AtDir = m_pLocalPlayer->TRANS->GetUp();
	Vec3 Dir1, Dir2;
	Dir1 = m_pEventCamera[EVENT_CAMERA1]->TRANS->GetWorldPos() - m_pLocalPlayer->TRANS->GetWorldPos();
	Dir2 = m_pEventCamera[EVENT_CAMERA2]->TRANS->GetWorldPos() - m_pLocalPlayer->TRANS->GetWorldPos();

	float Dot1, Dot2;
	Dot1 = DXMath::Vector2Dot(Dir1.XZ(), AtDir.XZ());
	Dot2 = DXMath::Vector2Dot(Dir2.XZ(), AtDir.XZ());

	if ( DXMath::Vector2Dot(Dir1.XZ(), AtDir.XZ()) <  DXMath::Vector2Dot(Dir2.XZ(), AtDir.XZ()))
	{
		m_CurEventCam = EVENT_CAMERA1;
	}
	else 
	{
		m_CurEventCam = EVENT_CAMERA2;
	}


	m_CurEventCam = EVENT_CAMERA2;
	SCENE->ChangeMainCamera(m_pEventCamera[m_CurEventCam]);
	m_pEventCamera[m_CurEventCam]->SetCameraBluring(2.5f, m_pLocalPlayer->GetPlayerCamera(), 3.f);

	CSoundPlayer::GlobalPlaySound(L"DoorOpen", 3.2f);
}

void CDBDRule::PushEventCamera(CCamera* _Cam1, CCamera* _Cam2)
{
	m_pEventCamera[EVENT_CAMERA1] = _Cam1;
	m_pEventCamera[EVENT_CAMERA2] = _Cam2;
}
void CDBDRule::UpdateLiftUI(CPlayer* _LocalPlayer)
{
	if (nullptr == _LocalPlayer)
		return;

	switch (_LocalPlayer->m_LifeCount)
	{
	case 0 :
	{
		m_SceneUI[MAINUI_LIFECOUNT]->SetUITexture(L"Number0");
	}
		break;
	case 1:
	{
		m_SceneUI[MAINUI_LIFECOUNT]->SetUITexture(L"Number1");
	}
	break;
	case 2:
	{
		m_SceneUI[MAINUI_LIFECOUNT]->SetUITexture(L"Number2");
	}
	break;
	default:
		break;
	}

}
void CDBDRule::UpdateSurvivorUI(CPlayer* _SurvivorPlayer)
{
	switch (_SurvivorPlayer->m_LifeCount)
	{
	case 0:
	{
		m_SceneUI[_SurvivorPlayer->m_SurvivorID]->SetUITexture(L"HitTwo");
	}
	break;
	case 1:
	{
		m_SceneUI[_SurvivorPlayer->m_SurvivorID]->SetUITexture(L"HitTwo");
	}
	break;
	case 2:
	{
		m_SceneUI[_SurvivorPlayer->m_SurvivorID]->SetUITexture(L"HitOne");
	}
	break;
	default:
		break;
	}
}

void CDBDRule::CreateEscapeDoor()
{
	m_EscapeDoor = m_EscapeDoor = CreateActor(UPDATE_LAYER_NORMAL, L"HookActor")->AddCom<CDoor>();
}

void CDBDRule::TriggerGameEnd()
{
	m_bGameEnd = true;
	m_AccEndTime = 0.f;

	if (nullptr != DBD::pMainParticle)
		DBD::pMainParticle->SetFadeEffect(4.f, FADE_OUT, FADE_COLOR_BLACK);
}