#pragma once
#include<SSDCore.h>

#ifdef _DEBUG
#ifdef X64
#pragma comment(lib,"SJ_SplashWindowDx64")
#else 
#pragma comment(lib,"SJ_SplashWindowD")
#endif
#else
#ifdef X64
#pragma comment(lib,"SJ_SplashWindowx64")
#else 
#pragma comment(lib,"SJ_SplashWindow")
#endif
#endif

class DeadByDaylightBuilder : public CCoreBuilder
{
	bool								m_bOpenServer;
	
public:
	const BOOL PreBuild() override;
	const BOOL Build() override;
	void BuilderProc(HWND _hWnd, UINT _Msg, WPARAM _wParam, LPARAM _lParam);
	const BOOL Release() override;

public:
	void DeadClientBuild();
	void ClientBuild();
	void TestClientBuild();


public:
	DeadByDaylightBuilder();
	~DeadByDaylightBuilder();
};

