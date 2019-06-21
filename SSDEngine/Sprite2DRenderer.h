#pragma once
#include "Renderer.h"


typedef struct _tagSPRITE2DDATA 
{
	RENDER_LAYER					RenderLayer;
	SPRITE_RENSIZE_TYPE				SizeType;
	Vec3							SizeFactor;
	//float							SpriteSizeMag;

	_tagSPRITE2DDATA();
	_tagSPRITE2DDATA(const RENDER_LAYER& _Layer,const SPRITE_RENSIZE_TYPE& _SizeType,const Vec3& _CustomSize);
	_tagSPRITE2DDATA(const RENDER_LAYER& _Layer, const SPRITE_RENSIZE_TYPE& _SizeType, const float& _SpriteMag);
	~_tagSPRITE2DDATA();

}SPRITE2D_RENDATA;

typedef class CSprite2DRenderer : public CRenderer
{
private:
	size_t								m_SpriteIndex;
	SPTR<class CSprite>					m_Sprite;
	SPTR<class CMesh>					m_Mesh;
	SPTR<class CVtxShader>				m_VS;
	SPTR<class CPixShader>				m_PS;

private:
	SPRITE_RENSIZE_TYPE					m_eSizeType;
	Vec3								m_SpriteSizeMag;

public:
	const bool Init(const RENDATA& _InitData) override;
	const bool Init(const SPRITE2D_RENDATA& _InitData) ;
	void Render(SPTR<CCamera> _Cam) override;
	void RenderUpdate(SPTR<CCamera> _Cam) override;

public:
	const bool SetSprite(const wchar_t* _SpriteKey);
	size_t SetSpriteIndex(const size_t& _Index);

	const Vec4 GetSpriteUV();
	const size_t GetSpriteIndex();

public:
	void SetSpriteSizeMag(const float& _Mag);
	void SetSpriteSizeMag(const Vec2& _Mag);
	void SetSpriteSizeXMag(const float& _Mag);
	void SetSpriteSizeYMag(const float& _Mag);

public:
	// 화면의 랜더링시의 월드 포스를 반환
	const Vec3 GetRenWorldPos();
	const Vec3 GetRenSize();
	const Vec3 GetRenRotate();
	const Vec3 GetRenRevolve();
	const Vec3 GetCurSpriteSize();
	SPTR<class CTexture> GetSpriteTexture();



public:
	CSprite2DRenderer();
	~CSprite2DRenderer();




//public:
//	Vec4								m_SpriteClipData;
//	void SetClipPositionX(const float& _XPos, const bool _Less = true);
//	void SetClipPositionY(const float& _YPos, const bool _Less = true);
//	void ClipPositionOff();
//


} CFixRenderer, Fix2DRenderer,Sprite2DRen;

