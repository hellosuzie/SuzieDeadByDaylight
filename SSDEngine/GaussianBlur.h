#pragma once
#include "FilterEffect.h"

class CGaussianBlur : public CFilterEffect
{
private:
	SPTR<class CMaterial>					m_GauMat;
	CBUFFER::GaussianBuffer					m_GaussianBuf;
	float									m_Sigma;

public:
	void Init() override;
	void Update(SPTR<MultiRenderTarget> _Target) override;


public:
	// Sigma = 가우시안 분포 마스크값의 분포 정도 (Sigma 가 작으면 자신의 픽셀을 더 많이 영향받는다.)
	// Range = 가우시안 블러를 먹일 픽셀 갯수 (Range x Range 영역 만큼) 
	void SetGaussianBuffer(const float& _Sigma, const int& _Range = 5);

	void SetSigma(float _Sigma)
	{
		SetGaussianBuffer(_Sigma, m_GaussianBuf.Range);
	}
	void SetRange(int _Range)
	{
		SetGaussianBuffer(m_Sigma, _Range);
	}
	float GetSigma()
	{
		return m_Sigma;
	}
	int GetRange()
	{
		return m_GaussianBuf.Range;
	}
	CBUFFER::GaussianBuffer GetGaussianBuffer()
	{
		return m_GaussianBuf;
	}

public:
	CGaussianBlur();
	~CGaussianBlur();

};

