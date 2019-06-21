#pragma once
#include "DXHeader.h"
#include "GameObject.h"
#include "DXMacro.h"
#include "Engine.h"
#include <list>
#include <set>
#include "BaseComponent.h"
#include "DXEnum.h"


class CTransForm;
class CScript;
class CCameraScript;
class CActor :public CGameObject
{
	friend Engine;
	friend class CScene;


protected:
	CTransForm*												m_pTransForm;
	ACTOR_UPDATE_LAYER										m_UpdateLayer;


private:
	CScene*													m_pParentScene;
	CActor*													m_pParentActor;
	std::list<SPTR<CBaseComponent>>							m_listComponent;



public:
	template<typename COM>
	COM* AddCom()
	{
		if (false == COM::IsMultiComponentCheck(this))
			return nullptr;

		COM* NewCom = Engine::CreateGameObject<COM>();
		NewCom->SetParentActor(this);
		if (true == NewCom->IsTransForm())
			SetTransform(NewCom);
		else
		{
			NewCom->SetTransForm(m_pTransForm);
			NewCom->SetBelongScene(this->GetScene());
		}

		if (false == NewCom->Init())
		{
			SAFE_DELETE(NewCom)
			return nullptr;
		}

		if (true == Dynamic_Cast_Check<CCameraScript, COM>::IsParent) 
		{
			
			if (false == PushCameraScript(NewCom)) 
			{
				SAFE_DELETE(NewCom)
				return nullptr;
			}

	/*		SPTR<CCamera> Cam = FindCom<CCamera>();
			if (nullptr == Cam) 
			{
				SAFE_DELETE(NewCom)
				return nullptr;
			}
			else 
				Cam->PushCameraScript(NewCom);*/
		}
		else 
			m_listComponent.push_back(NewCom);

		return NewCom;
	}

	template<typename COM ,typename COMDATA>
	COM* AddCom(const COMDATA& _InitData)
	{
		if (false == COM::IsMultiComponentCheck(this))
			return nullptr;

		COM* NewCom = Engine::CreateGameObject<COM>();
		NewCom->SetParentActor(this);

		if (true == NewCom->IsTransForm())
			SetTransform(NewCom);
		else
		{
			NewCom->SetTransForm(m_pTransForm);
			NewCom->SetBelongScene(this->GetScene());
		}

		if (false == NewCom->Init(_InitData))
		{
			SAFE_DELETE(NewCom)
			return nullptr;
		}

		if (true == Dynamic_Cast_Check<CCameraScript, COM>::IsParent)
		{

			if (false == PushCameraScript(NewCom))
			{
				SAFE_DELETE(NewCom)
				return nullptr;
			}

			/*
			SPTR<CCamera> Cam = FindCom<CCamera>();
			if (nullptr == Cam)
			{
				SAFE_DELETE(NewCom)
				return nullptr;
			}
			else
				Cam->PushCameraScript(NewCom);*/
		}
		else
			m_listComponent.push_back(NewCom);

		return NewCom;
	}
	 
	//UI용 init에서 false리턴해도 삭제안하는 함수
	class CUIBase* AddUICom(const struct _tagUIDATA& _InitData);

	template<typename FINDCOM>
	SPTR<FINDCOM> FindCom()
	{
		std::list<SPTR<CBaseComponent>>::iterator Start = m_listComponent.begin();
		std::list<SPTR<CBaseComponent>>::iterator End = m_listComponent.end();

		for (; Start!= End; ++Start)
		{
			if (true == (*Start)->EqualType<FINDCOM>())
				return *Start;
		}

		return nullptr;
	}

	template<typename FINDCOM>
	SPTR<FINDCOM> FindLastCom()
	{
		std::list<SPTR<CBaseComponent>>::reverse_iterator Start = m_listComponent.rbegin();
		std::list<SPTR<CBaseComponent>>::reverse_iterator End = m_listComponent.rend();

		for (; Start != End; ++Start)
		{
			if (true == (*Start)->EqualType<FINDCOM>())
				return *Start;
		}

		return nullptr;
	}

	template<typename FINDCOM>
	void FindAllCom(std::list<SPTR<FINDCOM>>* _ComList )
	{
		if (nullptr == _ComList)
			return;

		std::list<SPTR<CBaseComponent>>::iterator Start = m_listComponent.begin();
		std::list<SPTR<CBaseComponent>>::iterator End = m_listComponent.end();

		for (; Start != End; ++Start)
		{
			if (true == (*Start)->EqualType<FINDCOM>())
				_ComList->push_back(*Start);
		}
		return;
	}

	template<typename FINDCOM>
	void FindChildCom(std::list<SPTR<FINDCOM>>* _ComList)
	{
		if (nullptr == _ComList)
			return;

		std::list<SPTR<CBaseComponent>>::iterator Start = m_listComponent.begin();
		std::list<SPTR<CBaseComponent>>::iterator End = m_listComponent.end();

		for (; Start != End; ++Start)
		{
			if (nullptr != (*Start).ConvertCheck<FINDCOM>())
			{ 
				_ComList->push_back(*Start);
			}
		}

		std::list<SPTR<CActor>>::iterator StartIter = m_listChild.begin();
		std::list<SPTR<CActor>>::iterator EndIter = m_listChild.end();
		for (; StartIter != EndIter; ++StartIter)
		{
			(*StartIter)->FindChildCom(_ComList);
		}
	}


protected:
	void PushComponent(CBaseComponent* _Com) 
	{
		if (nullptr == _Com ||   true == _Com->IsTransForm())
			return;

		m_listComponent.push_back(_Com);
	}

public:
	// 자신이 속해있는 씬을 반환한다.
	void SetParentScene(CScene* _pScene) { if (nullptr != _pScene)m_pParentScene = _pScene; }
	void SetTransform(CBaseComponent* _pTrans)
	{
		if (nullptr == _pTrans || false  == _pTrans->IsTransForm())
			return;
		m_pTransForm = (CTransForm*)_pTrans;
	}
	void SetUpdateLayer(const ACTOR_UPDATE_LAYER& _Layer) 
	{
		m_UpdateLayer = _Layer;
	}


private:
	virtual void Init() {}
	void PreUpdate(const float& _DeltaTime);
	void Update(const float& _DeltaTime);
	void PostUpdate(const float& _DeltaTime);
	void LastUpdate(const float& _DeltaTime);
	void Release();



public:
	void SetSize(const Vec3& _Size);
	void SetPos(const Vec3& _Pos);
	void SetPos(const Vec2& _Pos);

public:
	CScene * GetScene() { return m_pParentScene; }
	CTransForm* GetTrans() { return m_pTransForm; }
	const Vec3 GetPos();
	const Vec3 GetWorldPos();

private:
	const bool PushCameraScript(CBaseComponent* _CamScript);


// Child 구조 
private:
	std::set<SPTR<CActor>>								m_setWaitChild;
	std::list<SPTR<CActor>>								m_listChild;

	void PushWaitChild();
public:
	void SetChild(SPTR<CActor> _Actor);

public:
	void Death() override;


protected:
	CActor();
	CActor(CScene* _pScene);
	~CActor();
};



