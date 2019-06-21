#include "ComShader.h"



CComShader::CComShader()
	:CShader(SHADER_TYPE::SHADER_COMPUTE), m_pComShader(nullptr)
{
}


CComShader::~CComShader()
{
	SAFE_RELEASE(m_pComShader)
}

const bool CComShader::Load(const LOAD_DESC& _Desc)
{
	m_VH = _Desc.iVH;
	m_VL = _Desc.iVL;

	UINT iFlag = 0;
#ifdef _DEBUG
	iFlag = D3D10_SHADER_DEBUG;
#endif
	//iFlag = D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;

	char szBuffer[256] = {};
	wsprintfA(szBuffer, "cs_%d_%d", m_VH, m_VL);

	if (S_OK != D3DCompileFromFile(GetPath().c_str(), nullptr
		, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _Desc.FuncName.c_str(), szBuffer, iFlag, 0, &m_pBlob, &m_pErrBlob))
	{
		std::string ErrorText = (char*)m_pErrBlob->GetBufferPointer();
		TASSERT(true);
		return FALSE;
	}

	if (S_OK != DXDEVICE->CreateComputeShader(
		m_pBlob->GetBufferPointer(),
		m_pBlob->GetBufferSize(),
		nullptr, &m_pComShader))
	{
		TASSERT(true);
		return false;
	}

	return true;

}
void CComShader::Update()
{
	DXCONTEXT->CSSetShader(m_pComShader, 0, 0);
}
