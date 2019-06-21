#pragma once
#include "Shader.h"

class CGeoShader : public CShader
{
private:
	ID3D11GeometryShader*						m_pGeoShader;

public:
	void Update() override;

public:
	ID3D11GeometryShader* GetShader() { return m_pGeoShader; }

public:
	const bool CreateGeoShaderWithStreamOutput(D3D11_SO_DECLARATION_ENTRY* pDecl, const UINT& __SODeclCount
		, const UINT* _StrideBuff, const UINT& _StrideCount);
	const bool CreateGeoShaderWithStreamOutput(D3D11_SO_DECLARATION_ENTRY* pDecl, const UINT& __SODeclCount
		, const UINT* _StrideBuff, const UINT& _StrideCount,const UINT& _RasterizeStream);

public:
	const bool Load(const LOAD_DESC& _Desc) override;

public:
	CGeoShader();
	~CGeoShader();
};
