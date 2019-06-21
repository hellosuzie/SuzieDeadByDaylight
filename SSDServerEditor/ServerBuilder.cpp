#include "stdafx.h"
#include "ServerBuilder.h"
#include <Collider.h>
#include <Scene.h>
#include <FilePathMgr.h>
#include <DeadByDaylightGlobal.h>
#include "ServerScene.h"


CServerBuilder::CServerBuilder()
{
}


CServerBuilder::~CServerBuilder()
{
}






const BOOL CServerBuilder::Build()
{
	GlobalDBD::Init();
	CCollider::ColliderInit();

	CScene::CreateScene<CServerScene>(L"MainScene", true);
	CScene::ChangeScene(L"MainScene");


	return TRUE;
}

void CServerBuilder::BuilderProc(HWND _hWnd, UINT _Msg, WPARAM _wParam, LPARAM _lParam)
{

}

const BOOL CServerBuilder::Release()
{
	GlobalDBD::Release();

	return TRUE;
}
