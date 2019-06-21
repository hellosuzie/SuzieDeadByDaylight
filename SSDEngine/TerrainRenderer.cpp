#include "TerrainRenderer.h"
#include "NavigationMesh.h"
#include "Mesh.h"
#include "ResourceMgr.h"
#include "Texture.h"
#include "Material.h"
#include "Scene.h"
#include "Device.h"
#include "FbxRenderer.h"
#include "GlobalDebug.h"
#include "NaviMeshArea.h"
#include "RayCollider.h"
#include "KeyMgr.h"
#include "SSDCore.h"
#include "TransForm.h"
#include "Terrain.h"
#include "SSDRandom.h"
#include "DXStruct.h"
#include "Navigator.h"
#include "Frustum.h"

CNaviLinkTri::CNaviLinkTri()
	:m_LinkNaviMesh(nullptr), m_LinkNaviTriIdx(-1), m_LinkTerrainIdx(NONESELECTINDEX)
	, m_LinkTerrain(nullptr)
{
	m_LinkVtx[0].VtxID = -1;
	m_LinkVtx[1].VtxID = -1;
	m_LinkVtx[2].VtxID = -1;

	Off();
}
CNaviLinkTri::CNaviLinkTri(CTerrainRenderer* _pTerrainRen)
	:m_LinkNaviMesh(nullptr), m_LinkNaviTriIdx(-1), m_LinkTerrainIdx(NONESELECTINDEX)
	, m_LinkTerrain(_pTerrainRen)
{
	m_LinkVtx[0].VtxID = -1;
	m_LinkVtx[1].VtxID = -1;
	m_LinkVtx[2].VtxID = -1;

	if (m_LinkTerrain->GetSelectTriIdx() != NONESELECTINDEX)
	{
		m_LinkTerrainIdx = m_LinkTerrain->GetSelectTriIdx();
	}

	Off();
}
CNaviLinkTri::~CNaviLinkTri() 
{

}

const bool CNaviLinkTri::Update()
{
	bool Check = false;

	for (size_t i = 0; i < 3; i++)
	{
		if (m_LinkVtx[i].VtxID == -1)
			return false;

		if (true == m_LinkVtx[i].bNaviVtx) 
		{
			m_LinkVtx[i].VtxPos = m_LinkNaviMesh->GetNaviMeshVtxPos(m_LinkNaviTriIdx, m_LinkVtx[i].VtxID);
		}
		else 
		{
			m_LinkVtx[i].VtxPos = m_LinkTerrain->GetTerrainVtxPos(m_LinkTerrainIdx, m_LinkVtx[i].VtxID);
		}
	}

	return true;
}


void CNaviLinkTri::Init()
{
	m_LinkNaviMesh = nullptr;
	m_LinkNaviTriIdx = -1;
	m_LinkTerrainIdx = NONESELECTINDEX;
}
void CNaviLinkTri::LinkNavigationMesh(CNavigationMesh* _LinkNaviMesh) 
{
	m_LinkNaviMesh = _LinkNaviMesh;

	if (-1 != m_LinkNaviMesh->GetSelectTriIdex())
	{
		m_LinkNaviTriIdx = m_LinkNaviMesh->GetSelectTriIdex();
	}
}
void CNaviLinkTri::LinkTerrainRen(CTerrainRenderer* _TerrainRen)
{
	m_LinkTerrain = _TerrainRen;

	if (NONESELECTINDEX != _TerrainRen->GetSelectTriIdx()) 
	{
		m_LinkTerrainIdx = _TerrainRen->GetSelectTriIdx();
	}
}
void CNaviLinkTri::SetNaviMeshTriIdx(const UINT& _TriIdx)
{
	m_LinkNaviTriIdx = _TriIdx;
}
void CNaviLinkTri::SetLinkTerrainIdx(const IVec2& _TerIdx) 
{
	m_LinkTerrainIdx = _TerIdx;
}

void CNaviLinkTri::SetLinkVtx(const UINT& _Idx, const bool& _NaviMesh, const UINT& _ID)
{
	if (_Idx >= 3)
	{
		return;
	}

	m_LinkVtx[_Idx].bNaviVtx = _NaviMesh;
	m_LinkVtx[_Idx].VtxID = _ID;

	for (size_t i = 0; i < 3; i++)
	{
		// 모든 정점정보에 값이 채워졋을때만 랜더링
		if (m_LinkVtx[i].VtxID == -1)
		{
			Off();
			return;
		}
	}
	
	On();
}

const bool CNaviLinkTri::IsLinkTriangle()
{
	for (size_t i = 0; i < 3; i++)
	{
		// 모든 정점정보에 값이 채워졋을때만 랜더링
		if (m_LinkVtx[i].VtxID == -1)
		{
			return false;
		}
	}
	return true;
}

NaviTri* CNaviLinkTri::GetLinkNaviTri()
{
	return m_LinkNaviMesh->GetNaviTri(m_LinkNaviTriIdx);
}
NaviTri* CNaviLinkTri::GetLinkTerrainTri(const bool& _bBottomTri)
{
	return m_LinkTerrain->GetTerrainNaviTri(m_LinkTerrainIdx, _bBottomTri);
}
NaviTri* CNaviLinkTri::GetTerrainNaviTri(const IVec2& _TerIdx, const bool& _bBottomTri)
{
	return m_LinkTerrain->GetTerrainNaviTri(_TerIdx, _bBottomTri);
}

///////////////////////////////////////////////				Terrain		Renderer			/////////////////////////////////////
CTerrainRenderer::CTerrainRenderer()
	:m_TerrainNaviMesh(nullptr), m_bDebugRen(false), m_TerrainGeoMesh(nullptr)
	, m_FocusTriIdx(NONESELECTINDEX), m_TerrainLBCallBack(nullptr)
	, m_TerrainRBCallBack(nullptr), m_SelectTriIdx(NONESELECTINDEX)
	, m_MakingLinkTri(nullptr), m_LinkTriMesh(nullptr), m_LinkTriMtl(nullptr)
	, m_pTerrain(nullptr), m_SelectLinkTriIdx(-1), m_fNaviHeight(0.f)
	, m_LinkNavigationMesh(nullptr), m_ViewCamera(nullptr)
{
	m_TerrainBuffer.FloorCount = 0;
	InitTerrainVtxColor();
}


CTerrainRenderer::~CTerrainRenderer()
{
	m_TerrainGeoMesh = nullptr;
}

