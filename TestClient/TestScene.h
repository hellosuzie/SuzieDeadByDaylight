#pragma once
#include <Scene.h>
#include <MiddleEnum.h>
#include <unordered_map>

class CPlayer;
class CTestScene : public CSceneScript
{
private:
	float															m_fAccTime;
	ASYNC_LOADING_STEP												m_eLoadStep;
	
	unsigned __int64												m_LocalPlayerID;
	SPTR<CPlayer>													m_LocalPlayer;
	std::unordered_map<unsigned __int64, SPTR<CPlayer>>				m_mapPlayer;
	std::unordered_map<unsigned __int64, SPTR<CPlayer>>::iterator   m_mapPlayerIter;
	std::unordered_map<unsigned __int64, SPTR<CPlayer>>::iterator	m_mapPlayerEndIter;
	unsigned int													m_UpdateCount;


public:
	const bool IsAsyncLoadingComplete();
	const ASYNC_LOADING_STEP GetLoadingStep();
	const void CompleteAsyncLoading();

public:
	const bool Loading() override;
	const bool AsyncLoading(void* _Arg) override;

	void WakeUp()override;
	void PreUpdate(const float& _DeltaTime) override;
	void PostUpdate(const float& _DeltaTime) override;
	void LastUpdate(const float& _DeltaTime) override;
	void DebugRender()override;

private:
	CPlayer* CreatePlayer(const PlayerData& _Data, const bool& _Local = false);
	void InsertPlayer(CPlayer* _pPlayer);
	void DeletePlayer(CPlayer* _pPlayer);
					
public:
	void PacketProcessing(const float& _DeltaTime);



public:
	void LoadTexture();
	void LoadFBX();

public:
	CTestScene(CScene* _PairScene);
	~CTestScene();
};

