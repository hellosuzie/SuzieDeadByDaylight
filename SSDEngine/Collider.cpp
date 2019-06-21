#include "Collider.h"
#include "Actor.h"
#include "Scene.h"
#include "DXMath.h"
#include "Base2DCollider.h"
#include "VtxShader.h"
#include "PixShader.h"
#include "Mesh.h"
#include "ResourceMgr.h"
#include "Material.h"
#include "TransForm.h"


SPTR<CMesh>					CCollider::Collider2D_Mesh = nullptr;
SPTR<CMaterial>				CCollider::Collider2D_Material = nullptr;

void CCollider::ColliderInit()
{
	Collider2D_Mesh = CResourceMgr<CMesh>::Find(L"2DMesh");
	Collider2D_Material = CResourceMgr<CMaterial>::Find(L"Base2D")->Clone();

}
CCollider::CCollider()
	:m_CollisionChannel(L""), m_pFi(nullptr)
	, m_EnterCallBack(nullptr), m_StayCallBack(nullptr)
	, m_ExitCallBack(nullptr), m_bCollision(false)
	, m_DebugColor(Vec3(1.f, 1.f, 1.f)), m_ColSize(Vec2()), m_ColPivot(Vec3())
	, m_fBorderSize(2.f)
{
	m_setCollider.clear();
	m_setEndIter = m_setCollider.end();
}


CCollider::~CCollider()
{
	SAFE_DELETE(m_pFi);
	m_setCollider.clear();
}

void CCollider::CallEnter(SPTR<CCollider> _This, SPTR<CCollider> _Other) 
{
	if (nullptr == m_EnterCallBack)
		return;

	m_EnterCallBack(_This, _Other);
}
void CCollider::CallStay(SPTR<CCollider> _This, SPTR<CCollider> _Other) 
{
	if (nullptr == m_StayCallBack)
		return;

	m_StayCallBack(_This, _Other);
}

void CCollider::CallExit(SPTR<CCollider> _This, SPTR<CCollider> _Other) 
{
	if (nullptr == m_ExitCallBack)
		return;

	m_ExitCallBack(_This, _Other);
}


const bool CCollider::Init(const COLDATA& _InitData)
{
	m_CollisionChannel = _InitData.CollisionChannel;


	m_pBelongScene->PushCollider(_InitData.eSceneCollision,this);
	return true;
}

void CCollider::ReleaseSet() 
{
	for (SPTR<CCollider> SetCollider : m_setCollider)
	{
		if (true == SetCollider->IsActive()) 
		{
			SetCollider->CallExit(SetCollider,this);
			SetCollider->CollisionEndEvent();
		}
	}

	m_setCollider.clear();
	m_setEndIter = m_setCollider.end();
}

void CCollider::ClearSet()
{
	m_setCollider.clear();
	m_setEndIter = m_setCollider.end();
}

void CCollider::EraseSet(SPTR<CCollider> _Other) 
{
	if (true == m_setCollider.empty())
		return;

	m_setFindIter = m_setCollider.find(_Other);

	if (m_setFindIter != m_setCollider.end()) 
	{
		m_setCollider.erase(m_setFindIter);
		m_setEndIter = m_setCollider.end();
	}
}
void CCollider::DumpDeathCollider()
{
	std::unordered_set<CCollider*>::iterator Start = m_setCollider.begin();
	std::unordered_set<CCollider*>::iterator End = m_setCollider.end();

	for (; Start != End; )
	{
		if (nullptr == (*Start)->ACTOR )
			Start = m_setCollider.erase(Start);
		else
			++Start;
	}
}
void CCollider::Collision(SPTR<CCollider> _Other)
{
	if (nullptr == _Other)
		return;

	m_setFindIter = m_setCollider.find(_Other);


	if (true == CollisionCheck(_Other)) 
	{
		CollisionEvent();
		_Other->CollisionEvent();
		if (m_setFindIter == m_setEndIter) 
		{
			CallEnter(this,_Other);
			_Other->CallEnter(_Other, this);
			m_setCollider.insert(_Other);
			m_setEndIter = m_setCollider.end();
		}
		else 
		{
			CallStay(this, _Other);
			_Other->CallStay(_Other,this);
		}
	}
	else 
	{
		if (m_setFindIter != m_setEndIter || true == _Other->IsDeath()) 
		{
			CollisionEndEvent();
			_Other->CollisionEndEvent();
			CallExit(this, _Other);
			_Other->CallExit(_Other,this);
			m_setCollider.erase(m_setFindIter);
			m_setEndIter = m_setCollider.end();
		}
	}
}