void CTerrainRenderer::PostUpdate(const float & _DeltaTime)
{
	if (false == Core::CheckMouseOnScreen())
		return;

	//if (true == m_pTerrain->IsTerrainRayCollsion())
	//	return;

	if (true == KEY("LB") && NONESELECTINDEX != m_FocusTriIdx && nullptr != m_TerrainLBCallBack)
	{
		m_TerrainLBCallBack(this);
	}
	else if (true == KEY("RB") && NONESELECTINDEX != m_FocusTriIdx && nullptr != m_TerrainRBCallBack)
	{
		m_TerrainRBCallBack(this);
	}


	if (true == KEYDOWN("TerrainDebugMode")) 
	{
		m_bDebugRen = !m_bDebugRen;
	}
}
const bool CTerrainRenderer::Init()
{
	m_MakingLinkTri = new CNaviLinkTri(this);

	m_LinkTriVtx.VtxColor = Vec4(0.5f, 0.5f, 0.5f, 0.5f);

	memset(m_AllLinkTriVtx, 0, sizeof(m_AllLinkTriVtx));

	for (size_t i = 0; i < MAXLINKCOUNT; i++)
	{
		m_AllLinkTriVtx[i].VtxColor = Vec4::PastelBlue;
	}

	return true;
}
const bool CTerrainRenderer::Init(const RENDATA & _InitData)
{
	PushMaterial(L"GeoTerrain");
	PushMaterial(L"DebugTerrain", 1);
	GetMaterial(1)->SetRasterizerState(L"WireFrame");

	m_LinkTriMtl = CResourceMgr<CMaterial>::Find(L"TriangleMaterial");

	m_MakingLinkTri = new CNaviLinkTri(this);

	//m_LinkTriVtx.VtxColor = Vec4::PastelBlue;
	m_LinkTriVtx.VtxColor = Vec4(0.5f, 0.5f, 0.5f, 0.5f);
	CMesh::CREATE_DESC Desc = { &m_LinkTriVtx, sizeof(NAVITRIVTX), (UINT)1,D3D11_USAGE_DYNAMIC
		, nullptr ,IDX32::MemSize() ,0 ,D3D11_USAGE_DYNAMIC ,IDX32::Format() ,D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_POINTLIST };

	m_LinkTriMesh = CResourceMgr<CMesh>::Create(Desc);

	memset(m_AllLinkTriVtx, 0, sizeof(m_AllLinkTriVtx));

	for (size_t i = 0; i < MAXLINKCOUNT; i++)
	{
		m_AllLinkTriVtx[i].VtxColor = Vec4::PastelBlue;
	}

	Desc = { &m_AllLinkTriVtx[0], sizeof(NAVITRIVTX) ,  (UINT)MAXLINKCOUNT ,D3D11_USAGE_DYNAMIC
		, nullptr ,IDX32::MemSize() ,0 ,D3D11_USAGE_DYNAMIC ,IDX32::Format() ,D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_POINTLIST };

	m_AllLinkTriMesh = CResourceMgr<CMesh>::Create(Desc);

	return CRenderer::Init(_InitData);
}

void CTerrainRenderer::RenderUpdate(SPTR<CCamera> _Cam)
{
	CRenderer::RenderUpdate(_Cam);
	DXCONTEXT->PSSetShaderResources(9, (UINT)m_vecShaderResource.size(), &m_vecShaderResource[0]);
	GetMaterial(0)->SetConstantBufferData(L"TerrainBuffer", m_TerrainBuffer);
}

void CTerrainRenderer::Render(SPTR<CCamera> _Cam)
{
	if (CSceneRenderMgr::IsDeferred != m_vecMaterial[0]->IsDeferred())
	{
		return;
	}

	m_vecMaterial[0]->Update();
	m_TerrainGeoMesh->Update();
	m_TerrainGeoMesh->VtxRender(0);
	m_vecMaterial[0]->StateRollBack();
	m_vecMaterial[0]->TexSamRollBack();
	CDevice::ResetShaderResourceTexture();

	
	if (m_bDebugRen)
	{
		m_vecMaterial[1]->Update();
		m_TerrainGeoMesh->VtxRender(0);
		m_vecMaterial[1]->StateRollBack();
		m_vecMaterial[1]->TexSamRollBack();


		if (m_SelectTriIdx != NONESELECTINDEX)
		{
			Vec4 Height = m_vecTerrainGeoVtx[(int)(m_TerrainBuffer.SizeX) * m_SelectTriIdx.iy + m_SelectTriIdx.ix].TerrainHeight;
	
			const Mat& World = GetSubTransCRWorld();
			Vec3 LB,LT,RT,RB;

			LB = World.Mul_Coord(Vec3((float)m_SelectTriIdx.ix, Height.x, (float)m_SelectTriIdx.iy));
			LT = World.Mul_Coord(Vec3((float)m_SelectTriIdx.ix , Height.y, (float)(m_SelectTriIdx.iy + 1)));
			RT = World.Mul_Coord(Vec3((float)(m_SelectTriIdx.ix + 1), Height.z, (float)(m_SelectTriIdx.iy + 1)));
			RB = World.Mul_Coord(Vec3((float)(m_SelectTriIdx.ix + 1), Height.w, (float)m_SelectTriIdx.iy));
	
			GlobalDebug::DrawDebugSphere(_Cam->GetVPMatrix(), LB, 5.f, m_TerrainVtxColor[0]);
			GlobalDebug::DrawDebugSphere(_Cam->GetVPMatrix(), LT, 5.f, m_TerrainVtxColor[1]);
			GlobalDebug::DrawDebugSphere(_Cam->GetVPMatrix(), RT, 5.f, m_TerrainVtxColor[2]);
			GlobalDebug::DrawDebugSphere(_Cam->GetVPMatrix(), RB, 5.f, m_TerrainVtxColor[3]);
		}
	}
	

	// 테레인 네비게이션 메쉬를 베이크 하지 않은 경우
	if (nullptr == m_TerrainNaviMesh) 
	{
		const Mat& VP = _Cam->GetVPMatrix();
		const Mat& V = _Cam->GetViewMatrix();
		const Mat& P = _Cam->GetProjMatrix();

		
		if (true == m_MakingLinkTri->IsActive() && true == m_MakingLinkTri->Update())
		{

			//m_LinkTriVtx.TriVtx[0] = World.Mul_Coord(m_MakingLinkTri->m_LinkVtx[0].VtxPos);
			//m_LinkTriVtx.TriVtx[1] = World.Mul_Coord(m_MakingLinkTri->m_LinkVtx[1].VtxPos);
			//m_LinkTriVtx.TriVtx[2] = World.Mul_Coord(m_MakingLinkTri->m_LinkVtx[2].VtxPos);
			m_LinkTriVtx.TriVtx[0] = VP.Mul_Coord(m_MakingLinkTri->m_LinkVtx[0].VtxPos);
			m_LinkTriVtx.TriVtx[1] = VP.Mul_Coord(m_MakingLinkTri->m_LinkVtx[1].VtxPos);
			m_LinkTriVtx.TriVtx[2] = VP.Mul_Coord(m_MakingLinkTri->m_LinkVtx[2].VtxPos);

			m_LinkTriMesh->VBUpdate(0, (void*)&m_LinkTriVtx);

			m_LinkTriMtl->Update();
			m_LinkTriMesh->Update();
			m_LinkTriMesh->VtxRender(0);
			m_LinkTriMtl->StateRollBack();
			m_LinkTriMtl->TexSamRollBack();
			
		}

		if (false == m_listLinkTri.empty())
		{
			UINT DrawCount = 0;


			for (SPTR<CNaviLinkTri> LinkTri : m_listLinkTri)
			{
				UINT ClipCount = 0;

				LinkTri->Update();
				Vec4 V1, V2, V3;
				V1 = VP.MulVec4(Vec4(LinkTri->m_LinkVtx[0].VtxPos.x, LinkTri->m_LinkVtx[0].VtxPos.y, LinkTri->m_LinkVtx[0].VtxPos.z, 1.f));
				V2 = VP.MulVec4(Vec4(LinkTri->m_LinkVtx[1].VtxPos.x, LinkTri->m_LinkVtx[1].VtxPos.y, LinkTri->m_LinkVtx[1].VtxPos.z, 1.f));
				V3 = VP.MulVec4(Vec4(LinkTri->m_LinkVtx[2].VtxPos.x, LinkTri->m_LinkVtx[2].VtxPos.y, LinkTri->m_LinkVtx[2].VtxPos.z, 1.f));
				
				if (V1.z < 0.f && V2.z < 0.f && V3.z < 0.f)
				{
					continue;
				}
				//m_AllLinkTriVtx[DrawCount].TriVtx[0] = VP.Mul_Coord(LinkTri->m_LinkVtx[0].VtxPos);
				//m_AllLinkTriVtx[DrawCount].TriVtx[1] = VP.Mul_Coord(LinkTri->m_LinkVtx[1].VtxPos);
				//m_AllLinkTriVtx[DrawCount].TriVtx[2] = VP.Mul_Coord(LinkTri->m_LinkVtx[2].VtxPos);

				m_AllLinkTriVtx[DrawCount].TriVtx[0] = (V1 / abs(V1.w)).vec3;
				m_AllLinkTriVtx[DrawCount].TriVtx[1] = (V2 / abs(V2.w)).vec3;
				m_AllLinkTriVtx[DrawCount].TriVtx[2] = (V3 / abs(V3.w)).vec3;
				++DrawCount;
			}

			m_LinkTriMtl->Update();
			m_AllLinkTriMesh->Update();
			m_AllLinkTriMesh->VBUpdate(0, (void*)&m_AllLinkTriVtx[0]);
			m_AllLinkTriMesh->VtxRender(0, DrawCount);
			m_LinkTriMtl->StateRollBack();
			m_LinkTriMtl->TexSamRollBack();
		}


	}
	CDevice::ResetShaderResource();
}

