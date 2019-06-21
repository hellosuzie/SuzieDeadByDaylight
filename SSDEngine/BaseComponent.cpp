#include "BaseComponent.h"
#include"Actor.h"
#include"TransForm.h"
#include"Camera.h"
#include"Scene.h"



const bool CBaseComponent::IsMultiComponentCheck(CActor* _pActor)
{
	return true;
}

CBaseComponent::CBaseComponent()
	: m_pParentActor(nullptr), m_pTrans(nullptr), m_pBelongScene(nullptr)
{
}
CBaseComponent::CBaseComponent(CActor* _Actor)
	: m_pParentActor(_Actor), m_pTrans(nullptr), m_pBelongScene(nullptr)
{
	
}

CBaseComponent::~CBaseComponent()
{
}

const bool CBaseComponent::IsActive()
{
	if (nullptr == m_pParentActor )
		return false;

	return m_pParentActor->IsActive() && CGameObject::IsActive();
}
const bool CBaseComponent::IsDeath()
{
	if (nullptr == m_pParentActor)
		return true;

	return m_pParentActor->IsDeath() || CGameObject::IsDeath();
}
const bool CBaseComponent::IsDebug()
{
	if (nullptr == m_pParentActor)
		return false;

	return  CGameObject::IsDebug() || m_pParentActor->IsDebug() ;
}
const bool CBaseComponent::IsTransForm()
{
	return EqualType<CTransForm>();
}
CScene * CBaseComponent::GetScene()
{
	return m_pBelongScene; 
}
CActor * CBaseComponent::GetActor() 
{
	return m_pParentActor; 
}
CTransForm* CBaseComponent::GetTrans() 
{	
	return m_pTrans; 
}
CCamera* CBaseComponent::GetSceneMainCamera()
{
	if (nullptr == m_pBelongScene)
		return nullptr;

	//m_pBelongScene->GetSceneCamera();
	return m_pBelongScene->GetMainViewCamera();
}
CCamera* CBaseComponent::GetSceneUICamera()
{
	if (nullptr == m_pBelongScene)
		return nullptr;

	return m_pBelongScene->GetUICamera();
}
const bool CBaseComponent::IsTransMove()
{
	if (nullptr == m_pTrans)
		return false;

	return m_pTrans->IsMove();
}
void CBaseComponent::ActorOn()
{
	if (nullptr == m_pParentActor)
		return;

	m_pParentActor->On();
}
void CBaseComponent::ActorOff() 
{
	if (nullptr == m_pParentActor)
		return;

	m_pParentActor->Off();
}
