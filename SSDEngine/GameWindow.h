#pragma once
#include <string>
#include "DXHeader.h"


#define DXWNDCLASSNAME L"DXWindow"

class CGameWindow
{
	friend class SSDCore;
	friend class CDevice;

private:
	static bool							bCreateWindow;
	static bool							bFullScreen;
	static POINT						MonitorSize;

public:
	static const Vec2 GetMoniterSize() { return Vec2(MonitorSize.x, MonitorSize.y); }

private:
	static HINSTANCE					hInst;
	static HWND							hWnd;
	static HDC							hDC;
	static POINT						WndPos;
	static POINT						WndSize;
	static POINT						ClientSize;
	static RECT							WndRect;
	static RECT							WndClampRect;
	static RECT							ClientRect;
	static HCURSOR						hCursor;
	static std::wstring					WindowTitle;

public:
	static ATOM RegistWindowClass(HINSTANCE hInstance);
	static const BOOL CreateGameWindow(const wchar_t* _Title);
	static const BOOL CreateGameWindow(const wchar_t* _Title, const POINT& _Size, const POINT& _Pos ,const bool& _Show );
	static const BOOL CreateGameWindow(const HWND& _hWnd,const wchar_t* _Title);
	static const BOOL CreateGameWindow(const HINSTANCE& _hInst, const wchar_t* _TitleName,const HWND& _hWnd);


private:
	static const POINT MonitorPosToWndPos(const POINT& _WorldPoint);
	static const POINT WndPosToWorldPoint(const POINT& _WndPos);
	static const POINT WorldPointToWnd(const POINT& _WndPos);
	static BOOL SetWindowSize(const POINT& _Size, const POINT& _Pos = POINT{ MAXLONG, MAXLONG });
	static BOOL ChangeBaseCursor(const std::wstring& _Path);
	static BOOL ChangeIcon(const std::wstring& _Path);
	static BOOL ChangeSmallIcon(const std::wstring& _Path);
	static BOOL ChangeWndTitle(const std::wstring& _Title);

public:
	static const POINT GetMousePos();

private:
	// 마우스가 클라이언트 안에 있는지 확인 하는 함수
	static const bool CheckMouseOnScreen();
	static const bool CheckMouseOnClampScreen();

public:
	static void SyncroWindowScreen();
	static const POINT SetMousePoint(const Vec2& _WorldPos);
	static const POINT SetMousePoint(POINT& _ScreenPos);
	static const POINT GetMousePoint();
	// 윈도우가 전체 화면 모드 인지 확인
	static const bool IsFullScreen();
	// 윈도우가 윈도우 화면 모드인지 확인
	static const bool IsWindowMode();

public:
	static const RECT GetWindowClampRect();
	static const RECT GetWindowRc();
	static const RECT GetClientRc();

public:
	static void Progress(const float& _DeltaTime);

public:
	static HWND	GethWnd()
	{
		return hWnd;
	}

private:
	CGameWindow();
	CGameWindow(HWND _hWnd);
	virtual ~CGameWindow() = 0;
};