void CTerrainRenderer::CreateTerrain(const wchar_t * _Tex, const wchar_t * _BaseDiffuseTex, const wchar_t * _BaseBumpTex, int _X, int _Z, float _MaxHeight /* = 3 */ )
{
	m_vecTerrainVtx.clear();
	m_TerrainMesh = nullptr;

	if (_X <= 0)
		_X = 1;
	if (_Z <= 0) 
		_Z = 1;

	m_vecTerrainVtx.reserve((_X + 1) * (_Z + 1));
	m_vecTerrainGeoVtx.reserve(_X * _Z);

	//m_TerrainMesh = new CMesh();
	m_TerrainGeoMesh = new CMesh();

	FBXSTATICVTX TempVtx;

	m_HeightTexture = CResourceMgr<CTexture>::Find(_Tex);
	if (nullptr == m_HeightTexture)
	{
		TASSERT(true);
		return;
	}

	m_TerrainBuffer.SizeX = (float)_X;
	m_TerrainBuffer.SizeZ = (float)_Z;
	m_fMaxHeight = _MaxHeight;

 	for (int z = 0; z < _Z + 1; ++z)
	{
		for (int x = 0; x < _X + 1; ++x)
		{
			if (nullptr != m_HeightTexture)
			{
				float TextureX = m_HeightTexture->GetTextureSize().x / _X;
				float TextureZ = m_HeightTexture->GetTextureSize().y / _Z;

				Vec4 Color = m_HeightTexture->GetPixel((int)(x * TextureX), (int)m_HeightTexture->GetTextureSize().y - (int)(z * TextureZ));
      			TempVtx.Pos = Vec4((float)x, Color.x * m_fMaxHeight, (float)z, 1.f);
			}
			else
			{
				TempVtx.Pos = Vec4((float)x, 0.f, (float)z, 1.f);
			}

			TempVtx.TexCoord = Vec2((float)x, (float)(_Z - z));
			TempVtx.Normal = Vec4(0.f, 1.f, 0.f, 0.f);
			TempVtx.Tangent = Vec4(1.f, 0.f, 0.f, 0.f);
			TempVtx.BiNormal = Vec4(0.f, 0.f, -1.f, 0.f);


			m_vecTerrainVtx.push_back(TempVtx);

			if (z > 0 && x > 0)
			{
				float LB, LT, RT, RB;
				LB = m_vecTerrainVtx[(_X + 1) * (z - 1) + (x - 1)].Pos.y ;
				LT = m_vecTerrainVtx[(_X + 1) * z + (x - 1)].Pos.y ;
				RT = m_vecTerrainVtx[(_X + 1) * z + x].Pos.y ;
				RB = m_vecTerrainVtx[(_X + 1) * (z - 1) + x].Pos.y ;

				m_vecTerrainGeoVtx.push_back(GEOTERRAINVTX{ Vec2((float)(x - 1),(float)(z - 1)),Vec4(LB,LT,RT,RB), NAVITRICOLOR });
			}

		}
	}

	//m_vecTerrainIdx.resize(_X *_Z * 2);

	//DWORD Count = 0;

	//for (DWORD z = 0; z < (DWORD)_Z; ++z)
	//{
	//	for (DWORD x = 0; x < (DWORD)_X; ++x)
	//	{
	//		m_vecTerrainIdx[Count++] = IDX32{ ((z + 1) * (_X + 1)) + x ,(z * (_X + 1)) + x + 1,(z * (_X + 1)) + x };
	//		m_vecTerrainIdx[Count++] = IDX32{ ((z + 1) * (_X + 1)) + x,((z + 1) * (_X + 1)) + x + 1,(z * (_X + 1)) + x + 1 };
	//	}
	//}

	GetMaterial(0)->PushTexture(0, _BaseDiffuseTex, SHADER_PIXEL, TEXTYPE_LOAD_TEXTURE);
	GetMaterial(0)->PushTexture(1, _BaseBumpTex, SHADER_PIXEL, TEXTYPE_LOAD_TEXTURE);


	CMesh::CREATE_DESC Desc = CMesh::CREATE_DESC{ &m_vecTerrainGeoVtx[0], sizeof(GEOTERRAINVTX), (UINT)m_vecTerrainGeoVtx.size(), D3D11_USAGE_DYNAMIC
		, nullptr , IDX32::MemSize(), (UINT)0 , D3D11_USAGE_DEFAULT, IDX32::Format(), D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_POINTLIST };

	m_TerrainGeoMesh = CResourceMgr<CMesh>::Create(Desc);
}

