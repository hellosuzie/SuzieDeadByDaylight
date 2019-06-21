#include "SkeletalFbxRenderer.h"
#include "Texture.h"
#include "ResourceMgr.h"
#include "Camera.h"
#include "DXMath.h"
#include "TransForm.h"
#include "Scene.h"
#include "Actor.h"
#include <ppl.h>
#include "TimerMgr.h"
//#include <amp.h>

/////////////////////////////////////			Skeletal				//////////////////////////////////

CSkeletalFbxRenderer::CBoneAnimation::CBoneAnimation()
	:m_iFbxAnimIdx(0), m_iAnimFPS(0), m_fAnimationSpeed(1.f), m_fCurAnimTime(0.f), m_fAccUpdateTime(0.f)
	, m_iStartFrame(0), m_iEndFrame(0), m_fAnimStartTime(0.f), m_fAnimEndTime(0.f), m_fOneFrameTime(0.f)
	, m_iLoopCount(0), m_bAnimEnd(false)
{

}
CSkeletalFbxRenderer::CBoneAnimation::~CBoneAnimation()
{

}

void CSkeletalFbxRenderer::FrameEvent::PlayEvent(unsigned int _StartFrame)
{
	if (FRAME_EVENT_TYPE::EVENT_CONSTANT == EventType)
	{
		if (nullptr != EventFunc)
		{
			EventFunc(EventPara);
		}
	}
	else if (FRAME_EVENT_TYPE::EVENT_TEMPORARY == EventType)
	{
		if (true == TimesCheck)
		{
			if (nullptr != EventFunc)
			{
				EventFunc(EventPara);
				TimesCheck = false;
			}
		}
	}
}

void CSkeletalFbxRenderer::CBoneAnimation::SetFbx(SPTR<CFbx> _pFbx, const int& _FbxAnimIdx, const int& _StartFrame /*= -1 */, const int& _EndFrame /*= -1*/)
{
	m_iFbxAnimIdx = _FbxAnimIdx;
	m_FbxAnimKey = _pFbx->m_AllAniInfoVec[m_iFbxAnimIdx]->Name;
	m_iAnimFPS = _pFbx->GetAnimFPS(m_iFbxAnimIdx);
	m_fOneFrameTime = _pFbx->GetOneFrameTime(m_iFbxAnimIdx);

	if (-1 == _StartFrame && -1 == _EndFrame)
	{
		m_iStartFrame = (int)_pFbx->m_AllAniInfoVec[m_iFbxAnimIdx]->StartFrame;
		m_iEndFrame = (int)_pFbx->m_AllAniInfoVec[m_iFbxAnimIdx]->EndFrame - 1;
	}
	else 
	{
		m_iStartFrame = _StartFrame;
		m_iEndFrame = min(_EndFrame, (int)_pFbx->m_AllAniInfoVec[m_iFbxAnimIdx]->EndFrame - 1);
	}

	// StartFrame ~ EndFrame 사이의 시간 계산 
	//for (size_t i = 0; i < _pFbx->m_AllBoneVec.size(); i++)
	//{
	//	if (true == _pFbx->m_AllBoneVec[i]->KeyFrameMap.empty())
	//		continue;

	//	if (false == _pFbx->m_AllBoneVec[i]->KeyFrameMap[m_iFbxAnimIdx].empty())
	//	{
	//		m_fAnimStartTime = (float)_pFbx->m_AllBoneVec[i]->KeyFrameMap[m_iFbxAnimIdx][m_iStartFrame].dTime;
	//		m_fAnimEndTime = (float)_pFbx->m_AllBoneVec[i]->KeyFrameMap[m_iFbxAnimIdx][m_iEndFrame].dTime + m_fOneFrameTime;
	//		break;
	//	}
	//}
	
	m_fAnimStartTime = m_fOneFrameTime * (float)m_iStartFrame;
	m_fAnimEndTime = m_fOneFrameTime * (float)m_iEndFrame + m_fOneFrameTime;

	m_fAnimLength = m_fAnimEndTime - m_fAnimStartTime;

	m_SkinBuffer.BoneCount = _pFbx->m_AllBoneCount;
	m_SkinBuffer.KeyFrameCount = m_iEndFrame - m_iStartFrame;
}

