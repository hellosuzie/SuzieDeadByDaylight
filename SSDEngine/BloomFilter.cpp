#include "BloomFilter.h"
#include "Material.h"
#include "ResourceMgr.h"
#include "Mesh.h"
#include "DXStruct.h"
#include "MultiRenderTarget.h"
#include "Camera.h"
#include "DXMath.h"
#include "SSDCore.h"

CBloomFilter::CBloomFilter()
	: m_BloomMat(nullptr), m_MiniMat(nullptr), m_BloomBufferSize(0.f), m_MiniTarget(nullptr), m_BloomResTarget(nullptr), m_BloomResNum(0)
	, m_BloomBuffer(MINI_64), m_Sigma(0.f)
{

}


CBloomFilter::~CBloomFilter()
{
}


void CBloomFilter::Init(const MINIMIZE_BUFFER_TYPE& _Type)
{
	//축소버퍼 만들기
	//m_MiniMat = CResourceMgr<CMaterial>::Find(L"Minimize64");
	//CBUFFER::MinimizeBuffer Temp;
	//Temp.Amount = 0.7f;
	//Temp.Luminance = Vec3(0.33333333334f, 0.33333333334f, 0.33333333334f);
	//Temp.Multiple = 1.f;

	//m_MiniMat->SetConstantBufferData(L"MinimizeBuffer", Temp);

	m_BloomMiniBufferType = _Type;
	m_BloomMat = CResourceMgr<CMaterial>::Find(L"Bloom");

	m_BloomMiniBuffer.MiniType = MINIMIZE_LUMINANCE;
	m_BloomMiniBuffer.Amount = 0.5f;
	m_BloomMiniBuffer.Luminance = Vec3(0.33333333334f, 0.33333333334f, 0.33333333334f);
	m_BloomMiniBuffer.Multiple = 1.f;

	switch (m_BloomMiniBufferType)
	{
	case MINI_64:
		m_BloomBufferSize = 64.f;
		m_MiniTarget = CResourceMgr<MRT>::Find(L"Minimize64");
		break;
	case MINI_128:
		m_BloomBufferSize = 128.f;
		m_MiniTarget = CResourceMgr<MRT>::Find(L"Minimize128");
		break;
	case MINI_256:
		m_BloomBufferSize = 256.f;
		m_MiniTarget = CResourceMgr<MRT>::Find(L"Minimize256");
		break;
	case MINI_512:
		m_BloomBufferSize = 512.f;
		m_MiniTarget = CResourceMgr<MRT>::Find(L"Minimize512");
		break;
	default:
		break;
	}

	m_BloomBuffer.WinSize = Vec2((float)Core::GetWindowWidth(), (float)Core::GetWindowHeight());
	SetGaussianBuffer(20.f);

	return CFilterEffect::Init();
}

void CBloomFilter::Update(SPTR<MultiRenderTarget> _Target)
{
	//CDevice::SetDS(L"PostDepth");
	//MiniTarget->Clear();
	//MiniTarget->SetMultiRenderTarget(true);

	//_Target->GetRenderTarget(0)->GetRTTexture()->Update(0);
	//m_MiniMat->Update();
	//m_FilterMesh->Update();
	//m_FilterMesh->Render();
	//_Target->GetRenderTarget(0)->GetRTTexture()->PopShaderResource(0);
	//m_MiniMat->TexSamRollBack();

	MiniBufferUpdate(m_BloomMiniBufferType, m_BloomResTarget, m_BloomMiniBuffer, m_BloomResNum);

	CDevice::SetBackBufferView();
	CDevice::ViewPortUpdate();

	CDevice::SetDS(L"PostDepth");
	SetOutputTarget();

	SetMiniBufferUpdate(m_BloomMiniBufferType);
	m_BloomBuffer.TargetSize = m_BloomBufferSize;
	m_BloomMat->SetConstantBufferData(L"BloomBuffer", m_BloomBuffer);

	m_MiniTarget->GetRenderTarget(0)->GetRTTexture()->Update(0);
	_Target->GetRenderTarget(0)->GetRTTexture()->Update(1);
	m_BloomMat->Update();
	m_FilterMesh->Update();
	m_FilterMesh->Render();
	m_MiniTarget->GetRenderTarget(0)->GetRTTexture()->PopShaderResource(0);
	_Target->GetRenderTarget(0)->GetRTTexture()->PopShaderResource(1);
	m_BloomMat->TexSamRollBack();
}

void CBloomFilter::SetMiniBufferUpdate(MINIMIZE_BUFFER_TYPE _BloomMiniBufferType)
{
	m_BloomMiniBufferType = _BloomMiniBufferType;
	switch (m_BloomMiniBufferType)
	{
	case MINI_64:
		m_BloomBufferSize = 64.f;
		m_MiniTarget = CResourceMgr<MRT>::Find(L"Minimize64");
		break;
	case MINI_128:
		m_BloomBufferSize = 128.f;
		m_MiniTarget = CResourceMgr<MRT>::Find(L"Minimize128");
		break;
	case MINI_256:
		m_BloomBufferSize = 256.f;
		m_MiniTarget = CResourceMgr<MRT>::Find(L"Minimize256");
		break;
	case MINI_512:
		m_BloomBufferSize = 512.f;
		m_MiniTarget = CResourceMgr<MRT>::Find(L"Minimize512");
		break;
	default:
		break;
	}
}

void CBloomFilter::SetBloomResTarget(const wchar_t * _MultiTargetName, const UINT _SlotNum)
{
	m_BloomResTarget = CResourceMgr<MRT>::Find(_MultiTargetName);
	m_BloomResNum = _SlotNum;
}

void CBloomFilter::SetBloomResTarget(SPTR<class CCamera> _TargetCamera)
{
	m_BloomResTarget = _TargetCamera->GetCamMultiTarget();
	m_BloomResNum = 0;
}

void CBloomFilter::SetGaussianBuffer(const float & _Sigma, const int & _Range)
{
	ZeroMemory(&m_BloomBuffer.GaussianMap, sizeof(m_BloomBuffer.GaussianMap));
	m_BloomBuffer.Range = DXMath::Clamp(_Range, 1, MAXGAUSSIANRANGE);
	m_Sigma = _Sigma;

	Gaussian MapFunc(_Sigma, 1.f);
	float Total = 0.f;
	float StartX = (float)(_Range / 2) * -1.f;
	float StartY = (float)(_Range / 2);
	m_BloomBuffer.TotalGaussianMap = 0.f;

	for (int X = 0; X < _Range; ++X)
	{
		for (int Y = 0; Y < _Range; ++Y)
		{
			m_BloomBuffer.GaussianMap[X * MAXGAUSSIANRANGE + Y] = MapFunc(StartX + (float)X, StartY - (float)Y);
			m_BloomBuffer.TotalGaussianMap += m_BloomBuffer.GaussianMap[X * MAXGAUSSIANRANGE + Y];
		}
	}
}
