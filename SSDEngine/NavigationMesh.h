#pragma once
#include "Base3DRenderer.h"
#include "DXHeader.h"
#include "DXVertex.h"
#include <list>
#include <vector>
#include "Base3DRenderer.h"
#include "Mesh.h"

#define INITNAVIHIEGHT 100000.f
#define MAXNAVIHEIGHT  200000.f

#define NOADJVTXCOLOR Vec4(0.5f,0.5f,0.5f,1.f)
#define CURVTXCOLOR Vec4::PastelGreen
#define ADJVTXCOLOR Vec4::PastelBlue
#define NOENTERTRI Vec4::PastelRed
#define COLTRICOLOR Vec4::Magenta
#define LINKTRICOLOR Vec4::LightYellow
  
#define BASEVTXCOLOR Vec4::PastelBlue

typedef struct _tagNAVITRIANLGE
{
public:
	class CNavigationMesh*						pBelongNaviMesh;
	_tagNAVITRIANLGE**							pTriHead;
	unsigned int								Index;
	Vec3										Tri[3];

	// 다른 네비게이션 메쉬와 링크된 삼각형 목록
	std::list<_tagNAVITRIANLGE*>				listLinkNaviTri;


	// 선분을 공유(두 정점을 공유)하는 삼각형의 인덱스 
	int											AdjacentTriIdx[3];
	int											AdjCount;
	// 한 정점을 공유하는 삼각형의 인덱스
	std::list<int>								SharePointTriIdx;
	float										RayPara;
	Vec3										Ray;

	const bool TriangleTest(const Vec3& _Pos, const Mat& _WorldMat);
	const bool TriangleTest(const Vec3& _Pos);
	_tagNAVITRIANLGE* IsMelee(const Vec3& _Pos, const Mat& _WorldMat);
	const float GetNavigationHeight(const Vec3& _Pos, const Mat& _MeshMat);
	const bool IsIsolatedTri();

	void PushLinkTri(_tagNAVITRIANLGE* _LinkTri);

public:
	void InitCurNaviTriColor();
	void ChangeNaviTriColor();
	const bool CheckMoveable(const Vec3& _Pos);
	const bool CheckMoveable(const Vec3& _Pos, _tagNAVITRIANLGE** pMeleeTri);

	CNavigationMesh* CheckHandOverNaviMesh(const Vec3& _Pos, _tagNAVITRIANLGE** _pHandOverTri);

	_tagNAVITRIANLGE()
		:Index(0), AdjCount(0), RayPara(MAXNAVIHEIGHT), Ray(Vec3())
		,pBelongNaviMesh(nullptr)
	{
		ZeroMemory(Tri, sizeof(Tri));
		ZeroMemory(AdjacentTriIdx, sizeof(AdjacentTriIdx));
	}
	_tagNAVITRIANLGE(CNavigationMesh* _pNaviMesh)
		:Index(0), AdjCount(0), RayPara(MAXNAVIHEIGHT), Ray(Vec3())
		, pBelongNaviMesh(_pNaviMesh)
	{
		ZeroMemory(Tri, sizeof(Tri));
		ZeroMemory(AdjacentTriIdx, sizeof(AdjacentTriIdx));
	}
	~_tagNAVITRIANLGE() {}
}NaviTri;


struct NAVIMESH_DATA 
{
	RENDER_LAYER			Layer;
	UINT					SerialNumber;
	std::wstring			NaviObjName;
};

