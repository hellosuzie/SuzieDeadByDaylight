#pragma once
#include"Base2DCollider.h"


class CPointCollider : public CBase2DCollider
{

public:
	void DebugRender()override;

public:
	CPointCollider();
	~CPointCollider();
};

