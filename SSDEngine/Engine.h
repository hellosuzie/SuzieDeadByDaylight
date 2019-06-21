#pragma once
#include <unordered_map>
#include "DXVertex.h"

#define OBJMAG 0.4f

class Engine 
{
	friend class SSDCore;

public:
	static std::unordered_map<UINT,class CNavigationMesh*>				mapNavigationMesh;

public:
	static void AddNavigationMesh(CNavigationMesh* _NaviMesh);
	static void DelNavigationMesh(CNavigationMesh* _NaviMesh);
	static CNavigationMesh* FindNavigationMesh(const UINT _ID);
	static void ClearNaviMeshMap();
	static void InitAllNavigationMeshLinkTriList();
public:
	template<typename OBJ>
	static OBJ* CreateTypeObject()
	{
		OBJ* NewObj = new OBJ();
		NewObj->InitType();
		return NewObj;
	}


	template<typename RES>
	static RES* CreateResourceObject()
	{
		RES* NewRes = new RES();
		NewRes->TypeInit();
		return NewRes;
	}

	template<typename RES,typename RESDATA>
	static RES* CreateResourceObject(const RESDATA& _Data)
	{
		RES* NewRes = new RES(_Data);
		NewRes->TypeInit();
		return NewRes;
	}

	template<typename RES>
	static RES* CreateGameObject()
	{
		RES* NewRes = new RES();
		NewRes->TypeInit();
		return NewRes;
	}

	template<typename RES,typename PARA>
	static RES* CreateGameObject(PARA* _P)
	{
		RES* NewRes = new RES(_P);
		NewRes->TypeInit();
		return NewRes;
	}


private:
	static void CreateEngineGraphicResource();

private:
	static void Create2DResource();
	static void Create3DResource();
	static void CreateBaseState();
	static void CreateMaterial();
	static void CreateRenderTarget();
	static void CreateSphereMesh(const int& _DetailLv);
	static void CreateConeMesh(const int& _DetailLv);
	static void CreateCylinder(const int& _DetailLv);
	static void CreateGeosphere(const unsigned int& _DetailLv);
	static void CreateGizmoMesh(const float& _Radius, const float& _Height);
	static void CreateFrameCube(const float& _InnerLen, const float& _OutLen, const float& _Height);
	static void CreateTubeMesh(const float& _InnerRad, const float& _OuterRad, const float& _Height);


private:
	static void Subdivide(std::vector<BASICVTX>& _vecVtx, std::vector<IDX16>& _vexIdx);


public:
	static bool LoadObjFile(const wchar_t* _FileName, bool _TextureCheck = true);
	static bool LoadNaviObjFile(const wchar_t* _FileName);

};

