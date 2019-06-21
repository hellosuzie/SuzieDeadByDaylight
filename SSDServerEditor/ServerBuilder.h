#pragma once
#include <SSDCore.h>

class CServerBuilder : public CCoreBuilder
{

public:
	const BOOL Build() override;
	void BuilderProc(HWND _hWnd, UINT _Msg, WPARAM _wParam, LPARAM _lParam);
	const BOOL Release() override;




public:
	CServerBuilder();
	~CServerBuilder();
};