void CTerrainRenderer::CreateTerrain(int _X, int _Z, float _MaxHeight /*= 3.f*/)
{
	m_vecTerrainVtx.clear();
	m_TerrainMesh = nullptr;

	if (_X <= 0)
		_X = 1;
	if (_Z <= 0) 
		_Z = 1;

	m_vecTerrainVtx.reserve((_X + 1) * (_Z + 1));
	m_vecTerrainGeoVtx.reserve(_X * _Z);

	m_TerrainGeoMesh = new CMesh();

	FBXSTATICVTX TempVtx;


	m_TerrainBuffer.SizeX = (float)_X;
	m_TerrainBuffer.SizeZ = (float)_Z;
	m_fMaxHeight = _MaxHeight;

 	for (int z = 0; z < _Z + 1; ++z)
	{
		for (int x = 0; x < _X + 1; ++x)
		{
		
			TempVtx.Pos = Vec4((float)x, 0.f, (float)z, 1.f);
			TempVtx.TexCoord = Vec2((float)x, (float)(_Z - z));
			TempVtx.Normal = Vec4(0.f, 1.f, 0.f, 0.f);
			TempVtx.Tangent = Vec4(1.f, 0.f, 0.f, 0.f);
			TempVtx.BiNormal = Vec4(0.f, 0.f, -1.f, 0.f);


			m_vecTerrainVtx.push_back(TempVtx);

			if (z > 0 && x > 0)
			{
				float LB, LT, RT, RB;
				LB = m_vecTerrainVtx[(_X + 1) * (z - 1) + (x - 1)].Pos.y ;
				LT = m_vecTerrainVtx[(_X + 1) * z + (x - 1)].Pos.y ;
				RT = m_vecTerrainVtx[(_X + 1) * z + x].Pos.y ;
				RB = m_vecTerrainVtx[(_X + 1) * (z - 1) + x].Pos.y ;

				m_vecTerrainGeoVtx.push_back(GEOTERRAINVTX{ Vec2((float)(x - 1),(float)(z - 1)),Vec4(LB,LT,RT,RB), NAVITRICOLOR });
			}

		}
	}

	//CMesh::CREATE_DESC Desc = CMesh::CREATE_DESC{ &m_vecTerrainGeoVtx[0], sizeof(GEOTERRAINVTX), (UINT)m_vecTerrainGeoVtx.size(), D3D11_USAGE_DYNAMIC
	//	, nullptr , IDX32::MemSize(), (UINT)0 , D3D11_USAGE_DEFAULT, IDX32::Format(), D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_POINTLIST };

	//m_TerrainGeoMesh = CResourceMgr<CMesh>::Create(Desc);
}

void CTerrainRenderer::InitTerrainHeight()
{
	for (int z = 0; z < (int)m_TerrainBuffer.SizeZ + 1; ++z)
	{
		for (int x = 0; x < (int)m_TerrainBuffer.SizeX + 1; ++x)
		{
			FBXSTATICVTX& Vtx = m_vecTerrainVtx[((int)m_TerrainBuffer.SizeX + 1) * z + x];
			
			if (nullptr != m_HeightTexture)
			{

				float TextureX = m_HeightTexture->GetTextureSize().x / m_TerrainBuffer.SizeX;
				float TextureZ = m_HeightTexture->GetTextureSize().y / m_TerrainBuffer.SizeZ;

				Vec4 Color = m_HeightTexture->GetPixel((int)(x * TextureX), (int)m_HeightTexture->GetTextureSize().y - (int)(z * TextureZ));
				Vtx.Pos = Vec4((float)x, Color.x * m_fMaxHeight, (float)z, 1.f);
			}
			else
			{
				Vtx.Pos = Vec4((float)x, 0.f, (float)z, 1.f);
			}

			if (z > 0 && x > 0)
			{
				
				m_vecTerrainGeoVtx[(int)(m_TerrainBuffer.SizeX ) * (z - 1) + (x - 1)].TerrainHeight.x = m_vecTerrainVtx[((int)m_TerrainBuffer.SizeX + 1) * (z - 1) + (x - 1)].Pos.y;
				m_vecTerrainGeoVtx[(int)(m_TerrainBuffer.SizeX ) * (z - 1) + (x - 1)].TerrainHeight.y = m_vecTerrainVtx[((int)m_TerrainBuffer.SizeX + 1) * z + (x - 1)].Pos.y ;
				m_vecTerrainGeoVtx[(int)(m_TerrainBuffer.SizeX ) * (z - 1) + (x - 1)].TerrainHeight.z = m_vecTerrainVtx[((int)m_TerrainBuffer.SizeX + 1) * z + x].Pos.y ;
				m_vecTerrainGeoVtx[(int)(m_TerrainBuffer.SizeX ) * (z - 1) + (x - 1)].TerrainHeight.w = m_vecTerrainVtx[((int)m_TerrainBuffer.SizeX + 1) * (z - 1) + x].Pos.y ;

				m_vecTerrainGeoVtx[(int)(m_TerrainBuffer.SizeX)  * (z - 1) + (x - 1)].Color = NAVITRICOLOR;
			}
		}
	}
}
void CTerrainRenderer::NaviRectAreaCollisionEvent(const _tagNAVIAREADATA& _AreaData ,const int& _X, const int& _Z)
{
	Vec2 LB = Vec2(_X* m_CellSize, _Z* m_CellSize);
	Vec2 LT = Vec2(_X* m_CellSize, (_Z + 1)* m_CellSize);
	Vec2 RT = Vec2((_X + 1)* m_CellSize, (_Z + 1)* m_CellSize);
	Vec2 RB = Vec2((_X + 1)* m_CellSize, _Z* m_CellSize);


	bool Check = false;

	if (true == CollisionFunc::PointToRect(Vec4(LB.x, LB.y, 0.f, 0.f), Vec4(_AreaData.AreaPos.x, _AreaData.AreaPos.y, _AreaData.AreaSize.x, _AreaData.AreaSize.y)))
	{
		Check = true;
		m_vecTerrainGeoVtx[(int)(m_TerrainBuffer.SizeX) * _Z + _X].TerrainHeight.x = BASEHEIGHT;
	}
	
	if (true == CollisionFunc::PointToRect(Vec4(LT.x, LT.y, 0.f, 0.f), Vec4(_AreaData.AreaPos.x, _AreaData.AreaPos.y, _AreaData.AreaSize.x, _AreaData.AreaSize.y)))
	{
		Check = true;
		m_vecTerrainGeoVtx[(int)(m_TerrainBuffer.SizeX) * _Z + _X].TerrainHeight.y = BASEHEIGHT;
	}

	if (true == CollisionFunc::PointToRect(Vec4(RT.x, RT.y, 0.f, 0.f), Vec4(_AreaData.AreaPos.x, _AreaData.AreaPos.y, _AreaData.AreaSize.x, _AreaData.AreaSize.y)))
	{
		Check = true;
		m_vecTerrainGeoVtx[(int)(m_TerrainBuffer.SizeX) * _Z + _X].TerrainHeight.z = BASEHEIGHT;
	}

	if (true == CollisionFunc::PointToRect(Vec4(RB.x, RB.y, 0.f, 0.f), Vec4(_AreaData.AreaPos.x, _AreaData.AreaPos.y, _AreaData.AreaSize.x, _AreaData.AreaSize.y)))
	{
		Check = true;
		m_vecTerrainGeoVtx[(int)(m_TerrainBuffer.SizeX) * _Z + _X].TerrainHeight.w = BASEHEIGHT;
	}
		
	if (true == Check) 
	{
		m_vecTerrainGeoVtx[(int)(m_TerrainBuffer.SizeX ) * _Z + _X].Color = NONAVITRICOLOR;
	}

}

