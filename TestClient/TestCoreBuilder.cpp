#include <ClientConnectMgr.h>
#include "TestCoreBuilder.h"
#include <Collider.h>
#include <Scene.h>
#include <FilePathMgr.h>
#include "TestScene.h"
#include "TestScene2.h"
#include "IntroScene.h"
#include <DeadByDaylightGlobal.h>

CTestCoreBuilder::CTestCoreBuilder()
{
}


CTestCoreBuilder::~CTestCoreBuilder()
{
}
const BOOL CTestCoreBuilder::Build()
{
	GlobalDBD::Init();
	GETSINGLE(CClientConnectMgr).Init();
	CCollider::ColliderInit();
	

	// 씬로딩과  접속 로직은 쓰레드로 돌리고
	// 두 작업이 끝나면 로비 씬으로 이동
	// 1. 인트로 씬 로딩 : 텍스쳐,스프라이트 와같은 그래픽 리소스 로드
	// 2. 로비 씬 로딩 : 로비 UI 및  로비씬용 캐릭터 미리 만들기
	// 3. 메인(게임) 씬 로딩 : 프리팹 데이터 로드 및 오브젝트 미리 생성
	// 서버가 연결 된 씬 
	//CScene::CreateScene<CTestScene>(L"TestScene", true);
	//CScene::CreateScene<CIntroScene>(L"IntroScene", true);
	//CScene::ChangeScene(L"IntroScene");



	// 서버가 연결 안된 씬
	CScene::CreateScene<CTestScene2>(L"TestScene2", true);
	CScene::ChangeScene(L"TestScene2");

	return TRUE;
}

void CTestCoreBuilder::BuilderProc(HWND _hWnd, UINT _Msg, WPARAM _wParam, LPARAM _lParam)
{

}
const BOOL CTestCoreBuilder::Release()
{
	GlobalDBD::Release();
	//GETSINGLE(ClientConnector).DisConnectLogicServer();
	return TRUE;
}


CORESTART(CTestCoreBuilder, L"Test Client", Vec2(1360, 768), Vec2(0, 15))