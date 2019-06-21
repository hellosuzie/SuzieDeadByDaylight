#pragma once
#include "Renderer.h"
class CSkybox :public CRenderer
{

public:
	const bool Init(const RENDATA& _InitData) override;

public:
	void SetSkyBoxSize(const float& _Size);
	void SetTexture(const wchar_t* _TexKey);

public:
	void RenderUpdate(SPTR<class CCamera> _Cam) override;

public:
	CSkybox();
	~CSkybox();
};

