#pragma once
#include "Renderer.h"
#include "DXVertex.h"
#include "NavigationMesh.h"
#include "RayCollider.h"
#include "DXStruct.h"
#include <unordered_map>
#include <set>

#define PIVOTHIEGHT 1.f

#define NAVITRICOLOR Vec4::PastelBlue
#define NONAVITRICOLOR Vec4::PastelRed
#define SELECTTRICOLOR Vec4::Magenta

#define BASEHEIGHT 0.f

#define NONESELECTINDEX Vec2(-1.f,-1.f)
#define BASETERRAINVTXCOLOR Vec4::PastelBlue

#define MAXLINKCOUNT 1024

#define NAVIHEIGHT 0.2f

#define CLIPMAGICNUMBER 1.1f

class CNaviLinkTri : public CGameObject
{
private:
	friend class CTerrainRenderer;
private:
	CNavigationMesh*					m_LinkNaviMesh;
	CTerrainRenderer*					m_LinkTerrain;
	UINT								m_LinkNaviTriIdx;
	IVec2								m_LinkTerrainIdx;
public:
	LinkVtx								m_LinkVtx[3];


public:
	void Init();
	const bool Update();
	
public:
	void LinkNavigationMesh(CNavigationMesh* _LinkNaviMesh);
	void LinkTerrainRen(CTerrainRenderer* _TerrainRen);
	void SetNaviMeshTriIdx(const UINT& _TriIdx);
	void SetLinkTerrainIdx(const IVec2& _TerIdx);

public:
	void SetLinkVtx(const UINT& _Idx, const bool& _NaviMesh, const UINT& _ID);
	const bool IsLinkTriangle();

public:
	CNavigationMesh* GetLinkNaviMesh() { return m_LinkNaviMesh; }
	const UINT GetLinkNaviTriIdx() { return m_LinkNaviTriIdx; }
	CTerrainRenderer* GetLinkTerrain() { return m_LinkTerrain; }
	const IVec2 GetLinkTerrainIdx() { return m_LinkTerrainIdx; }
	NaviTri* GetLinkNaviTri();
	NaviTri* GetLinkTerrainTri(const bool& _bBottomTri);
	NaviTri* GetTerrainNaviTri(const IVec2& _TerIdx,const bool& _bBottomTri);

public:
	CNaviLinkTri();
	CNaviLinkTri(CTerrainRenderer* _pTerrainRen);
	~CNaviLinkTri();
};


class CTerrainRenderer : public CRenderer
{
private:
	friend class CTerrain;
	friend CNavigationMesh;

private:
	CTerrain*												m_pTerrain;
	SPTR<class CMesh>										m_TerrainMesh;
	SPTR<CMesh>												m_TerrainGeoMesh;
	
	std::vector<FBXSTATICVTX>								m_vecTerrainVtx;
	std::vector<GEOTERRAINVTX>								m_vecTerrainGeoVtx;
	std::vector<IDX32>										m_vecTerrainIdx;

	SPTR<class CTexture>									m_HeightTexture;
	SPTR<CTexture>											m_BaseTexture;
	SPTR<CTexture>											m_BaseBumpTexture;

	CBUFFER::TerrainBuffer									m_TerrainBuffer;
	float													m_fMaxHeight;
	float													m_fNaviHeight;
	float													m_CellSize;

	bool													m_bDebugRen;

public:
	std::vector<SPTR<class CMultiTexture>>					m_vecMultiTex;
	std::vector<ID3D11ShaderResourceView*>					m_vecShaderResource;



public:
	SPTR<CNavigationMesh>									m_TerrainNaviMesh;
	SPTR<CNavigationMesh>									m_LinkNavigationMesh;

	IVec2													m_FocusTriIdx;
	IVec2													m_SelectTriIdx;
	Vec4													m_PrevColor;
	Vec4													m_TerrainVtxColor[4];


private:
	SPTR<CNaviLinkTri>										m_MakingLinkTri;
	NAVITRIVTX												m_LinkTriVtx;
	SPTR<CMesh>												m_LinkTriMesh;
	SPTR<CMaterial>											m_LinkTriMtl;


	std::list<SPTR<CNaviLinkTri>>							m_listLinkTri;
	SPTR<CMesh>												m_AllLinkTriMesh;
	NAVITRIVTX												m_AllLinkTriVtx[MAXLINKCOUNT];
	size_t													m_SelectLinkTriIdx;

private:
	std::set<SPTR<CNavigationMesh>>							m_setLinkingNaviMesh;

private:
	std::function<void(SPTR<CTerrainRenderer>)>				m_TerrainLBCallBack;
	std::function<void(SPTR<CTerrainRenderer>)>				m_TerrainRBCallBack;

public:
	SPTR<CCamera>											m_ViewCamera;

public:
	template<typename T>
	void SetLBCallBackFunc(T* _Obj, void(T::*_pFunc)(SPTR<CTerrainRenderer>))
	{
		if (nullptr == _Obj)
			return;

		m_TerrainLBCallBack = std::bind(_pFunc, _Obj, std::placeholders::_1);
	}

