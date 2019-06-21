#pragma once
#include "FbxRenderer.h"
#include <unordered_map>


#define ANIMTEXSLOT		10
#define DEFANIM			L"DefaultAnim"


typedef struct _tagANIMSOCKET 
{
	UINT								BoneIdx;

	SPTR<CTransForm>					ConnectTrans;
	std::string							BoneName;

}AnimSocket;





class CFbx;
class CTexture;
typedef class CSkeletalFbxRenderer : public CFbxRenderer
{ 
public:
	class FrameEvent : public CReferenceBase
	{
	public:
		unsigned int							PrevFrame;
		unsigned int							CurFrame;

		bool									TimesCheck;
		FRAME_EVENT_TYPE						EventType;
		unsigned int							StartFrame;
		void*									EventPara;
		std::function<void(void*)>				EventFunc;

	public:
		FrameEvent() : PrevFrame(-1), CurFrame(0), TimesCheck(true), StartFrame(0), EventPara(nullptr) {}
		~FrameEvent() {}

	public:
		void PlayEvent(unsigned int _StartFrame);
	};


public :
	typedef class CBoneAnimation : public CReferenceBase , public CNameBase
	{
		friend class CSkeletalFbxRenderer;

	private:
		std::wstring										m_FbxAnimKey;			// Fbx안에서의 애니메이션 키 프레임 이름
		int													m_iFbxAnimIdx;			// 몇번째 애니메이션을 사용할지
		int													m_iAnimFPS;				// 초당 애니메이션 프레임 수
		float												m_fOneFrameTime;		// 한 프레임에서의 시간
		float												m_fAnimationSpeed;		// 애니메이션 속도
		float												m_fCurAnimTime;			// 현재 재생 시간
		float												m_fAccUpdateTime;		// 누적 업데이트 시간
		bool												m_bAnimEnd;
		
	private:
		float												m_fAnimStartTime;
		float												m_fAnimEndTime;
		float												m_fAnimLength;			// 자신에게 지정된 애니메이션 길이
	
	private:
		int													m_iStartFrame;
		int													m_iEndFrame;
		int													m_iLoopCount;

	private:
		std::vector<KeyFrame>								m_vecKeyFrame;
		CBUFFER::SkinningBuffer								m_SkinBuffer;

	public:						
		std::unordered_map<unsigned int, SPTR<FrameEvent>>						m_EventMap;
		std::unordered_map<unsigned int, SPTR<FrameEvent>>::iterator			m_EventFindIter;

	public:
		void SetFbx(SPTR<CFbx> _pFbx,const int& _FbxAnimIdx ,const int& _StartFrame = -1, const int& _EndFrame = -1);
	public:
		void Update(const float& _DeltaTime,SPTR<CSkeletalFbxRenderer> _Ren);
		void CalKeyFrame(const float& _DeltaTime, SPTR<CSkeletalFbxRenderer> _Ren);


		void PlayEvent(unsigned int _StartFrame);

	public:
		void ResetAnimation() 
		{
			m_fCurAnimTime = 0.f;
			m_fAccUpdateTime = 0.f;
			m_bAnimEnd = false;
			m_iLoopCount = 0;
		}

		void SetAnimationSpeed(const float& _Speed) { m_fAnimationSpeed = _Speed; }

	public:
		const KeyFrame GetCurKeyFrame(const UINT& _BoneIdx) { return m_vecKeyFrame[_BoneIdx]; }
	public:
		CBoneAnimation();
		~CBoneAnimation();
	}CSkeletalAnimation;

private:
	friend CBoneAnimation;


protected:
	std::unordered_map<std::wstring, SPTR<CBoneAnimation>>				m_mapBoneAnim;
	std::unordered_map<std::wstring, SubTransData>						m_mapAnimSubTrans;
	SPTR<CBoneAnimation>												m_PrevBoneAnim;
	SPTR<CBoneAnimation>												m_CurBoneAnim;
	float																m_fAccBlendingTime;
	float																m_fBlendingTime;
	bool																m_bAnimBlending;

protected:
	std::vector<Mat>													m_vecCurAnimMat;			// 현재 계산된 애니메이션 키 프레임 행렬
	std::vector<Mat>													m_vecCurSkinningmMat;		// 스키닝 전용 키 프레임 행렬 = 본 오프셋 행렬 * 애니메이션 키 프레임 행렬

	SPTR<CTexture>														m_BoneTexture;				// 계산된 스키닝 행렬을 담은 텍스쳐 (CPU용)
	SPTR<CTexture>														m_CSBoneTexture;			// 계산된 스키닝 행렬을 담은 텍스쳐 (GPU용)
protected:
	std::vector<AnimSocket>												m_vecAnimSocket;

private:
	void(CSkeletalFbxRenderer::*pAnimationUpdate)(const float& );

public:
	CBoneAnimation* CreateDefaultAnimation();
	CBoneAnimation* CreateBoneAnimation(const wchar_t* _AnimKey, const int& _AnimIdx);
	CBoneAnimation* CreateBoneAnimation(const wchar_t* _AnimKey,const int& _AnimIdx,
		const int& _StartFrame,const int& _EndFrame);
	void SetAnimationSubTrans(const wchar_t* _AnimKey, const Vec3 _Pivot, const Vec3 _Rot);

	SPTR<CBoneAnimation> ChangeDefaultAnimation();
	SPTR<CBoneAnimation> ChangeBoneAnimation(const wchar_t* _AnimKey);
	SPTR<CBoneAnimation> ChangeBoneAnimation(const wchar_t* _AnimKey,const float& _BlendingTime);
	SPTR<CBoneAnimation> ChangePrevAnimation();
	SPTR<CBoneAnimation> ChangeNextAnimation();
	void SyncroAnimationSubTrans();
public:
	template<typename T>
	void PushBoneAnimEvent(const wchar_t* _AnimKey, unsigned int _StartFrame, void(T::*pFunc)(void*), T* _Obj, void* _EventPara, FRAME_EVENT_TYPE _EventType = FRAME_EVENT_TYPE::EVENT_TEMPORARY)
	{
		std::unordered_map<std::wstring, SPTR<CBoneAnimation>>::iterator FindAnim = m_mapBoneAnim.find(_AnimKey);

		if (FindAnim == m_mapBoneAnim.end())
		{
#ifdef _DEBUG
			assert(nullptr);
#endif
			return ;
		}

		SPTR<FrameEvent> NewEvent = new FrameEvent();
		NewEvent->StartFrame = _StartFrame;
		NewEvent->EventType = _EventType;
		NewEvent->EventPara = _EventPara;

		std::function<void(void*)> EventFunc = std::bind(pFunc, _Obj, std::placeholders::_1);
		if (nullptr == EventFunc)
		{
			return;
		}
		NewEvent->EventFunc = EventFunc;
		FindAnim->second->m_EventMap.insert(std::unordered_map<int, SPTR<FrameEvent>>::value_type(_StartFrame, NewEvent));
	}

	void PushFbxMesh(const int& _MeshIdx) override
	{
		SPTR<CFBXMesh> Mesh = m_RenderingFbx->GetFBXMesh(_MeshIdx, L"SkeletalMaterial");

		if (nullptr == Mesh)
		{
#ifdef _DEBUG
			assert(nullptr);
			return;
#endif
		}
		else
			m_vecMesh.push_back(Mesh);
	}

public:
	void SetFbx(const wchar_t* _FbxKey) override;
	void SetFbx(const wchar_t* _FbxKey, const wchar_t* _MatKey);
	void Update(const float& _DeltaTime) override;
	void PostUpdate(const float& _DeltaTime) override;
	void RenderUpdate(SPTR<CCamera> _Cam) override;
	void RenderRelease() override;
	
private:
	void BaseAnimUpdate(const float& _DeltaTime);
	void BlendAnimUpdate(const float& _DeltaTime);

public:
	void AnimSocketUpdate(const float& _DeltaTime);
public:
	void AddAnimSocket(const char* _BoneKey, SPTR<CTransForm> _Trans);

public:
	const bool CheckAnimationEnd();


public:
	CSkeletalFbxRenderer();
	~CSkeletalFbxRenderer();
}CSkeletalRenderer;

