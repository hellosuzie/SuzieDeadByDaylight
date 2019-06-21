#include "NavigationObject.h"
#include <Actor.h>
#include <FbxRenderer.h>
#include <NavigationMesh.h>
#include <ResourceMgr.h>
#include <Mesh.h>
#include <NaviMeshArea.h>
#include <Terrain.h>
#include <Gizmo.h>


CNavigationObject::CNavigationObject()
	:m_DrawRen(nullptr), m_NaviRen(nullptr), m_NaviArea(nullptr), m_ObjGizmo(nullptr)
{
}


CNavigationObject::~CNavigationObject()
{
}

const bool CNavigationObject::Init()
{
	//m_DrawRen = ACTOR->AddCom<CStaticFbxRenderer>(RENDATA{ RENDER_LAYER_PLAYER });
	//m_DrawRen->SetFbx(L"SM_ST_OfficeRoom");
	//m_DrawRen->SettingFbxMesh(0);
	
	//m_NaviRen = ACTOR->AddCom<CNavigationMesh>(RENDATA{ RENDER_LAYER_DEBUG });
	//m_NaviRen->CreateNavigationMesh(CResourceMgr<COBJMesh>::Find(L"SM_ST_OfficeRoomNavi"));

	//m_ObjGizmo = ACTOR->AddCom<CGizmo>(RENDATA{RENDER_LAYER_DEBUG});
	return true;
}

SPTR<CFbxRenderer> CNavigationObject::CreateFbxRenderer(const RENDATA& _InitData) 
{
	return m_DrawRen = ACTOR->AddCom<CStaticFbxRenderer>(_InitData);
}
SPTR<CNavigationMesh> CNavigationObject::CreateNavigationRenderer()
{
	return m_NaviRen = ACTOR->AddCom<CNavigationMesh>(RENDATA{ RENDER_LAYER_DEBUG });
}
SPTR<CNavigationMesh> CNavigationObject::CreateNavigationRenderer(const NAVIMESH_DATA& _InitData)
{
	return m_NaviRen = ACTOR->AddCom<CNavigationMesh>(_InitData);
}

SPTR<class CNaviMeshArea> CNavigationObject::CreateNaviArea(SPTR<CTerrain> _pTerrain)
{
	return m_NaviArea = ACTOR->AddCom<CNaviMeshArea>(_pTerrain);
}
void CNavigationObject::SetDrawFbx(const wchar_t* _FbxKey)
{
	m_DrawRen->SetFbx(_FbxKey);
}
void CNavigationObject::SettingFbxMesh(const int& _MeshIdx)
{
	m_DrawRen->SettingFbxMesh(_MeshIdx);
}
void CNavigationObject::CreateNavigationMesh(const wchar_t* _Key, const NAVIMESH_TYPE& _MeshType)
{
	switch (_MeshType)
	{
	case NAVIMESH_OBJ:
		return m_NaviRen->CreateNavigationMesh(CResourceMgr<COBJMesh>::Find(_Key));
	case NAVIMESH_FBX:
		return;
	case MAX_NAVIMESH_TYPE:
		return;
	default:
		return;
	}

}

SPTR<CFbxRenderer> CNavigationObject::GetDrawRen()
{
	return m_DrawRen;
}
SPTR<CNavigationMesh> CNavigationObject::GetNaviRen() 
{
	return m_NaviRen;
}
SPTR<CNaviMeshArea>	CNavigationObject::GetNaviArea()
{
	return m_NaviArea;
}

void CNavigationObject::SetGizmoPivot(const Vec3& _Pivot)
{
	m_ObjGizmo->SetGizmoPivot(_Pivot);
}
