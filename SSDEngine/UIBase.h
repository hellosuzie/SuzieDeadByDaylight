#pragma once
#include "SubTrans.h"
#include "RectCollider.h"
#include "DXStruct.h"
#include "DXEnum.h"
#include "DXMath.h"
#include "SSDCore.h"

typedef struct _tagUIDATA
{
	UI_LAYER								Layer;
	class CCamera*							Camptr;
	UI_COLL_TYPE							Type;
	std::wstring							BaseTexName;

}UI_DATA;

class CUIBase : public CSubTrans
{
private:
	UI_DATA									m_UIData;

private:
	class CMesh*							m_UIMesh;
	class CMaterial*						m_UIMaterial;

	CBUFFER::RenBaseBuffer					m_BaseBuffer;
	Vec4									m_UIColorFactor;

	bool									m_IsTex;

private:
	Vec3									m_SizeRatio;
	Vec3									m_PosRatio;

private:
	Vec3									m_MinSize;
	Vec3									m_MaxSize;
	Vec3									m_StartPos;

	Vec3									m_MinRatio;
	Vec3									m_MaxRatio;
	Vec3									m_StartPosRatio;

	float									m_fCurGauge;

public:
	const bool Init(const UI_DATA& _Data);
	void PreUpdate(const float& _DeltaTime);
	void Update(const float& _DeltaTime);
	void PostUpdate(const float& _DeltaTime);
	void LastUpdate(const float& _DeltaTime);
	void Release() {}

public:
	void UIRenderUpdate();
	void UIRender();

public:
	Vec3 GetSubPivot()
	{
		return CSubTrans::GetSubData()->SubPivot;
	}

	void SetSubSize(const Vec3& _Size)
	{
		m_SizeRatio.x = _Size.x / Core::GetClientWidth();
		m_SizeRatio.y = _Size.y / Core::GetClientHeight();
		m_SizeRatio.z = _Size.z;
		
		CSubTrans::SetSubSize(_Size);
	}
	void SetSubPivot(const Vec3& _Pivot)
	{
		m_PosRatio.x = _Pivot.x / Core::GetClientWidth();
		m_PosRatio.y = _Pivot.y / Core::GetClientHeight();
		m_PosRatio.z = _Pivot.z;
		
		CSubTrans::SetSubPivot(_Pivot);
	}
	void SetUIColorFactor(Vec4 _Color)
	{
		m_UIColorFactor = _Color;
	}
	void SetUIAplha(const float& _Alpha)
	{
		m_UIColorFactor.a = _Alpha;
	}
	void SetUIColor(const Vec3& _Color)
	{
		m_UIColorFactor.vec3 = _Color;
	}
	void SetMaxGauge(const Vec3& _MaxSize) 
	{
		m_MaxRatio.x = _MaxSize.x / Core::GetClientWidth();
		m_MaxRatio.y = _MaxSize.y / Core::GetClientHeight();
		m_MaxRatio.z = _MaxSize.z;

		m_MaxSize = _MaxSize;
	}
	void SetMinGauge(const Vec3& _MinSize) 
	{
		m_MinRatio.x = _MinSize.x / Core::GetClientWidth();
		m_MinRatio.y = _MinSize.y / Core::GetClientHeight();
		m_MinRatio.z = _MinSize.z;

		m_MinSize = _MinSize;
	}
	void SetGaugeStartPos(const Vec3& _Pos) 
	{
		m_StartPosRatio.x = _Pos.x / Core::GetClientWidth();
		m_StartPosRatio.y = _Pos.y / Core::GetClientHeight();
		m_StartPosRatio.z = _Pos.z;

		m_StartPos = _Pos;
	}
	void SetUIGauge(const float& _fGauge) 
	{
		m_fCurGauge = DXMath::Clamp(_fGauge, 0.f, 1.f);

		m_MaxSize = Vec3(m_MaxRatio.x *Core::GetClientWidth(), m_MaxRatio.y *Core::GetClientHeight(), m_MaxRatio.z);
		m_MinSize = Vec3(m_MinRatio.x *Core::GetClientWidth(), m_MinRatio.y *Core::GetClientHeight(), m_MinRatio.z);
		m_StartPos = Vec3(m_StartPosRatio.x *Core::GetClientWidth(), m_StartPosRatio.y *Core::GetClientHeight(), m_StartPosRatio.z);
		
		SetSubSize(DXMath::Vec3Lerp(m_MinSize, m_MaxSize, m_fCurGauge));
		Vec3 GaugePos = (m_MaxSize - m_MinSize) * m_fCurGauge * 0.5f;
		SetSubPivot(m_StartPos + GaugePos);
	}

	
/////////////충돌
private:
	bool									m_bIsColl;
	bool									m_bColl;
	Vec3									m_CollPivot;
	Vec3									m_CollSize;

private:
	std::function<void()>					m_EnterCallBack;
	std::function<void()>					m_StayCallBack;
	std::function<void()>					m_ExitCallBack;

	//함수포인터 포문돌면서 호출해주는 함수
	void CallEnter();
	void CallStay();
	void CallExit();

public:
	template<typename T>
	void SetEnterCallBack(T* _Obj, void(T::*_pFunc)())
	{
		if (nullptr == _Obj)
			return;

		m_EnterCallBack = std::bind(_pFunc, _Obj);
	}

	template<typename T>
	void SetStayCallBack(T* _Obj, void(T::*_pFunc)())
	{
		if (nullptr == _Obj)
			return;

		m_StayCallBack = std::bind(_pFunc, _Obj);
	}

	template<typename T>
	void SetExitCallBack(T* _Obj, void(T::*_pFunc)())
	{
		if (nullptr == _Obj)
			return;

		m_ExitCallBack = std::bind(_pFunc, _Obj);
	}

private:
	//충돌체크해서 상태함수 호출해주는 함수
	void UICollision();
	//충돌 체크함수
	bool UICollisionCheck();

protected:
	virtual void CollisionEvent()
	{
		m_bColl = true;
	}
	virtual void CollisionEndEvent()
	{
		m_bColl = false;
	}

public:
	void SetCollSize(const Vec3& _Size)
	{
		m_CollSize = _Size;
	}
	void SetCollPivot(const Vec3& _Pivot)
	{
		m_CollPivot = _Pivot;
	}
	void SetUITexture(const wchar_t* _TexName);


public:
	void SyncronizationUI();


public:
	CUIBase();
	~CUIBase();
};


//디버그