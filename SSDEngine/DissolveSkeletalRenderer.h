#pragma once
#include "SkeletalFbxRenderer.h"
#include <functional>

#define DISSOVEOFF 0
#define DISSOVEACTIVE 1
#define NOISESLOT 8
#define BURNSLOT 9



class CDissolveSkeletalRenderer : public CSkeletalFbxRenderer
{
private:
	CBUFFER::DissolveBuffer					m_DissolveBuffer;
	float									m_fAccTime;
	Vec2									m_DissolveTime;
	

private:
	std::list<std::function<void(void)>>	m_listDissolveCallBack;
	std::list<std::function<void(void)>>	m_listReDissolveCallBack;

private:
	void(CDissolveSkeletalRenderer::*m_pDissolveUpdate)(const float& _DeltaTime);

public:
	template<typename T>
	void SetDissolveCallBack(T* _Obj, void(T::*_pFunc)(void))
	{
		if (nullptr == _Obj)
			return;

		m_listDissolveCallBack.push_back(std::bind(_pFunc, _Obj));
	}

	template<typename T>
	void SetReverseDissolveCallBack(T* _Obj, void(T::*_pFunc)(void))
	{
		if (nullptr == _Obj)
			return;

		m_listReDissolveCallBack.push_back(std::bind(_pFunc, _Obj));
	}

public:
	void PushFbxMesh(const int& _MeshIdx) override;
	void SetAllFbxMesh();

public:
	void Update(const float& _DeltaTime)override;
	void SetFbx(const wchar_t* _FbxKey) override;
	void RenderUpdate(SPTR<class CCamera> _Camera)override;

private:
	void DissolveUpdate(const float& _DeltaTime);
	void ReverseDissolveUpdate(const float& _DeltaTime);
	void DessolveBufferUpdate();

public:
	void SetDissolve(const float& _Time);
	void SetDissolve(const float& _WaitTime, const float& _Time);
	void SetReverseDissolve(const float& _Time);
	void SetReverseDissolve(const float& _WaitTime, const float& _Time);

private:
	void CallDissolveEvent();
	void CallReverseDissolveEvent();

public:
	CDissolveSkeletalRenderer();
	~CDissolveSkeletalRenderer();
};