void CTerrainRenderer::NaviRectAreaCollisionEvent(const Vec2& _Area0, const Vec2& _Area1, const Vec2& _Area2, const Vec2& _Area3,const int& _X, const int& _Z)
{
	Vec2 LB = Vec2(_X* m_CellSize, _Z* m_CellSize);
	Vec2 LT = Vec2(_X* m_CellSize, (_Z + 1)* m_CellSize);
	Vec2 RT = Vec2((_X + 1)* m_CellSize, (_Z + 1)* m_CellSize);
	Vec2 RB = Vec2((_X + 1)* m_CellSize, _Z* m_CellSize);

	Vec2 Axis0 = (_Area1 - _Area0);
	Vec2 Axis1 = (_Area3 - _Area0);

	bool Check = false;

	if (true == CollisionFunc::RotateOrthoRectToPoint(_Area0, Axis0, Axis1, LB))
	{
		Check = true;
		m_vecTerrainGeoVtx[(int)(m_TerrainBuffer.SizeX) * _Z + _X].TerrainHeight.x = BASEHEIGHT;
	}

	if (true == CollisionFunc::RotateOrthoRectToPoint(_Area0, Axis0, Axis1, LT))
	{
		Check = true;
		m_vecTerrainGeoVtx[(int)(m_TerrainBuffer.SizeX) * _Z + _X].TerrainHeight.y = BASEHEIGHT;
	}

	if (true == CollisionFunc::RotateOrthoRectToPoint(_Area0, Axis0, Axis1, RT))
	{
		Check = true;
		m_vecTerrainGeoVtx[(int)(m_TerrainBuffer.SizeX) * _Z + _X].TerrainHeight.z = BASEHEIGHT;
	}

	if (true == CollisionFunc::RotateOrthoRectToPoint(_Area0, Axis0, Axis1, RB))
	{
		Check = true;
		m_vecTerrainGeoVtx[(int)(m_TerrainBuffer.SizeX) * _Z + _X].TerrainHeight.w = BASEHEIGHT;
	}

	if (true == Check)
	{
		m_vecTerrainGeoVtx[(int)(m_TerrainBuffer.SizeX) * _Z + _X].Color = NONAVITRICOLOR;
	}
}
void CTerrainRenderer::NaviCircleAreaCollisionEvent(const struct _tagNAVIAREADATA& _AreaData, const int& _X, const int& _Z)
{
	bool Check = false;
	float Diameter = _AreaData.AreaSize.x;

	Vec2 LB = Vec2(_X* m_CellSize, _Z* m_CellSize);
	Vec2 LT = Vec2(_X* m_CellSize, (_Z + 1)* m_CellSize);
	Vec2 RT = Vec2((_X + 1)* m_CellSize, (_Z + 1)* m_CellSize);
	Vec2 RB = Vec2((_X + 1)* m_CellSize, _Z* m_CellSize);


	DirectX::TriangleTests::Intersects(Vec3(), Vec3(), Vec3(), Vec3(), Vec3(), Vec3());
	if (true == CollisionFunc::PointToCircle(Vec4(LB.x, LB.y, 0.f, 0.f), Vec4(_AreaData.AreaPos.x, _AreaData.AreaPos.y, Diameter, Diameter)))
	{
		Check = true;
		m_vecTerrainGeoVtx[(int)(m_TerrainBuffer.SizeX) * _Z + _X].TerrainHeight.x = BASEHEIGHT;
	}

	if (true == CollisionFunc::PointToCircle(Vec4(LT.x, LT.y, 0.f, 0.f), Vec4(_AreaData.AreaPos.x, _AreaData.AreaPos.y, Diameter, Diameter)))
	{
		Check = true;
		m_vecTerrainGeoVtx[(int)(m_TerrainBuffer.SizeX) * _Z + _X].TerrainHeight.y = BASEHEIGHT;
	}

	if (true == CollisionFunc::PointToCircle(Vec4(RT.x, RT.y, 0.f, 0.f), Vec4(_AreaData.AreaPos.x, _AreaData.AreaPos.y, Diameter, Diameter)))
	{
		Check = true;
		m_vecTerrainGeoVtx[(int)(m_TerrainBuffer.SizeX) * _Z + _X].TerrainHeight.z = BASEHEIGHT;
	}

	if (true == CollisionFunc::PointToCircle(Vec4(RB.x, RB.y, 0.f, 0.f), Vec4(_AreaData.AreaPos.x, _AreaData.AreaPos.y, Diameter, Diameter)))
	{
		Check = true;
		m_vecTerrainGeoVtx[(int)(m_TerrainBuffer.SizeX) * _Z + _X].TerrainHeight.w = BASEHEIGHT;
	}

	if (true == Check) 
	{
		m_vecTerrainGeoVtx[(int)(m_TerrainBuffer.SizeX) * _Z + _X].Color = NONAVITRICOLOR;
	}

}
void CTerrainRenderer::CreateFloor(const wchar_t * _MultiTexName)
{
	m_TerrainBuffer.FloorCount += 1;

	SPTR<CMultiTexture> MultiTex = CResourceMgr<CMultiTexture>::Find(_MultiTexName);
	if (nullptr == MultiTex)
	{
		assert(nullptr);
		return;
	}

	m_vecMultiTex.push_back(MultiTex);
	m_vecShaderResource.push_back(MultiTex->GetShaderResourceView());
}

void CTerrainRenderer::SetCellSize(const float& _CellSize)
{
	m_CellSize = _CellSize;
	SetRenSize(Vec3(m_CellSize, 10.f, m_CellSize));
}

