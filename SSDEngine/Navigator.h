#pragma once
#include "BaseComponent.h"
#include "NavigationMesh.h"

#define NAVIDISTFACTOR 0.05f

class CNavigator : public CBaseComponent
{
private:
	friend CNavigationMesh;

private:
	SPTR<CNavigationMesh>							m_CurNaviMesh;
	NaviTri*										m_CurNaviTri;
	Vec3											m_PrevPos;


private:
	void PostUpdate(const float& _DeltaTime) override;
	void LastUpdate(const float& _DeltaTime) override;

public:
	void SetCurNaviMesh(SPTR<CNavigationMesh> _Navi);
	void InitCurNaviMesh();
public:
	//void RollBackPrevPos(const Vec3& _CurPos );
	void CalculateMovablePos(const Vec3& _CurPos , const Mat& _TriPos);
	void RisingNavigator(const float& _Height);

public:
	void SetPrevPos(const Vec3& _PrevPos) { m_PrevPos = _PrevPos; }
	const Vec3 GetNaviPrevPos() { return m_PrevPos; }

public:
	CNavigator();
	~CNavigator();
};

