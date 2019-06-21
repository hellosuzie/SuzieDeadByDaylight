#pragma once
#include "DXHeader.h"
#include <string>
#include <crtdbg.h>


class CCoreBuilder
{
	friend class SSDCore;

protected:
	virtual const BOOL PreBuild() { return TRUE; };
	virtual const BOOL Build() = 0;
	virtual void BuilderProc(HWND _hWnd, UINT _Msg, WPARAM _wParam, LPARAM _lParam) = 0;
	virtual const BOOL Release() = 0;;


protected:
	CCoreBuilder();
	virtual ~CCoreBuilder();
};


typedef class SSDCore
{
private:
	static	bool				m_bCoreProgress;
	static	bool				m_bFocusing;
	static  CCoreBuilder*		m_pBuilder;


private:
	static BOOL CoreInit(CCoreBuilder* _Builder);
	static BOOL CoreInit(CCoreBuilder* _Builder, const HINSTANCE& _hInst
		, const wchar_t* _TitleName
		, const HWND& _hWnd
		);

	static void CoreLoop();
	static void CoreRelease();
	
public:


public:
	template<typename BuilderType>
	static BOOL CoreInit(const HINSTANCE& _hInst, const wchar_t* _TitleName ,const HWND& _hWnd)
	{
		static BuilderType Builder;
		return CoreInit(&Builder, _hInst, _TitleName, _hWnd);
	}

public:
	static void ReduceLoop();
	static void CoreOperation(CCoreBuilder* _Builder);
	// _Release 가 true이면 엔진의 메모리 정리도 한꺼번에 수행
	static void QuitCore(const bool _Release =false);
	static LRESULT CALLBACK WndProc(HWND _hWnd, UINT _message, WPARAM _wParam, LPARAM _lParam);

public:
	static const bool CoreLive();

	// Window 생성 함수
public:
	static BOOL CreateMainWindow(const HINSTANCE _hInst, const wchar_t* _Title);
	static BOOL CreateMainWindow(const HINSTANCE _hInst, const wchar_t* _Title, const POINT& _Size, const POINT& _Pos, const bool& _Show = true);
	static BOOL CreateMainWindow(const HINSTANCE _hInst, const wchar_t* _Title, const Vec2& _Size, const Vec2& _Pos, const bool& _Show = true);
	static BOOL CreateMainWindow(const HINSTANCE _hInst, const HWND& _hWnd, const wchar_t* _Title);



	// Window 제어 함수
public:
	static BOOL SetWindowSize(const POINT& _Size);
	static BOOL SetWindowSize(const POINT& _Size, const POINT& _Pos);
	static BOOL ChangeWindowTitle(std::wstring _Title);
	static BOOL ChangeMouseCursor(std::wstring _Path);
	static BOOL ChangeIcon(std::wstring _Path);
	static BOOL ChangeSmallIcon(std::wstring _Path);

public:
	static const POINT WolrdPosToWndPos(const POINT& _WorldPos);


public:
	static const Vec2 GetWindowSize();
	static const POINT GetWindowPSize();
	static const LONG GetWindowWidth();
	static const LONG GetWindowHeight();
	static const float GetWindowWidthHalf();
	static const float GetWindowHeightHalf();
	static const float GetWindowHypotenuse();	// 윈도우 대각선 길이
	static const float GetWindowAspect();
	static const Vec3 GetScreenMousePos();
	static const HINSTANCE GetWindowInstance();
	static const HWND GetWindowWnd();
	static const bool CheckMouseOnScreen();
	static const bool CheckMouseOnClampScreen();

	static const LONG GetClientWidth();
	static const LONG GetClientHeight();
	static const float GetClientWidthHalf();
	static const float GetClientHeightHalf();
	static const float GetClientHypotenuse();	// 클라이언트창 대각선 길이
	static const float GetClientAspect();
	static void FocusingClient(const bool& _Focus);
	static const bool IsClientFocus();
public:
	static void ShowGameWindow(const bool& _Show);

public:
	static void SetProcessPrioriy(const int& _Priority);



public:
	SSDCore();
	~SSDCore();

}CORE, Core;


#define  CORESTART( BuilderType , Title , Size , Pos) \
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, \
	_In_opt_ HINSTANCE hPrevInstance, \
	_In_ LPWSTR    lpCmdLine, \
	_In_ int       nCmdShow) \
{ \
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); \
	BuilderType Temp = BuilderType(); \
	if (!SSDCore::CreateMainWindow( hInstance,Title , Size, Pos) )\
		return -1;\
	SSDCore::CoreOperation(&Temp); \
	return 0; \
}

#define  CORESTART_HIDE( BuilderType , Title , Size , Pos) \
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, \
	_In_opt_ HINSTANCE hPrevInstance, \
	_In_ LPWSTR    lpCmdLine, \
	_In_ int       nCmdShow) \
{ \
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); \
	BuilderType Temp = BuilderType(); \
	if (!SSDCore::CreateMainWindow( hInstance, Title , Size, Pos , false) )\
		return -1;\
	SSDCore::CoreOperation(&Temp); \
	return 0; \
}

#define  SIMPLECORESTART( BuilderType , Title) \
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, \
	_In_opt_ HINSTANCE hPrevInstance, \
	_In_ LPWSTR    lpCmdLine, \
	_In_ int       nCmdShow) \
{ \
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); \
	BuilderType Temp = BuilderType(); \
	if (!SSDCore::CreateMainWindow(hInstance,Title))\
		return -1;\
	SSDCore::CoreOperation(&Temp); \
	return 0; \
}