const bool CTerrainRenderer::CheckGenerableNaviTri(const int& _GeoVtxIdx)
{
	if (_GeoVtxIdx >= m_vecTerrainGeoVtx.size())
	{
		TASSERT(true);
		return false;
	}

	return m_vecTerrainGeoVtx[_GeoVtxIdx].Color != NONAVITRICOLOR;
}
const bool CTerrainRenderer::CheckMoveablePosition(const Vec3& _Pos)
{
	int Row, Col;
	Row = (int)(_Pos.x / m_CellSize);
	Col = (int)(_Pos.z / m_CellSize);

	if (Row < 0 || Row >= m_TerrainBuffer.SizeX)
		return false;

	if (Col < 0 || Col >= m_TerrainBuffer.SizeZ)
		return false;


	int GeoVtxIdx = (int)(m_TerrainBuffer.SizeX) * Col + Row;

	return CheckGenerableNaviTri(GeoVtxIdx);
}
const Vec3 CTerrainRenderer::GetMiddlePos()
{
	return Vec3(m_TerrainBuffer.SizeX * m_CellSize * 0.5f, 10.f, m_TerrainBuffer.SizeZ * m_CellSize* 0.5f);
}
const Vec3 CTerrainRenderer::GetTerrainSize()
{
	return Vec3(m_TerrainBuffer.SizeX * m_CellSize, 0.f, m_TerrainBuffer.SizeZ * m_CellSize);
}

SPTR<CNavigationMesh> CTerrainRenderer::BakeNavigationMesh()
{
	m_TerrainNaviMesh = ACTOR->AddCom<CNavigationMesh>(RENDATA{ RENDER_LAYER_DEBUG });
	m_LinkNavigationMesh = ACTOR->AddCom<CNavigationMesh>(RENDATA{ RENDER_LAYER_DEBUG });

	m_TerrainNaviMesh->CreateNavigationMesh(this, NAVIHEIGHT);
	m_LinkNavigationMesh->CreateLinkNavigationMesh(&m_listLinkTri, this);
	m_bDebugRen = false;

	return m_TerrainNaviMesh;
}

SPTR<CNavigationMesh> CTerrainRenderer::BakeNavigationMeshInServer()
{
	m_TerrainNaviMesh = ACTOR->AddCom<CNavigationMesh>();
	m_LinkNavigationMesh = ACTOR->AddCom<CNavigationMesh>();

	m_TerrainNaviMesh->CreateNavigationMesh(this, NAVIHEIGHT);
	m_LinkNavigationMesh->CreateLinkNavigationMesh(&m_listLinkTri, this);
	m_bDebugRen = false;

	return m_TerrainNaviMesh;
}

SPTR<CNavigationMesh> CTerrainRenderer::GetTerrainNaviMesh()
{
	return m_TerrainNaviMesh;
}

void CTerrainRenderer::DeleteNavigationMesh()
{
	m_TerrainNaviMesh->Death();
	m_LinkNavigationMesh->Death();

	m_TerrainNaviMesh = nullptr;
	m_LinkNavigationMesh = nullptr;
	m_bDebugRen = true;
	Engine::InitAllNavigationMeshLinkTriList();
}

const IVec2 CTerrainRenderer::GetSelectTriIdx()
{
	return m_SelectTriIdx;
}
const bool CTerrainRenderer::IsSelectTri()
{
	return NONESELECTINDEX != m_SelectTriIdx;
}
const bool CTerrainRenderer::ConnectableTerrain()
{
	if (NONESELECTINDEX == m_FocusTriIdx)
		return false;

	return 	(NAVITRICOLOR == m_PrevColor);
}
void CTerrainRenderer::SelectTriUpdate(SPTR<CRayCollider> _pRay)
{
	if (true == _pRay->IsCollision())
		return;

	Vec3 Origin, Ray;
	float T;

	
	Origin = _pRay->GetRayOrigin();
	Ray = _pRay->GetRayDir();

	if (Ray.y == 0.f) 
	{
		if (m_FocusTriIdx != NONESELECTINDEX)
		{
			m_vecTerrainGeoVtx[(int)(m_TerrainBuffer.SizeX) * m_FocusTriIdx.iy + m_FocusTriIdx.ix].Color = m_PrevColor;
			m_FocusTriIdx = NONESELECTINDEX;
			m_TerrainGeoMesh->VBUpdate(0, (void*)&m_vecTerrainGeoVtx[0]);
		}
		//m_FocusTriIdx = NONESELECTINDEX;
		return;
	}

	T = (-Origin.y / Ray.y);

	if (T < 0.f) 
	{
		if (m_FocusTriIdx != NONESELECTINDEX)
		{
			m_vecTerrainGeoVtx[(int)(m_TerrainBuffer.SizeX) * m_FocusTriIdx.iy + m_FocusTriIdx.ix].Color = m_PrevColor;
			m_FocusTriIdx = NONESELECTINDEX;
			m_TerrainGeoMesh->VBUpdate(0, (void*)&m_vecTerrainGeoVtx[0]);
		}
		//m_FocusTriIdx = NONESELECTINDEX;
		return;
	}

	Vec3 Pos = Origin + (Ray * T);

	int X, Z;
	X = (int)(Pos.x / m_CellSize);
	Z = (int)(Pos.z / m_CellSize);

	if ( (X >= 0 && X < m_TerrainBuffer.SizeX) && (Z >= 0 && Z < m_TerrainBuffer.SizeZ))
	{	
		if ( m_FocusTriIdx != IVec2(X, Z)) 
		{
			if (m_FocusTriIdx != NONESELECTINDEX)
			{
				m_vecTerrainGeoVtx[(int)(m_TerrainBuffer.SizeX) * m_FocusTriIdx.iy + m_FocusTriIdx.ix].Color = m_PrevColor;
			}

			m_FocusTriIdx = IVec2(X, Z);
			m_PrevColor = m_vecTerrainGeoVtx[(int)(m_TerrainBuffer.SizeX) * m_FocusTriIdx.iy + m_FocusTriIdx.ix].Color;
			m_vecTerrainGeoVtx[(int)(m_TerrainBuffer.SizeX) * m_FocusTriIdx.iy + m_FocusTriIdx.ix].Color = SELECTTRICOLOR;
			m_TerrainGeoMesh->VBUpdate(0, (void*)&m_vecTerrainGeoVtx[0]);
		}
		
		//m_FocusTriIdx = IVec2(X, Z);
	}
	else 
	{
		if (m_FocusTriIdx != NONESELECTINDEX)
		{
			m_vecTerrainGeoVtx[(int)(m_TerrainBuffer.SizeX) * m_FocusTriIdx.iy + m_FocusTriIdx.ix].Color = m_PrevColor;
			m_FocusTriIdx = NONESELECTINDEX;
			m_TerrainGeoMesh->VBUpdate(0, (void*)&m_vecTerrainGeoVtx[0]);
		}
	}
}

void CTerrainRenderer::SelectTriOn() 
{
	if (m_PrevColor == NONAVITRICOLOR) 
	{
		m_PrevColor = NAVITRICOLOR;
	}
	
}
void CTerrainRenderer::SelectTriOff() 
{
	if (m_PrevColor == NAVITRICOLOR)
	{
		m_PrevColor = NONAVITRICOLOR;
	}	
	
}

