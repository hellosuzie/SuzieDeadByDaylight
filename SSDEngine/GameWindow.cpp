#include "GameWindow.h"
#include "SSDCore.h"
#include "Device.h"
#include "Scene.h"
#include "TimerMgr.h"
#include <atlstr.h>


bool						CGameWindow::bCreateWindow = false;
bool						CGameWindow::bFullScreen = false;
POINT						CGameWindow::MonitorSize = POINT{ GetSystemMetrics(SM_CXSCREEN) ,GetSystemMetrics(SM_CYSCREEN) };



HINSTANCE					CGameWindow::hInst = NULL;
HWND						CGameWindow::hWnd = NULL;
HDC							CGameWindow::hDC = NULL;
POINT						CGameWindow::WndPos;
POINT						CGameWindow::WndSize;
POINT						CGameWindow::ClientSize;
RECT						CGameWindow::WndRect;
RECT						CGameWindow::WndClampRect;
RECT						CGameWindow::ClientRect;
HCURSOR						CGameWindow::hCursor;
std::wstring				CGameWindow::WindowTitle = L"";

CGameWindow::CGameWindow()
{

}
CGameWindow::CGameWindow(HWND _hWnd)
{

}

ATOM CGameWindow::RegistWindowClass(HINSTANCE hInstance)
{
	hInst = hInstance;

	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = SSDCore::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = nullptr;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = DXWNDCLASSNAME;
	wcex.hIconSm = nullptr;

	return RegisterClassExW(&wcex);
}

const BOOL CGameWindow::CreateGameWindow(const wchar_t* _Title)
{
	hWnd = CreateWindowW(DXWNDCLASSNAME, _Title, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInst, nullptr);

	if (!hWnd)
		return FALSE;

	hDC = GetDC(hWnd);
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	bCreateWindow = true;

	return TRUE;
}
const BOOL CGameWindow::CreateGameWindow(const wchar_t* _Title, const POINT& _Size, const POINT& _Pos, const bool& _Show)
{
	hWnd = CreateWindowW(DXWNDCLASSNAME, _Title, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInst, nullptr);

	if (!hWnd)
		return FALSE;

	hDC = GetDC(hWnd);
	SetWindowSize(_Size, _Pos);
	//CDevice::Init();
	
	if (_Show) 
	{
		ShowWindow(hWnd, SW_SHOW);
		UpdateWindow(hWnd);
	}

	bCreateWindow = true;
	return TRUE;
}
const BOOL CGameWindow::CreateGameWindow(const HWND& _hWnd, const wchar_t* _Title)
{
	hWnd = _hWnd;
	SetWindowTextW(_hWnd, _Title);

	hDC = GetDC(hWnd);
	//CDevice::Init();

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	bCreateWindow = true;
	return TRUE;
}

