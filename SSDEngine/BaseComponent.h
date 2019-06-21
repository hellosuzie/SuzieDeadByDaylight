#pragma once
#include "GameObject.h"

class CBaseComponent : public CGameObject
{
private:
	friend class CActor;

protected:
	class CScene *												m_pBelongScene;
	class CActor *												m_pParentActor;
	class CTransForm*											m_pTrans;


public:
	static const bool IsMultiComponentCheck(CActor* _pActor);

public:
	void Death() override 
	{
		m_pParentActor = nullptr;
		CGameObject::Death();
	}
	const bool IsActive() override;
	const bool IsDeath() override;
	const bool IsDebug() override;
	const bool IsTransForm();


private:
	void SetBelongScene(CScene* _Scene) { if (nullptr != _Scene) m_pBelongScene = _Scene; }
	void SetParentActor(CActor* _Actor) { if (nullptr != _Actor) m_pParentActor = _Actor; }
	void SetTransForm(CTransForm* _Trans) { if (nullptr != _Trans)m_pTrans = _Trans; }

public:
	CScene * GetScene();
	CActor * GetActor();
	CTransForm* GetTrans();
	class CCamera* GetSceneMainCamera();
	CCamera* GetSceneUICamera();

private:
	virtual const bool Init() { return true; }

protected:
	virtual void PreUpdate(const float& _DeltaTime) {}
	virtual void Update(const float& _DeltaTime){}
	virtual void PostUpdate(const float& _DeltaTime) {}
	virtual void LastUpdate(const float& _DeltaTime){}
	virtual void Render() {}
	virtual void Release() {}

public:
	const bool IsTransMove();

public:
	void ActorOn();
	void ActorOff();


public:
	CBaseComponent();
	CBaseComponent(CActor* _Actor);
	~CBaseComponent();
};

#define ACTOR GetActor()
#define TRANS GetTrans()
#define SCENE GetScene()
#define MAINCAM GetSceneMainCamera()
#define UICAM GetSceneUICamera()