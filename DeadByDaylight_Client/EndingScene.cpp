#include "PreHeader.h"
#include "EndingScene.h"
#include <UIBase.h>
#include <Camera.h>
#include <DeadByDaylightGlobal.h>


CEndingScene::CEndingScene(CScene* _PairScene) 
	: CSceneScript(_PairScene), m_WinUI(nullptr), m_DefeatUI(nullptr),m_EndingUI(nullptr)
{
}


CEndingScene::~CEndingScene()
{
}

const bool CEndingScene::Loading()
{
	SPTR<CCamera> UICam = GetUICamera();

	m_EndingUI =  UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Win" });
	m_EndingUI->SetSubSize(Vec3(1600.f, 900.f, 100.f));
	m_EndingUI->SetSubPivot(Vec3(0.f,0.f,100.f));
	m_EndingUI->Off();

	/*m_WinUI = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Win" });
	m_WinUI->SetSubSize(Vec3(1600.f, 900.f, 0.f));
	m_WinUI->SetSubPivot(Vec3());
	m_WinUI->Off();

	m_DefeatUI = UICam->ACTOR->AddUICom(UI_DATA{ UL_BASE, UICam.PTR, UCT_BASE, L"Defeat" });
	m_DefeatUI->SetSubSize(Vec3(1600.f, 900.f, 0.f));
	m_DefeatUI->SetSubPivot(Vec3());
	m_DefeatUI->Off();*/

	return true;
}

void CEndingScene::WakeUp()
{
	if (nullptr == m_EndingUI)
		return;

	if (DBD::bGameWin) 
	{
		m_EndingUI->SetUITexture(L"Win");
		m_EndingUI->On();
	}
	else 
	{
		m_EndingUI->SetUITexture(L"Defeat");
		m_EndingUI->On();
	}
}
void CEndingScene::PreUpdate(const float & _DeltaTime)
{
}

void CEndingScene::PostUpdate(const float & _DeltaTime)
{
}

void CEndingScene::LastUpdate(const float & _DeltaTime)
{
}

void CEndingScene::DebugRender()
{
}
