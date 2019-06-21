#include "SSDCore.h"
#include "GameWindow.h"
#include "FilePathMgr.h"
#include "TimerMgr.h"
#include "ResourceMgr.h"
#include "Mesh.h"
#include "VtxShader.h"
#include "PixShader.h"
#include "ConstantBuffer.h"
#include "SamplerState.h"
#include "BlendState.h"
#include "DXVertex.h"
#include "KeyMgr.h"
#include "DXMacro.h"
#include "Sound.h"
#include "DXStruct.h"
#include "Collider.h"
#include "GlobalDebug.h"
#include "SoundPlayer.h"
#include "Device.h"
#include "Engine.h"
#include "ResourceMgr.h"
#include "Fbx.h"
#include "Scene.h"
#include "Profiler.h"
#include "InstancingBuffer.h"

//////////////////////////////////////////////////				CoreBuilder		//////////////////////////////////////////////
CCoreBuilder::CCoreBuilder()
{

}
CCoreBuilder::~CCoreBuilder()
{

}




///////////////////////////////////////////////////				Core			////////////////////////////////////////////

bool				SSDCore::m_bCoreProgress = true;
bool				SSDCore::m_bFocusing = true;
CCoreBuilder*		SSDCore::m_pBuilder = nullptr;



SSDCore::SSDCore()
{
}


SSDCore::~SSDCore()
{
}
BOOL SSDCore::CoreInit(CCoreBuilder* _Builder)
{
	if (!_Builder)
		return FALSE;

#ifdef _DEBUG
	new int;
#endif 

	m_pBuilder = _Builder;

	CSound::CreateSoundSystem();
	CFilePathMgr::Init();
	m_pBuilder->PreBuild();
	CTimerMgr::Init();
	CKeyMgr::Init(CGameWindow::hInst, CGameWindow::hWnd);
	CFbx::Init();
	CDevice::Init();
	Engine::CreateEngineGraphicResource();
	GlobalDebug::InitGlobalDebug();
	CSceneRenderMgr::Init();
	CProfiler::Init();
	CInstancingBuffer::Inst()->Init();

	return _Builder->Build();
}
BOOL SSDCore::CoreInit(CCoreBuilder* _Builder, const HINSTANCE& _hInst, const wchar_t* _TitleName, const HWND& _hWnd)
{
	if (!_Builder)
		return FALSE;

#ifdef _DEBUG
	new int;
#endif 

	m_pBuilder = _Builder;

	CSound::CreateSoundSystem();
	CFilePathMgr::Init();
	m_pBuilder->PreBuild();
	CTimerMgr::Init();
	CGameWindow::CreateGameWindow(_hInst, _TitleName, _hWnd);
	CKeyMgr::Init(_hInst, _hWnd, WINDOW_INPUT);
	CFbx::Init();
	CDevice::Init();
	Engine::CreateEngineGraphicResource();
	GlobalDebug::InitGlobalDebug();
	CSceneRenderMgr::Init();
	CProfiler::Init();
	CInstancingBuffer::Inst()->Init();

	return _Builder->Build();
}
void SSDCore::CoreLoop()
{
	MSG msg;

	while (m_bCoreProgress)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Window Death Time
			CSound::SoundSystemUpdate();
			if (true == CTimerMgr::Update()) 
			{
				GlobalDebug::Update();
				CKeyMgr::Update(CTimerMgr::DeltaTime());
				CSoundPlayer::GlobalChannelUpdate(CTimerMgr::DeltaTime());
				CGameWindow::Progress(CTimerMgr::DeltaTime());
				CKeyMgr::ResetWheelValue();
			}

		}
	}
}
void SSDCore::CoreRelease()
{
	if (nullptr != m_pBuilder)
		m_pBuilder->Release();

	CFilePathMgr::Release();
	CResourceMgr<CSound>::Release();
	CSound::ReleaseSoundSystem();
	GlobalDebug::ReleaseGlobalDebug();
	CKeyMgr::Release();
	CScene::SceneRelease();
	CResourceMgr<CScene>::Release();
	CResourceMgr<CFbx>::Release();
}
void SSDCore::ReduceLoop()
{
	CSound::SoundSystemUpdate();
	CTimerMgr::Update();
	GlobalDebug::Update();
	CKeyMgr::Update(CTimerMgr::DeltaTime());
	CSoundPlayer::GlobalChannelUpdate(CTimerMgr::DeltaTime());
	CGameWindow::Progress(CTimerMgr::DeltaTime());
	CKeyMgr::ResetWheelValue();
}
void SSDCore::CoreOperation(CCoreBuilder* _Builder)
{
	if (!CoreInit(_Builder))
		return;

	CoreLoop();
	CoreRelease();
}
void SSDCore::QuitCore(const bool _Release /*= false*/)
{
	m_bCoreProgress = false;

	if (true == _Release)
		CoreRelease();
}

LRESULT CALLBACK SSDCore::WndProc(HWND _hWnd, UINT _Msg, WPARAM _wParam, LPARAM _lParam)
{
	switch (_Msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		SSDCore::QuitCore();
		break;
	case WM_MOUSEWHEEL:
		CKeyMgr::MouseWheelEvent(GET_WHEEL_DELTA_WPARAM(_wParam));
	}

	if (m_pBuilder)
		m_pBuilder->BuilderProc(_hWnd, _Msg, _wParam, _lParam);

	return DefWindowProc(_hWnd, _Msg, _wParam, _lParam);
}
//

const bool SSDCore::CoreLive()
{
	return m_bCoreProgress;
}

BOOL SSDCore::CreateMainWindow(const HINSTANCE _hInst, const wchar_t* _Title)
{
	if (true == CGameWindow::bCreateWindow)
		return FALSE;

	CGameWindow::RegistWindowClass(_hInst);
	return CGameWindow::CreateGameWindow(_Title);
}