class CNavigator;
class CMeshCollider;
typedef class CNavigationMesh : public CBase3DRenderer
{
private:
	friend NaviTri;

private:
	friend CMeshCollider;
	static IVec2										LinkAdjTriMatrix[3][3];

private:
	std::vector<NaviTri*>								m_vecNaviTri;
	UINT												m_SerialNumber;
	SPTR<CMesh>											m_DrawNaviMesh;
	//NaviTri*											m_pCurTri;

private:
	std::vector<NAVITRIVTX>								m_GeoNaviVtx;
	SPTR<CMesh>											m_GeoNaviMesh;
	SPTR<class CMeshCollider>							m_NaviMeshCol;
	UINT												m_ColTriIdx;
	UINT												m_SelectTriIdx;
	Vec4												m_VtxColor[3];

private:
	bool												m_bNoClick;


private:
	std::function<void(SPTR<CNavigationMesh>)>			m_NaviMeshClickCallBack;


public:
	template<typename T>
	void SetNaviMeshClickFunc(T* _Obj, void(T::*_pFunc)(SPTR<CNavigationMesh>))
	{
		if (nullptr == _Obj)
			return;

		m_NaviMeshClickCallBack = std::bind(_pFunc, _Obj, std::placeholders::_1);
	}


public:
	const bool Init() override;
	const bool Init(const RENDATA& _InitData);
	const bool Init(const NAVIMESH_DATA& _InitData);
	void PreUpdate(const float& _DeltaTime) override;
	void Update(const float& _DeltaTime) override;
	void PostUpdate(const float& _DeltaTime) override;
	void RenderUpdate(SPTR<CCamera> _Cam) override;
	void Render(SPTR<CCamera> _Cam) override;

public:
	void SetNaviData(const NAVIMESH_DATA& _InitData);
	NaviTri* SearchNaviTri(const Vec3& _Pos);
	const bool ExistNaviTri(const Vec3& _Pos);
	void NaviMeshUpdate();
//	const bool CheckMoveable(const Vec3& _Pos);
//	void MoveNavigaion(const Vec3& _Pos,SPTR<CTransForm> _NavigatorTrans);
//	void MoveNavigaion(const Vec3& _Pos, SPTR<CNavigator> _Navigator);
//	const float GetNavigationHeight(const Vec3& _Pos);
//	CNavigationMesh* CheckHandOverNaviMesh(const Vec3& _Pos);
//
//public:
//	void ChangeCurNaviTri(NaviTri* _pTri);
//	void InitCurNaviTriColor();

public:
	void CreateNavigationMesh(std::vector<Vec3>& _Vtx, std::vector<IDX32>& _Idx, const float& _Min, const float& _Max);
	void CreateNavigationMesh(SPTR<class CTerrainRenderer> _Terrain, const float& _NaviHeight);
	void CreateNavigationMesh(SPTR<class COBJMesh> _pObjMesh);
	void CreateNavigationTriangle(SPTR<COBJMesh> _pObjMesh);
	void CreateLinkNavigationMesh(std::list<SPTR<class CNaviLinkTri>>* _LinkList,SPTR<CTerrainRenderer> _Terrain);

public:
	void CreateNaviMeshCollider(const wchar_t* _ColChannel);
public:
	void ExitNavigatorEvent();

public:
	SPTR<CMesh> GetGeoNaviMesh();
	NaviTri* GetNaviTri(const UINT& _Idx) { return m_vecNaviTri[_Idx]; }

public:
	void NaviColEnter(SPTR<class CCollider> _This, SPTR<class CCollider> _Other);
	void NaviColStay(SPTR<class CCollider> _This, SPTR<class CCollider> _Other);
	void NaviColExit(SPTR<class CCollider> _This, SPTR<class CCollider> _Other);


public:
	void SelectTriIdx();
	void SetSelectTriVtx(const UINT& _VtxIdx,const Vec4& _Color);
	void InitSelectVtxColor();
	void InitSelectTriIdx();
	void InitSelectTriIdx(const UINT& _VtxIdx);

public:
	const Vec3 GetNaviMeshVtxPos(const UINT& _TriIdx, const UINT& _VtxIdx);
	const UINT GetSelectTriIdex();
	const UINT GetSerialNumber();

public:
	void SetSerialNumber(const UINT& _SerialNumber) { m_SerialNumber = _SerialNumber; }
	const bool IsNavigationTriangle(const UINT& _Idx);

public:
	void FindAllConnectedTri(std::list<_tagNAVITRIANLGE*>* _pList,int* _pLinkTriData);
	void ClearLinkTriList();

public:
	void SetNaviTriColor(const UINT& _Idx, const Vec4& _Color);


public:
	CNavigationMesh();
	~CNavigationMesh();
}CNaviMesh, CNavigationMeshRenderer;

