#include "Actor.h"
#include"Engine.h"
#include<algorithm>
#include"Camera.h"
#include"TransForm.h"
#include "UIBase.h"

CActor::CActor()
	: m_pParentScene(nullptr), m_pTransForm(nullptr)
	, m_pParentActor(nullptr), m_UpdateLayer(MAX_ACTOR_UPDATE_LAYER)
{
	m_listComponent.clear();
}
CActor::CActor(CScene* _pScene) 
	: m_pParentScene(_pScene), m_pTransForm(nullptr)
	, m_pParentActor(nullptr), m_UpdateLayer(MAX_ACTOR_UPDATE_LAYER)
{
	m_listComponent.clear();
}

CActor::~CActor()
{
	if (false == m_listComponent.empty())
	{
		m_listComponent.clear();
	}
}

void  CActor::PreUpdate(const float& _DeltaTime)
{
	if (false == m_listComponent.empty()) 
	{
		for (SPTR<CBaseComponent> Com : m_listComponent)
		{
			if (true == Com->IsActive())
				Com->PreUpdate(_DeltaTime);
		}
	}


	if (false == m_listChild.empty()) 
	{
		for (SPTR<CActor> Child : m_listChild)
		{
			if (true == Child->IsActive())
				Child->PreUpdate(_DeltaTime);
		}
	}


}
void CActor::Update(const float& _DeltaTime)
{
	if (false == m_listComponent.empty())
	{
		for (SPTR<CBaseComponent> Com : m_listComponent)
		{
			if (true == Com->IsActive())
				Com->Update(_DeltaTime);
		}
	}


	if (false == m_listChild.empty())
	{
		for (SPTR<CActor> Child : m_listChild)
		{
			if (true == Child->IsActive())
				Child->Update(_DeltaTime);
		}
	}

}
void CActor::PostUpdate(const float& _DeltaTime) 
{
	if (false == m_listComponent.empty())
	{
		for (SPTR<CBaseComponent> Com : m_listComponent)
		{
			if (true == Com->IsActive())
				Com->PostUpdate(_DeltaTime);
		}
	}

	if (false == m_listChild.empty())
	{
		for (SPTR<CActor> Child : m_listChild)
		{
			if (true == Child->IsActive())
				Child->PostUpdate(_DeltaTime);
		}
	}

}
void CActor::LastUpdate(const float& _DeltaTime)
{
	
	if (false == m_listComponent.empty())
	{
		for (SPTR<CBaseComponent> Com : m_listComponent)
		{
			if (true == Com->IsActive())
				Com->LastUpdate(_DeltaTime);
		}
	}

	if (false == m_listChild.empty())
	{
		for (SPTR<CActor> Child : m_listChild)
		{
			if (true == Child->IsActive())
				Child->LastUpdate(_DeltaTime);
		}
	}

}
void CActor::Release()
{
	auto ComIter = m_listComponent.begin();
	                                                                                                                                                                     
	while (ComIter != m_listComponent.end()) 
	{
		if (true == (*ComIter)->IsDeath())
			ComIter = m_listComponent.erase(ComIter);
		else
			++ComIter;
	}

	if (false == m_listChild.empty())
	{
		m_listChild.erase(std::remove_if(m_listChild.begin(), m_listChild.end(), [](SPTR<CActor> _Child)->bool
		{
			return _Child->IsDeath();
		}), m_listChild.end());
	}

}

void CActor::SetSize(const Vec3& _Size)
{
	m_pTransForm->SetScale(_Size); 
}
void CActor::SetPos(const Vec3& _Pos)
{                                                                                                                                                                        
	m_pTransForm->SetPos(_Pos);
}
void CActor::SetPos(const Vec2& _Pos)
{
	m_pTransForm->SetPos(_Pos); 
}

const Vec3 CActor::GetPos() 
{
	return m_pTransForm->GetLocalPos();
}
const Vec3 CActor::GetWorldPos()
{
	return m_pTransForm->GetWorldPos(); 
}

const bool CActor::PushCameraScript(CBaseComponent* _CamScript)
{
	SPTR<CCamera> Cam = FindCom<CCamera>();
	if (nullptr == Cam)
		return false;
	

	Cam->PushCameraScript(_CamScript);
	return true;
}


void CActor::SetChild(SPTR<CActor> _Actor)
{
	if (_Actor == nullptr)
		return;

	m_setWaitChild.insert(_Actor);
	//_Actor->m_pParentActor = this;
}
void CActor::Death()
{
	for (SPTR<CBaseComponent> Com : m_listComponent)
	{
		if (nullptr != Com)
			Com->Death();
	}

	if (nullptr != m_pParentActor)
		m_pTransForm->SetParent(nullptr);

	for (SPTR<CActor> Child : m_listChild)
	{
		if (nullptr != Child)
			Child->Death();
	}

	m_listChild.clear();
	m_setWaitChild.clear();
	CGameObject::Death();
}

void CActor::PushWaitChild()
{
	if (true == m_setWaitChild.empty())
		return;


	for (SPTR<CActor> Child : m_setWaitChild)
	{
		Child->m_pParentActor = this;
		if (nullptr != Child->m_pTransForm)
			Child->m_pTransForm->SetParent(this->m_pTransForm);
		
		Child->PushWaitChild();
		m_listChild.push_back(Child);
	}

	m_setWaitChild.clear();
}

CUIBase* CActor::AddUICom(const UI_DATA& _InitData)
{
	CUIBase* NewCom = Engine::CreateGameObject<CUIBase>();
	NewCom->SetParentActor(this);
	NewCom->SetBelongScene(this->GetScene());
	NewCom->SetTransForm(m_pTransForm);
	NewCom->Init(_InitData);

	return NewCom;
}