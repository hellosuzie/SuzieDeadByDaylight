#include "PixShader.h"
#include"DXMacro.h"
#include"Device.h"

CPixShader::CPixShader()
	:CShader(SHADER_TYPE::SHADER_PIXEL)
	, m_pPixShader(nullptr)
{

}


CPixShader::~CPixShader()
{
	SAFE_RELEASE(m_pPixShader)
}
const bool CPixShader::Load(const LOAD_DESC& _Desc)
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
	wsprintfA(szBuffer, "ps_%d_%d", m_VH, m_VL);

	if (S_OK != D3DCompileFromFile(GetPath().c_str(), nullptr
		, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _Desc.FuncName.c_str(), szBuffer, iFlag, 0, &m_pBlob, &m_pErrBlob))
	{
		std::string ErrorText = (char*)m_pErrBlob->GetBufferPointer();
		TASSERT(true);
		return FALSE;
	}

	if (S_OK != DXDEVICE->CreatePixelShader(
		m_pBlob->GetBufferPointer(),
		m_pBlob->GetBufferSize(),
		nullptr, &m_pPixShader))
	{
		TASSERT(true);
		return false;
	}

	return true;

}
void CPixShader::Update()
{
	DXCONTEXT->PSSetShader(m_pPixShader, 0, 0);
}



