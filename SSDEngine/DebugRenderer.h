#pragma once
#include "Base3DRenderer.h"

enum DEBUGFI_TYPE
{
	DF_SPHERE = 1,
	DF_CUBE = 2,
	DF_CONE = 3,
	DF_FRUSTUM = 4,
	MAX_DEBUGFI_TYPE
};

typedef struct _tagDEBUG_RENDATA 
{
	RENDER_LAYER			RenLayer;
	DEBUGFI_TYPE			FiOption;

}DEBUG_RENDATA;


class CDebugRenderer : public CBase3DRenderer
{
private:
	bool						m_bChangeCB;
	DEBUGFI_TYPE				m_DebugFiType;
	CBUFFER::DebugBuffer		m_DebugBuf;

public:
	const bool Init(const DEBUG_RENDATA& _InitData);
	void RenderUpdate(SPTR<class CCamera> _Cam);

	bool ChangeDebugType(DEBUGFI_TYPE _Type);

public:
	void SetBoaderSize(const float& _Size) 
	{
		m_bChangeCB = true;
		m_DebugBuf.BoarderSize = _Size;
	}

	void SetFrustumMat(const Mat& _FrustumMat)
	{
		m_bChangeCB = true;
		m_DebugBuf.FrustumMat = _FrustumMat.GetTransposeMat();
	}

public:
	CDebugRenderer();
	~CDebugRenderer();
};