void CSkeletalFbxRenderer::CBoneAnimation::Update(const float& _DeltaTime, SPTR<CSkeletalFbxRenderer> _Ren)
{
	if (nullptr == _Ren)
		return;
	
	m_bAnimEnd = false;
	m_fCurAnimTime = 0.f;
	m_fAccUpdateTime += _DeltaTime * m_fAnimationSpeed;

	CFbx* SkelFbx = _Ren->m_RenderingFbx;
	CTexture* BoneTex = _Ren->m_BoneTexture;

	// 애니메이션 누적시간이 FBX의 애니메이션 길이보다 긴경우
	// 누적시간을 초기화(Animation 이 Loop 라면)
	while (m_fAccUpdateTime >= m_fAnimLength)
	{
		m_fAccUpdateTime = DXMath::Clamp(m_fAccUpdateTime - m_fAnimLength, 0.f, m_fAnimLength);
		m_bAnimEnd = true;
	}


	// m_fCurAnimTime   = 애니메이션 누적시간 + FBX 애니메이션 첫 프레임 시작 시간
	m_fCurAnimTime = m_fAccUpdateTime + m_fAnimStartTime;

	int iFrame = DXMath::Clamp( (int)(m_fCurAnimTime * m_iAnimFPS),0, m_iEndFrame);
	int iNextFrame = 0;

	//저장된 이벤트 호출하기
	PlayEvent(iFrame);

	iNextFrame = iFrame + 1;

	if (iNextFrame >= m_iEndFrame + 1)
		iNextFrame = m_iStartFrame;

	size_t MaxBone = SkelFbx->m_AllBoneVec.size();
	
	// 모든 본의 애니메이션 행렬을 계산한다


	concurrency::parallel_for(size_t(0), MaxBone, [&](size_t i) 
	{
		// 현재 본에 키프레임이 없는경우 본 행렬이 곧 애니메이션 행렬
		if (SkelFbx->m_AllBoneVec[i]->KeyFrameMap.size() <= m_iFbxAnimIdx ||
			true == SkelFbx->m_AllBoneVec[i]->KeyFrameMap[m_iFbxAnimIdx].empty())
		{
			if (nullptr == SkelFbx->m_AllBoneVec[i]->Parent)
			{
				_Ren->m_vecCurSkinningmMat[i] = SkelFbx->m_AllBoneVec[i]->BoneMat;
			}
		}
		else 
		{
			KeyFrame CurFrame = SkelFbx->m_AllBoneVec[i]->KeyFrameMap[m_iFbxAnimIdx][iFrame];
			KeyFrame NextFrame = SkelFbx->m_AllBoneVec[i]->KeyFrameMap[m_iFbxAnimIdx][iNextFrame];

			float fCurFrameTime = (float)CurFrame.dTime;


			// 프레임 사이의 보간 비율을 계산한다 ( 0.f < T < 1.f)
			// (애니메이션 진행시간 - 현재 프레임의 시작 시간) / (한 프레임 시간)
			float T = (float)(m_fCurAnimTime - fCurFrameTime) / m_fOneFrameTime;
			T = DXMath::Clamp(T, 0.f, 1.f);

			m_vecKeyFrame[i].Scale = DirectX::XMVectorLerp(CurFrame.Scale, NextFrame.Scale, T);
			m_vecKeyFrame[i].Trans = DirectX::XMVectorLerp(CurFrame.Trans, NextFrame.Trans, T);
			m_vecKeyFrame[i].Quat = DirectX::XMQuaternionSlerp(CurFrame.Quat, NextFrame.Quat, T);

			_Ren->m_vecCurAnimMat[i] = DirectX::XMMatrixAffineTransformation(m_vecKeyFrame[i].Scale, Vec4::Zero, m_vecKeyFrame[i].Quat, m_vecKeyFrame[i].Trans);
			_Ren->m_vecCurSkinningmMat[i] = SkelFbx->m_AllBoneVec[i]->OffsetMat * _Ren->m_vecCurAnimMat[i];
		}
		});
}


