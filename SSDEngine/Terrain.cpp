#include "Terrain.h"
#include "Actor.h"
#include "TerrainRenderer.h"
#include "DXStruct.h"
#include "TransForm.h"
#include "DXMath.h"
#include "RayCollider.h"
#include "Navigator.h"

CTerrain::CTerrain()
	:m_TerrainRen(nullptr), m_pTerrainTriRay(nullptr)
{
}


CTerrain::~CTerrain()
{
	m_vecLinkArea.clear();
}

void CTerrain::On() 
{
	if (nullptr != m_TerrainRen)
		m_TerrainRen->On();

	CGameObject::On();
}
void CTerrain::Off()
{
	if (nullptr != m_TerrainRen)
		m_TerrainRen->Off();

	CGameObject::Off();
}
const bool CTerrain::Init()
{
	m_TerrainRen = ACTOR->AddCom<CTerrainRenderer>(RENDATA{ RENDER_LAYER_MAP });
	m_TerrainRen->m_pTerrain = this;

	return true;
}
const bool CTerrain::Init(const bool _bClient)
{
	if (true == _bClient) 
	{
		m_TerrainRen = ACTOR->AddCom<CTerrainRenderer>(RENDATA{ RENDER_LAYER_MAP });
		m_TerrainRen->m_pTerrain = this;
	}
	else 
	{
		m_TerrainRen = ACTOR->AddCom<CTerrainRenderer>();
		m_TerrainRen->m_pTerrain = this;
	}

	return true;
}
void CTerrain::PostUpdate(const float& _DeltaTime)
{
	TerrainAreaUpdate(_DeltaTime);

	if (nullptr != m_pTerrainTriRay && true == m_pTerrainTriRay->IsActive()) 
	{
		m_TerrainRen->SelectTriUpdate(m_pTerrainTriRay);
	}
}


void CTerrain::TerrainAreaUpdate(const float& _DeltaTime)
{
	size_t MaxArea = m_vecLinkArea.size();

	for (size_t i = 0; i < MaxArea; ++i)
	{
		if (m_vecLinkArea[i]->IsMoveArea()) 
		{
			m_vecLinkArea[i]->m_bUpdateArea = false;
			Vec3 AreaMidPos = m_vecLinkArea[i]->TRANS->GetWorldPos();

			for (size_t j = 0; j < m_vecLinkArea[i]->m_vecNaviArea.size(); ++j)
			{
				m_vecLinkArea[i]->m_vecNaviArea[j].bChangeValue = false;
				m_listAreaData.push_back(m_vecLinkArea[i]->m_vecNaviArea[j]);
				m_listAreaData.back().AreaPos += Vec2(AreaMidPos.x, AreaMidPos.z);
			}
		}
		else if (true == m_vecLinkArea[i]->IsUpdateArea()) 
		{
			m_vecLinkArea[i]->m_bUpdateArea = false;
			Vec3 AreaMidPos = m_vecLinkArea[i]->TRANS->GetWorldPos();

			for (size_t j = 0; j < m_vecLinkArea[i]->m_vecNaviArea.size(); ++j)
			{
				if (true == m_vecLinkArea[i]->m_vecNaviArea[j].bChangeValue) 
				{
					m_vecLinkArea[i]->m_vecNaviArea[j].bChangeValue = false;
					m_listAreaData.push_back(m_vecLinkArea[i]->m_vecNaviArea[j]);
					m_listAreaData.back().AreaPos += Vec2(AreaMidPos.x, AreaMidPos.z);
				}
			}
		}
	}


	if (true == m_listAreaData.empty())
		return;

	m_TerrainRen->InitTerrainHeight();

	std::list<NaviAreaData>::iterator Start = m_listAreaData.begin();
	std::list<NaviAreaData>::iterator End = m_listAreaData.end();


	for (;Start!= End ;++Start)
	{
		IVec2 X;
		IVec2 Z;

		Vec2 R = Start->AreaAxisX * Start->AreaSize.x * 0.5f;
		Vec2 F = Start->AreaAxisZ * Start->AreaSize.y * 0.5f;

		float HW = DXMath::ABS(R.x) + DXMath::ABS(F.x);
		float HH = DXMath::ABS(R.y) + DXMath::ABS(F.y);
		//X.ix = (int)((Start->AreaPos.x - Start->AreaSize.x * 0.5f) / m_TerrainRen->m_CellSize);
		//X.iy = (int)((Start->AreaPos.x + Start->AreaSize.x * 0.5f) / m_TerrainRen->m_CellSize);

		//Z.ix = (int)((Start->AreaPos.y - Start->AreaSize.y * 0.5f) / m_TerrainRen->m_CellSize);
		//Z.iy = (int)((Start->AreaPos.y + Start->AreaSize.y * 0.5f) / m_TerrainRen->m_CellSize);

		X.ix = (int)((Start->AreaPos.x - HW) / m_TerrainRen->m_CellSize);
		X.iy = (int)((Start->AreaPos.x + HW) / m_TerrainRen->m_CellSize);

		Z.ix = (int)((Start->AreaPos.y - HH) / m_TerrainRen->m_CellSize);
		Z.iy = (int)((Start->AreaPos.y + HH) / m_TerrainRen->m_CellSize);

		X.ix = DXMath::Clamp(X.ix, 0, (int)m_TerrainRen->m_TerrainBuffer.SizeX - 1);
		X.iy = DXMath::Clamp(X.iy, 0, (int)m_TerrainRen->m_TerrainBuffer.SizeX - 1);

		Z.ix = DXMath::Clamp(Z.ix, 0, (int)m_TerrainRen->m_TerrainBuffer.SizeZ - 1);
		Z.iy = DXMath::Clamp(Z.iy, 0, (int)m_TerrainRen->m_TerrainBuffer.SizeZ - 1);


		switch ((NAVIAREATYPE)(Start->AreaType))
		{
		case NAVIAREA_RECT: 
		{
			Vec2 AreaLB, AreaLT, AreaRB, AreaRT;
			AreaLB = Start->AreaPos - R - F;
			AreaLT = Start->AreaPos - R + F;
			AreaRB = Start->AreaPos + R - F;
			AreaRT = Start->AreaPos + R + F;

			for (int z = Z.ix; z <= Z.iy; ++z)
			{
				for (int x = X.ix; x <= X.iy; ++x)
				{
					//m_TerrainRen->NaviRectAreaCollisionEvent(*Start, x, z);
					m_TerrainRen->NaviRectAreaCollisionEvent(AreaLB, AreaLT, AreaRT, AreaRB ,x, z);
				}
			}

		}
			break;
		case NAVIAREA_CIRCLE: 
		{
			for (int z = Z.ix; z <= Z.iy; ++z)
			{
				for (int x = X.ix; x <= X.iy; ++x)
				{
					m_TerrainRen->NaviCircleAreaCollisionEvent(*Start, x, z);
				}
			}
		}
			break;
		default:
			TASSERT(true);
			break;
		}

	}

	m_TerrainRen->m_TerrainGeoMesh->VBUpdate(0,(void*)&m_TerrainRen->m_vecTerrainGeoVtx[0]);
	m_listAreaData.clear();
}

