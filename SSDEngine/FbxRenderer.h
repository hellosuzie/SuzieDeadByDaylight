#pragma once
#include "Base3DRenderer.h"
#include "Fbx.h"



typedef class CFbxRenderer : public CBase3DRenderer
{
protected:
	SPTR<CFbx>										m_RenderingFbx;

public:
	SPTR<CFbx> GetRenderingFbx()
	{
		return m_RenderingFbx;
	}

public:
	virtual void SetFbx(const wchar_t* _FbxKey);
	void Render(SPTR<class CCamera> _Cam) override;
	void InstancingRender(SPTR<class CCamera> _Cam);
	void Clear();

public:
	void SetAllFbxMesh(const wchar_t* _MatKey = L"BaseFBXMaterial");
	void DrawOn(const UINT& _Idx);
	void DrawOff(const UINT& _Idx);

	template<typename ...Rest>
	void SettingFbxMesh(Rest ..._Arg)
	{
		if (nullptr == m_RenderingFbx)
		{
#ifdef _DEBUG
			assert(nullptr);
#endif
			return;
		}

		SetFbxMesh(_Arg...);
	}

	// 와이어 프레임용 FBXMesh 제작
	template<typename ...Rest>
	void SettingWireFbxMesh(Rest ..._Arg)
	{
		if (nullptr == m_RenderingFbx)
		{
#ifdef _DEBUG
			assert(nullptr);
#endif
			return;
		}

		SetWireFbxMesh(_Arg...);
	}


private:
	template<typename ...Rest>
	void SetFbxMesh(const int& _MeshIdx, Rest ..._Arg)
	{
		SPTR<CFBXMesh> Mesh = m_RenderingFbx->GetFBXMesh(_MeshIdx, L"BaseFBXMaterial");

		if (nullptr == Mesh)
		{
#ifdef _DEBUG
			assert(nullptr);
			return;
#endif
		}
		else
			m_vecMesh.push_back(Mesh);

		SetFbxMesh(_Arg...);
	}


	void SetFbxMesh(const int& _MeshIdx)
	{
		SPTR<CFBXMesh> Mesh = m_RenderingFbx->GetFBXMesh(_MeshIdx, L"BaseFBXMaterial");

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

	template<typename ...Rest>
	void SetWireFbxMesh(const int& _MeshIdx, Rest ..._Arg)
	{
		SPTR<CFBXMesh> Mesh = m_RenderingFbx->GetFBXMesh(_MeshIdx, L"WireFBXMaterial");

		if (nullptr == Mesh)
		{
#ifdef _DEBUG
			assert(nullptr);
			return;
#endif
		}
		else
			m_vecMesh.push_back(Mesh);

		SetWireFbxMesh(_Arg...);
	}


	void SetWireFbxMesh(const int& _MeshIdx)
	{
		SPTR<CFBXMesh> Mesh = m_RenderingFbx->GetFBXMesh(_MeshIdx, L"WireFBXMaterial");

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
	virtual void PushFbxMesh(const int& _MeshIdx)
	{
		SPTR<CFBXMesh> Mesh = m_RenderingFbx->GetFBXMesh(_MeshIdx, L"BaseFBXMaterial");

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

	const std::vector<SPTR<class CMesh>>& GetAllMeshVec()
	{
		return m_vecMesh;
	}

	SPTR<CFBXMesh> GetFbxMesh(const UINT& _Idx)
	{
		return (SPTR<CFBXMesh>)m_vecMesh[_Idx];
	}

public:
	CFbxRenderer();
	~CFbxRenderer();
}CStaticFbxRenderer;

