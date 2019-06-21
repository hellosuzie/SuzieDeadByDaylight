#pragma once
#include <Script.h>

class CTest : public CScript
{
private:
	SPTR<class CSphereCollider>				m_Sphere;
	SPTR<class COBBCollider>				m_Obb;
	SPTR<class COBBCollider>				m_Obb2;
	SPTR<class CSkeletalFbxRenderer>		m_AnimRen;

	SPTR<class CFbxRenderer>				m_FbxRen;
	

public:
	const bool Init();
	void Update(const float& _DeltaTime) override;
	void LastUpdate(const float& _DeltaTime) override;

public:
	void KillerInit();
	void SurvivorInit();
public:
	void Enter(SPTR<class CCollider> _This, SPTR<class CCollider> _Other);

public:
	CTest();
	~CTest();
};