	template<typename T>
	void SetRBCallBackFunc(T* _Obj, void(T::*_pFunc)(SPTR<CTerrainRenderer>))
	{
		if (nullptr == _Obj)
			return;

		m_TerrainRBCallBack = std::bind(_pFunc, _Obj, std::placeholders::_1);
	}


protected:
	void PostUpdate(const float& _DeltaTime) override;

public:
	const bool Init()override;
	const bool Init(const RENDATA& _InitData);

	void RenderUpdate(SPTR<CCamera> _Cam) override;
	void Render(SPTR<class CCamera> _Cam) override;

public:
	void InitTerrainHeight();
	void NaviRectAreaCollisionEvent(const struct _tagNAVIAREADATA& _AreaData ,const int& _X, const int& _Z);
	void NaviRectAreaCollisionEvent(const Vec2& _Area0, const Vec2& _Area1, const Vec2& _Area2, const Vec2& _Area3, const int& _X, const int& _Z);
	void NaviCircleAreaCollisionEvent(const struct _tagNAVIAREADATA& _AreaData, const int& _X, const int& _Z);
	void CreateTerrain(const wchar_t* _Tex, const wchar_t* _BaseDiffuseTex, const wchar_t* _BaseBumpTex, int _X, int _Z, float _MaxHeight = 3.f);
	void CreateTerrain(int _X, int _Z, float _MaxHeight = 3.f);

	void CreateFloor(const wchar_t* _MultiTexName);
	void SetCellSize(const float& _CellSize);

public:
	const bool CheckGenerableNaviTri(const int& _GeoVtxIdx);
	const bool CheckMoveablePosition(const Vec3& _Pos);

public:
	const Vec3 GetMiddlePos();
	const Vec3 GetTerrainSize();
	const float GetCellSize() { return m_CellSize; }
	const float GetMaxHeight() { return m_fMaxHeight; }
	
public:
	SPTR<CNavigationMesh> BakeNavigationMesh();
	SPTR<CNavigationMesh> BakeNavigationMeshInServer();
	SPTR<CNavigationMesh> GetTerrainNaviMesh();
	void DeleteNavigationMesh();

public:
	const IVec2 GetSelectTriIdx();
	const bool IsSelectTri();
	const bool ConnectableTerrain();
public:
	void SelectTriUpdate(SPTR<CRayCollider> _pRay);
	void SelectTriOn();
	void SelectTriOff();

public:
	void SelectTerrainTri();
	void InitSelectTerrainTri();

public:
	void InitTerrainVtxColor();
	void InitTerrainVtxColor(const UINT& _Idx);
	void SetTerrainVtxColor(const UINT& _Idx,const Vec4& _Color);


public:
	void AddLinkVtx();
	const bool DeleteLinkVtx(SPTR<CNaviLinkTri> _LinkTri);
	const bool DeleteLinkVtx(const UINT& _Idx);
	const bool IsConnectableTerrainTri(const IVec2& _Idx, const bool _BotTri);

public:
	SPTR<CNaviLinkTri>	GetMakingLinkTri();
	const Vec3 GetTerrainVtxPos(const IVec2& _TerrainIdx, const UINT& _Vtx);
	const Vec3 GetNaviTerrainVtxPos(const IVec2& _TerrainIdx, const UINT& _Vtx);

public:
	NaviTri* GetTerrainNaviTri(const IVec2& _TerrIdx,const bool& _BotTri);
	const size_t GetLinkTriCount();
	void SelectLinkTri(const UINT& _Idx);

public:
	const GEOTERRAINVTX& GetGeoTerrainVtx(const size_t& _Idx);
	const UINT GetTerrainRowCount();
	const UINT GetTerrainColmCount();
	std::list<SPTR<CNaviLinkTri>>* GetLinkTriList();

public:
	void LoadTerrainData(const struct TerrainData& _LoadData);
	void LoadTerrainData(const TerrainData& _LoadData, std::list<SPTR<CNavigationMesh>>* _listNavi);
public:
	void InitNavigatorPosition(SPTR<CNavigator> _Navigator);


public:
	CTerrainRenderer();
	~CTerrainRenderer();
};