void CTerrain::InitTerrainHeight()
{
	m_TerrainRen->InitTerrainHeight();
}

void CTerrain::CreateTerrain(const wchar_t* _Tex, const wchar_t* _BaseDiffuseTex, const wchar_t* _BaseBumpTex, int _X, int _Z, float _Height /*= 3.f*/)
{
	m_TerrainRen->CreateTerrain(_Tex, _BaseDiffuseTex, _BaseBumpTex, _X, _Z, _Height);
}
void CTerrain::CreateTerrain( int _X, int _Z, float _Height)
{
	m_TerrainRen->CreateTerrain( _X, _Z, _Height);
}
void CTerrain::CreateFloor(const wchar_t* _MultiTexName) 
{
	m_TerrainRen->CreateFloor(_MultiTexName);
}
void CTerrain::SetCellSize(const float& _CellSize) 
{
	m_TerrainRen->SetCellSize(_CellSize);
	m_TerrainRen->SubTransUpdate();
}
void CTerrain::BakeNavigationMesh()
{
	m_TerrainRen->BakeNavigationMesh();
	InitNavigatorPosition();
}

void CTerrain::BakeNavigationMeshInServer() 
{
	m_TerrainRen->BakeNavigationMeshInServer();
	//InitNavigatorPosition();
}

void CTerrain::DeleteNavigationMesh()
{
	for (SPTR<CNavigator> Navi : m_listNavigator)
	{
		Navi->InitCurNaviMesh();
	}

	m_TerrainRen->DeleteNavigationMesh();
}
const bool CTerrain::IsNavigationBake()
{
	return (nullptr != m_TerrainRen->m_TerrainNaviMesh);
}
const bool CTerrain::CheckMoveablePosition(const Vec3& _Pos)
{
	return m_TerrainRen->CheckMoveablePosition(_Pos);
}
SPTR<CTerrainRenderer> CTerrain::GetTerrainRen()
{
	return m_TerrainRen;
}
const Vec3 CTerrain::GetMiddlePos()
{
	CBUFFER::TerrainBuffer& Buff = m_TerrainRen->m_TerrainBuffer;

	return Vec3(Buff.SizeX * m_TerrainRen->m_CellSize * 0.5f, 30.f, Buff.SizeZ * m_TerrainRen->m_CellSize* 0.5f);
}
const Vec3 CTerrain::GetTerrainSize()
{
	CBUFFER::TerrainBuffer& Buff = m_TerrainRen->m_TerrainBuffer;

	return Vec3(Buff.SizeX * m_TerrainRen->m_CellSize, 0.f, Buff.SizeZ * m_TerrainRen->m_CellSize);
}

void CTerrain::LinkNaviMeshArea(SPTR<CNaviMeshArea> _pArea)
{
	if (nullptr == _pArea)
		return;

	m_vecLinkArea.push_back(_pArea);
}
void CTerrain::SetTerrainColRay(SPTR<CRayCollider> _pRay)
{
	m_pTerrainTriRay = _pRay;
}
const bool CTerrain::IsTerrainRayCollsion()
{
	return m_pTerrainTriRay->IsCollision();
}
void CTerrain::LoadTerrainData(const TerrainData& _LoadData)
{
	m_TerrainRen->LoadTerrainData(_LoadData);
}
void CTerrain::LoadTerrainData(const struct TerrainData& _LoadData, std::list<SPTR<CNavigationMesh>>* _listNavi)
{
	m_TerrainRen->LoadTerrainData(_LoadData, _listNavi);
}

void CTerrain::PushNavigator(SPTR<CNavigator> _Navi)
{
	if (nullptr == _Navi)
		return;

	m_listNavigator.push_back(_Navi);
	m_TerrainRen->InitNavigatorPosition(_Navi);
}

void CTerrain::InitNavigatorPosition()
{
	std::list<SPTR<CNavigator>>::iterator Start = m_listNavigator.begin();
	std::list<SPTR<CNavigator>>::iterator End = m_listNavigator.end();

	for (; Start != End; ++Start) 
	{
		m_TerrainRen->InitNavigatorPosition(*Start);
	}

}