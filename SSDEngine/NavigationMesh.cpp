#include "NavigationMesh.h"
#include "Actor.h"
#include "TransForm.h"
#include "ResourceMgr.h"
#include "Mesh.h"
#include "Camera.h"
#include "Device.h"
#include "DXMath.h"
#include "ResourceMgr.h"
#include "Fbx.h"
#include "Material.h"
#include "TerrainRenderer.h"
#include "Navigator.h"
#include "Material.h"
#include "MeshCollider.h"
#include "KeyMgr.h"
#include "SSDCore.h"
#include "GlobalDebug.h"

///////////////////////////////////////////////				Navigation Triangle				//////////////////////////

const bool _tagNAVITRIANLGE::TriangleTest(const Vec3& _Pos, const Mat& _MeshMat)
{
	Vec3 Pos = _Pos;
	Pos.y = INITNAVIHIEGHT;
	Vec3 T0, T1, T2;

	T0 = _MeshMat.Mul_Coord(Tri[0]);
	T1 = _MeshMat.Mul_Coord(Tri[1]);
	T2 = _MeshMat.Mul_Coord(Tri[2]);

	Ray = Vec3::Down3;
	RayPara = MAXNAVIHEIGHT;
	return DirectX::TriangleTests::Intersects(Pos, Ray, T0, T1, T2, RayPara);
}
const bool _tagNAVITRIANLGE::TriangleTest(const Vec3& _Pos)
{
	Vec3 Pos = _Pos;
	Pos.y = INITNAVIHIEGHT;
	Vec3 T0, T1, T2;

	T0 = Tri[0];
	T1 = Tri[1];
	T2 = Tri[2];

	Ray = Vec3::Down3;
	RayPara = MAXNAVIHEIGHT;
	return DirectX::TriangleTests::Intersects(Pos, Ray, T0, T1, T2, RayPara);
}
_tagNAVITRIANLGE* _tagNAVITRIANLGE::IsMelee(const Vec3& _Pos, const Mat& _MeshMat)
{
	for (int i = 0; i < AdjCount; i++)
	{
		if (true == pTriHead[AdjacentTriIdx[i]]->TriangleTest(_Pos, _MeshMat))
		{
			return pTriHead[AdjacentTriIdx[i]];
		}
	}

	std::list<int>::iterator Start = SharePointTriIdx.begin();
	std::list<int>::iterator End = SharePointTriIdx.end();

	for (; Start != End; ++Start)
	{
		if (true == pTriHead[*Start]->TriangleTest(_Pos, _MeshMat))
		{
			return pTriHead[*Start];
		}
	}

	return nullptr;
}


const float _tagNAVITRIANLGE::GetNavigationHeight(const Vec3& _Pos, const Mat& _MeshMat)
{
	Vec3 Pos = _Pos;
	Pos.y = INITNAVIHIEGHT;
	Vec3 T0, T1, T2;

	T0 = _MeshMat.Mul_Coord(Tri[0]);
	T1 = _MeshMat.Mul_Coord(Tri[1]);
	T2 = _MeshMat.Mul_Coord(Tri[2]);

	DirectX::TriangleTests::Intersects(Pos, Ray, T0, T1, T2, RayPara);
	float Y = (Pos + Ray * RayPara).y;

	if (Y >= 10000.f)
		int a = 0;
	return Y;

}

const bool _tagNAVITRIANLGE::IsIsolatedTri()
{
	return ((0 == AdjCount) && SharePointTriIdx.empty());
}

void _tagNAVITRIANLGE::PushLinkTri(_tagNAVITRIANLGE* _LinkTri)
{
	std::list<_tagNAVITRIANLGE*>::iterator Start = listLinkNaviTri.begin();
	std::list<_tagNAVITRIANLGE*>::iterator End = listLinkNaviTri.end();

	for (;Start!= End; ++Start)
	{
		if ((*Start) == _LinkTri) 
		{
			return;
		}
	}

	listLinkNaviTri.push_back(_LinkTri);
}


void _tagNAVITRIANLGE::InitCurNaviTriColor()
{

	if (false == listLinkNaviTri.empty())
	{
		std::list<NaviTri*>::iterator Start = listLinkNaviTri.begin();
		std::list<NaviTri*>::iterator End = listLinkNaviTri.end();

		for (; Start != End; ++Start)
		{
			(*Start)->pBelongNaviMesh->SetNaviTriColor((*Start)->Index, NOADJVTXCOLOR);
			/*	
			(*Start)->pBelongNaviMesh->m_GeoNaviVtx[(*Start)->Index].VtxColor = NOADJVTXCOLOR;
			if (nullptr != (*Start)->pBelongNaviMesh->m_GeoNaviMesh) 
			{
				(*Start)->pBelongNaviMesh->m_GeoNaviMesh->VBUpdate(0, &(*Start)->pBelongNaviMesh->m_GeoNaviVtx[0]);
			}
			*/
		}
	}

	pBelongNaviMesh->m_GeoNaviVtx[Index].VtxColor = NOADJVTXCOLOR;

	for (int i = 0; i < AdjCount; i++)
	{
		pBelongNaviMesh->m_GeoNaviVtx[AdjacentTriIdx[i]].VtxColor = NOADJVTXCOLOR;
	}

	if (false == SharePointTriIdx.empty())
	{
		std::list<int>::iterator Start = SharePointTriIdx.begin();
		std::list<int>::iterator End = SharePointTriIdx.end();

		for (; Start != End; ++Start)
		{
			pBelongNaviMesh->m_GeoNaviVtx[*Start].VtxColor = NOADJVTXCOLOR;
		}
	}

	pBelongNaviMesh->NaviMeshUpdate();
}

void _tagNAVITRIANLGE::ChangeNaviTriColor()
{
	if (false == listLinkNaviTri.empty())
	{
		std::list<NaviTri*>::iterator Start = listLinkNaviTri.begin();
		std::list<NaviTri*>::iterator End = listLinkNaviTri.end();

		for (; Start != End; ++Start)
		{
			(*Start)->pBelongNaviMesh->SetNaviTriColor((*Start)->Index, LINKTRICOLOR);

			/*		
			(*Start)->pBelongNaviMesh->m_GeoNaviVtx[(*Start)->Index].VtxColor = LINKTRICOLOR;
			if ((*Start)->pBelongNaviMesh->m_GeoNaviMesh != nullptr) 
			{
				(*Start)->pBelongNaviMesh->m_GeoNaviMesh->VBUpdate(0, &(*Start)->pBelongNaviMesh->m_GeoNaviVtx[0]);
			}
			*/
		}
	}

	pBelongNaviMesh->m_GeoNaviVtx[Index].VtxColor = CURVTXCOLOR;

	for (int i = 0; i < AdjCount; i++)
	{
		pBelongNaviMesh->m_GeoNaviVtx[AdjacentTriIdx[i]].VtxColor = ADJVTXCOLOR;
	}

	if (false == SharePointTriIdx.empty())
	{
		std::list<int>::iterator Start = SharePointTriIdx.begin();
		std::list<int>::iterator End = SharePointTriIdx.end();

		for (; Start != End; ++Start)
		{
			pBelongNaviMesh->m_GeoNaviVtx[*Start].VtxColor = ADJVTXCOLOR;
		}
	}

	pBelongNaviMesh->NaviMeshUpdate();
}

const bool _tagNAVITRIANLGE::CheckMoveable(const Vec3& _Pos)
{
	Vec3 Pos = _Pos;

	if (true == TriangleTest(Pos, pBelongNaviMesh->GetSubTransCRWorld()))
	{
		return true;
	}

	NaviTri* FindNTRI = IsMelee(Pos, pBelongNaviMesh->GetSubTransCRWorld());

	if (nullptr != FindNTRI)
	{
		InitCurNaviTriColor();
		FindNTRI->ChangeNaviTriColor();
		return true;
	}


	return false;
}
const bool _tagNAVITRIANLGE::CheckMoveable(const Vec3& _Pos, _tagNAVITRIANLGE** pMeleeTri)
{
	Vec3 Pos = _Pos;

	if (true == TriangleTest(Pos, pBelongNaviMesh->GetSubTransCRWorld()))
	{
		return true;
	}

	NaviTri* FindNTRI = IsMelee(Pos, pBelongNaviMesh->GetSubTransCRWorld());

	if (nullptr != FindNTRI)
	{
		InitCurNaviTriColor();
		FindNTRI->ChangeNaviTriColor();
		*pMeleeTri = FindNTRI;
		return true;
	}


	return false;
}