const BOOL CGameWindow::CreateGameWindow(const HINSTANCE& _hInst, const wchar_t* _TitleName, const HWND& _hWnd)
{
	RegistWindowClass(_hInst);
	hWnd = _hWnd;
	SetWindowText(hWnd, _TitleName);

	GetClientRect(hWnd, &WndRect);
	GetClientRect(hWnd, &ClientRect);

	WndSize.x = WndRect.right - WndRect.left;
	WndSize.y = WndRect.bottom - WndRect.top;

	ClientSize.x = ClientRect.right - ClientRect.left;
	ClientSize.y = ClientRect.bottom - ClientRect.top;
	
	hDC = GetDC(hWnd);
	return TRUE;
}
const POINT CGameWindow::MonitorPosToWndPos(const POINT& _WorldPoint)
{
	POINT ChangePos;

	ChangePos.x = (_WorldPoint.x + MonitorSize.x / 2);
	ChangePos.y = -(_WorldPoint.y - MonitorSize.y / 2);

	return ChangePos;
}
const POINT CGameWindow::WndPosToWorldPoint(const POINT& _WndPos)
{
	POINT ChangePos;

	ChangePos.x = (_WndPos.x - WndSize.x/ 2);
	ChangePos.y = -(_WndPos.y - WndSize.y / 2);

	return ChangePos;
}
const POINT CGameWindow::WorldPointToWnd(const POINT& _WorldPoint)
{
	POINT ChangePos;

	ChangePos.x = (_WorldPoint.x + WndSize.x / 2);
	ChangePos.y = -(_WorldPoint.y - WndSize.y / 2);

	return ChangePos;
}
const POINT CGameWindow::GetMousePos()
{
	return WndPosToWorldPoint(POINT{ (LONG)Core::GetScreenMousePos().x,(LONG)Core::GetScreenMousePos().y });
}
BOOL CGameWindow::SetWindowSize(const POINT& _Size, const POINT& _Pos /*= POINT{MAXLONG,MAXLONG}*/)
{
	RECT Rc = { 0,0, _Size.x, _Size.y };

	if (_Pos.x == MAXLONG || _Pos.y == MAXLONG)
	{
		WndPos.x = MonitorSize.x / 2;
		WndPos.y = MonitorSize.y / 2;
	}
	else
		WndPos = MonitorPosToWndPos(_Pos);

	AdjustWindowRect(&Rc, WS_OVERLAPPEDWINDOW, false);

	//윈도우 틀을 포함한 윈도우 크기가 홀수인경우 Client 영역의 크기를 보정

	int biasX = 0;
	int biasY = 0;

	if (1 == (Rc.right - Rc.left) % 2)
	{
		biasX = 1;
	}
	if (1 == (Rc.bottom - Rc.top) % 2)
	{
		biasY = 1;
	}

	int SizeHX = (Rc.right - Rc.left) / 2;
	int SizeHY = (Rc.bottom - Rc.top) / 2;


	BOOL Check = SetWindowPos(hWnd, nullptr
		, WndPos.x - SizeHX - biasX
		, WndPos.y - SizeHY - biasY
		, Rc.right - Rc.left
		, Rc.bottom - Rc.top
		, SWP_NOZORDER
	);

	WndClampRect.left = WndPos.x - SizeHX - biasX;
	WndClampRect.top = WndPos.y - SizeHY - biasY;
	WndClampRect.right = WndClampRect.left + Rc.right - Rc.left;
	WndClampRect.bottom = WndClampRect.top + Rc.bottom - Rc.top;


	WndSize = { Rc.right - Rc.left, Rc.bottom - Rc.top };
	ClientSize = _Size;
	GetWindowRect(hWnd, &WndRect);
	GetClientRect(hWnd, &ClientRect);

	WndClampRect = ClientRect;
	WndClampRect.left +=3;
	WndClampRect.top += 3;
	WndClampRect.right -= 3;
	WndClampRect.bottom -= 3;
	//WndRect = Rc;


	return Check;
}

