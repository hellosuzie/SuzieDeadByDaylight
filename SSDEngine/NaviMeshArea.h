#pragma once
#include "Base3DRenderer.h"
#include <vector>
#include "DXVertex.h"

enum NAVIAREATYPE
{
	NAVIAREA_RECT,			//	디버깅 모형은 큐브
	NAVIAREA_CIRCLE,		//  디버깅 모형은 실린더
	MAX_NAVIAREATYPE,
};

typedef struct _tagNAVIAREADATA 
{
	UINT				AreaNo;
	UINT				AreaType;
	bool				bChangeValue;
	Vec2				AreaPos;
	Vec2				AreaSize;
	Vec2				AreaAxisX;
	Vec2				AreaAxisZ;

}NaviAreaData;

class CNaviMeshArea :public CBase3DRenderer
{
private:
	static bool										bAreaRender;

public:
	static void ChangeAreaRendering();
private:
	friend class CTerrain;

private:
	std::vector<NaviAreaData>						m_vecNaviArea;
	bool											m_bUpdateArea;
	CBUFFER::NaviAreaBuffer							m_AreaBuffer;

public:
	const bool Init(SPTR<CTerrain> _pTerrain);
	void PostUpdate(const float& _DeltaTime)override;
	void LastUpdate(const float& _DeltaTime)override;
	void Render(SPTR<class CCamera> _Cam) override;
public:
	const size_t AddAreaData(const NAVIAREATYPE& _Type, const Vec2& _Pivot, const Vec2& _Size);
	const size_t AddRectArea(const Vec2& _Pivot, const Vec2& _Size);
	const size_t AddCircleArea(const Vec2& _Pivot, const float& _Radius);

	void ModifyAreaPivot(const int& _Idx, const Vec2& _Pivot);
	void ModifyAreaSize(const int& _Idx, const Vec2& _Size);


public:
	const bool IsMoveArea();
	const bool IsUpdateArea();

public:
	const size_t GetAreaCount();
	const NaviAreaData& GetAreaData(const int& _Idx);

public:
	CNaviMeshArea();
	~CNaviMeshArea();
};

