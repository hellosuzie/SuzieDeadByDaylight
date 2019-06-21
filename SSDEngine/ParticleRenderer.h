#pragma once
#include "Renderer.h"
#include "DXVertex.h"

#define MAXPARTICLECOUNT 512
#define FRAMETIME  0.01f

//Vec3				ParticlePos;
//Vec3				ParticleViewPos;
//float				ParticleTex;
//float				SizeX;
//float				SizeY;
//Vec2				ParticleTime;
//Vec4				Check;
//Vec3				AxisX;
//Vec3				AxisY;
//Vec4				AnimUV;

enum FADE_TYPE
{
	FADE_IN,
	FADE_OUT
};

enum FADE_COLOR
{
	FADE_COLOR_BLACK,
	FADE_COLOR_WHITE,
	FADE_COLOR_RED,
};

class PARTICLEANIM
{
public:
	//누적시간
	float		AccTime;
	bool		IsLoop;
	//가로세로 인덱스
	IVec2		SpriteCount;
	//몇번째 스프라이트까지 재생할것인지
	int			CurFrame;
	int			FrameCount;

public:
	PARTICLEANIM(bool _Loop, Vec2 _Count, int _FrameCount) :AccTime(0.f), CurFrame(0), IsLoop(_Loop), SpriteCount(_Count), FrameCount(_FrameCount)
	{}
	PARTICLEANIM() {}
	~PARTICLEANIM() {}

};

class CParticleRenderer :
	public CRenderer
{
private:
	SPTR<class CMesh>							m_ParticleMesh;
	class CMaterial*							m_ParticleMat;

private:
	std::vector<PARTICLEVTX>					m_vecParticleVtx;
	int											m_VtxCount;

	CBUFFER::ParticleBuffer						m_ParticleBuffer;
	std::vector<ID3D11ShaderResourceView*>		m_vecParticleTex;
	std::vector<ID3D11ShaderResourceView*>		m_vecFadeParticleTex;

	float										m_AccTime;
	SPTR<class CMultiTexture>					m_ParticleTex;

private:
	//파티클 애니메이션 정보
	std::vector<PARTICLEANIM>					m_vecAnim;

private:
	PARTICLEVTX									m_FadeParticle;
	CBUFFER::ParticleBuffer						m_FadeBuffer;
	bool										m_bFade;
	float										m_fAccFadeTime;
	float										m_fFadeTime;
	Vec4										m_fFadeColor;
	bool										m_bFadeIn;
	static float								m_FadeColor;

public:
	const bool Init(const RENDATA& _InitData);
	void PostUpdate(const float& _DeltaTime) override;

	void Render(SPTR<CCamera> _Cam) override;
	void RenderUpdate(SPTR<CCamera> _Cam) override;

public:
	int PushParticle(const PARTICLEVTX& _Particle);
	int UpdateParticleData(int _Index, const PARTICLEVTX& _Particle);
	void PushParticleTex(const wchar_t * _MultiTexName, const wchar_t * _FadeMultiTexName);
	void PushFadeParticleTex(const wchar_t * _FadeMultiTexName);
	void PushParticleAnimation(int _ParticleIndex, const PARTICLEANIM& _Anim);

	void ParticleSort(SPTR<CCamera> _Cam);
	void MoveParticle(const int& _Index, const float& _AccTime, const float& _Speed);

	void UpdateParticleAnim(int _Index, PARTICLEANIM& _Anim, float _DeltaTime);

public:
	void SetParticleColor(Vec4 _Color)
	{
		m_ParticleBuffer.ParticleColor = _Color;
	}
	void SetParticleAlpha(float _Alpha)
	{
		m_ParticleBuffer.ParticleColor = Vec4(1.f, 1.f, 1.f, _Alpha);
	}

public:
	// 필요에 따라서 페이드 색상을 인자로 받을수 있다
	void SetFadeEffect(const float & _Time, FADE_TYPE _Type = FADE_TYPE::FADE_IN, FADE_COLOR _Color = FADE_COLOR::FADE_COLOR_BLACK);

public:
	CParticleRenderer();
	~CParticleRenderer();
};

//랜더순서 소팅