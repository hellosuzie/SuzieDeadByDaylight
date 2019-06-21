#pragma once
#include "Resource.h"
#include "DXHeader.h"
#include <vector>

class CSprite : public CResource
{
private:
	SPTR<class CTexture>						m_Texture;
	SPTR<class CSamplerState>					m_Sampler;
	std::vector<Vec4>							m_vecSpriteUv;
	Vec3										m_SpriteSize;

public:
	const size_t GetSpriteCount() { return m_vecSpriteUv.size(); }
	const size_t GetMaxSpriteIndex() { return m_vecSpriteUv.size() - 1; }
	const Vec4 GetSpriteUV(const size_t& _Index) { return m_vecSpriteUv[_Index]; }
	Vec4* GetSpriteUV_Pointer(const size_t& _Index) { return &m_vecSpriteUv[_Index]; }
	const Vec2 GetTextureSize();
	const Vec3 GetSpriteSize() { return m_SpriteSize; }


public:
	SPTR<CTexture> GetTexture();

public:
	void Update();
	void PostRender();

public:
	CSprite();
	~CSprite();


public:
	NONE_LOAD_RESOURCE(CSprite)
	CREATE_DESC_BODY(std::wstring TexKey;
			std::wstring SamplerKey;
			Vec2 StartPos;
			Vec2 Size;
			UINT XCount;
			UINT YCount;
			UINT StartFrame;
			UINT FrameCount;
	)

	const bool Create(const CREATE_DESC& _Desc);
};