void CTerrainRenderer::SelectTerrainTri() 
{
	if (m_FocusTriIdx != NONESELECTINDEX) 
	{
		if (m_FocusTriIdx != m_SelectTriIdx) 
		{
			InitTerrainVtxColor();
		}
		m_SelectTriIdx = m_FocusTriIdx;

	}
}
void CTerrainRenderer::InitSelectTerrainTri() 
{
	m_SelectTriIdx = NONESELECTINDEX;
	InitTerrainVtxColor();
}

void CTerrainRenderer::InitTerrainVtxColor() 
{
	m_TerrainVtxColor[0] = BASETERRAINVTXCOLOR;
	m_TerrainVtxColor[1] = BASETERRAINVTXCOLOR;
	m_TerrainVtxColor[2] = BASETERRAINVTXCOLOR;
	m_TerrainVtxColor[3] = BASETERRAINVTXCOLOR;
}
void CTerrainRenderer::InitTerrainVtxColor(const UINT& _Idx)
{
	if (_Idx > 4)
		return;

	m_TerrainVtxColor[_Idx] = BASETERRAINVTXCOLOR;
}
void CTerrainRenderer::SetTerrainVtxColor(const UINT& _Idx, const Vec4& _Color) 
{
	if (_Idx > 4)
	{
		return;
	}
	m_TerrainVtxColor[_Idx] = _Color;
}


void CTerrainRenderer::AddLinkVtx()
{
	m_MakingLinkTri->Update();
	m_listLinkTri.push_back(m_MakingLinkTri);

	m_MakingLinkTri = new CNaviLinkTri(this);
	
}

const bool CTerrainRenderer::DeleteLinkVtx(SPTR<CNaviLinkTri> _LinkTri)
{
	std::list<SPTR<CNaviLinkTri>>::iterator Start = m_listLinkTri.begin();
	std::list<SPTR<CNaviLinkTri>>::iterator End = m_listLinkTri.end();

	for (;Start!= End ;++Start)
	{
		if ((*Start) == _LinkTri) 
		{
			m_listLinkTri.erase(Start);
			return true;
		}
	}

	return false;
}

const bool CTerrainRenderer::DeleteLinkVtx(const UINT& _Idx)
{
	if (m_listLinkTri.size() <= _Idx)
	{
		return false;
	}

	std::list<SPTR<CNaviLinkTri>>::iterator Start = m_listLinkTri.begin();
	std::list<SPTR<CNaviLinkTri>>::iterator End = m_listLinkTri.end();

	for ( size_t Count = 0 ; Start != End; ++Start,++Count)
	{
		if (Count == _Idx) 
		{
			m_listLinkTri.erase(Start);
			return true;
		}
	}

	return false;

}
const bool CTerrainRenderer::IsConnectableTerrainTri(const IVec2& _Idx, const bool _BotTri)
{
	if (_Idx.ix < 0 || _Idx.ix >= (int)m_TerrainBuffer.SizeX)
		return false;

	if (_Idx.iy < 0 || _Idx.iy >= (int)m_TerrainBuffer.SizeZ)
		return false;
		

	UINT Idx = (_Idx.iy * (UINT)m_TerrainBuffer.SizeX + _Idx.ix) * 2;

	if (false == _BotTri)
	{
		++Idx;
	}

	if (nullptr == m_TerrainNaviMesh)
		return false;

	return 	m_TerrainNaviMesh->IsNavigationTriangle(Idx);
}
SPTR<CNaviLinkTri>	CTerrainRenderer::GetMakingLinkTri()
{
	return m_MakingLinkTri;
}

const Vec3 CTerrainRenderer::GetTerrainVtxPos(const IVec2& _TerrainIdx, const UINT& _Vtx)
{

	UINT Idx = ((int)m_TerrainBuffer.SizeX) * _TerrainIdx.iy + _TerrainIdx.ix;
	float X = (float)_TerrainIdx.ix;
	float Z = (float)_TerrainIdx.iy;
	switch (_Vtx)
	{
	// LB
	case 0: 
		return GetSubTransCRWorld().Mul_Coord(Vec3(X, m_vecTerrainGeoVtx[Idx].TerrainHeight.x, Z));
	// LT
	case 1:
		return GetSubTransCRWorld().Mul_Coord(Vec3(X, m_vecTerrainGeoVtx[Idx].TerrainHeight.y, Z + 1.f));
	// RT
	case 2:
		return GetSubTransCRWorld().Mul_Coord(Vec3(X + 1.f, m_vecTerrainGeoVtx[Idx].TerrainHeight.z, Z + 1.f));
	// RB
	case 3:
		return GetSubTransCRWorld().Mul_Coord(Vec3(X + 1.f, m_vecTerrainGeoVtx[Idx].TerrainHeight.w, Z));
	}
	
	TASSERT(true);
	return Vec3();
}


const Vec3 CTerrainRenderer::GetNaviTerrainVtxPos(const IVec2& _TerrainIdx, const UINT& _Vtx)
{
	if (nullptr == m_TerrainNaviMesh)
		return Vec3();

	UINT Idx = ((int)m_TerrainBuffer.SizeX) * _TerrainIdx.iy + _TerrainIdx.ix;
	float X = (float)_TerrainIdx.ix;
	float Z = (float)_TerrainIdx.iy;

	switch (_Vtx)
	{
		// LB
	case 0:
		return GetSubTransCRWorld().Mul_Coord(Vec3(X, m_vecTerrainGeoVtx[Idx].TerrainHeight.x + NAVIHEIGHT, Z));
		// LT
	case 1:
		return GetSubTransCRWorld().Mul_Coord(Vec3(X, m_vecTerrainGeoVtx[Idx].TerrainHeight.y + NAVIHEIGHT, Z + 1.f));
		// RT
	case 2:
		return GetSubTransCRWorld().Mul_Coord(Vec3(X + 1.f, m_vecTerrainGeoVtx[Idx].TerrainHeight.z + NAVIHEIGHT, Z + 1.f));
		// RB
	case 3:
		return GetSubTransCRWorld().Mul_Coord(Vec3(X + 1.f, m_vecTerrainGeoVtx[Idx].TerrainHeight.w + NAVIHEIGHT, Z));
	}

	TASSERT(true);
	return Vec3();
}

NaviTri* CTerrainRenderer::GetTerrainNaviTri(const IVec2& _TerrIdx,const bool& _BotTri)
{
	UINT Idx = (_TerrIdx.iy * (UINT)m_TerrainBuffer.SizeX + _TerrIdx.ix) * 2;

	if (false == _BotTri) 
	{
		++Idx;
	}

	return m_TerrainNaviMesh->GetNaviTri(Idx);
}
const size_t CTerrainRenderer::GetLinkTriCount()
{
	return m_listLinkTri.size();
}