const bool CCollider::CollisionCheck(SPTR<CCollider> _Other)
{
	this->CollisionSyncro();
	_Other->CollisionSyncro();

	// 자신의 충돌체 타입을 검사
	switch (m_pFi->m_eColType)
	{
	case CT_POINT:
		return PointCollision(_Other->m_pFi);
	case CT_RECT:
		return RectCollision(_Other->m_pFi);
	case CT_CIRCLE:
		return CircleCollision(_Other->m_pFi);
	case CT_2DPOLYGON:
		return false;
	case CT_3DSPHERE:
		return SphereCollision(_Other->m_pFi);
	case CT_3DAABB:
		return false;
	case CT_3DOBB:
		return OBBCollision(_Other->m_pFi);
	case CT_RAY:
		return RayCollision(_Other->m_pFi);
	case CT_3DMESH:
		return false;
	case CT_FRUSTUM:
		return FrustumCollision(_Other->m_pFi);
	case MAXCOLTYPE:
		return false;
	}

	return false;
}

const bool CCollider::PointCollision(BASE_FI* _OtherFi)
{
	// 상대의 충돌체 검사 수행
	switch (_OtherFi->m_eColType)
	{
	case CT_POINT:
		return CollisionFunc::PointToPoint(((BASE2D_FI*)m_pFi)->m_Fi, ((BASE2D_FI*)_OtherFi)->m_Fi);
	case CT_RECT:
		return CollisionFunc::PointToRect(((BASE2D_FI*)m_pFi)->m_Fi, ((BASE2D_FI*)_OtherFi)->m_Fi);
	case CT_CIRCLE:
		return CollisionFunc::PointToCircle(((BASE2D_FI*)m_pFi)->m_Fi, ((BASE2D_FI*)_OtherFi)->m_Fi);
	case CT_2DPOLYGON:
		return false;
	case CT_3DSPHERE:
		return false;
	case CT_3DAABB:
		return false;
	case CT_3DOBB:
		return false;
	case CT_RAY:
		return false;
	case CT_3DMESH:
		return false;
	case MAXCOLTYPE:
		return false;
	}

	return false;
}
const bool CCollider::RectCollision(BASE_FI* _OtherFi)
{
	// 상대의 충돌체 검사 수행
	switch (_OtherFi->m_eColType)
	{
	case CT_POINT:
		return CollisionFunc::PointToRect(((BASE2D_FI*)_OtherFi)->m_Fi,((BASE2D_FI*)m_pFi)->m_Fi);
	case CT_RECT:
		return CollisionFunc::RectToRect(((BASE2D_FI*)m_pFi)->m_Fi, ((BASE2D_FI*)_OtherFi)->m_Fi);
	case CT_CIRCLE:
		return CollisionFunc::RectToCircle(((BASE2D_FI*)m_pFi)->m_Fi, ((BASE2D_FI*)_OtherFi)->m_Fi);
	case CT_2DPOLYGON:
		return false;
	case CT_3DSPHERE:
		return false;
	case CT_3DAABB:
		return false;
	case CT_3DOBB:
		return false;
	case CT_RAY:
		return false;
	case CT_3DMESH:
		return false;
	case MAXCOLTYPE:
		return false;
	}

	return false;
}
const bool CCollider::CircleCollision(BASE_FI* _OtherFi)
{
	// 상대의 충돌체 검사 수행
	switch (_OtherFi->m_eColType)
	{
	case CT_POINT:
		return CollisionFunc::PointToCircle(((BASE2D_FI*)_OtherFi)->m_Fi, ((BASE2D_FI*)m_pFi)->m_Fi);
	case CT_RECT:
		return CollisionFunc::RectToCircle(((BASE2D_FI*)_OtherFi)->m_Fi,((BASE2D_FI*)m_pFi)->m_Fi);
	case CT_CIRCLE:
		return CollisionFunc::CircleToCircle(((BASE2D_FI*)m_pFi)->m_Fi, ((BASE2D_FI*)_OtherFi)->m_Fi);
	case CT_2DPOLYGON:
		return false;
	case CT_3DSPHERE:
		return false;
	case CT_3DAABB:
		return false;
	case CT_3DOBB:
		return false;
	case CT_RAY:
		return false;
	case CT_3DMESH:
		return false;
	case MAXCOLTYPE:
		return false;
	}

	return false;
}