void  CSkeletalFbxRenderer::CBoneAnimation::CalKeyFrame(const float& _DeltaTime, SPTR<CSkeletalFbxRenderer> _Ren)
{
	if (nullptr == _Ren)
		return;

	m_bAnimEnd = false;
	m_fCurAnimTime = 0.f;
	m_fAccUpdateTime += _DeltaTime * m_fAnimationSpeed;

	CFbx* SkelFbx = _Ren->m_RenderingFbx;
	CTexture* BoneTex = _Ren->m_BoneTexture;

	// 애니메이션 누적시간이 FBX의 애니메이션 길이보다 긴경우
	// 누적시간을 초기화(Animation 이 Loop 라면)
	while (m_fAccUpdateTime >= m_fAnimLength)
	{
		m_fAccUpdateTime = DXMath::Clamp(m_fAccUpdateTime - m_fAnimLength, 0.f, m_fAnimLength);
		m_bAnimEnd = true;
	}


	// m_fCurAnimTime   = 애니메이션 누적시간 + FBX 애니메이션 첫 프레임 시작 시간
	m_fCurAnimTime = m_fAccUpdateTime + m_fAnimStartTime;

	int iFrame = (int)(m_fCurAnimTime * m_iAnimFPS);
	int iNextFrame = 0;

	//저장된 이벤트 호출하기
	PlayEvent(iFrame);

	iNextFrame = iFrame + 1;

	if (iNextFrame >= m_iEndFrame + 1)
		iNextFrame = m_iStartFrame;

	size_t MaxBone = SkelFbx->m_AllBoneVec.size();

	// 모든 본의 애니메이션 행렬을 계산한다


	concurrency::parallel_for(size_t(0), MaxBone, [&](size_t i)
	{
		// 현재 본에 키프레임이 없는경우 본 행렬이 곧 애니메이션 행렬
		if (SkelFbx->m_AllBoneVec[i]->KeyFrameMap.size() <= m_iFbxAnimIdx ||
			true == SkelFbx->m_AllBoneVec[i]->KeyFrameMap[m_iFbxAnimIdx].empty())
		{
			if (nullptr == SkelFbx->m_AllBoneVec[i]->Parent)
			{
				_Ren->m_vecCurSkinningmMat[i] = SkelFbx->m_AllBoneVec[i]->BoneMat;
			}
		}
		else
		{
			KeyFrame CurFrame = SkelFbx->m_AllBoneVec[i]->KeyFrameMap[m_iFbxAnimIdx][iFrame];
			KeyFrame NextFrame = SkelFbx->m_AllBoneVec[i]->KeyFrameMap[m_iFbxAnimIdx][iNextFrame];

			float fCurFrameTime = (float)CurFrame.dTime;


			// 프레임 사이의 보간 비율을 계산한다 ( 0.f < T < 1.f)
			// (애니메이션 진행시간 - 현재 프레임의 시작 시간) / (한 프레임 시간)
			float T = (float)(m_fCurAnimTime - fCurFrameTime) / m_fOneFrameTime;
			T = DXMath::Clamp(T, 0.f, 1.f);

			m_vecKeyFrame[i].Scale = DirectX::XMVectorLerp(CurFrame.Scale, NextFrame.Scale, T);
			m_vecKeyFrame[i].Trans = DirectX::XMVectorLerp(CurFrame.Trans, NextFrame.Trans, T);
			m_vecKeyFrame[i].Quat = DirectX::XMQuaternionSlerp(CurFrame.Quat, NextFrame.Quat, T);
		}
	});
}