CNavigationMesh* _tagNAVITRIANLGE::CheckHandOverNaviMesh(const Vec3& _Pos, _tagNAVITRIANLGE** _pHandOverTri)
{
	if (true == TriangleTest(_Pos, pBelongNaviMesh->GetSubTransCRWorld()))
	{
		return nullptr;
	}

	std::list<NaviTri*>::iterator Start = listLinkNaviTri.begin();
	std::list<NaviTri*>::iterator End = listLinkNaviTri.end();

	for (; Start != End; ++Start)
	{
		CNavigationMesh* LinkMesh = (*Start)->pBelongNaviMesh;
		if (true == (*Start)->TriangleTest(_Pos, LinkMesh->GetSubTransCRWorld()))
		{
			InitCurNaviTriColor();
			(*Start)->ChangeNaviTriColor();
			*_pHandOverTri = (*Start);
			return (*Start)->pBelongNaviMesh;
		}

	}

	return nullptr;
}

///////////////////////////////////////////////				Navigation Mesh				//////////////////////////
IVec2						CNavigationMesh::LinkAdjTriMatrix[3][3] = { IVec2(0,0), };


CNavigationMesh::CNavigationMesh()
	: m_DrawNaviMesh(nullptr), m_NaviMeshCol(nullptr)
	, m_ColTriIdx(-1), m_NaviMeshClickCallBack(nullptr)
	, m_SelectTriIdx(-1)
	, m_SerialNumber(-1)
	, m_bNoClick(false)
{
	m_VtxColor[0] = BASEVTXCOLOR;
	m_VtxColor[1] = BASEVTXCOLOR;
	m_VtxColor[2] = BASEVTXCOLOR;

}


CNavigationMesh::~CNavigationMesh()
{
	//m_pCurTri = nullptr;
	m_NaviMeshCol = nullptr;

	for (size_t i = 0; i < m_vecNaviTri.size(); i++)
	{
		SAFE_DELETE(m_vecNaviTri[i]);
	}
	m_vecNaviTri.clear();

	Engine::DelNavigationMesh(this);
}
const bool CNavigationMesh::Init()
{
	m_bNoClick = true;
	return true;
}
const bool CNavigationMesh::Init(const RENDATA& _InitData)
{
	PushMaterial(L"NaviTriMaterial");
	//GetMaterial(0)->SetRasterizerState(L"WireFrame");
	GetMaterial(0)->SetRasterizerState(L"NoneCulling");
	SetColorFactor(Vec4(0.1f, 0.8f, 0.2f, 0.8f));

	Engine::AddNavigationMesh(this);

	return CRenderer::Init(_InitData);
}
const bool CNavigationMesh::Init(const NAVIMESH_DATA& _InitData)
{
	PushMaterial(L"NaviTriMaterial");
	GetMaterial(0)->SetRasterizerState(L"WireFrame");
	SetColorFactor(Vec4(0.1f, 0.8f, 0.2f, 0.8f));
	m_SerialNumber = _InitData.SerialNumber;

	if (nullptr == CResourceMgr<COBJMesh>::Find(_InitData.NaviObjName))
	{
		if (false == Engine::LoadNaviObjFile(_InitData.NaviObjName.c_str()))
		{
			TASSERT(true);
			return false;
		}
	}

	Engine::AddNavigationMesh(this);

	return CRenderer::Init(RENDATA{ _InitData.Layer });
}

void CNavigationMesh::SetNaviData(const NAVIMESH_DATA& _InitData)
{
	m_SerialNumber = _InitData.SerialNumber;

	SPTR<COBJMesh> ObjMesh;
	if (nullptr == CResourceMgr<COBJMesh>::Find(_InitData.NaviObjName))
	{
		if (false == Engine::LoadNaviObjFile(_InitData.NaviObjName.c_str()))
		{
			TASSERT(true);
		}

	}
	
	ObjMesh = CResourceMgr<COBJMesh>::Find(_InitData.NaviObjName);
	CreateNavigationTriangle(ObjMesh);

}
void CNavigationMesh::PreUpdate(const float& _DeltaTime) 
{

}
void CNavigationMesh::Update(const float& _DeltaTime)
{
	SubTransUpdate();
}
void CNavigationMesh::PostUpdate(const float& _DeltaTime)
{
	if (true == m_bNoClick)
		return;

	if (false == Core::CheckMouseOnScreen())
		return;

	if ( true == KEYDOWN("LB") && -1 != m_ColTriIdx && nullptr != m_NaviMeshClickCallBack)
	{
		m_NaviMeshClickCallBack(this);
	}
}
void CNavigationMesh::RenderUpdate(SPTR<CCamera> _Cam)
{
	m_BaseBuffer.World = GetSubTransCRWorld().GetTransposeMat();
	m_BaseBuffer.InvWorld = m_BaseBuffer.World.GetInverseMat().GetTransposeMat();
	m_BaseBuffer.NormalWorld = GetSubTransCRWorld().GetInverseMat();

	m_BaseBuffer.View = _Cam->GetViewMatrix().GetTransposeMat();
	m_BaseBuffer.Proj = _Cam->GetProjMatrix().GetTransposeMat();
	m_BaseBuffer.WV = m_BaseBuffer.View * m_BaseBuffer.World;
	m_BaseBuffer.NormalWV = m_BaseBuffer.WV.GetInverseMat().GetTransposeMat();

	m_BaseBuffer.VP = m_BaseBuffer.Proj * m_BaseBuffer.View;
	m_BaseBuffer.WVP = m_BaseBuffer.Proj * m_BaseBuffer.WV;

	m_BaseBuffer.ColorFactor = m_RenColorFactor;
	m_BaseBuffer.CamWorldPos = _Cam->GetTrans()->GetWorldPos();

	if (nullptr != CDevice::RenBaseCB)
		CDevice::RenBaseCB->Update(m_BaseBuffer);
}

void CNavigationMesh::Render(SPTR<CCamera> _Cam)
{
	m_vecMaterial[0]->Update();

	for (size_t i = 0; i < m_vecMesh.size(); i++)
	{
		m_vecMesh[i]->Update();
		m_vecMesh[i]->VtxRender(0);
	}

	m_vecMaterial[0]->StateRollBack();
	m_vecMaterial[0]->TexSamRollBack();


	if (m_SelectTriIdx != -1)
	{
		const Mat& World = GetSubTransCRWorld();
		Vec3 Vtx0, Vtx1, Vtx2;
		
		Vtx0 = World.Mul_Coord(m_GeoNaviVtx[m_SelectTriIdx].TriVtx[0].vec3);
		Vtx1 = World.Mul_Coord(m_GeoNaviVtx[m_SelectTriIdx].TriVtx[1].vec3);
		Vtx2 = World.Mul_Coord(m_GeoNaviVtx[m_SelectTriIdx].TriVtx[2].vec3);

		GlobalDebug::DrawDebugSphere(_Cam->GetVPMatrix(), Vtx0, 5.f, m_VtxColor[0]);
		GlobalDebug::DrawDebugSphere(_Cam->GetVPMatrix(), Vtx1, 5.f, m_VtxColor[1]);
		GlobalDebug::DrawDebugSphere(_Cam->GetVPMatrix(), Vtx2, 5.f, m_VtxColor[2]);
	}
}
NaviTri* CNavigationMesh::SearchNaviTri(const Vec3& _Pos)
{
	unsigned int MaxNaviCount = (unsigned int)m_vecNaviTri.size();

	for (unsigned int i = 0; i < MaxNaviCount; i++)
	{
		if (true == m_vecNaviTri[i]->TriangleTest(_Pos, GetSubTransCRWorld())) 
		{
			//ChangeCurNaviTri(m_vecNaviTri[i]);
			return m_vecNaviTri[i];
		}
	}

	return nullptr;
}