void CTerrainRenderer::SelectLinkTri(const UINT& _Idx)
{
	if (m_SelectLinkTriIdx == _Idx)
	{
		return;
	}


	if (m_listLinkTri.size() <= _Idx)
	{
		return;
	}

	if (m_listLinkTri.size() >= m_SelectLinkTriIdx)
	{
		m_AllLinkTriVtx[m_SelectLinkTriIdx].VtxColor = Vec4::PastelBlue;
	}

	m_SelectLinkTriIdx = _Idx;
	m_AllLinkTriVtx[m_SelectLinkTriIdx].VtxColor = Vec4::Magenta;


}
const GEOTERRAINVTX& CTerrainRenderer::GetGeoTerrainVtx(const size_t& _Idx)
{
	return m_vecTerrainGeoVtx[_Idx];
}
const UINT CTerrainRenderer::GetTerrainRowCount() 
{
	return (UINT)m_TerrainBuffer.SizeX;
}
const UINT CTerrainRenderer::GetTerrainColmCount() 
{
	return (UINT)m_TerrainBuffer.SizeZ;
}

std::list<SPTR<CNaviLinkTri>>* CTerrainRenderer::GetLinkTriList()
{
	return &m_listLinkTri;
}


void CTerrainRenderer::LoadTerrainData(const TerrainData& _LoadData)
{
	InitTerrainHeight();

	if (nullptr != m_TerrainNaviMesh) 
	{
		m_TerrainNaviMesh->Death();
		m_TerrainNaviMesh = nullptr;
	}
	if (nullptr != m_LinkNavigationMesh)
	{
		m_LinkNavigationMesh->Death();
		m_LinkNavigationMesh = nullptr;
	}

	m_bDebugRen = true;


	size_t MaxVtx = _LoadData.TerrainDataVec.size();

	for (size_t i = 0; i < MaxVtx; i++)
	{
		m_vecTerrainGeoVtx[i] = _LoadData.TerrainDataVec[i];
	}

	m_TerrainGeoMesh->VBUpdate(0, &m_vecTerrainGeoVtx[0]);
	
	m_listLinkTri.clear();
	memset(m_AllLinkTriVtx, 0, sizeof(m_AllLinkTriVtx));

	for (size_t i = 0; i < MAXLINKCOUNT; i++)
	{
		m_AllLinkTriVtx[i].VtxColor = BASETERRAINVTXCOLOR;
	}
	
	std::list<TerrainData::NaviLinkData>::const_iterator Start = _LoadData.NaviLinkTriList.cbegin();
	std::list<TerrainData::NaviLinkData>::const_iterator End = _LoadData.NaviLinkTriList.cend();


	for (; Start != End; ++Start)
	{
		CNaviLinkTri* LoadTri = new CNaviLinkTri();
		LoadTri->m_LinkNaviMesh = Engine::FindNavigationMesh(Start->NaviMeshID);

		if (m_setLinkingNaviMesh.end() == m_setLinkingNaviMesh.find(LoadTri->m_LinkNaviMesh)) 
		{
			m_setLinkingNaviMesh.insert(LoadTri->m_LinkNaviMesh);
		}

		LoadTri->m_LinkTerrain = this;
		LoadTri->m_LinkNaviTriIdx = Start->NaviTriIdx;
		LoadTri->m_LinkTerrainIdx = Start->TerrainTriIdx;

		for (size_t i = 0; i < 3; i++)
		{
			LoadTri->m_LinkVtx[i] = Start->LinkVtxData[i];
		}
		LoadTri->Update();
		m_listLinkTri.push_back(LoadTri);
	}

}
void CTerrainRenderer::LoadTerrainData(const TerrainData& _LoadData, std::list<SPTR<CNavigationMesh>>* _listNavi)
{
	InitTerrainHeight();

	if (nullptr != m_TerrainNaviMesh)
	{
		m_TerrainNaviMesh->Death();
		m_TerrainNaviMesh = nullptr;
	}
	if (nullptr != m_LinkNavigationMesh)
	{
		m_LinkNavigationMesh->Death();
		m_LinkNavigationMesh = nullptr;
	}

	m_bDebugRen = false;


	size_t MaxVtx = _LoadData.TerrainDataVec.size();

	for (size_t i = 0; i < MaxVtx; i++)
	{
		m_vecTerrainGeoVtx[i] = _LoadData.TerrainDataVec[i];
	}

	m_TerrainGeoMesh->VBUpdate(0, &m_vecTerrainGeoVtx[0]);

	m_listLinkTri.clear();
	memset(m_AllLinkTriVtx, 0, sizeof(m_AllLinkTriVtx));

	for (size_t i = 0; i < MAXLINKCOUNT; i++)
	{
		m_AllLinkTriVtx[i].VtxColor = BASETERRAINVTXCOLOR;
	}

	std::list<TerrainData::NaviLinkData>::const_iterator Start = _LoadData.NaviLinkTriList.cbegin();
	std::list<TerrainData::NaviLinkData>::const_iterator End = _LoadData.NaviLinkTriList.cend();


	std::list<SPTR<CNavigationMesh>>::iterator StartNavi;
	std::list<SPTR<CNavigationMesh>>::iterator EndNavi;
	for (; Start != End; ++Start)
	{
		CNaviLinkTri* LoadTri = new CNaviLinkTri();

		StartNavi = _listNavi->begin();
		EndNavi = _listNavi->end();

		for (; StartNavi != EndNavi; ++StartNavi)
		{
			if ((*StartNavi)->GetSerialNumber() == Start->NaviMeshID) 
			{
				LoadTri->m_LinkNaviMesh = *StartNavi;
			}
		}
	
		if (m_setLinkingNaviMesh.end() == m_setLinkingNaviMesh.find(LoadTri->m_LinkNaviMesh))
		{
			m_setLinkingNaviMesh.insert(LoadTri->m_LinkNaviMesh);
		}

		LoadTri->m_LinkTerrain = this;
		LoadTri->m_LinkNaviTriIdx = Start->NaviTriIdx;
		LoadTri->m_LinkTerrainIdx = Start->TerrainTriIdx;

		for (size_t i = 0; i < 3; i++)
		{
			LoadTri->m_LinkVtx[i] = Start->LinkVtxData[i];
		}
		LoadTri->Update();
		m_listLinkTri.push_back(LoadTri);
	}
}

void CTerrainRenderer::InitNavigatorPosition(SPTR<CNavigator> _Navigator)
{
	if (nullptr == _Navigator || nullptr == m_TerrainNaviMesh)
		return;

	for (SPTR<CNavigationMesh> NaviMesh : m_setLinkingNaviMesh)
	{
		if (true == NaviMesh->ExistNaviTri(_Navigator->TRANS->GetWorldPos())) 
		{
			_Navigator->SetCurNaviMesh(NaviMesh);
			return;
		}
	}


	_Navigator->SetCurNaviMesh(m_TerrainNaviMesh);
}
