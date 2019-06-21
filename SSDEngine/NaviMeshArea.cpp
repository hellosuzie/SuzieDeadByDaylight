#include "NaviMeshArea.h"
#include "TransForm.h"
#include "Terrain.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"

bool		CNaviMeshArea::bAreaRender = true;

void CNaviMeshArea::ChangeAreaRendering()
{
	bAreaRender = !bAreaRender;
}
CNaviMeshArea::CNaviMeshArea()
	:m_bUpdateArea(false)
{
}


CNaviMeshArea::~CNaviMeshArea()
{
}
const bool CNaviMeshArea::Init(SPTR<CTerrain> _pTerrain)
{
	_pTerrain->LinkNaviMeshArea(this);
	PushMaterial(L"NaviAreaMaterial");
	PushMesh(L"FrameCube");
	PushMesh(L"Tube",1);
	SetColorFactor(Vec4::PastelMag);
	
	return CRenderer::Init(RENDATA{ RENDER_LAYER_DEBUG });
}
void CNaviMeshArea::PostUpdate(const float& _DeltaTime)
{
	if (true == TRANS->IsRotate()) 
	{
		m_bUpdateArea = true;

		size_t MaxArea = m_vecNaviArea.size();

		for (size_t i = 0; i < MaxArea; i++)
		{
			m_vecNaviArea[i].bChangeValue = true;
			m_vecNaviArea[i].AreaAxisX = TRANS->GetRight().XZ();
			m_vecNaviArea[i].AreaAxisZ = TRANS->GetFront().XZ();
		}
	}
}
void CNaviMeshArea::LastUpdate(const float& _DeltaTime)
{
	//m_bUpdateArea = false;
}
void CNaviMeshArea::Render(SPTR<CCamera> _Cam)
{
	if (false == bAreaRender)
	{
		return;
	}

	size_t MaxCount = m_vecNaviArea.size();

	for (size_t i = 0; i < MaxCount; i++)
	{
		SetRenPivot(Vec3(m_vecNaviArea[i].AreaPos.x, 0.f, m_vecNaviArea[i].AreaPos.y));
		SetRenSize(Vec3(m_vecNaviArea[i].AreaSize.x, 200.f, m_vecNaviArea[i].AreaSize.y));
		TRANS->CalSubTransWorld(m_pSubData);
		m_AreaBuffer.AreaWVP = m_pSubData->SubWorldMat;
		m_AreaBuffer.AreaWVP._42 = 0.f;
		m_AreaBuffer.AreaWVP *= _Cam->GetVPMatrix();
		m_AreaBuffer.AreaWVP.TransposeMat();

		GetMaterial(0)->SetConstantBufferData(L"NaviBuffer", m_AreaBuffer);
		GetMaterial(0)->Update();

		switch (m_vecNaviArea[i].AreaType)
		{
		case NAVIAREA_RECT: 
		{
			GetMesh(0)->Update();
			GetMesh(0)->Render();
		}
			break;
		case NAVIAREA_CIRCLE: 
		{
			GetMesh(1)->Update();
			GetMesh(1)->Render();
		}
			break;
		default:
		{
			TASSERT(true);
		}
		return;
		}

	}

	GetMaterial(0)->StateRollBack();
	GetMaterial(0)->TexSamRollBack();
}
const size_t CNaviMeshArea::AddAreaData(const NAVIAREATYPE& _Type, const Vec2& _Pivot, const Vec2& _Size)
{
	if (_Type >= MAX_NAVIAREATYPE)
	{
		return -1;
	}

	m_bUpdateArea = true;
	m_vecNaviArea.push_back(NaviAreaData{ (UINT)m_vecNaviArea.size(),(UINT)_Type,true,_Pivot,_Size , TRANS->GetRight().XZ(), TRANS->GetFront().XZ() });
	return m_vecNaviArea.size() - 1;

}
const size_t CNaviMeshArea::AddRectArea(const Vec2& _Pivot, const Vec2& _Size)
{
	m_bUpdateArea = true;
	m_vecNaviArea.push_back(NaviAreaData{ (UINT)m_vecNaviArea.size(),(UINT)NAVIAREA_RECT,true,_Pivot,_Size , TRANS->GetRight().XZ(), TRANS->GetFront().XZ() });
	return m_vecNaviArea.size() - 1;
}
const size_t CNaviMeshArea::AddCircleArea(const Vec2& _Pivot, const float& _Radius)
{
	m_bUpdateArea = true;
	m_vecNaviArea.push_back(NaviAreaData{ (UINT)m_vecNaviArea.size(),(UINT)NAVIAREA_CIRCLE,true,_Pivot,Vec2(_Radius * 2.f ,_Radius * 2.f) , TRANS->GetRight().XZ(), TRANS->GetFront().XZ() });
	return m_vecNaviArea.size() - 1;
}
void CNaviMeshArea::ModifyAreaPivot(const int& _Idx, const Vec2& _Pivot) 
{
	if (_Idx >= m_vecNaviArea.size())
		return;

	m_bUpdateArea = true;
	m_vecNaviArea[_Idx].bChangeValue = true;
	m_vecNaviArea[_Idx].AreaPos = _Pivot;
}
void CNaviMeshArea::ModifyAreaSize(const int& _Idx, const Vec2& _Size) 
{
	if (_Idx >= m_vecNaviArea.size())
		return;

	m_bUpdateArea = true;
	m_vecNaviArea[_Idx].bChangeValue = true;
	m_vecNaviArea[_Idx].AreaSize = _Size;
}
const bool CNaviMeshArea::IsMoveArea()
{
	return IsTransMove();
}
const bool CNaviMeshArea::IsUpdateArea() 
{
	return  m_bUpdateArea;
}
const size_t CNaviMeshArea::GetAreaCount()
{
	return m_vecNaviArea.size();
}
const NaviAreaData& CNaviMeshArea::GetAreaData(const int& _Idx)
{
	return m_vecNaviArea[_Idx];
}