BOOL CGameWindow::ChangeBaseCursor(const std::wstring& _Path)
{
	HCURSOR Cursor = nullptr;

	Cursor = LoadCursorFromFileW(_Path.c_str());

	if (nullptr == Cursor)
		return FALSE;

	hCursor = Cursor;
	return TRUE;
}
BOOL CGameWindow::ChangeIcon(const std::wstring& _Path)
{
	HICON Icon = (HICON)LoadImageW(
		hInst
		, _Path.c_str()
		, IMAGE_ICON
		, 0, 0
		, LR_LOADFROMFILE
	);

	if (nullptr == Icon)
		return FALSE;

#ifdef X64
	SetClassLongPtr(hWnd, GCLP_HICON, (LONG_PTR)Icon);
#else
	//SetClassLong(hWnd, GCL_HICON, (LONG)Icon);
	SetClassLong(hWnd, GCLP_HICON, (LONG)Icon);
#endif
	SetWindowTextW(hWnd, WindowTitle.c_str());
	return TRUE;
}
BOOL CGameWindow::ChangeSmallIcon(const std::wstring& _Path)
{
	HICON Icon = (HICON)LoadImageW(
		hInst
		, _Path.c_str()
		, IMAGE_ICON
		, 0, 0
		, LR_LOADFROMFILE
	);

	if (nullptr == Icon)
		return FALSE;

#ifdef X64
	SetClassLongPtr(hWnd, GCLP_HICONSM, (LONG_PTR)Icon);
#else
	//SetClassLong(hWnd, GCL_HICONSM, (LONG)Icon);
	SetClassLong(hWnd, GCLP_HICONSM, (LONG)Icon);
#endif

	SetWindowTextW(hWnd, WindowTitle.c_str());
	return TRUE;
}
BOOL CGameWindow::ChangeWndTitle(const std::wstring& _Title)
{
	if (FALSE == SetWindowTextW(hWnd, _Title.c_str()))
		return FALSE;

	WindowTitle = _Title;
	return TRUE;
} 
const bool  CGameWindow::CheckMouseOnScreen()
{
	POINT MousePos;
	GetCursorPos(&MousePos);
	ScreenToClient(hWnd, &MousePos);

	//if (MousePos.x < WndRect.left || MousePos.x > WndRect.right)
	//	return false;

	//if (MousePos.y < WndRect.top || MousePos.y > WndRect.bottom)
	//	return false;

	if (MousePos.x < ClientRect.left || MousePos.x > ClientRect.right)
		return false;

	if (MousePos.y < ClientRect.top || MousePos.y > ClientRect.bottom)
		return false;

	return true;
}
const bool CGameWindow::CheckMouseOnClampScreen()
{
	POINT MousePos;
	GetCursorPos(&MousePos);
	ScreenToClient(hWnd, &MousePos);

	if (MousePos.x < WndClampRect.left || MousePos.x > WndClampRect.right)
		return false;

	if (MousePos.y < WndClampRect.top || MousePos.y > WndClampRect.bottom)
		return false;

	return true;
}
void CGameWindow::SyncroWindowScreen()
{
	RECT Check;
	GetClientRect(hWnd, &Check);

	if (Check.left - Check.right == 0.f &&Check.bottom - Check.top== 0.f) 
	{
		return;
	}

	GetWindowRect(hWnd, &WndRect);

	WndSize.x = WndRect.right - WndRect.left;
	WndSize.y = WndRect.bottom - WndRect.top;

	GetClientRect(hWnd, &ClientRect);

	ClientSize.x = ClientRect.right - ClientRect.left;
	ClientSize.y = ClientRect.bottom - ClientRect.top;


	WndClampRect = ClientRect;
	WndClampRect.left += 3;
	WndClampRect.top += 3;
	WndClampRect.right -= 3;
	WndClampRect.bottom -= 3;
}
const POINT CGameWindow::SetMousePoint(const Vec2& _WndPos) 
{
	POINT Pos = MonitorPosToWndPos(POINT{ (LONG)_WndPos.x, (LONG)_WndPos.y });
	SetMousePoint(Pos);
	return Pos;
}
const POINT CGameWindow::SetMousePoint(POINT& _ScreenPos)
{
	POINT Pos = _ScreenPos;
	ClientToScreen(hWnd,& Pos);
	SetCursorPos(Pos.x, Pos.y);

	return Pos;
}
const POINT CGameWindow::GetMousePoint()
{
	POINT MousePos;
	GetCursorPos(&MousePos);
	ScreenToClient(hWnd, &MousePos);
	return MousePos;
}

const bool CGameWindow::IsFullScreen() 
{
	return bFullScreen;
}
const bool CGameWindow::IsWindowMode() 
{
	return !bFullScreen;
}
const RECT CGameWindow::GetWindowClampRect()
{
	return WndClampRect;
}
const RECT CGameWindow::GetWindowRc() 
{
	return ClientRect;
}
const RECT CGameWindow::GetClientRc()
{
	return WndRect;
}
void CGameWindow::Progress(const float& _DeltaTime)
{
	CScene::SceneProgress(_DeltaTime);
}