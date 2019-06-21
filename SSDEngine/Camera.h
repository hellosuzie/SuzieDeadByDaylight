#pragma once
#include "BaseComponent.h"
#include "DXHeader.h"
#include "DXEnum.h"
#include "FilterEffect.h"
#include "UIBase.h"

#include <map>
#include <list>
#define MAXTRACKRADIUS 50.f 

struct CAMDATA 
{
	CAMERA_GROUP			CamGroup;
	float					Fov;
	float					Near;
	float					Far;

};

class CCamera final : public CBaseComponent
{
	friend class ASceneCamera;
	friend class CSceneRenderMgr;
	friend class CActor;

	enum CAMERAMODE
	{
		ORTHO_CAMERA,
		PERSPECTIVE_CAMERA
	}m_eProjMode;

	enum SCREENVIEW_MODE
	{
		SCREENVIEW_WINDOW,
		SCREENVIEW_FREE
	}m_eScreenViewMode;

public:
	enum VIEW_AT_TYPE 
	{
		VIEW_AT_Z,
		VIEW_AT_INVZ,
		VIEW_AT_Y,
		MAX_VIEW_AT_TYPE,
	}m_eViewAtType;

public:
	void ChangeViewAxisZ() { m_eViewAtType = VIEW_AT_Z; }
	void ChangeViewAxisInvZ() { m_eViewAtType = VIEW_AT_INVZ; }
	void ChangeViewAxisY() { m_eViewAtType = VIEW_AT_Y; }
	const VIEW_AT_TYPE GetCameraViewType() { return m_eViewAtType; }
public:
	static const bool IsMultiComponentCheck(CActor* _pActor);

private:
	CAMERA_GROUP							m_CameraGroup;
	std::vector<RENDER_LAYER>				m_vecViewRenderLayer;

	bool									m_bWheelZoom;
	float									m_CurZoomMag;

	// 가시각 (디그리)
	float									m_fFov;
	float									m_fFar;
	float									m_fNear;

	Vec2									m_ScreenViewSize;
	Mat										m_matView;
	Mat										m_matProj;
	Mat										m_FrustumProj;

private:
	SPTR<class MultiRenderTarget>			m_CameraViewTarget;
	SPTR<class CMesh>						m_CameraMesh;
	SPTR<class CMaterial>					m_CameraMaterial;
	D3D11_VIEWPORT							m_CamViewPort;

	SPTR<class CFrustum>					m_CamFrustum;

private:
	bool									m_bBluring;
	float									m_fAccBlurTime;
	float									m_fBlurTime;
	float									m_fBlurPow;
	Mat										m_BlurViewMat;
	Vec3									m_BlurViewPos;

public:
	bool CameraFrustumCheck(SPTR<class CRenderer> _Ren);
	bool CameraLengthCheck(SPTR<class CRenderer> _Ren);
	SPTR<CFrustum>	GetCameraFrustum();
public:
	void SetCamViewTarget();
	void ClearCamTarget();
	void CamViewTargetRender();
	void SetCamViewPort(Vec2 _StartPos, IVec2 _Size);

public:
	template<typename ...Rest>
	void SettingViewLayer(Rest ..._Arg)
	{
		m_vecViewRenderLayer.reserve(sizeof...(_Arg));
		PushViewLayer(_Arg...);
	}

	template<typename ...Rest>
	void PushViewLayer(RENDER_LAYER _Layer, Rest ..._Arg)
	{
		for (auto Value : m_vecViewRenderLayer)
		{
			if (Value == _Layer)
				PushViewLayer(_Arg...);
		}

		m_vecViewRenderLayer.push_back(_Layer);
		PushViewLayer(_Arg...);
	}

	void PushViewLayer(RENDER_LAYER _Layer);


public:
	void SetPerspectiveMode() { m_eProjMode = PERSPECTIVE_CAMERA; }
	void SetOrthogonalMode() { m_eProjMode = ORTHO_CAMERA; }

public:
	void SetScreenView(const Vec2& _ScreenSize)
	{
		m_eScreenViewMode = SCREENVIEW_FREE;
		m_ScreenViewSize = _ScreenSize;
	}
	void SetWindowScreenMode()
	{
		m_eScreenViewMode = SCREENVIEW_WINDOW;
	}
	void SetCamData(const CAMDATA& _Data);


public:
	std::vector<RENDER_LAYER>* GetLayerVec() { return &m_vecViewRenderLayer; }


public:
	const bool Init(const CAMDATA& _InitData);
	void PreUpdate(const float& _DeltaTime) override;
	void Update(const float& _DeltaTime) override;
	void PostUpdate(const float& _DeltaTime) override;
	void LastUpdate(const float& _DeltaTime) override;
	void Release() override;

public:
	void SetPos(const Vec3& _Pos);
	void SetPos(const Vec2& _Pos);
	void SetWheelZoom(const bool& _Wheel) { m_bWheelZoom = _Wheel; }

public:
	const Vec2 GetOrthoViewSize();
	const Mat GetViewMatrix() { return m_matView; }
	const Mat GetProjMatrix() { return m_matProj; }
	const Mat GetFrustumProjMatrix() { return m_FrustumProj; }
	const Mat GetVPMatrix() { return m_matView * m_matProj; }


