#include "stdafx.h"
#include "ToolCoreBuilder.h"
#include <Collider.h>
#include <Scene.h>
#include <FilePathMgr.h>
#include "ToolMainScene.h"
#include "SSDToolGlobal.h"
#include <DeadByDaylightGlobal.h>
#include "SSDToolGlobal.h"

CToolCoreBuilder::CToolCoreBuilder()
{

}


CToolCoreBuilder::~CToolCoreBuilder()
{

}


const BOOL CToolCoreBuilder::Build()
{
	GlobalDBD::Init();
	CCollider::ColliderInit();

	CScene::CreateScene<CToolMainScene>(L"MainScene", true);
	CScene::ChangeScene(L"MainScene");

	SSDToolGlobal::ToolGlobalScene = CScene::FindScene(L"MainScene");


	return TRUE;
}

void CToolCoreBuilder::BuilderProc(HWND _hWnd, UINT _Msg, WPARAM _wParam, LPARAM _lParam)
{

}

const BOOL CToolCoreBuilder::Release()
{
	GlobalDBD::Release();
	SSDToolGlobal::Release();

	return TRUE;
}
