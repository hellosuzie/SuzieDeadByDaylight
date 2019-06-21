#pragma once
#include"Base2DCollider.h"


class CRectCollider : public CBase2DCollider
{

public:
	void DebugRender() override;




public:
	CRectCollider();
	~CRectCollider();
};