const bool CNavigationMesh::ExistNaviTri(const Vec3& _Pos)
{
	unsigned int MaxNaviCount = (unsigned int)m_vecNaviTri.size();

	for (unsigned int i = 0; i < MaxNaviCount; i++)
	{
		if (true == m_vecNaviTri[i]->TriangleTest(_Pos, GetSubTransCRWorld()))
		{
			return true;
		}
	}

	return false;
}
void CNavigationMesh::NaviMeshUpdate()
{
	if (nullptr == m_GeoNaviMesh)
		return;

	m_GeoNaviMesh->VBUpdate(0, &m_GeoNaviVtx[0]);
}
//const bool CNavigationMesh::CheckMoveable(const Vec3& _Pos)
//{
//	Vec3 Pos = _Pos;
//
//	if (true == m_pCurTri->TriangleTest(Pos, GetSubTransCRWorld()))
//	{
//		int a = 0;
//		return true;
//	}
//
//	NaviTri* FindNTRI = m_pCurTri->IsMelee(Pos, GetSubTransCRWorld());
//
//	if (nullptr != FindNTRI)
//	{
//		ChangeCurNaviTri(FindNTRI);
//		return true;
//	}
//	
//
//	return false;
//}
//
//void CNavigationMesh::MoveNavigaion(const Vec3& _Pos, SPTR<CTransForm> _NavigatorTrans)
//{
//	if (false == CheckMoveable(_Pos)) 
//	{
//		return;
//	}
//
//	_NavigatorTrans->SetPosY(m_pCurTri->GetNavigationHeight(_Pos, GetSubTransCRWorld()));
//}
//
//void CNavigationMesh::MoveNavigaion(const Vec3& _Pos, SPTR<CNavigator> _Navigator)
//{
//	CNavigationMesh* HandOverNavi = CheckHandOverNaviMesh(_Pos);
//
//	if (nullptr != HandOverNavi) 
//	{
//		m_pCurTri = nullptr;
//		_Navigator->m_CurNaviMesh = HandOverNavi;
//		_Navigator->RisingNavigator(_Navigator->m_CurNaviMesh->m_pCurTri->GetNavigationHeight(_Pos, _Navigator->m_CurNaviMesh->GetSubTransCRWorld()));
//		return;
//	}
//
//	if (false == CheckMoveable(_Pos))
//	{
//		_Navigator->CalculateMovablePos(_Pos, Mat(m_pCurTri->Tri[0].GetVec4_Coord(), m_pCurTri->Tri[1].GetVec4_Coord(), m_pCurTri->Tri[2].GetVec4_Coord())*GetSubTransCRWorld());
//		return;
//	}
//
//	_Navigator->RisingNavigator(m_pCurTri->GetNavigationHeight(_Pos, GetSubTransCRWorld()));
//
//}
//
//const float CNavigationMesh::GetNavigationHeight(const Vec3& _Pos)
//{
//	return m_pCurTri->GetNavigationHeight(_Pos, GetSubTransCRWorld());
//}
//
//CNavigationMesh* CNavigationMesh::CheckHandOverNaviMesh(const Vec3& _Pos)
//{
//	if (true == m_pCurTri->TriangleTest(_Pos, GetSubTransCRWorld())) 
//	{
//		return nullptr;
//	}
//
//	std::list<NaviTri*>::iterator Start = m_pCurTri->listLinkNaviTri.begin();
//	std::list<NaviTri*>::iterator End = m_pCurTri->listLinkNaviTri.end();
//
//	for (; Start != End; ++Start)
//	{
//		CNavigationMesh* LinkMesh = (*Start)->pBelongNaviMesh;
//		if (true == (*Start)->TriangleTest(_Pos, LinkMesh->GetSubTransCRWorld())) 
//		{
//			InitCurNaviTriColor();
//			(*Start)->pBelongNaviMesh->ChangeCurNaviTri((*Start));
//			return (*Start)->pBelongNaviMesh;
//		}
//
//	}
//
//	return nullptr;
//}
//void CNavigationMesh::ChangeCurNaviTri(NaviTri* _pTri)
//{
//	if (nullptr == _pTri)
//	{
//		TASSERT(true);
//		return;
//	}
//
//	if (nullptr != m_pCurTri)
//	{
//		if (false == m_pCurTri->listLinkNaviTri.empty())
//		{
//			std::list<NaviTri*>::iterator Start = m_pCurTri->listLinkNaviTri.begin();
//			std::list<NaviTri*>::iterator End = m_pCurTri->listLinkNaviTri.end();
//
//			for (; Start != End; ++Start)
//			{
//				(*Start)->pBelongNaviMesh->m_GeoNaviVtx[(*Start)->Index].VtxColor = NOADJVTXCOLOR;
//				(*Start)->pBelongNaviMesh->m_GeoNaviMesh->VBUpdate(0, &(*Start)->pBelongNaviMesh->m_GeoNaviVtx[0]);
//			}
//		}
//
//		m_GeoNaviVtx[m_pCurTri->Index].VtxColor = NOADJVTXCOLOR;
//
//		for (int i = 0; i < m_pCurTri->AdjCount; i++)
//		{
//			m_GeoNaviVtx[m_pCurTri->AdjacentTriIdx[i]].VtxColor = NOADJVTXCOLOR;
//		}
//
//		if (false == m_pCurTri->SharePointTriIdx.empty())
//		{
//			std::list<int>::iterator Start = m_pCurTri->SharePointTriIdx.begin();
//			std::list<int>::iterator End = m_pCurTri->SharePointTriIdx.end();
//
//			for ( ; Start != End ; ++Start)
//			{
//				m_GeoNaviVtx[*Start].VtxColor = NOADJVTXCOLOR;
//			}
//		}
//	}
//
//	m_pCurTri = _pTri;
//
//	if (nullptr != m_pCurTri)
//	{
//		if (false == m_pCurTri->listLinkNaviTri.empty())
//		{
//			std::list<NaviTri*>::iterator Start = m_pCurTri->listLinkNaviTri.begin();
//			std::list<NaviTri*>::iterator End = m_pCurTri->listLinkNaviTri.end();
//
//			for (; Start != End; ++Start)
//			{
//				(*Start)->pBelongNaviMesh->m_GeoNaviVtx[(*Start)->Index].VtxColor = LINKTRICOLOR;
//				(*Start)->pBelongNaviMesh->m_GeoNaviMesh->VBUpdate(0, &(*Start)->pBelongNaviMesh->m_GeoNaviVtx[0]);
//			}
//		}
//
//		m_GeoNaviVtx[m_pCurTri->Index].VtxColor = CURVTXCOLOR;
//
//		for (int i = 0; i < m_pCurTri->AdjCount; i++)
//		{
//			m_GeoNaviVtx[m_pCurTri->AdjacentTriIdx[i]].VtxColor = ADJVTXCOLOR;
//		}
//
//		if (false == m_pCurTri->SharePointTriIdx.empty())
//		{
//			std::list<int>::iterator Start = m_pCurTri->SharePointTriIdx.begin();
//			std::list<int>::iterator End = m_pCurTri->SharePointTriIdx.end();
//
//			for (; Start != End; ++Start)
//			{
//				m_GeoNaviVtx[*Start].VtxColor = ADJVTXCOLOR;
//			}
//		}
//	}
//
//	m_GeoNaviMesh->VBUpdate(0, &m_GeoNaviVtx[0]);
//
//}
//
//void CNavigationMesh::InitCurNaviTriColor()
//{
//	if (nullptr != m_pCurTri)
//	{
//		if (false == m_pCurTri->listLinkNaviTri.empty())
//		{
//			std::list<NaviTri*>::iterator Start = m_pCurTri->listLinkNaviTri.begin();
//			std::list<NaviTri*>::iterator End = m_pCurTri->listLinkNaviTri.end();
//
//			for (; Start != End; ++Start)
//			{
//				(*Start)->pBelongNaviMesh->m_GeoNaviVtx[(*Start)->Index].VtxColor = NOADJVTXCOLOR;
//				(*Start)->pBelongNaviMesh->m_GeoNaviMesh->VBUpdate(0, &(*Start)->pBelongNaviMesh->m_GeoNaviVtx[0]);
//			}
//		}
//
//		m_GeoNaviVtx[m_pCurTri->Index].VtxColor = NOADJVTXCOLOR;
//
//		for (int i = 0; i < m_pCurTri->AdjCount; i++)
//		{
//			m_GeoNaviVtx[m_pCurTri->AdjacentTriIdx[i]].VtxColor = NOADJVTXCOLOR;
//		}
//
//		if (false == m_pCurTri->SharePointTriIdx.empty())
//		{
//			std::list<int>::iterator Start = m_pCurTri->SharePointTriIdx.begin();
//			std::list<int>::iterator End = m_pCurTri->SharePointTriIdx.end();
//
//			for (; Start != End; ++Start)
//			{
//				m_GeoNaviVtx[*Start].VtxColor = NOADJVTXCOLOR;
//			}
//		}
//
//	}
//
//}
void CNavigationMesh::CreateNavigationMesh(std::vector<Vec3>& _Vtx, std::vector<IDX32>& _Idx, const float& _Min, const float& _Max)
{
	 m_vecNaviTri.resize(_Idx.size());
	 unsigned  int NeviCount = (unsigned  int)m_vecNaviTri.size();

	for (unsigned  int i = 0; i < NeviCount; ++i)
	{
		m_vecNaviTri[i] = new NaviTri();
		m_vecNaviTri[i]->pTriHead = &m_vecNaviTri[0];
		m_vecNaviTri[i]->Index = i;
		m_vecNaviTri[i]->Tri[0] = _Vtx[_Idx[i]._1];
		m_vecNaviTri[i]->Tri[1] = _Vtx[_Idx[i]._2];
		m_vecNaviTri[i]->Tri[2] = _Vtx[_Idx[i]._3];
	}

	int Count = 0;
	int Adj = 0;
	
	for (unsigned int i = 0; i < NeviCount; ++i)
	{
		for (unsigned int j = i + 1; j < NeviCount; ++j)
		{
			for (size_t x = 0; x < 3; ++x)
			{
				bool Check = false;

				for (size_t y = 0; y < 3 && false == Check ; ++y)
				{
					if (_Idx[i].pIdx[x] == _Idx[j].pIdx[y])
					{
						++Count;
						Check = true;
						continue;
					}	

				}
			}

			if ( 2 == Count ) 
			{
				m_vecNaviTri[i]->AdjacentTriIdx[m_vecNaviTri[i]->AdjCount] = j;
				m_vecNaviTri[j]->AdjacentTriIdx[m_vecNaviTri[j]->AdjCount] = i;
				++m_vecNaviTri[i]->AdjCount;
				++m_vecNaviTri[j]->AdjCount;
			}
			else if (1 == Count) 
			{
				m_vecNaviTri[i]->SharePointTriIdx.push_back(j);
				m_vecNaviTri[j]->SharePointTriIdx.push_back(i);
			}

			Count = 0;
		}
	}

	m_DrawNaviMesh = Engine::CreateResourceObject<CMesh>();

	COLORVTX* TempVtx = new COLORVTX[_Vtx.size()];
	ZeroMemory(TempVtx, sizeof(COLORVTX) *_Vtx.size());

	for (size_t i = 0; i < _Vtx.size(); i++)
	{
		TempVtx[i].Pos = _Vtx[i];
	}

	CMesh::CREATE_DESC Desc = { TempVtx, sizeof(COLORVTX) ,  (UINT)_Vtx.size() ,D3D11_USAGE_DYNAMIC
		, &_Idx[0] ,IDX32::MemSize() ,(UINT)(3 * _Idx.size()) ,D3D11_USAGE_DYNAMIC ,IDX32::Format() ,D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
	m_DrawNaviMesh =  CResourceMgr<CMesh>::Create( Desc);

	PushMesh(m_DrawNaviMesh, 0);

	delete[] TempVtx;

}


void CNavigationMesh::CreateNavigationMesh(SPTR<CTerrainRenderer> _Terrain, const float& _NaviHeight)
{
	if (nullptr == _Terrain)
		return;

	this->SetRenSize(_Terrain->GetRenSize());
	IVec2 TerCount = IVec2((int)_Terrain->m_TerrainBuffer.SizeX, (int)_Terrain->m_TerrainBuffer.SizeZ);
	// 1. 네비게이션 메쉬의 삼각형 목록을 먼저 제작
	m_vecNaviTri.resize(_Terrain->m_vecTerrainGeoVtx.size() * 2);

	int MaxTerrain = (int)_Terrain->m_vecTerrainGeoVtx.size();
	
	for (int i = 0; i < MaxTerrain; i++)
	{
		// Idx1 : 터레인 하부 삼각형 , Idx2 : 터레인 상부 삼각형
		int Idx1 = 2 * i, Idx2 = 2 * i + 1;
		int X = (int)_Terrain->m_vecTerrainGeoVtx[i].TerrainIdx.x;
		int Z = (int)_Terrain->m_vecTerrainGeoVtx[i].TerrainIdx.y;

		m_vecNaviTri[Idx1] = new NaviTri(this);
		m_vecNaviTri[Idx2] = new NaviTri(this);

		// 하부 삼각형(LB LT RB) 정보 채우기
		m_vecNaviTri[Idx1]->Index = Idx1;
		m_vecNaviTri[Idx1]->pTriHead = &m_vecNaviTri[0];
		m_vecNaviTri[Idx1]->Tri[0] = Vec3((float)X, _Terrain->m_vecTerrainGeoVtx[i].TerrainHeight.x + _NaviHeight, (float)Z);
		m_vecNaviTri[Idx1]->Tri[1] = Vec3((float)X, _Terrain->m_vecTerrainGeoVtx[i].TerrainHeight.y + _NaviHeight, (float)(Z + 1) );
		m_vecNaviTri[Idx1]->Tri[2] = Vec3((float)(X + 1 ) , _Terrain->m_vecTerrainGeoVtx[i].TerrainHeight.w + _NaviHeight, (float)Z );

		// 상부 삼각형(LT RT RB) 정보 채우기
		m_vecNaviTri[Idx2]->Index = Idx2;
		m_vecNaviTri[Idx2]->pTriHead = &m_vecNaviTri[0];
		m_vecNaviTri[Idx2]->Tri[0] = Vec3((float)X, _Terrain->m_vecTerrainGeoVtx[i].TerrainHeight.y + _NaviHeight, (float)(Z + 1));
		m_vecNaviTri[Idx2]->Tri[1] = Vec3((float)(X + 1), _Terrain->m_vecTerrainGeoVtx[i].TerrainHeight.z + _NaviHeight, (float)(Z + 1));
		m_vecNaviTri[Idx2]->Tri[2] = Vec3((float)(X + 1), _Terrain->m_vecTerrainGeoVtx[i].TerrainHeight.w + _NaviHeight, (float)Z );

		// 상부, 하부 삼각형들의 인접 정보를 채운다.
		int GeoIdx = 0;

		if (false == _Terrain->CheckGenerableNaviTri(i))
			continue;
	
		m_vecNaviTri[Idx1]->AdjacentTriIdx[m_vecNaviTri[Idx1]->AdjCount++] = Idx2;
		m_vecNaviTri[Idx2]->AdjacentTriIdx[m_vecNaviTri[Idx2]->AdjCount++] = Idx1;

		if (X  >  0)
		{
			GeoIdx = (X - 1) + Z *(TerCount.ix);
			
			if (_Terrain->CheckGenerableNaviTri(GeoIdx))
			{
				m_vecNaviTri[Idx1]->AdjacentTriIdx[m_vecNaviTri[Idx1]->AdjCount++] = 2 * GeoIdx + 1;
				m_vecNaviTri[Idx1]->SharePointTriIdx.push_back(2 * GeoIdx);

				m_vecNaviTri[Idx2]->SharePointTriIdx.push_back(2 * GeoIdx + 1);
			}

			if (Z > 0)
			{
				GeoIdx = (X - 1) + (Z - 1) * (TerCount.ix);

				if (_Terrain->CheckGenerableNaviTri(GeoIdx))
				{
					m_vecNaviTri[Idx1]->SharePointTriIdx.push_back(2 * GeoIdx + 1 );
				}

			}

			if (Z < TerCount.iy - 1 )
			{
				GeoIdx = (X - 1) + (Z + 1) * (TerCount.ix);

				if (_Terrain->CheckGenerableNaviTri(GeoIdx))
				{
					m_vecNaviTri[Idx1]->SharePointTriIdx.push_back(2 * GeoIdx);
					m_vecNaviTri[Idx1]->SharePointTriIdx.push_back(2 * GeoIdx + 1);

					m_vecNaviTri[Idx2]->SharePointTriIdx.push_back(2 * GeoIdx);
					m_vecNaviTri[Idx2]->SharePointTriIdx.push_back(2 * GeoIdx + 1);

				}
			}
		}

		if (X < TerCount.ix - 1  )
		{
			GeoIdx = (X + 1) + Z *(TerCount.ix);

			if (_Terrain->CheckGenerableNaviTri(GeoIdx))
			{
				m_vecNaviTri[Idx2]->AdjacentTriIdx[m_vecNaviTri[Idx2]->AdjCount++] = 2 * GeoIdx;
				m_vecNaviTri[Idx2]->SharePointTriIdx.push_back(2 * GeoIdx + 1);

				m_vecNaviTri[Idx1]->SharePointTriIdx.push_back(2 * GeoIdx );
			}

			if (Z > 0)
			{
				GeoIdx = (X + 1) + (Z - 1) * (TerCount.ix);

				if (_Terrain->CheckGenerableNaviTri(GeoIdx))
				{
					m_vecNaviTri[Idx1]->SharePointTriIdx.push_back(2 * GeoIdx);
					m_vecNaviTri[Idx1]->SharePointTriIdx.push_back(2 * GeoIdx + 1);

					m_vecNaviTri[Idx2]->SharePointTriIdx.push_back(2 * GeoIdx);
					m_vecNaviTri[Idx2]->SharePointTriIdx.push_back(2 * GeoIdx + 1);
				}

			}

			if (Z < TerCount.iy - 1 )
			{
				GeoIdx = (X + 1) + (Z + 1) * (TerCount.ix);

				if (_Terrain->CheckGenerableNaviTri(GeoIdx))
				{
					m_vecNaviTri[Idx2]->SharePointTriIdx.push_back(2 * GeoIdx );
				}
			}
		}

		if (Z > 0) 
		{
			GeoIdx = X + (Z - 1) * (TerCount.ix);

			if (_Terrain->CheckGenerableNaviTri(GeoIdx))
			{
				m_vecNaviTri[Idx1]->AdjacentTriIdx[m_vecNaviTri[Idx1]->AdjCount++] = 2 * GeoIdx + 1;
				m_vecNaviTri[Idx1]->SharePointTriIdx.push_back(2 * GeoIdx);

				m_vecNaviTri[Idx2]->SharePointTriIdx.push_back(2 * GeoIdx + 1);
			}
		}

		if (Z < TerCount.iy - 1 )
		{
			GeoIdx = X + (Z + 1) * (TerCount.ix);

			if (_Terrain->CheckGenerableNaviTri(GeoIdx))
			{
				m_vecNaviTri[Idx2]->AdjacentTriIdx[m_vecNaviTri[Idx2]->AdjCount++] = 2 * GeoIdx ;
				m_vecNaviTri[Idx2]->SharePointTriIdx.push_back(2 * GeoIdx + 1 );

				m_vecNaviTri[Idx1]->SharePointTriIdx.push_back(2 * GeoIdx);
			}
		}
	}

	// 2. 디버깅용 네비게이션 메쉬 삼각형 제작하기
	m_GeoNaviVtx.resize(m_vecNaviTri.size());
	size_t MaxTriCount = m_GeoNaviVtx.size();
	
	for (size_t i = 0; i < MaxTriCount; i++)
	{
		m_GeoNaviVtx[i].TriVtx[0].vec3 = m_vecNaviTri[i]->Tri[0];
		m_GeoNaviVtx[i].TriVtx[1].vec3 = m_vecNaviTri[i]->Tri[1];
		m_GeoNaviVtx[i].TriVtx[2].vec3 = m_vecNaviTri[i]->Tri[2];

		if (true == m_vecNaviTri[i]->IsIsolatedTri()) 
		{
			m_GeoNaviVtx[i].VtxColor = NOENTERTRI;
		}
		else 
		{
			m_GeoNaviVtx[i].VtxColor = NOADJVTXCOLOR;
		}
	}

	CMesh::CREATE_DESC Desc = { &m_GeoNaviVtx[0], sizeof(NAVITRIVTX) ,  (UINT)m_GeoNaviVtx.size() ,D3D11_USAGE_DYNAMIC
		, nullptr ,IDX32::MemSize() ,0 ,D3D11_USAGE_DYNAMIC ,IDX32::Format() ,D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_POINTLIST };

	m_GeoNaviMesh = CResourceMgr<CMesh>::Create(Desc);
	m_vecMesh.push_back(m_GeoNaviMesh);

}
void CNavigationMesh::CreateNavigationMesh(SPTR<COBJMesh> _pObjMesh)
{
	if (nullptr != m_GeoNaviMesh) 
	{
		m_GeoNaviVtx.clear();
		m_GeoNaviMesh = nullptr;
		m_vecMesh.clear();
	}

	SetName(_pObjMesh->GetName());

	m_GeoNaviVtx.reserve(_pObjMesh->m_vecIdx.size());
	size_t MaxTri = _pObjMesh->m_vecIdx.size();

	Vec4 T1, T2, T3;

	for (size_t i = 0; i < MaxTri; i++)
	{
		T1.vec3 = _pObjMesh->m_vecColorVtx[_pObjMesh->m_vecIdx[i]._1].Pos;
		T2.vec3 = _pObjMesh->m_vecColorVtx[_pObjMesh->m_vecIdx[i]._2].Pos;
		T3.vec3 = _pObjMesh->m_vecColorVtx[_pObjMesh->m_vecIdx[i]._3].Pos;

		T1.iw = (int)_pObjMesh->m_vecIdx[i]._1;
		T2.iw = (int)_pObjMesh->m_vecIdx[i]._2;
		T3.iw = (int)_pObjMesh->m_vecIdx[i]._3;

		m_GeoNaviVtx.push_back(NAVITRIVTX{ T1,T2,T3, NOADJVTXCOLOR });
	}

	CMesh::CREATE_DESC Desc = { &m_GeoNaviVtx[0], sizeof(NAVITRIVTX) ,  (UINT)m_GeoNaviVtx.size() ,D3D11_USAGE_DYNAMIC
		, nullptr ,IDX32::MemSize() ,0 ,D3D11_USAGE_DYNAMIC ,IDX32::Format() ,D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_POINTLIST };
	
	m_GeoNaviMesh = CResourceMgr<CMesh>::Create(Desc);
	m_GeoNaviMesh->SetName(_pObjMesh->GetName());

	m_vecMesh.push_back(m_GeoNaviMesh);

	m_vecNaviTri.resize(m_GeoNaviVtx.size());
	unsigned  int NeviCount = (unsigned  int)m_GeoNaviVtx.size();

	for (unsigned int i = 0; i < NeviCount; ++i)
	{
		m_vecNaviTri[i] = new NaviTri(this);
		m_vecNaviTri[i]->pTriHead = &m_vecNaviTri[0];
		m_vecNaviTri[i]->Index = i;
		m_vecNaviTri[i]->Tri[0] = m_GeoNaviVtx[i].TriVtx[0].vec3;
		m_vecNaviTri[i]->Tri[1] = m_GeoNaviVtx[i].TriVtx[1].vec3;
		m_vecNaviTri[i]->Tri[2] = m_GeoNaviVtx[i].TriVtx[2].vec3;
	}

	// 그걸 가지고 모든 삼각형 다돌면서 인접 삼각형을 계산한다.
	// 포인터로 들고있지말고. 인덱스로 들고있는게 좋다.

	int Count = 0;
	int Adj = 0;

	for (unsigned int i = 0; i < NeviCount; ++i)
	{
		for (unsigned int j = i + 1; j < NeviCount; ++j)
		{
			for (size_t x = 0; x < 3; ++x)
			{
				bool Check = false;

				for (size_t y = 0; y < 3 && false == Check; ++y)
				{
					if (m_GeoNaviVtx[i].TriVtx[x].iw == m_GeoNaviVtx[j].TriVtx[y].iw)
					{
						++Count;
						Check = true;
						continue;
					}

				}
			}

			if (2 == Count)
			{
				m_vecNaviTri[i]->AdjacentTriIdx[m_vecNaviTri[i]->AdjCount] = j;
				m_vecNaviTri[j]->AdjacentTriIdx[m_vecNaviTri[j]->AdjCount] = i;
				++m_vecNaviTri[i]->AdjCount;
				++m_vecNaviTri[j]->AdjCount;
			}
			else if (1 == Count)
			{
				m_vecNaviTri[i]->SharePointTriIdx.push_back(j);
				m_vecNaviTri[j]->SharePointTriIdx.push_back(i);
			}

			Count = 0;
		}
	}

	if (nullptr != m_NaviMeshCol) 
	{
		m_NaviMeshCol->SetColTri(this);
	}
}

void CNavigationMesh::CreateNavigationTriangle(SPTR<COBJMesh> _pObjMesh)
{
	SetName(_pObjMesh->GetName());

	m_GeoNaviVtx.reserve(_pObjMesh->m_vecIdx.size());
	size_t MaxTri = _pObjMesh->m_vecIdx.size();

	Vec4 T1, T2, T3;

	for (size_t i = 0; i < MaxTri; i++)
	{
		T1.vec3 = _pObjMesh->m_vecColorVtx[_pObjMesh->m_vecIdx[i]._1].Pos;
		T2.vec3 = _pObjMesh->m_vecColorVtx[_pObjMesh->m_vecIdx[i]._2].Pos;
		T3.vec3 = _pObjMesh->m_vecColorVtx[_pObjMesh->m_vecIdx[i]._3].Pos;

		T1.iw = (int)_pObjMesh->m_vecIdx[i]._1;
		T2.iw = (int)_pObjMesh->m_vecIdx[i]._2;
		T3.iw = (int)_pObjMesh->m_vecIdx[i]._3;

		m_GeoNaviVtx.push_back(NAVITRIVTX{ T1,T2,T3, NOADJVTXCOLOR });
	}

	m_vecNaviTri.resize(m_GeoNaviVtx.size());
	unsigned  int NeviCount = (unsigned  int)m_GeoNaviVtx.size();

	for (unsigned int i = 0; i < NeviCount; ++i)
	{
		m_vecNaviTri[i] = new NaviTri(this);
		m_vecNaviTri[i]->pTriHead = &m_vecNaviTri[0];
		m_vecNaviTri[i]->Index = i;
		m_vecNaviTri[i]->Tri[0] = m_GeoNaviVtx[i].TriVtx[0].vec3;
		m_vecNaviTri[i]->Tri[1] = m_GeoNaviVtx[i].TriVtx[1].vec3;
		m_vecNaviTri[i]->Tri[2] = m_GeoNaviVtx[i].TriVtx[2].vec3;
	}

	// 그걸 가지고 모든 삼각형 다돌면서 인접 삼각형을 계산한다.
	// 포인터로 들고있지말고. 인덱스로 들고있는게 좋다.

	int Count = 0;
	int Adj = 0;

	for (unsigned int i = 0; i < NeviCount; ++i)
	{
		for (unsigned int j = i + 1; j < NeviCount; ++j)
		{
			for (size_t x = 0; x < 3; ++x)
			{
				bool Check = false;

				for (size_t y = 0; y < 3 && false == Check; ++y)
				{
					if (m_GeoNaviVtx[i].TriVtx[x].iw == m_GeoNaviVtx[j].TriVtx[y].iw)
					{
						++Count;
						Check = true;
						continue;
					}

				}
			}

			if (2 == Count)
			{
				m_vecNaviTri[i]->AdjacentTriIdx[m_vecNaviTri[i]->AdjCount] = j;
				m_vecNaviTri[j]->AdjacentTriIdx[m_vecNaviTri[j]->AdjCount] = i;
				++m_vecNaviTri[i]->AdjCount;
				++m_vecNaviTri[j]->AdjCount;
			}
			else if (1 == Count)
			{
				m_vecNaviTri[i]->SharePointTriIdx.push_back(j);
				m_vecNaviTri[j]->SharePointTriIdx.push_back(i);
			}

			Count = 0;
		}
	}

}
void CNavigationMesh::CreateLinkNavigationMesh(std::list<SPTR<CNaviLinkTri>>* _LinkList, SPTR<CTerrainRenderer> _Terrain)
{
	this->SetRenSize(_Terrain->GetRenSize());
	SubTransUpdate();
	Mat InvWorld = GetSubTransCRWorld().GetInverseMat();

	m_vecNaviTri.resize(_LinkList->size());

	std::list<SPTR<CNaviLinkTri>>::iterator Start = _LinkList->begin();
	std::list<SPTR<CNaviLinkTri>>::iterator End = _LinkList->end();

	UINT TriCount = 0;
	IVec2 TerIdx = IVec2();


	for (; Start != End; ++Start)
	{
		memset(LinkAdjTriMatrix, 0, sizeof(LinkAdjTriMatrix));

		TerIdx = (*Start)->GetLinkTerrainIdx();

		m_vecNaviTri[TriCount] = new NaviTri(this);
		m_vecNaviTri[TriCount]->pTriHead = &m_vecNaviTri[0];
		m_vecNaviTri[TriCount]->Index = TriCount;

		IVec4 MeshLink = IVec4(-1, -1, -1, -1);
		int Count = 0;

		for (size_t i = 0; i < 3; i++)
		{
			// 연결된 정점이 네비게이션 메쉬 위의 정점일때
			if (true == (*Start)->m_LinkVtx[i].bNaviVtx) 
			{
				m_vecNaviTri[TriCount]->Tri[i] = InvWorld.Mul_Coord((*Start)->m_LinkVtx[i].VtxPos);

				MeshLink.pi[Count] = (*Start)->GetLinkNaviTriIdx();
				MeshLink.pi[Count + 1] = (*Start)->m_LinkVtx[i].VtxID;
				Count += 2;
				//(*Start)->GetLinkNaviTri()->PushLinkTri(m_vecNaviTri[TriCount]);
				//m_vecNaviTri[TriCount]->PushLinkTri((*Start)->GetLinkNaviTri());
				
			}
			// 연결된 정점이 테레인위의 점일때
			else 
			{
				// Matrix ix = 하부 삼각형 iy = 상부 삼각형
				switch ((*Start)->m_LinkVtx[i].VtxID)
				{
					//LB
				case 0:
				{
					++LinkAdjTriMatrix[1][1].ix;
					++LinkAdjTriMatrix[1][0].ix;
					++LinkAdjTriMatrix[1][0].iy;
					++LinkAdjTriMatrix[2][1].ix;
					++LinkAdjTriMatrix[2][1].iy;
					++LinkAdjTriMatrix[2][0].iy;
				}
					break;
					//LT
				case 1:
				{
					++LinkAdjTriMatrix[1][1].ix;
					++LinkAdjTriMatrix[1][1].iy;
					++LinkAdjTriMatrix[1][0].iy;
					++LinkAdjTriMatrix[0][0].ix;
					++LinkAdjTriMatrix[0][0].iy;
					++LinkAdjTriMatrix[0][1].ix;
				}
					break;
					//RT
				case 2:
				{
					++LinkAdjTriMatrix[1][1].iy;
					++LinkAdjTriMatrix[0][1].ix;
					++LinkAdjTriMatrix[0][1].iy;
					++LinkAdjTriMatrix[0][2].ix;
					++LinkAdjTriMatrix[1][2].ix;
					++LinkAdjTriMatrix[1][2].iy;
				}
					break;
					//RB 
				case 3:
				{
					++LinkAdjTriMatrix[1][1].ix;
					++LinkAdjTriMatrix[1][1].iy;
					++LinkAdjTriMatrix[1][2].ix;
					++LinkAdjTriMatrix[2][2].ix;
					++LinkAdjTriMatrix[2][2].iy;
					++LinkAdjTriMatrix[2][1].iy;
				}
					break;
				default:
					break;
				}

				Vec3 Pos = _Terrain->GetNaviTerrainVtxPos(TerIdx, (*Start)->m_LinkVtx[i].VtxID);
				m_vecNaviTri[TriCount]->Tri[i] = InvWorld.Mul_Coord(Pos);	
			}
		}


		// 네비게이션 메쉬쪽의 삼각형과 자신을 링크
		std::list<NaviTri*> TriList;
		std::list<NaviTri*>::iterator TriStart;
		std::list<NaviTri*>::iterator TriEnd;
		(*Start)->GetLinkNaviMesh()->FindAllConnectedTri(&TriList, MeshLink.pi);
		
		TriStart = TriList.begin();
		TriEnd = TriList.end();

		for (; TriStart != TriEnd; ++TriStart)
		{
			m_vecNaviTri[TriCount]->PushLinkTri((*TriStart));
			(*TriStart)->PushLinkTri(m_vecNaviTri[TriCount]);
		}

		// 터레인쪽의 네비게이션 삼각형과 자신을 링크
		IVec2 AdjIdx = IVec2(-1, 1);
		for (size_t y = 0; y < 3; ++y)
		{
			for (size_t x = 0; x < 3; ++x)
			{
				IVec2 Idx = IVec2(0, 0);
				Idx.ix = TerIdx.ix + AdjIdx.ix;
				Idx.iy = TerIdx.iy + AdjIdx.iy;

				// 테레인의 하부 삼각형과 링크 검사
				if (0 != LinkAdjTriMatrix[y][x].ix) 
				{
					if (true == (*Start)->GetLinkTerrain()->IsConnectableTerrainTri(Idx, true)) 
					{
						m_vecNaviTri[TriCount]->PushLinkTri((*Start)->GetTerrainNaviTri(Idx,true));
						(*Start)->GetTerrainNaviTri(Idx, true)->PushLinkTri(m_vecNaviTri[TriCount]);
					}
				}

				// 테레인의 상부 삼각형과 링크 검사
				if (0 != LinkAdjTriMatrix[y][x].iy)
				{
					if (true == (*Start)->GetLinkTerrain()->IsConnectableTerrainTri(Idx, false))
					{
						m_vecNaviTri[TriCount]->PushLinkTri((*Start)->GetTerrainNaviTri(Idx, false));
						(*Start)->GetTerrainNaviTri(Idx, false)->PushLinkTri(m_vecNaviTri[TriCount]);
					}
				}

				AdjIdx.ix += 1;
			}

			AdjIdx.iy -= 1;
			AdjIdx.ix = -1;
		}


		++TriCount;
	}


	UINT NaviCount = (UINT)m_vecNaviTri.size();
	int Count = 0;
	int Adj = 0;

	for (UINT i = 0; i < NaviCount; ++i)
	{
		for (UINT j = i + 1; j < NaviCount; ++j)
		{
			for (size_t x = 0; x < 3; ++x)
			{
				bool Check = false;

				for (size_t y = 0; y < 3 && false == Check; ++y)
				{
					if ( m_vecNaviTri[i]->Tri[x] == m_vecNaviTri[j]->Tri[y])
					{
						++Count;
						Check = true;
						continue;
					}

				}
			}

			if (2 == Count)
			{
				m_vecNaviTri[i]->AdjacentTriIdx[m_vecNaviTri[i]->AdjCount] = j;
				m_vecNaviTri[j]->AdjacentTriIdx[m_vecNaviTri[j]->AdjCount] = i;
				++m_vecNaviTri[i]->AdjCount;
				++m_vecNaviTri[j]->AdjCount;
			}
			else if (1 == Count)
			{
				m_vecNaviTri[i]->SharePointTriIdx.push_back(j);
				m_vecNaviTri[j]->SharePointTriIdx.push_back(i);
			}

			Count = 0;
		}
	}


	m_GeoNaviVtx.resize(m_vecNaviTri.size());
	size_t MaxTriCount = m_GeoNaviVtx.size();

	for (size_t i = 0; i < MaxTriCount; i++)
	{
		m_GeoNaviVtx[i].TriVtx[0].vec3 = m_vecNaviTri[i]->Tri[0];
		m_GeoNaviVtx[i].TriVtx[1].vec3 = m_vecNaviTri[i]->Tri[1];
		m_GeoNaviVtx[i].TriVtx[2].vec3 = m_vecNaviTri[i]->Tri[2];
		m_GeoNaviVtx[i].VtxColor = NOADJVTXCOLOR;
	}

	CMesh::CREATE_DESC Desc = { &m_GeoNaviVtx[0], sizeof(NAVITRIVTX) ,  (UINT)m_GeoNaviVtx.size() ,D3D11_USAGE_DYNAMIC
		, nullptr ,IDX32::MemSize() ,0 ,D3D11_USAGE_DYNAMIC ,IDX32::Format() ,D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_POINTLIST };

	m_GeoNaviMesh = CResourceMgr<CMesh>::Create(Desc);
	m_vecMesh.push_back(m_GeoNaviMesh);

}
void CNavigationMesh::CreateNaviMeshCollider(const wchar_t* _ColChannel)
{
	m_NaviMeshCol = ACTOR->AddCom<CMeshCollider>(COLDATA{ _ColChannel ,COLLISION_3D });
	m_NaviMeshCol->SetEnterCallBack(this, &CNavigationMesh::NaviColEnter);
	m_NaviMeshCol->SetStayCallBack(this, &CNavigationMesh::NaviColStay);
	m_NaviMeshCol->SetExitCallBack(this, &CNavigationMesh::NaviColExit);
}

void CNavigationMesh::ExitNavigatorEvent()
{
	size_t MaxVtx = m_GeoNaviVtx.size();

	for (size_t i = 0; i < MaxVtx; i++)
	{
		m_GeoNaviVtx[i].VtxColor = NOADJVTXCOLOR;
	}

	m_GeoNaviMesh->VBUpdate(0, &m_GeoNaviVtx[0]);
}

SPTR<CMesh> CNavigationMesh::GetGeoNaviMesh()
{
	return m_GeoNaviMesh;
}

void CNavigationMesh::NaviColEnter(SPTR<class CCollider> _This, SPTR<class CCollider> _Other) 
{
	m_ColTriIdx = m_NaviMeshCol->GetColTriIdx();

	m_GeoNaviVtx[m_ColTriIdx].VtxColor = COLTRICOLOR;
	m_GeoNaviMesh->VBUpdate(0, &m_GeoNaviVtx[0]);

}
void CNavigationMesh::NaviColStay(SPTR<class CCollider> _This, SPTR<class CCollider> _Other) 
{
	UINT Idx = m_NaviMeshCol->GetColTriIdx();
	
	if (Idx != m_ColTriIdx) 
	{
		m_GeoNaviVtx[m_ColTriIdx].VtxColor = NOADJVTXCOLOR;
		m_GeoNaviVtx[Idx].VtxColor = COLTRICOLOR;
		m_GeoNaviMesh->VBUpdate(0, &m_GeoNaviVtx[0]);
		m_ColTriIdx = Idx;
	}
}
void CNavigationMesh::NaviColExit(SPTR<class CCollider> _This, SPTR<class CCollider> _Other)
{
	m_GeoNaviVtx[m_ColTriIdx].VtxColor = NOADJVTXCOLOR;
	m_GeoNaviMesh->VBUpdate(0, &m_GeoNaviVtx[0]);
	m_ColTriIdx = -1;
}



void CNavigationMesh::SelectTriIdx()
{
	if (m_ColTriIdx != -1) 
	{
		if (m_ColTriIdx != m_SelectTriIdx) 
		{
			InitSelectVtxColor();
		}
		m_SelectTriIdx = m_ColTriIdx;
	}
}
void CNavigationMesh::InitSelectTriIdx()
{
	m_SelectTriIdx = -1;
	InitSelectVtxColor();
}

void CNavigationMesh::SetSelectTriVtx(const UINT& _VtxIdx,const Vec4& _Color)
{
	if (_VtxIdx > 3) 
	{
		return;
	}
	m_VtxColor[_VtxIdx] = _Color;
}

void CNavigationMesh::InitSelectVtxColor()
{
	m_VtxColor[0] = BASEVTXCOLOR;
	m_VtxColor[1] = BASEVTXCOLOR;
	m_VtxColor[2] = BASEVTXCOLOR;
}


void CNavigationMesh::InitSelectTriIdx(const UINT& _VtxIdx)
{
	if (_VtxIdx > 3)
		return;

	m_VtxColor[_VtxIdx] = BASEVTXCOLOR;
}

const Vec3 CNavigationMesh::GetNaviMeshVtxPos(const UINT& _TriIdx, const UINT& _VtxIdx)
{
	return GetSubTransCRWorld().Mul_Coord(m_GeoNaviVtx[_TriIdx].TriVtx[_VtxIdx].vec3);
}


const UINT CNavigationMesh::GetSelectTriIdex()
{
	return m_SelectTriIdx;
}

const UINT CNavigationMesh::GetSerialNumber()
{
	return m_SerialNumber;
}

const bool CNavigationMesh::IsNavigationTriangle(const UINT& _Idx)
{
	return (NOENTERTRI != m_GeoNaviVtx[_Idx].VtxColor);
}


void CNavigationMesh::FindAllConnectedTri(std::list<_tagNAVITRIANLGE*>* _pList, int* _pLinkTriData)
{
	if (nullptr == _pList || nullptr == _pLinkTriData)
		return;

	if (-1 != _pLinkTriData[0]) 
	{
		_pList->push_back(m_vecNaviTri[_pLinkTriData[0]]);
	}

	if(-1 != _pLinkTriData[2])
	{
		_pList->push_back(m_vecNaviTri[_pLinkTriData[2]]);
	}

	for (size_t T = 0; T < m_vecNaviTri.size(); ++T)
	{

		if (-1 != _pLinkTriData[0])
		{
			int Tri = _pLinkTriData[0];
			int Vtx = _pLinkTriData[1];

			if (Tri == T) 
			{
				continue;
			}

		
			bool Check = false;
			for (size_t V = 0; V < 3; ++V)
			{
				if (m_vecNaviTri[T]->Tri[V] == m_vecNaviTri[Tri]->Tri[Vtx]) 
				{
					_pList->push_back(m_vecNaviTri[T]);
					Check = true;
					continue;
				}
			}

			if (true == Check)
				continue;
		}

		if (-1 != _pLinkTriData[2])
		{
			int Tri = _pLinkTriData[2];
			int Vtx = _pLinkTriData[3];

			if (Tri == T)
			{
				continue;
			}


			for (size_t V = 0; V < 3; ++V)
			{
				if (m_vecNaviTri[T]->Tri[V] == m_vecNaviTri[Tri]->Tri[Vtx])
				{
					_pList->push_back(m_vecNaviTri[T]);
					continue;
				}
			}

		}

	}


}
void CNavigationMesh::ClearLinkTriList()
{
	size_t MaxCount = m_vecNaviTri.size();
	for (size_t i = 0; i < MaxCount; i++)
	{
		m_vecNaviTri[i]->listLinkNaviTri.clear();
	}
}

void CNavigationMesh::SetNaviTriColor(const UINT& _Idx, const Vec4& _Color)
{
	if (m_GeoNaviVtx.size() < _Idx)
		return;

	m_GeoNaviVtx[_Idx].VtxColor = _Color;

	if (nullptr != m_GeoNaviMesh)
	{
		m_GeoNaviMesh->VBUpdate(0, &m_GeoNaviVtx[0]);
	}
}
//void CNavigationMesh::CreateNavigationMesh(SPTR<class CTerrainRenderer> _Terrain, const float& _NaviHeight)
//{
//	if (nullptr == _Terrain)
//		return;
//
//	std::vector<Vec3> TempVtx;
//	std::vector<IDX32> TempIdx;
//
//	this->SetRenSize(_Terrain->GetRenSize());
//	TempVtx.resize(_Terrain->m_vecTerrainVtx.size());
//	float CellSize = _Terrain->m_CellSize;
//
//	for (size_t i = 0; i < TempVtx.size(); i++)
//	{
//		TempVtx[i] = _Terrain->m_vecTerrainVtx[i].Pos + Vec3(0.f, _NaviHeight, 0.f);
//	}
//
//
//	UINT X, Z,Row,Colm;
//	int Tile, LT, L, LB, B, RB, R, RT, T;
//
//	X = (UINT)_Terrain->m_TerrainBuffer.SizeX;
//	Z = (UINT)_Terrain->m_TerrainBuffer.SizeZ;
//	Row = 2 * X;
//	Colm = 2 * Z;
//
//
//	m_vecNaviTri.resize(_Terrain->m_vecTerrainIdx.size());
//	int NeviCount = (int)m_vecNaviTri.size();
//
//	for ( int i = 0; i < NeviCount; ++i)
//	{
//		m_vecNaviTri[i] = new NaviTri();
//		m_vecNaviTri[i]->pTriHead = &m_vecNaviTri[0];
//		m_vecNaviTri[i]->Index = i;
//		IDX32& Idx = _Terrain->m_vecTerrainIdx[i];
//		m_vecNaviTri[i]->Tri[0] = TempVtx[Idx._1];
//		m_vecNaviTri[i]->Tri[1] = TempVtx[Idx._2];
//		m_vecNaviTri[i]->Tri[2] = TempVtx[Idx._3];
//
//		Tile = i / 2;
//		LT = Tile + X - 1;
//		L = Tile  - 1;
//		LB = Tile - X - 1;
//		B = Tile - X ;
//		RB = Tile - X + 1;
//		R = Tile + 1;
//		RT = Tile + X + 1;
//		T = Tile + X;
//
//		// 인덱스가 짝수인경우
//		if (i % 2 == 0)
//		{
//			m_vecNaviTri[i]->AdjacentTriIdx[m_vecNaviTri[i]->AdjCount++] = i + 1;
//
//			// 왼쪽 터레인 타일 검사
//			if (Tile / X == L / X) 
//			{
//				m_vecNaviTri[i]->AdjacentTriIdx[m_vecNaviTri[i]->AdjCount++] = i - 1;
//				m_vecNaviTri[i]->SharePointTriIdx.push_back(i - 2);
//			}
//
//			// 오른쪽 터레인 타일 검사
//			if (Tile / X == R / X)
//			{
//				m_vecNaviTri[i]->SharePointTriIdx.push_back(i + 2);
//			}
//
//
//			// 최상단의 경우 위쪽 타일을 검사하지 않는다.
//			if (Tile / X < Colm - 1) 
//			{
//				// 왼쪽 위 테레인 타일 검사
//				if ((LT / X)  >  (Tile / X))
//				{
//					m_vecNaviTri[i]->SharePointTriIdx.push_back(i + Row - 1);
//					m_vecNaviTri[i]->SharePointTriIdx.push_back(i + Row - 2);
//				}
//
//				// 위쪽 터레인 타일 검사
//				if (Tile / X < T / X)
//				{
//					m_vecNaviTri[i]->SharePointTriIdx.push_back(i + Row);
//				}
//			}
//
// 
//			// 최하단 터레인 타일은 하단 검사를 수행하지 않는다.
//			if (Tile / X > 0)
//			{
//				// 왼쪽 아래 테레인 타일 검사
//				if (LB / X < Tile / X)
//				{
//					m_vecNaviTri[i]->SharePointTriIdx.push_back(i - Row - 1);
//				}
//
//				// 아래 터레인 타일 검사
//				if ( B / X  < Tile / X)
//				{
//					m_vecNaviTri[i]->AdjacentTriIdx[m_vecNaviTri[i]->AdjCount++] = i + 1 - Row;
//					m_vecNaviTri[i]->SharePointTriIdx.push_back(i - Row);
//				}
//
//				// 오른쪽 아래 테레인 타일 검사
//				if ( RB / X < Tile / X)
//				{
//					m_vecNaviTri[i]->SharePointTriIdx.push_back(i - Row + 2);
//					m_vecNaviTri[i]->SharePointTriIdx.push_back(i - Row + 3);
//				}
//			
//			}
//
//		}
//		else 
//		{
//			m_vecNaviTri[i]->AdjacentTriIdx[m_vecNaviTri[i]->AdjCount++] = i - 1;
//
//			// 왼쪽 터레인 타일 검사
//			if (Tile / X == L/ X)
//			{
//				m_vecNaviTri[i]->SharePointTriIdx.push_back(i - 2);
//			}
//
//			// 오른쪽 터레인 타일 검사
//			if (Tile / X == R/ X)
//			{
//				m_vecNaviTri[i]->AdjacentTriIdx[m_vecNaviTri[i]->AdjCount++] = i + 1;
//				m_vecNaviTri[i]->SharePointTriIdx.push_back(i + 2);
//			}
//
//			// 최상단의 경우 위쪽 타일을 검사하지 않는다.
//			if (Tile / X < Colm - 1)
//			{
//				// 왼쪽 위 테레인 타일 검사
//				if (LT / X > Tile / X)
//				{
//					m_vecNaviTri[i]->SharePointTriIdx.push_back(i + Row - 2);
//					m_vecNaviTri[i]->SharePointTriIdx.push_back(i + Row - 3);
//				}
//
//				// 오른쪽 위 테레인 타일 검사
//				if (RT / X > Tile / X)
//				{
//					m_vecNaviTri[i]->SharePointTriIdx.push_back(i + Row + 1);
//				}
//
//				// 위쪽 터레인 타일 검사
//				if ( Tile / X  < T / X)
//				{
//					m_vecNaviTri[i]->AdjacentTriIdx[m_vecNaviTri[i]->AdjCount++] = i + Row - 1;
//					m_vecNaviTri[i]->SharePointTriIdx.push_back(i + Row);
//				}
//
//			}
//			// 최하단 터레인 타일은 하단 검사를 수행하지 않는다.
//			if (Tile / X > 0)
//			{
//				// 아래 터레인 타일 검사
//				if (B / X < Tile / X)
//				{
//					m_vecNaviTri[i]->SharePointTriIdx.push_back(i - Row);
//				}
//
//				// 오른쪽 아래 테레인 타일 검사
//				if (RB / X < Tile / X)
//				{
//					m_vecNaviTri[i]->SharePointTriIdx.push_back(i - Row + 1);
//					m_vecNaviTri[i]->SharePointTriIdx.push_back(i - Row + 2);
//				}
//			}
//		}
//	}
//
//	COLORVTX* DrawVtx = new COLORVTX[TempVtx.size()];
//	ZeroMemory(DrawVtx, sizeof(COLORVTX) * TempVtx.size());
//
//	for (size_t i = 0; i < TempVtx.size(); i++)
//	{
//		DrawVtx[i].Pos = TempVtx[i];
//		DrawVtx[i].Color = Vec4::One;
//	}
//
//	CMesh::CREATE_DESC Desc = { &DrawVtx[0], sizeof(COLORVTX) ,  (UINT)TempVtx.size() ,D3D11_USAGE_DYNAMIC
//		, &(_Terrain->m_vecTerrainIdx[0]) ,IDX32::MemSize() ,(UINT)(3 * _Terrain->m_vecTerrainIdx.size()) ,D3D11_USAGE_DYNAMIC ,IDX32::Format() ,D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
//	m_DrawNaviMesh = CResourceMgr<CMesh>::Create(Desc);
//
//	PushMesh(m_DrawNaviMesh, 0);
//
//	TempVtx.clear();
//	delete[] DrawVtx;
//}