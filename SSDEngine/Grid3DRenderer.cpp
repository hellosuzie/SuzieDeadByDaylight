#include "Grid3DRenderer.h"
#include "Material.h"
#include "Camera.h"
#include "TransForm.h"
#include "DXStruct.h"

Grid3DRenderer::Grid3DRenderer()
{
}


Grid3DRenderer::~Grid3DRenderer()
{
}

const bool Grid3DRenderer::Init(const RENDATA & _InitData)
{
	PushMesh(L"ColorMesh");
	PushMaterial(L"GridMaterial");
	return CRenderer::Init(_InitData);
}

void Grid3DRenderer::RenderUpdate(SPTR<CCamera> _Cam)
{
	CRenderer::RenderUpdate(_Cam);

	CBUFFER::GridBuffer TempData;
	TempData.MinRange = 50.0f;
	TempData.MaxRange = 500.0f;
	TempData.Step = 0;
	TempData.Y = abs(_Cam->GetTrans()->GetWorldPos().y);
	int Y = (int)TempData.Y;

	TempData.Border = 0.3f;
	TempData.CalMaxRange = TempData.MaxRange;

	while (0 != Y)
	{
		Y /= 10;
		++TempData.Step;
	}
	TempData.Step -= 1;

	float StapMul = 1.0f;
	for (int i = 1; i < TempData.Step; ++i)
	{
		StapMul *= 10.f;
		TempData.CalMaxRange *= 10.f;
	}

	TempData.MinStep = 10.0f * StapMul;
	TempData.MaxStep = 100.0f * StapMul;

	if (TempData.MinStep < 10.0f)
	{
		TempData.MinStep = 10.0f;
	}

	if (TempData.MaxStep < 100.0f)
	{
		TempData.MaxStep = 100.0f;
	}

	GetMaterial(0)->SetConstantBufferData(L"GridBuffer", TempData);
}