void CSkeletalFbxRenderer::CBoneAnimation::PlayEvent(unsigned int _StartFrame)
{
	m_EventFindIter = m_EventMap.find(_StartFrame);
	if (m_EventMap.end() == m_EventFindIter)
	{
		return;
	}
	m_EventFindIter->second->PlayEvent(m_EventFindIter->first);
}

/////////////////////////////////////			Skeletal Renderer		//////////////////////////////////



CSkeletalFbxRenderer::CSkeletalFbxRenderer()
	:CFbxRenderer(), m_CurBoneAnim(nullptr), m_PrevBoneAnim(nullptr) , m_fAccBlendingTime(0.f)
	, m_fBlendingTime(0.f),pAnimationUpdate(nullptr)
{

}

CSkeletalFbxRenderer::~CSkeletalFbxRenderer()
{
	m_BoneTexture = nullptr;

	m_vecCurAnimMat.clear();
	m_vecCurSkinningmMat.clear();
}
CSkeletalFbxRenderer::CBoneAnimation* CSkeletalFbxRenderer::CreateDefaultAnimation()
{
	if (nullptr == m_RenderingFbx)
		return nullptr;

	if (false == m_RenderingFbx->IsAni())
	{
#ifdef _DEBUG
		assert(nullptr);
#endif
		return nullptr;
	}

	CSkeletalAnimation* NewAnim = new CSkeletalAnimation();
	NewAnim->SetFbx(m_RenderingFbx, 0);
	NewAnim->m_fCurAnimTime = 0.f;
	NewAnim->m_fAccUpdateTime = 0.f;
	NewAnim->SetName(DEFANIM);
	NewAnim->m_vecKeyFrame.resize(m_RenderingFbx->m_AllBoneVec.size());
	auto Check = m_mapBoneAnim.insert(std::unordered_map<std::wstring, SPTR<CBoneAnimation>>::value_type(DEFANIM, NewAnim));

	if (false == Check.second)
	{
		Check.first->second = NewAnim;
	}

	return NewAnim;
}
CSkeletalFbxRenderer::CBoneAnimation* CSkeletalFbxRenderer::CreateBoneAnimation(const wchar_t* _AnimKey, const int& _AnimIdx)
{
	return CreateBoneAnimation(_AnimKey, _AnimIdx, -1, -1);
}
CSkeletalFbxRenderer::CBoneAnimation* CSkeletalFbxRenderer::CreateBoneAnimation(const wchar_t* _AnimKey
	, const int& _AnimIdx
	, const int& _StartFrame
	, const int& _EndFrame) 
{
	if (nullptr == m_RenderingFbx)
		return nullptr;

	if (false == m_RenderingFbx->IsAni() || (int)m_RenderingFbx->GetFbxAnimCount() <= _AnimIdx)
	{
#ifdef _DEBUG
		assert(nullptr);
#endif
		return nullptr;
	}

	std::unordered_map<std::wstring, SPTR<CBoneAnimation>>::iterator FindAnim =  m_mapBoneAnim.find(_AnimKey);

	if (FindAnim != m_mapBoneAnim.end()) 
	{
#ifdef _DEBUG
		assert(nullptr);
#endif
		return nullptr;
	}

	CSkeletalAnimation* NewAnim = new CSkeletalAnimation();
	NewAnim->SetFbx(m_RenderingFbx, _AnimIdx, _StartFrame, _EndFrame);
	NewAnim->m_fCurAnimTime = 0.f;
	NewAnim->m_fAccUpdateTime = 0.f;
	NewAnim->SetName(_AnimKey);
	NewAnim->m_vecKeyFrame.resize(m_RenderingFbx->m_AllBoneVec.size());
	m_mapBoneAnim.insert(std::unordered_map<std::wstring, SPTR<CBoneAnimation>>::value_type(_AnimKey, NewAnim));

	return NewAnim;
}
void CSkeletalFbxRenderer::SetAnimationSubTrans(const wchar_t* _AnimKey, const Vec3 _Pivot, const Vec3 _Rot)
{
	if (m_mapBoneAnim.end() == m_mapBoneAnim.find(_AnimKey)) 
	{
		return;
	}

	std::unordered_map<std::wstring, SubTransData>::iterator Find =  m_mapAnimSubTrans.find(_AnimKey);

	if (Find == m_mapAnimSubTrans.end()) 
	{
		m_mapAnimSubTrans.insert(std::unordered_map<std::wstring, SubTransData>::value_type(_AnimKey, SubTransData(_Pivot, Vec3::One3, _Rot)));
	}
	else 
	{
		Find->second.SubPivot = _Pivot;
		Find->second.SubRot = _Rot;
	}
}
SPTR<CSkeletalFbxRenderer::CBoneAnimation> CSkeletalFbxRenderer::ChangeDefaultAnimation()
{
	return ChangeBoneAnimation(DEFANIM);
}
SPTR<CSkeletalFbxRenderer::CBoneAnimation> CSkeletalFbxRenderer::ChangeBoneAnimation(const wchar_t* _AnimKey)
{
	std::unordered_map<std::wstring, SPTR<CBoneAnimation>>::iterator FindAnim = m_mapBoneAnim.find(_AnimKey);

	if (FindAnim == m_mapBoneAnim.end())
	{
#ifdef _DEBUG
		assert(nullptr);
#endif
		return nullptr;
	}

	pAnimationUpdate = &CSkeletalFbxRenderer::BaseAnimUpdate;
	m_PrevBoneAnim = nullptr;
	m_CurBoneAnim = FindAnim->second;
	m_CurBoneAnim->ResetAnimation();
	SyncroAnimationSubTrans();

	return m_CurBoneAnim;
}
SPTR<CSkeletalFbxRenderer::CBoneAnimation> CSkeletalFbxRenderer::ChangeBoneAnimation(const wchar_t* _AnimKey, const float& _BlendingTime)
{
	std::unordered_map<std::wstring, SPTR<CBoneAnimation>>::iterator FindAnim = m_mapBoneAnim.find(_AnimKey);

	if (FindAnim == m_mapBoneAnim.end())
	{
#ifdef _DEBUG
		assert(nullptr);
#endif
		return nullptr;
	}

	pAnimationUpdate = &CSkeletalFbxRenderer::BlendAnimUpdate;
	m_PrevBoneAnim = m_CurBoneAnim;
	m_CurBoneAnim = FindAnim->second;
	m_CurBoneAnim->ResetAnimation();
	m_fAccBlendingTime = 0.f;
	m_fBlendingTime = _BlendingTime;
	SyncroAnimationSubTrans();
	return m_CurBoneAnim;
}
SPTR<CSkeletalFbxRenderer::CBoneAnimation> CSkeletalFbxRenderer::ChangePrevAnimation()
{
	if (nullptr == m_CurBoneAnim)
		return ChangeDefaultAnimation();

	std::unordered_map<std::wstring, SPTR<CBoneAnimation>>::iterator Find = m_mapBoneAnim.find(m_CurBoneAnim->GetName());

	if (Find == m_mapBoneAnim.begin())
	{
		Find = m_mapBoneAnim.end();
	}

	--Find;

	return ChangeBoneAnimation(Find->second->GetName().c_str());
}
SPTR<CSkeletalFbxRenderer::CBoneAnimation> CSkeletalFbxRenderer::ChangeNextAnimation()
{
	if (nullptr == m_CurBoneAnim)
		return ChangeDefaultAnimation();

	std::unordered_map<std::wstring, SPTR<CBoneAnimation>>::iterator Find = m_mapBoneAnim.find(m_CurBoneAnim->GetName());

	++Find;

	if (Find == m_mapBoneAnim.end())
		return ChangeDefaultAnimation();

	return ChangeBoneAnimation(Find->second->GetName().c_str());
}
void CSkeletalFbxRenderer::SyncroAnimationSubTrans()
{
	std::unordered_map<std::wstring, SubTransData>::iterator Find = m_mapAnimSubTrans.find(m_CurBoneAnim->GetName());

	if (Find == m_mapAnimSubTrans.end()) 
	{
		SetSubPivot(Vec3());
		SetSubRot(Vec3());
	}
	else 
	{
		SetSubPivot(Find->second.SubPivot);
		SetSubRot(Find->second.SubRot);
	}
}
void CSkeletalFbxRenderer::SetFbx(const wchar_t* _FbxKey)
{
	CFbxRenderer::SetFbx(_FbxKey);
	SetAllFbxMesh(L"SkeletalMaterial");
	m_BoneTexture = m_RenderingFbx->m_BoneMatTex->GetCopyCreateTexture();
	m_CSBoneTexture = m_RenderingFbx->m_ComputeBoneMatTex->GetCopyCreateTexture();


	CreateDefaultAnimation();
	ChangeBoneAnimation(DEFANIM);

	m_vecCurAnimMat.resize(m_RenderingFbx->m_AllBoneVec.size());
	m_vecCurSkinningmMat.resize(m_RenderingFbx->m_AllBoneVec.size());
	m_vecAnimSocket.clear();
}
void CSkeletalFbxRenderer::SetFbx(const wchar_t* _FbxKey, const wchar_t* _MatKey)
{
	CFbxRenderer::SetFbx(_FbxKey);
	SetAllFbxMesh(_MatKey);
	m_BoneTexture = m_RenderingFbx->m_BoneMatTex->GetCopyCreateTexture();
	m_CSBoneTexture = m_RenderingFbx->m_ComputeBoneMatTex->GetCopyCreateTexture();


	CreateDefaultAnimation();
	ChangeBoneAnimation(DEFANIM);

	m_vecCurAnimMat.resize(m_RenderingFbx->m_AllBoneVec.size());
	m_vecCurSkinningmMat.resize(m_RenderingFbx->m_AllBoneVec.size());
	m_vecAnimSocket.clear();
}
void CSkeletalFbxRenderer::Update(const float& _DeltaTime)
{
	if (nullptr == m_RenderingFbx || nullptr == m_CurBoneAnim)
		return;

	// CPU Update
	//m_CurBoneAnim->Update(_DeltaTime, this);
	if (nullptr != pAnimationUpdate) 
	{
		(this->*pAnimationUpdate)(_DeltaTime);
	}

	m_BoneTexture->SetPixel(&m_vecCurSkinningmMat[0], sizeof(Mat)*(UINT)m_vecCurSkinningmMat.size());

}

