#pragma once
#include "Player.h"


class CTestPlayer : public CPlayer
{
	SPTR<class CBase3DRenderer>							m_TestRen;


public:
	const bool Init()override;


public:
	CTestPlayer();
	~CTestPlayer();
};

