#include "GaussianBlur.h"
#include "Mesh.h"
#include "Material.h"
#include "ResourceMgr.h"
#include "SSDCore.h"
#include "DXStruct.h"
#include "DXMath.h"

CGaussianBlur::CGaussianBlur() 
	:m_GauMat(nullptr), m_GaussianBuf(CBUFFER::GaussianBuffer()), m_Sigma(0.f)
{
}


CGaussianBlur::~CGaussianBlur()
{
}

void CGaussianBlur::Init()
{
	m_GauMat = CResourceMgr<CMaterial>::Find(L"Gaussian");
	
	m_GaussianBuf.WinSize = Vec2((float)Core::GetWindowWidth(), (float)Core::GetWindowHeight());
	SetGaussianBuffer(1.f);

	return CFilterEffect::Init();
}

void CGaussianBlur::Update(SPTR<MultiRenderTarget> _Target)
{
	CDevice::SetDS(L"PostDepth");
	SetOutputTarget();

	_Target->GetRenderTarget(0)->GetRTTexture()->Update(0);
	m_GauMat->Update();
	m_FilterMesh->Update();
	m_FilterMesh->Render();
	_Target->GetRenderTarget(0)->GetRTTexture()->PopShaderResource(0);
	m_GauMat->TexSamRollBack();
}


void CGaussianBlur::SetGaussianBuffer(const float& _Sigma, const int& _Range /*= 5*/)
{
	ZeroMemory(&m_GaussianBuf.GaussianMap, sizeof(m_GaussianBuf.GaussianMap));

	m_GaussianBuf.Range = DXMath::Clamp(_Range, 1, MAXGAUSSIANRANGE);
	m_Sigma = _Sigma;

	Gaussian MapFunc(_Sigma, 1.f);
	float Total = 0.f;
	float StartX = (float)(_Range / 2) * -1.f;
	float StartY = (float)(_Range / 2);
	m_GaussianBuf.TotalGaussianMap = 0.f;

	for (int X = 0; X < _Range; ++X)
	{
		for (int Y = 0; Y  < _Range; ++Y)
		{
			m_GaussianBuf.GaussianMap[X * MAXGAUSSIANRANGE + Y] = MapFunc(StartX + (float)X, StartY - (float)Y);
			m_GaussianBuf.TotalGaussianMap += m_GaussianBuf.GaussianMap[X * MAXGAUSSIANRANGE + Y];
		}
	}
	m_GauMat->SetConstantBufferData(L"GaussianBuffer", m_GaussianBuf);
}