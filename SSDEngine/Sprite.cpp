#include "Sprite.h"
#include "ResourceMgr.h"
#include "Texture.h"
#include "SamplerState.h"




CSprite::CSprite()
	:m_SpriteSize(Vec3(0.f,0.f,1.f))
{
}


CSprite::~CSprite()
{
}
const Vec2 CSprite::GetTextureSize()
{
	return m_Texture->GetTextureSize();
}
void CSprite::Update()
{
	m_Texture->Update(0);
	m_Sampler->Update(0);
}
void CSprite::PostRender()
{
	m_Texture->PopShaderResource(0);
	m_Sampler->PopSampler(0);
}
const bool CSprite::Create(const CREATE_DESC& _Desc)
{
	if (true == _Desc.TexKey.empty()||  nullptr == (m_Texture = CResourceMgr<CTexture>::Find(_Desc.TexKey.c_str())))
	{
		TASSERT(true);
		return false;
	}

	if (true == _Desc.SamplerKey.empty() || nullptr == (m_Sampler = CResourceMgr<CSamplerState>::Find(_Desc.SamplerKey.c_str())))
		m_Sampler = CResourceMgr<CSamplerState>::Find(POINTSAMPLER);
	
	Vec2 TexSize = m_Texture->GetTextureSize();
	
	Vec2 StartUV;
	StartUV.x = min(1.f, _Desc.StartPos.x / TexSize.x);
	StartUV.y = min(1.f, _Desc.StartPos.y / TexSize.y);

	Vec4 UvInterval;

	// 영역을 지정하지 않은 경우 전체 크기를 기준으로 짜른다.
	if (_Desc.Size == Vec2(0.f, 0.f))
	{
		UvInterval.Size.x = 1.f / (float)_Desc.XCount;
		UvInterval.Size.y = 1.f / (float)_Desc.YCount;
		m_SpriteSize.x = TexSize.x / (float)_Desc.XCount;
		m_SpriteSize.y = TexSize.y / (float)_Desc.YCount;
	}
	else 
	{
		float RatioX = min(1.f, _Desc.Size.x / TexSize.x);
		float RatioY = min(1.f, _Desc.Size.y / TexSize.y);
		UvInterval.Size.x = 1.f / (float)_Desc.XCount * RatioX;
		UvInterval.Size.y = 1.f / (float)_Desc.YCount * RatioY;
		m_SpriteSize.x = _Desc.Size.x / (float)_Desc.XCount;
		m_SpriteSize.y = _Desc.Size.y / (float)_Desc.YCount;
	}

	size_t Start = 0;

	for (size_t y = 0; y < _Desc.YCount; ++y)
	{
		for (size_t x = 0; x < _Desc.XCount; ++x)
		{
			if (m_vecSpriteUv.size() >= _Desc.FrameCount)
				break;

			if (Start > x + _Desc.XCount * y)
				continue;
	
			UvInterval.Pos.x = StartUV.x + x * UvInterval.Size.x;
			UvInterval.Pos.y = StartUV.y + y * UvInterval.Size.y;
			m_vecSpriteUv.push_back(UvInterval);
		}
	}

	return true;
}
SPTR<CTexture> CSprite::GetTexture()
{
	return m_Texture;
}