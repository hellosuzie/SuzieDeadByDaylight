#include "stdafx.h"
#include "SSDToolGlobal.h"

#include "MaterialDlg.h"
#include "LightDlg.h"
#include "FilterDlg.h"
#include "NaviDlg.h"
#include "FogDlg.h"
#include <Gizmo.h>
#include <NavigationObject.h>

CScene*					SSDToolGlobal::ToolGlobalScene = nullptr;
CToolMainScene*			SSDToolGlobal::ToolMainScene = nullptr;
CActor*					SSDToolGlobal::ToolSelectActor = nullptr;
CRenderer*				SSDToolGlobal::ToolSelectRenderer = nullptr;
CCollider*				SSDToolGlobal::ToolSelectCollider = nullptr;
CSkeletalFbxRenderer*	SSDToolGlobal::ToolSkelRender = nullptr;
CLight*					SSDToolGlobal::ToolSelectLight = nullptr;
CBloomFilter*			SSDToolGlobal::ToolSelectBloom = nullptr;
CFogFilter*				SSDToolGlobal::ToolSelectFog = nullptr;
CGaussianBlur*			SSDToolGlobal::ToolSelectGaussian = nullptr;

std::wstring			SSDToolGlobal::ToolSelectMesh;
std::wstring			SSDToolGlobal::ToolSelectMat;

CFbx*					SSDToolGlobal::ToolSelectFbx = nullptr;
CBoneMesh*				SSDToolGlobal::ToolBoneMesh = nullptr;
CFBXMesh*				SSDToolGlobal::ToolFbxMesh = nullptr;
int						SSDToolGlobal::ToolSelectMeshIndex = 0;
int						SSDToolGlobal::ToolSelectSubset = 0;
CGizmo*					SSDToolGlobal::ToolMainGizmo = nullptr;

CNavigationObject*		SSDToolGlobal::ToolSelectNaviObj = nullptr;
CNavigationMesh*		SSDToolGlobal::ToolSelectNaviMesh = nullptr;
CNaviMeshArea*			SSDToolGlobal::ToolSelectNaviArea = nullptr;

EditDlg*				SSDToolGlobal::ToolEditDlg = nullptr;
ObjectDlg*				SSDToolGlobal::ToolObjectDlg = nullptr;
SceneListDlg*			SSDToolGlobal::ToolSceneList = nullptr;
EditView*				SSDToolGlobal::ToolEditView = nullptr;
ControlDlg*				SSDToolGlobal::ToolControlDlg = nullptr;

LightDlg*				SSDToolGlobal::ToolLightDlg = nullptr;
MaterialDlg*			SSDToolGlobal::ToolMatDlg = nullptr;
FilterDlg*				SSDToolGlobal::ToolFilterDlg = nullptr;
NaviDlg*				SSDToolGlobal::ToolNaviDlg = nullptr;
FogDlg*					SSDToolGlobal::ToolFogDlg = nullptr;

bool					SSDToolGlobal::CollEdit = false;
std::unordered_set<CNavigationObject*>	SSDToolGlobal::setToolNaviObj;

void SSDToolGlobal::Release()
{
	setToolNaviObj.clear();
}

void SSDToolGlobal::ResetDynamicDlg()
{
	if (nullptr != SSDToolGlobal::ToolLightDlg)
	{
		SSDToolGlobal::ToolLightDlg->DestroyWindow();
		delete SSDToolGlobal::ToolLightDlg;
		SSDToolGlobal::ToolLightDlg = nullptr;
	}
	if (nullptr != SSDToolGlobal::ToolMatDlg)
	{
		SSDToolGlobal::ToolMatDlg->DestroyWindow();
		delete SSDToolGlobal::ToolMatDlg;
		SSDToolGlobal::ToolMatDlg = nullptr;
	}
	if (nullptr != SSDToolGlobal::ToolFilterDlg)
	{
		SSDToolGlobal::ToolFilterDlg->DestroyWindow();
		delete SSDToolGlobal::ToolFilterDlg;
		SSDToolGlobal::ToolFilterDlg = nullptr;
	}
	if (nullptr != SSDToolGlobal::ToolNaviDlg)
	{
		SSDToolGlobal::ToolNaviDlg->DestroyWindow();
		delete SSDToolGlobal::ToolNaviDlg;
		SSDToolGlobal::ToolNaviDlg = nullptr;
	}
	if (nullptr != SSDToolGlobal::ToolFogDlg)
	{
		SSDToolGlobal::ToolFogDlg->DestroyWindow();
		delete SSDToolGlobal::ToolFogDlg;
		SSDToolGlobal::ToolFogDlg = nullptr;
	}
}

const float SSDToolGlobal::InttoFloat(int _int)
{
	float RFloat = 0.f;
	RFloat = _int * 0.1f;
	return RFloat;
}

const int SSDToolGlobal::FloattoInt(float _float)
{
	int RInt = 0;
	RInt = (int)_float * 10;
	return RInt;
}

void SSDToolGlobal::SetSelectActor(class CActor* _Actor)
{
	if (nullptr == _Actor)
	{
		return;
	}
	if (ToolSelectActor != nullptr && ToolSelectActor != _Actor)
	{
		SPTR<CGizmo> Temp = ToolSelectActor->FindCom<CGizmo>();
		if (nullptr != Temp)
		{
			Temp->GizmoOff();
		}
	}
	ToolSelectActor = _Actor;
}

const bool SSDToolGlobal::SetSelectNavigationCom(CActor* _Actor)
{
	if (nullptr == _Actor) 
	{
		TASSERT(true);
		return false;
	}

	SPTR<CNavigationObject> SelectNaviObj = _Actor->FindCom<CNavigationObject>();

	if (nullptr == SelectNaviObj) 
	{
		ToolSelectNaviObj = nullptr;
		ToolSelectNaviMesh = nullptr;
		ToolSelectNaviArea = nullptr;
		return false;
	}

	if (ToolSelectNaviObj == SelectNaviObj) 
	{
		return false;
	}

	ToolSelectNaviObj = SelectNaviObj;
	ToolSelectNaviMesh = ToolSelectNaviObj->GetNaviRen();
	ToolSelectNaviArea = ToolSelectNaviObj->GetNaviArea();

	return true;
}

void SSDToolGlobal::EraseNaviObj(CNavigationObject* _pNaviObj)
{
	std::unordered_set<CNavigationObject*>::iterator Find = setToolNaviObj.find(_pNaviObj);

	if (Find != setToolNaviObj.end())
	{
		setToolNaviObj.erase(Find);
	}


}