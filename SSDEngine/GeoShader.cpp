#include "GeoShader.h"



CGeoShader::CGeoShader()
	:CShader(SHADER_TYPE::SHADER_GEOMETRY)
	, m_pGeoShader(nullptr)
{

}


CGeoShader::~CGeoShader()
{
	SAFE_RELEASE(m_pGeoShader);
}

void CGeoShader::Update()
{
	DXCONTEXT->GSSetShader(m_pGeoShader, 0, 0);
}

const bool CGeoShader::CreateGeoShaderWithStreamOutput(D3D11_SO_DECLARATION_ENTRY* pDecl, const UINT& __SODeclCount
	, const UINT* _StrideBuff, const UINT& _StrideCount)
{
	HRESULT hr = DXDEVICE->CreateGeometryShaderWithStreamOutput(
		m_pBlob->GetBufferPointer(),
		m_pBlob->GetBufferSize(),
		pDecl, __SODeclCount, _StrideBuff, _StrideCount, D3D11_SO_NO_RASTERIZED_STREAM, NULL, &m_pGeoShader);

	if (S_OK != hr)
	{
		TASSERT(true);
		return false;
	}

	return true;
}
const bool CGeoShader::CreateGeoShaderWithStreamOutput(D3D11_SO_DECLARATION_ENTRY* pDecl, const UINT& __SODeclCount
	, const UINT* _StrideBuff, const UINT& _StrideCount, const UINT& _RasterizeStream) 
{
	HRESULT hr = DXDEVICE->CreateGeometryShaderWithStreamOutput(
		m_pBlob->GetBufferPointer(),
		m_pBlob->GetBufferSize(),
		pDecl, __SODeclCount, _StrideBuff, _StrideCount, _RasterizeStream, NULL, &m_pGeoShader);

	if (S_OK != hr)
	{
		TASSERT(true);
		return false;
	}

	return true;
}
const bool CGeoShader::Load(const LOAD_DESC& _Desc) 
{
	m_VH = _Desc.iVH;
	m_VL = _Desc.iVL;

	UINT iFlag = 0;
#ifdef _DEBUG
	iFlag = D3D10_SHADER_DEBUG;
#endif
	//iFlag = D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;

	// ÇÈ¼¿ ½¦ÀÌ´õ(HLSL) ÄÄÆÄÀÏ ¹öÀü
	char szBuffer[256] = {};
	wsprintfA(szBuffer, "gs_%d_%d", m_VH, m_VL);

	if (S_OK != D3DCompileFromFile(GetPath().c_str(), nullptr
		, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _Desc.FuncName.c_str(), szBuffer, iFlag, 0, &m_pBlob, &m_pErrBlob))
	{
		std::string ErrorText = (char*)m_pErrBlob->GetBufferPointer();
		TASSERT(true);
		return FALSE;
	}

	if (false == _Desc.bCreateShader)
		return true;

	if (S_OK != DXDEVICE->CreateGeometryShader(
		m_pBlob->GetBufferPointer(),
		m_pBlob->GetBufferSize(),
		nullptr, &m_pGeoShader))
	{
		TASSERT(true);
		return false;
	}

	return true;
}
