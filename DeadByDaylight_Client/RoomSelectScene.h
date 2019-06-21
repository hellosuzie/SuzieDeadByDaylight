#pragma once
#include <Scene.h>
#include <DeadByDaylightGlobal.h>

class CUIBase;

struct RoomSelectUI 
{
	UINT					RoomNumber;
	SPTR<CUIBase>			RoomFrame;
	SPTR<CUIBase>			RoomFocusing;
	SPTR<CUIBase>			RoomNo;
	SPTR<CUIBase>			RoomUserCount;
	SPTR<CUIBase>			RoomSlash;
	SPTR<CUIBase>			RoomMaxUserCount;
	SPTR<CUIBase>			EmptyUI;

public:
	void SetRoomData(const UINT& _RoomNo, const UINT& _UserCount);
	void SetEmpty();
	void SetUIPos(const Vec3& _Pos);
	void On();
	void Off();
};

class CRoomSelectScene : public CSceneScript
{
	static Vec3					RoomUIPosition[5];
	static Vec3					RoomUISize;

private:
	std::wstring				TreeTypeName[TREETYPEMAX];
	SPTR<CUIBase>				SelectUI;
	SPTR<CUIBase>				BackUI;

	UINT						m_MaxSelectRoom;
	UINT						m_CurSelectIdx;
	RoomSelectUI				m_RoomUI[5];
	SPTR<CUIBase>				m_EmptyRoomUI[5];

	SPTR<CUIBase>				RoomUI1;
	SPTR<CUIBase>				RoomUI2;
	SPTR<CUIBase>				RoomUI3;
	SPTR<CUIBase>				RoomUI4;
	SPTR<CUIBase>				RoomUI5;
	SPTR<CUIBase>				RoomUISelect;

	SPTR<CUIBase>				ResetUI;

	bool						RoomSelectCheck;

public:
	void WakeUp()override;
	const bool Loading() override;

	void DebugRender()override;
public:
	void PreUpdate(const float& _DeltaTime) override;
	void PostUpdate(const float& _DeltaTime) override;
	void LastUpdate(const float& _DeltaTime) override;

public:
	void SelectEnter();
	void SelectStay();
	void SelectExit();

	void BackEnter();
	void BackStay();
	void BackExit();

	void Room1Enter();
	void Room1Stay();
	void Room1Exit();

	void Room2Enter();
	void Room2Stay();
	void Room2Exit();

	void Room3Enter();
	void Room3Stay();
	void Room3Exit();

	void Room4Enter();
	void Room4Stay();
	void Room4Exit();

	void Room5Enter();
	void Room5Stay();
	void Room5Exit();

	void ResetEnter();
	void ResetStay();
	void ResetExit();

public:
	void UpdateRoomUI();

public:
	void ShutDown();

public:
	CRoomSelectScene(CScene* _PairScene);
	~CRoomSelectScene();
};