void CSkeletalFbxRenderer::BaseAnimUpdate(const float& _DeltaTime) 
{
	m_CurBoneAnim->Update(_DeltaTime, this);
}
void CSkeletalFbxRenderer::BlendAnimUpdate(const float& _DeltaTime) 
{
	if (m_fAccBlendingTime >= m_fBlendingTime) 
	{
		m_PrevBoneAnim = nullptr;
		pAnimationUpdate = &CSkeletalFbxRenderer::BaseAnimUpdate;
		(this->*pAnimationUpdate)(_DeltaTime);
		return;
	}

	m_PrevBoneAnim->CalKeyFrame(_DeltaTime, this);
	m_CurBoneAnim->CalKeyFrame(_DeltaTime, this);

	size_t MaxBone = m_RenderingFbx->m_AllBoneVec.size();
	float T = DXMath::Clamp(m_fAccBlendingTime / m_fBlendingTime, 0.f, 1.f);
	T = powf(T, 2.f);
	// 모든 본의 애니메이션 행렬을 계산한다

	concurrency::parallel_for(size_t(0), MaxBone, [&](size_t i)
	{
		// 현재 본에 키프레임이 없는경우 본 행렬이 곧 애니메이션 행렬
		if ( true == m_RenderingFbx->m_AllBoneVec[i]->KeyFrameMap.empty())
		{
			if (nullptr == m_RenderingFbx->m_AllBoneVec[i]->Parent)
			{
				m_vecCurSkinningmMat[i] = m_RenderingFbx->m_AllBoneVec[i]->BoneMat;
			}
		}
		else
		{
			KeyFrame BlendFrame = m_PrevBoneAnim->GetCurKeyFrame((UINT)i);
			KeyFrame CurFrame = m_CurBoneAnim->GetCurKeyFrame((UINT)i);

			DirectX::XMVECTOR vS = DirectX::XMVectorLerp(BlendFrame.Scale, CurFrame.Scale, T);
			DirectX::XMVECTOR vT = DirectX::XMVectorLerp(BlendFrame.Trans, CurFrame.Trans, T);
			DirectX::XMVECTOR vQ= DirectX::XMQuaternionSlerp(BlendFrame.Quat, CurFrame.Quat, T);

			m_vecCurAnimMat[i] = DirectX::XMMatrixAffineTransformation(vS, Vec4::Zero, vQ, vT);
			m_vecCurSkinningmMat[i] = m_RenderingFbx->m_AllBoneVec[i]->OffsetMat * m_vecCurAnimMat[i];
		}
	});

	m_fAccBlendingTime += _DeltaTime;
}
void CSkeletalFbxRenderer::PostUpdate(const float& _DeltaTime)
{
	AnimSocketUpdate(_DeltaTime);
}
void CSkeletalFbxRenderer::AnimSocketUpdate(const float& _DeltaTime)
{
	SubTransUpdate();
	size_t MaxSock = m_vecAnimSocket.size();

	for (size_t i = 0; i < MaxSock; i++)
	{
		Mat SocketMat =   m_vecCurAnimMat[m_vecAnimSocket[i].BoneIdx] * GetSubTransCRWorld();
		m_vecAnimSocket[i].ConnectTrans->SetWolrdMat(SocketMat);
		//m_vecAnimSocket[i].ConnectTrans->SetPos(SocketMat.Row[3].vec3);
	}
}

