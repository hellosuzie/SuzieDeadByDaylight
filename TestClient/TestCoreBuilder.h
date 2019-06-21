#pragma once
#include<SSDCore.h>



class CTestCoreBuilder : public CCoreBuilder
{

public:
	const BOOL Build() override;
	void BuilderProc(HWND _hWnd, UINT _Msg, WPARAM _wParam, LPARAM _lParam);
	const BOOL Release() override;


public:
	CTestCoreBuilder();
	~CTestCoreBuilder();
};

