#pragma once
#include "BaseComponent.h"
#include"DXEnum.h"
#include"DXHeader.h"
#include<unordered_map>
#include"Sprite2DRenderer.h"


struct ANIDATA 
{
	std::wstring						MultiSpriteKey;
	RENDER_LAYER						RenderLayer;
};

typedef struct _tagANISPRITE2DDATA
{
	std::wstring						MultiSpriteKey;
	_tagSPRITE2DDATA					SpriteRenData;

}ANIDATA_SPRITE2D;

class CAnimator2D : public CBaseComponent
{
	class CAnimation2D : public CGameObject
	{

	private:
		friend CAnimator2D;
		friend Engine;
	private:
		bool						m_bLoop;
		bool						m_bOneFrameAni;
		bool						m_bEndAnimation;


		unsigned int				m_StartFrame;
		unsigned int				m_EndFrame;
		unsigned int				m_CurFrame;

		float						m_fFrameInterval;
		float						m_fAccTime;

		std::list<int>				m_listNotifyFrame;
		bool						m_bNotifyFlag;
	private:
		void ResetAnimation();
		// 리스트 안에있는 프레임으로 바뀌는 경우 노티파이 불값을 트루로 전환
		void RegisteNotify(const unsigned int& _Frame);
		const bool CheckNotifyFrame();
	private:
		size_t Update(const float& _DeltaTime);


		CAnimation2D()
			:m_bLoop(false), m_bEndAnimation(false), m_bOneFrameAni(false), m_StartFrame(0)
			, m_EndFrame(0), m_CurFrame(0), m_fFrameInterval(0.f), m_fAccTime(0.f)
			, m_bNotifyFlag(false)
		{}
		~CAnimation2D() {}
	};

private:
	CSprite2DRenderer*													m_SpriteRenderer;
	std::unordered_map<std::wstring, SPTR<CAnimation2D>>				m_mapAnimation;
	SPTR<CAnimation2D>													m_CurAnimation;
	// 현재 애니메이션을 일시 중지
	bool																m_bAnimationPause;

public:
	const bool Init(const ANIDATA& _InitData);	
	const bool Init(const ANIDATA_SPRITE2D& _InitData);

public:
	void Update(const float& _DeltaTime) override;
	void DebugOn() override;
	void DebugOff() override;
public:
	const Vec4 GetCurAnimationUV();
	const size_t GetCurAniSpriteIndex();

public:
	const bool ChangeSprite(const wchar_t* _SpriteKey);

public:
	void SetRenPivot(const Vec3& _Pivot);
	void SetRenSize(const Vec3& _Size);
	void SetColorFactor(const Vec4& _ColorFactor);
	void SetAutoColorFactor(const Vec4& _StartFactor,const Vec4& _EndFactor,const float& _Time);
	void SetAlpha(const float& _Alpha);
	void SetDepthState(const wchar_t* _DepthKey);
	void SetBlendState(const wchar_t* _BlendKey);

	
	void SetRenMag(const float& _Mag);		// X,Y 배율
	void SetRenXMag(const float& _Mag);		// X 배율
	void SetRenYMag(const float& _Mag);		// Y 배율


public:
	const Vec3 GetRenSize();
	const Vec3 GetRenWorldPos();
	SPTR<CSprite2DRenderer> GetAniSpriteRen();
	SPTR<class CTexture> GetspriteTexture();
	const std::wstring GetCurAnimtionKey();
public:
	void SetAnimationNotify(const wchar_t* _AniKey, const int& _NotifyFrame);

public:
	SPTR<CAnimation2D> FindAnimation(const wchar_t* _AniKey);
	SPTR<CAnimation2D> ChangeAnimation(const wchar_t* _AniKey);
	CAnimation2D*	CreateAnimation(const wchar_t* _AniKey, const unsigned int& _StartFrame, const unsigned int& _EndFrame, const float& _FrameTime = 0.1f, const bool& _Loop = true);
	CAnimation2D*	CreateAnimation(const wchar_t* _AniKey, const unsigned int& _StartFrame, const unsigned int& _EndFrame, const bool& _Loop , const float& _FrameTime = 0.1f);

public:
	const bool IsCurAniEnd() { return m_CurAnimation->m_bEndAnimation; }
	const bool IsNotified() { return m_CurAnimation->m_bNotifyFlag; }
	const void PauseAnimation(const bool _Pause) { m_bAnimationPause = _Pause; }

public:
	CAnimator2D();
	~CAnimator2D();
};