void CSkeletalFbxRenderer::RenderUpdate(const SPTR<CCamera> _Cam)
{
	CRenderer::RenderUpdate(_Cam);

	if (nullptr == m_BoneTexture)
	{
#ifdef _DEBUG
		assert(nullptr);
#endif
		return;
	}

	m_BoneTexture->Update(ANIMTEXSLOT, SHADER_VERTEX);

}

void CSkeletalFbxRenderer::RenderRelease()
{
	if (nullptr == m_BoneTexture)
	{
#ifdef _DEBUG
		assert(nullptr);
#endif
		return;
	}

	m_BoneTexture->PopShaderResource(ANIMTEXSLOT, SHADER_VERTEX);
}


void CSkeletalFbxRenderer::AddAnimSocket(const char* _BoneKey, SPTR<CTransForm> _Trans)
{
	SPTR<CBone> SocketBone = m_RenderingFbx->FindBone(_BoneKey);

	if (nullptr == SocketBone || nullptr == _Trans)
	{
#ifdef _DEBUG
		assert(nullptr);
#endif
		return;
	}

	AnimSocket Socket = AnimSocket{ SocketBone->Index,_Trans,_BoneKey };
	m_vecAnimSocket.push_back(Socket);
}
const bool CSkeletalFbxRenderer::CheckAnimationEnd()
{
	if (nullptr == m_CurBoneAnim)
		return false;

	return m_CurBoneAnim->m_bAnimEnd;
}


