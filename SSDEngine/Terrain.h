#pragma once
#include "Script.h"
#include <vector>
#include <list>
#include "NaviMeshArea.h"


class CNavigator;
class CTerrainRenderer;
class CTerrain : public CScript
{
private:
	SPTR<CTerrainRenderer>									m_TerrainRen;
	std::vector<SPTR<CNaviMeshArea>>						m_vecLinkArea;
	std::list<NaviAreaData>									m_listAreaData;


	SPTR<class CRayCollider>								m_pTerrainTriRay;
	
private:
	std::list<SPTR<CNavigator>>								m_listNavigator;

public:
	void On()override;
	void Off()override;
	const bool Init()override;
	const bool Init(const bool _bClient);
	void PostUpdate(const float& _DeltaTime) override;

public:
	void TerrainAreaUpdate(const float& _DeltaTime);

public:
	void InitTerrainHeight();
	void CreateTerrain(const wchar_t* _Tex, const wchar_t* _BaseDiffuseTex, const wchar_t* _BaseBumpTex, int _X, int _Z, float _Height );
	void CreateTerrain(int _X, int _Z, float _Height);
	void CreateFloor(const wchar_t* _MultiTexName);
	void SetCellSize(const float& _CellSize);
	void BakeNavigationMesh();
	void BakeNavigationMeshInServer();
	void DeleteNavigationMesh();

public:
	const bool IsNavigationBake();
	const bool CheckMoveablePosition(const Vec3& _Pos);

public:
	SPTR<CTerrainRenderer> GetTerrainRen();
	const Vec3 GetMiddlePos();
	const Vec3 GetTerrainSize();

public:
	void LinkNaviMeshArea(SPTR<CNaviMeshArea> _pArea);
	void SetTerrainColRay(SPTR<CRayCollider> _pRay);

public:
	const bool IsTerrainRayCollsion();
	void LoadTerrainData(const struct TerrainData& _LoadData);
	void LoadTerrainData(const struct TerrainData& _LoadData,std::list<SPTR<CNavigationMesh>>* _listNavi);

public:
	void PushNavigator(SPTR<CNavigator> _Navi);
	void InitNavigatorPosition();

public:
	CTerrain();
	~CTerrain();
};

