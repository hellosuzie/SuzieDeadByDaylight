#pragma once
#include "FilterEffect.h"

class CBloomFilter :
	public CFilterEffect
{
private:
	SPTR<class CMaterial>									m_BloomMat;
	SPTR<class CMaterial>									m_MiniMat;

private:
	MINIMIZE_BUFFER_TYPE									m_BloomMiniBufferType;
	CBUFFER::MinimizeBuffer									m_BloomMiniBuffer;

	CBUFFER::BloomBuffer									m_BloomBuffer;
	float													m_Sigma;
	float													m_BloomBufferSize;

	SPTR<class MultiRenderTarget>							m_MiniTarget;
	SPTR<class MultiRenderTarget>							m_BloomResTarget;
	UINT													m_BloomResNum;

public:
	void Init(const MINIMIZE_BUFFER_TYPE& _Type);
	void Update(SPTR<MultiRenderTarget> _Target) override;

	void SetMiniBufferUpdate(MINIMIZE_BUFFER_TYPE _BloomMiniBufferType);

public:
	void SetMiniBufferType(const MINIMIZE_BUFFER_TYPE& _Type)
	{
		m_BloomMiniBufferType = _Type;
	}
	void SetMiniBufferAmount(float _Amount)
	{
		m_BloomMiniBuffer.Amount = _Amount;
	}
	void SetMiniBufferLuminance(Vec3 _Luminance)
	{
		m_BloomMiniBuffer.Luminance = _Luminance;
	}
	void SetMiniBufferMultiple(float _Multiple)
	{
		m_BloomMiniBuffer.Multiple = _Multiple;
	}

	float GetMiniBufferAmount()
	{
		return m_BloomMiniBuffer.Amount;
	}
	Vec3 GetMiniBufferLuminance()
	{
		return m_BloomMiniBuffer.Luminance;
	}
	float GetMiniBufferMultiple()
	{
		return m_BloomMiniBuffer.Multiple;
	}
	MINIMIZE_BUFFER_TYPE GetBloomMiniBufferType()
	{
		return m_BloomMiniBufferType;
	}
	const CBUFFER::MinimizeBuffer& GetMiniBuffer()
	{
		return m_BloomMiniBuffer;
	}
	const CBUFFER::BloomBuffer& GetBloomBuffer()
	{
		return m_BloomBuffer;
	}

	float GetBloomBufferSize()
	{
		return m_BloomBufferSize;
	}
	UINT GetBloomResNum()
	{
		return m_BloomResNum;
	}

	void SetBloomResTarget(const wchar_t* _MultiTargetName, const UINT _SlotNum = 0);
	void SetBloomResTarget(SPTR<class CCamera> _TargetCamera);

	// Sigma = 가우시안 분포 마스크값의 분포 정도 (Sigma 가 작으면 자신의 픽셀을 더 많이 영향받는다.)
	// Range = 가우시안 블러를 먹일 픽셀 갯수 (Range x Range 영역 만큼) 
	void SetGaussianBuffer(const float& _Sigma, const int& _Range = 5);

	void SetSigma(float _Sigma)
	{
		SetGaussianBuffer(_Sigma, m_BloomBuffer.Range);
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
		return m_BloomBuffer.Range;
	}

public:
	CBloomFilter();
	~CBloomFilter();
};