BOOL SSDCore::CreateMainWindow(const HINSTANCE _hInst, const wchar_t* _Title, const POINT& _Size, const POINT& _Pos, const bool& _Show/* = true*/)
{
	if (true == CGameWindow::bCreateWindow)
		return FALSE;

	CGameWindow::RegistWindowClass(_hInst);
	return CGameWindow::CreateGameWindow(_Title, _Size, _Pos, _Show);
}

BOOL SSDCore::CreateMainWindow(const HINSTANCE _hInst, const wchar_t* _Title, const Vec2& _Size, const Vec2& _Pos, const bool& _Show /*= true*/)
{
	if (true == CGameWindow::bCreateWindow)
		return FALSE;

	CGameWindow::RegistWindowClass(_hInst);
	return CGameWindow::CreateGameWindow(_Title, { (LONG)_Size.ix,(LONG)_Size.iy }, { (LONG)_Pos.ix,(LONG)_Pos.iy }, _Show);
}

BOOL SSDCore::CreateMainWindow(const HINSTANCE _hInst, const HWND& _hWnd, const wchar_t* _Title)
{
	if (true == CGameWindow::bCreateWindow)
		return FALSE;

	CGameWindow::RegistWindowClass(_hInst);
	return CGameWindow::CreateGameWindow(_hWnd, _Title);
}

BOOL SSDCore::SetWindowSize(const POINT& _Size)
{
	return CGameWindow::SetWindowSize(_Size);
}

BOOL SSDCore::SetWindowSize(const POINT& _Size, const POINT& _Pos)
{
	return CGameWindow::SetWindowSize(_Size, _Pos);
}

BOOL SSDCore::ChangeWindowTitle(std::wstring _Title)
{
	return CGameWindow::ChangeWndTitle(_Title);
}

BOOL SSDCore::ChangeMouseCursor(std::wstring _Path)
{
	return CGameWindow::ChangeBaseCursor(_Path);
}

BOOL SSDCore::ChangeIcon(std::wstring _Path)
{
	return CGameWindow::ChangeIcon(_Path);
}

BOOL SSDCore::ChangeSmallIcon(std::wstring _Path)
{
	return CGameWindow::ChangeSmallIcon(_Path);
}

const POINT SSDCore::WolrdPosToWndPos(const POINT& _WorldPos)
{
	return CGameWindow::WorldPointToWnd(_WorldPos);
}

const Vec2 SSDCore::GetWindowSize()
{
	return Vec2((float)CGameWindow::WndSize.x, (float)CGameWindow::WndSize.y);
}

const POINT SSDCore::GetWindowPSize()
{
	return CGameWindow::WndSize;
}

const LONG SSDCore::GetWindowWidth()
{
	return (LONG)CGameWindow::WndSize.x;
}

const LONG SSDCore::GetWindowHeight()
{
	return (LONG)CGameWindow::WndSize.y;
}

const float SSDCore::GetWindowWidthHalf()
{
	return CGameWindow::WndSize.x * 0.5f;
}

const float SSDCore::GetWindowHeightHalf()
{
	return CGameWindow::WndSize.y * 0.5f;
}

const LONG SSDCore::GetClientWidth()
{
	return (LONG)CGameWindow::ClientSize.x;
}

const LONG SSDCore::GetClientHeight()
{
	return (LONG)CGameWindow::ClientSize.y;
}

const float SSDCore::GetClientWidthHalf()
{
	return (float)CGameWindow::ClientSize.x * 0.5f;
}

const float SSDCore::GetClientHeightHalf()
{
	return (float)CGameWindow::ClientSize.y * 0.5f;
}

const float SSDCore::GetClientHypotenuse()
{
	float W = (float)CGameWindow::ClientSize.x;
	float H = (float)CGameWindow::ClientSize.y;
	return sqrtf(W * W + H * H);
}

const float SSDCore::GetClientAspect()
{
	return (float)CGameWindow::ClientSize.x / CGameWindow::ClientSize.y;
}

 void SSDCore::FocusingClient(const bool& _Focus)
{
	 m_bFocusing = _Focus;
}
 const bool SSDCore::IsClientFocus()
 {
	 return m_bFocusing;
 }
const float SSDCore::GetWindowHypotenuse()
{
	float W = (float)CGameWindow::WndSize.x;
	float H = (float)CGameWindow::WndSize.y;
	return sqrtf(W * W + H * H);
}

const float SSDCore::GetWindowAspect()
{
	return (float)CGameWindow::WndSize.x / CGameWindow::WndSize.y;
}

const Vec3 SSDCore::GetScreenMousePos()
{
	return Vec3((float)CGameWindow::GetMousePoint().x, (float)CGameWindow::GetMousePoint().y, 0.f);
}

const HINSTANCE SSDCore::GetWindowInstance()
{
	return CGameWindow::hInst;
}

const HWND SSDCore::GetWindowWnd()
{
	return CGameWindow::hWnd;
}

const bool SSDCore::CheckMouseOnScreen()
{
	return CGameWindow::CheckMouseOnScreen();
}
const bool SSDCore::CheckMouseOnClampScreen()
{
	return CGameWindow::CheckMouseOnClampScreen();
}
void SSDCore::ShowGameWindow(const bool& _Show)
{
	if (_Show)
		ShowWindow(CGameWindow::hWnd, SW_SHOW);
	else
		ShowWindow(CGameWindow::hWnd, SW_HIDE);

	UpdateWindow(CGameWindow::hWnd);
}

void SSDCore::SetProcessPrioriy(const int& _Priority)
{
	BOOL Check = SetPriorityClass(GetCurrentProcess(), _Priority);
	if (FALSE == Check) 
	{
		TASSERT(true);
	}
}
