#pragma once
#include "Renderer.h"

typedef class Grid3DRenderer :public CRenderer
{
public:
	Grid3DRenderer();
	~Grid3DRenderer();

public:
	const bool Init(const RENDATA& _InitData) override;
	void RenderUpdate(SPTR<CCamera> _Cam) override;
	 
}CGridRenderer;