const bool CCollider::SphereCollision(BASE_FI* _OtherFi) 
{
	switch (_OtherFi->m_eColType)
	{
	case CT_POINT:
		return false;
	case CT_RECT:
		return false;
	case CT_CIRCLE:
		return false;
	case CT_2DPOLYGON:
		return false;
	case CT_3DSPHERE:
		return ((SPHERE_FI*)m_pFi)->SphereFi.Intersects(((SPHERE_FI*)_OtherFi)->SphereFi);
	case CT_3DAABB:
		return false;
	case CT_3DOBB:
		return ((SPHERE_FI*)m_pFi)->SphereFi.Intersects(((OBB_FI*)_OtherFi)->ObbFi);
	case CT_RAY:
		return false;
	case CT_3DMESH:
		return false;
	case MAXCOLTYPE:
		return false;
	}

	return false;
}
const bool CCollider::OBBCollision(BASE_FI* _OtherFi) 
{
	switch (_OtherFi->m_eColType)
	{
	case CT_POINT:
		return false;
	case CT_RECT:
		return false;
	case CT_CIRCLE:
		return false;
	case CT_2DPOLYGON:
		return false;
	case CT_3DSPHERE:
		return ((OBB_FI*)m_pFi)->ObbFi.Intersects(((SPHERE_FI*)_OtherFi)->SphereFi);
	case CT_3DAABB:
		return false;
	case CT_3DOBB:
		return ((OBB_FI*)m_pFi)->ObbFi.Intersects(((OBB_FI*)_OtherFi)->ObbFi);
	case CT_RAY:
		return false;
	case CT_3DMESH:
		return false;
	case MAXCOLTYPE:
		return false;
	}

	return false;
}
const bool CCollider::RayCollision(BASE_FI* _OtherFi)
{
	switch (_OtherFi->m_eColType)
	{
	case CT_POINT:
		return false;
	case CT_RECT:
		return false;
	case CT_CIRCLE:
		return false;
	case CT_2DPOLYGON:
		return false;
	case CT_3DSPHERE: 
	{
		Vec4 Origin = ((RAY_FI*)m_pFi)->RayOrigin;
		Vec4 Dir = ((RAY_FI*)m_pFi)->RayDir;
		float Len= ((RAY_FI*)m_pFi)->RayLength;

		return ((SPHERE_FI*)_OtherFi)->SphereFi.Intersects(Origin, Dir, Len);
	}
	case CT_3DAABB:
		return false;
	case CT_3DOBB: 
	{
		Vec4 Origin = ((RAY_FI*)m_pFi)->RayOrigin;
		Vec4 Dir = ((RAY_FI*)m_pFi)->RayDir;
		float Len = ((RAY_FI*)m_pFi)->RayLength;
		
		bool Check = ((OBB_FI*)_OtherFi)->ObbFi.Intersects(Origin, Dir, Len);
		
		if (Check == true) 
		{
			((OBB_FI*)_OtherFi)->CollisionLength = Len;
		}
		else 
		{
			((OBB_FI*)_OtherFi)->CollisionLength = -1.f;
		}
		return Check;
	}

	case CT_RAY:
		return false;
	case CT_3DMESH:
	{
		bool Check = false;

		RAY_FI* Fi = reinterpret_cast<RAY_FI*>(m_pFi);
		MESHCOL_FI* OtherFi = reinterpret_cast<MESHCOL_FI*>(_OtherFi);
		Vec4 Origin = Fi->RayOrigin;
		Vec4 Dir = Fi->RayDir;
		UINT MaxTriCount = (UINT)OtherFi->pColTri->size();
		OtherFi->ColTriIdx = -1;

		float MinDist = D3D11_FLOAT32_MAX;
		float Dist;

		Vec3 T0, T1, T2;

		for (UINT i = 0;  i < MaxTriCount;  ++i)
		{
			ColTri& CTri = (*OtherFi->pColTri)[i];
			T0 = OtherFi->ColWorldMat.Mul_Coord(CTri.Tri[0]);
			T1 = OtherFi->ColWorldMat.Mul_Coord(CTri.Tri[1]);
			T2 = OtherFi->ColWorldMat.Mul_Coord(CTri.Tri[2]);

			Dist = D3D11_FLOAT32_MAX;
			Check = DirectX::TriangleTests::Intersects(Origin, Dir, T0, T1, T2, Dist);

			if (true == Check) 
			{
				if (MinDist > Dist) 
				{
					MinDist = Dist;
					OtherFi->ColTriIdx = i;
				}
			}
		}

		if (D3D11_FLOAT32_MAX != MinDist) 
		{
			Fi->bCollsion = true;
			return true;
		}
		
		return false;
	}
	case MAXCOLTYPE:
		return false;
	}

	return false;
}

const bool CCollider::FrustumCollision(BASE_FI* _OtherFi)
{
	switch (_OtherFi->m_eColType)
	{
	case CT_POINT:
		return false;
	case CT_RECT:
		return false;
	case CT_CIRCLE:
		return false;
	case CT_2DPOLYGON:
		return false;
	case CT_3DSPHERE:
		return false;
	case CT_3DAABB:
		return false;
	case CT_3DOBB:
	{
		return ((FRUSTUM_FI*)m_pFi)->FrustumFi.Intersects(((OBB_FI*)_OtherFi)->ObbFi);
	}
	case CT_RAY:
		return false;
	case CT_3DMESH:
		return false;
	case MAXCOLTYPE:
		return false;
	}

	return false;
}
const COLTYPE CCollider::GetColType()
{
	if (nullptr == m_pFi)
		return MAXCOLTYPE;

	return m_pFi->m_eColType;
}

const Vec3 CCollider::GetColPos()
{
	return TRANS->GetWorldPos() + m_ColPivot;
}

void CCollider::AddIgnoreCollider(CCollider* _Col) 
{
	if (nullptr == _Col)
		return;

	m_setIgnoreCollider.insert(_Col);
}
const bool CCollider::IsIgnoreCollider(CCollider* _Col) 
{
	if (true == m_setIgnoreCollider.empty())
		return false;

	return m_setIgnoreCollider.end() != m_setIgnoreCollider.find(_Col);
}
void CCollider::On()
{
	CGameObject::On();
}
void CCollider::Off()
{
	CGameObject::Off();
	ReleaseSet();
}