	const CAMERA_GROUP GetCamGroup() { return m_CameraGroup; }
	const bool IsCameraMove();
	const Vec3 GetPos();
	const Vec3 GetRot();
	CActor* GetCamActor() { return ACTOR; }
	const float GetNear() { return m_fNear; }
	const float GetFar() { return m_fFar; }
	const Vec3 GetMousePos();
	const float GetZoomMag() { return m_CurZoomMag; }
	const float GetViewMinDepth();
	const D3D11_VIEWPORT GetCameraViewPort()
	{
		return m_CamViewPort;
	}
	const SPTR<class RenderTarget> GetCamTarget(const unsigned int& _TargetIdx);
	const SPTR<class MultiRenderTarget> GetCamMultiTarget()
	{
		return m_CameraViewTarget;
	}


public:
	void SetCameraBluring(const float& _BlurTime, SPTR<CCamera> _BlurCamera,const float& _Pow = 2.f);
	void BluringCamera(const float& _DeltaTime);



private:
	std::list<SPTR<class CCameraScript>>					m_listCameraScript;
	std::list<Vec3>											m_listEyePivot;

private:
	const bool PushCameraScript(CBaseComponent* _CamScript);
	void ClearPivotList();
public:
	void SetCameraPivot(const Vec3& _Pivot);
	void SetCameraZoom(const float& _ZoomMag);

public:
	template<typename T>
	SPTR<T> FindCameraScript()
	{
		if (true == m_listCameraScript.empty())
			return nullptr;

		for (SPTR<CCameraScript> Script : m_listCameraScript)
		{
			if (true == Script->EqualType<T>())
				return Script;
		}

		return nullptr;
	}

	template<typename T>
	SPTR<T> FindCameraScript(const wchar_t* _Tag)
	{
		if (true == m_listCameraScript.empty())
			return nullptr;

		for (SPTR<CCameraScript> Script : m_listCameraScript)
		{
			if (true == Script->EqualType<T>() && true == Script->ExistTag(_Tag))
				return Script;
		}

		return nullptr;
	}

	////////////////////////필터
private:
	SPTR<MultiRenderTarget>									m_LastTarget;

	std::list<SPTR<CFilterEffect>>							m_FilterList;
	std::list<SPTR<CFilterEffect>>::iterator				m_FilterStartIter;
	std::list<SPTR<CFilterEffect>>::iterator				m_FilterEndIter;

public:
	void FilterRender();

	template<typename T>
	SPTR<T> AddFilter()
	{
		T* NewFilter = new T();
		m_FilterList.push_back(NewFilter);
		NewFilter->TypeInit();
		NewFilter->Init();
		NewFilter->SetFilterBaseComponent(this);
		return NewFilter;
	}

	template<typename T, typename Value>
	SPTR<T> AddFilter(const Value& _Value)
	{
		T* NewFilter = new T();
		m_FilterList.push_back(NewFilter);
		NewFilter->TypeInit();
		NewFilter->Init(_Value);
		NewFilter->SetFilterBaseComponent(this);
		return NewFilter;
	}

	template<typename FINDCOM>
	SPTR<FINDCOM> FindFilter()
	{
		std::list<SPTR<CFilterEffect>>::iterator Start = m_FilterList.begin();
		std::list<SPTR<CFilterEffect>>::iterator End = m_FilterList.end();

		for (; Start != End; ++Start)
		{
			if (true == (*Start)->EqualType<FINDCOM>())
				return *Start;
		}
		return nullptr;
	}

	template<typename FINDCOM>
	SPTR<FINDCOM> FindLastFilter()
	{
		std::list<SPTR<CFilterEffect>>::reverse_iterator Start = m_FilterList.rbegin();
		std::list<SPTR<CFilterEffect>>::reverse_iterator End = m_FilterList.rend();

		for (; Start != End; ++Start)
		{
			if (true == (*Start)->EqualType<FINDCOM>())
				return *Start;
		}
		return nullptr;
	}

	template<typename FINDCOM>
	void FindAllFilter(std::list<SPTR<FINDCOM>>* _ComList)
	{
		if (nullptr == _ComList)
			return;

		std::list<SPTR<CFilterEffect>>::iterator Start = m_FilterList.begin();
		std::list<SPTR<CFilterEffect>>::iterator End = m_FilterList.end();

		for (; Start != End; ++Start)
		{
			if (true == (*Start)->EqualType<FINDCOM>())
				_ComList->push_back(*Start);
		}
		return;
	}

public:
	const std::list<SPTR<CFilterEffect>>& GetCamFilterList()
	{
		return m_FilterList;
	}
	void ResetCamFilter();

	////////////////////////UI
private:
	std::map<UI_LAYER, std::list<SPTR<CUIBase>>>						m_mapUIBase;
	std::map<UI_LAYER, std::list<SPTR<CUIBase>>>::iterator				m_UImapiter;

public:
	bool PushUICom(CUIBase* _UICom, UI_LAYER _Layer);
	void UIRender();

public:
	void SetUIPosition();

public:
	CCamera();
	CCamera(CActor* _Actor);
	~CCamera();
};

