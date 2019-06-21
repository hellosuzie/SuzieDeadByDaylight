#pragma once
#include "SubTrans.h"
#include "DXHeader.h"
#include "DXEnum.h"
#include "DXStruct.h"
#include <vector>


#define BASESLOT	13

struct RENDATA 
{
	RENDER_LAYER			RenderLayer;
};

class CCamera;
class CRenderer : public CSubTrans
{
	friend class CActor;
	friend class CSceneRenderMgr;

protected:
	RENDER_LAYER							m_RenderLayer;
	Vec3									m_RenSize;
	Vec3									m_RenPivot;
	Vec4									m_RenColorFactor;

protected:
	Vec4									m_StartColorFactor;
	Vec4									m_EndColorFactor;
	float									m_fFactorTime;
	float									m_fAccFactorTime;
	bool									m_bAutoFactor;

private:
	RENDER_MODE								m_RenderMode;

protected:
	CBUFFER::RenBaseBuffer					m_BaseBuffer;
	std::vector<SPTR<class CMesh>>			m_vecMesh;
	std::vector<SPTR<class CMaterial>>		m_vecMaterial;

//protected:
//	bool									m_bDeferrd;
//
//public:
//	void DeferredOn() {m_bDeferrd = true;}
//	void DeferredOff() { m_bDeferrd = false; }
//	const bool IsDeferred() { return m_bDeferrd; }

public:
	void PushMesh(const wchar_t* _MeshKey, const UINT& _Idx = 0 );
	void PushMesh(SPTR<CMesh> _Mesh, const UINT& _Idx = 0);
	void PushMaterial(const wchar_t* _MatKey, const UINT& _Idx = 0 );


public:
	void SetRenderMode(RENDER_MODE _Mode);
	RENDER_MODE GetRenderMode()
	{
		return m_RenderMode;
	}

	SPTR<CMesh> GetMesh(const UINT& _Idx);
	SPTR<CMaterial> GetMaterial(const UINT& _Idx);

protected:
	virtual void Render(SPTR<CCamera> _Cam);
	virtual void RenderUpdate(SPTR<CCamera> _Cam);
	virtual void RenderRelease() {}

	//모드에 따라 호출하는 렌더함수가 달라진다
	void(CRenderer::*RenderPtr)(SPTR<CCamera> _Cam);

	void NormalModeRender(SPTR<CCamera> _Cam);

	void BaseCBUpdate(SPTR<CCamera> _Cam, const Mat& _World);
	void SetBaseBuffer(const SHADER_TYPE& _Type);
	void SetBaseBuffer(const UINT& _ShaderFlag);

protected:
	virtual const bool Init(const RENDATA& _InitData);
	void PostUpdate(const float& _DeltaTime)override;

public:
	void SetRenPivot(const Vec3& _Pivot);
	void SetRenSize(const Vec3& _Size);
	void SetColorFactor(const Vec4& _Factor)
	{
		m_RenColorFactor = _Factor;
	}
	void SetColorFactor(const Vec3& _Color)
	{
		m_RenColorFactor.vec3 = _Color;
	}
	void SetAlpha(const float& _Alpha);

	void SetAutoColorFactor(const Vec4& _StartFactor, const Vec4& _EndFactor,const float& _Time) 
	{
		m_bAutoFactor = true;
		m_StartColorFactor = _StartFactor;
		m_EndColorFactor = _EndFactor;
		m_fFactorTime = _Time;
		m_fAccFactorTime = 0.f;
	}

public:
	const Vec3 GetRenSize();
	const Vec3 GetRenPivot();

public:
	const RENDER_LAYER GetRenderLayer() { return m_RenderLayer; }
	const void SetRenderLayer(const RENDER_LAYER& _Layer) 
	{ 
		m_RenderLayer = _Layer;
	}

	const Vec4 GetColorFactor() { return m_RenColorFactor; }

public:
	CRenderer();
	~CRenderer();
};

