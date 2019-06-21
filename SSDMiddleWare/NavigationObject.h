#pragma once
#include <Script.h>


enum NAVIMESH_TYPE
{
	NAVIMESH_OBJ,
	NAVIMESH_FBX,
	MAX_NAVIMESH_TYPE
};


class CNavigationObject : public CScript
{
private:
	SPTR<class CFbxRenderer>						m_DrawRen;
	SPTR<class CNavigationMesh>						m_NaviRen;
	SPTR<class CNaviMeshArea>						m_NaviArea;
	SPTR<class CGizmo>								m_ObjGizmo;

public:
	const bool Init();

public:
	SPTR<CFbxRenderer> CreateFbxRenderer(const struct RENDATA& _InitData);
	SPTR<CNavigationMesh> CreateNavigationRenderer();
	SPTR<CNavigationMesh> CreateNavigationRenderer(const struct NAVIMESH_DATA& _InitData);
	SPTR<class CNaviMeshArea> CreateNaviArea(SPTR<class CTerrain> _pTerrain);


public:
	void SetDrawFbx(const wchar_t* _FbxKey);
	void SettingFbxMesh(const int& _MeshIdx);
	void CreateNavigationMesh(const wchar_t* _Key,const NAVIMESH_TYPE& _MeshType);
	
public:
	SPTR<CFbxRenderer>		GetDrawRen();
	SPTR<CNavigationMesh>	GetNaviRen();
	SPTR<CNaviMeshArea>		GetNaviArea();

public:
	void SetGizmoPivot(const Vec3& _Pivot);

public:
	CNavigationObject();
	~CNavigationObject();
};

