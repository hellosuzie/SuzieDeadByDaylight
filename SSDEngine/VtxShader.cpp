#include "VtxShader.h"
#include"Device.h"
#include"ResourceMgr.h"


////////////////////////////////////////////			Input Layout			////////////////////////////////////////////
void CVtxShader::CInputLayout::PushInputDesc(const char* _SemanticName, const UINT& _Index
	, const DXGI_FORMAT& _Fmt, const UINT& _InputSlot
	, const UINT& _IDSR, D3D11_INPUT_CLASSIFICATION _eInput
/*= D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA*/)
{
	D3D11_INPUT_ELEMENT_DESC Desc{};

	Desc.SemanticName = _SemanticName;
	Desc.SemanticIndex = _Index;
	Desc.Format = _Fmt;
	Desc.InputSlot = _InputSlot;
	Desc.InstanceDataStepRate = _IDSR;
	Desc.InputSlotClass = _eInput;
	Desc.AlignedByteOffset = m_Offset[_InputSlot];

	UINT Size = SizeofFmt(Desc.Format);
	if (0 == Size)
	{
		TASSERT(true);
		return;
	}

	m_Offset[_InputSlot] += Size;
	m_vecInputDesc.push_back(Desc);
}


void CVtxShader::CInputLayout::CreateInputLayout()
{
	if (0 == m_vecInputDesc.size())
		return;

	if (S_OK != DXDEVICE->CreateInputLayout(&m_vecInputDesc[0], (UINT)m_vecInputDesc.size()
		, m_pParentShader->GetBIob()->GetBufferPointer()
		, m_pParentShader->GetBIob()->GetBufferSize()
		, &m_pInputLayout))
	{
		TASSERT(true);
		return;
	}
}

void CVtxShader::CInputLayout::Update()
{
	DXCONTEXT->IASetInputLayout(m_pInputLayout);
}




void CVtxShader::PushInputDesc(const char* _SemanticName, const UINT& _Index
	, const DXGI_FORMAT& _Fmt, const UINT& _InputSlot
	, const UINT& _IDSR, D3D11_INPUT_CLASSIFICATION _eInput /*= D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA*/)
{
	if (nullptr == m_pInputLayout)
		m_pInputLayout = new CInputLayout(this);

	m_pInputLayout->PushInputDesc(_SemanticName, _Index, _Fmt, _InputSlot, _IDSR, _eInput);
}
void CVtxShader::PushEndInputDesc(const char* _SemanticName, const UINT& _Index
	, const DXGI_FORMAT& _Fmt, const UINT& _InputSlot
	, const UINT& _IDSR, D3D11_INPUT_CLASSIFICATION _eInput /*= D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA*/)
{
	if (nullptr == m_pInputLayout)
		m_pInputLayout = new CInputLayout(this);

	m_pInputLayout->PushInputDesc(_SemanticName, _Index, _Fmt, _InputSlot, _IDSR, _eInput);
	m_pInputLayout->CreateInputLayout();
}




CVtxShader::CVtxShader()
	:CShader(SHADER_TYPE::SHADER_VERTEX)
	, m_pVtxShader(nullptr), m_pInputLayout(nullptr)
{
}


CVtxShader::~CVtxShader()
{
	SAFE_RELEASE(m_pVtxShader)
	m_mapConstBuffer.clear();

}
const bool CVtxShader::Load(const LOAD_DESC& _Desc)
{
	m_VH = _Desc.iVH;
	m_VL = _Desc.iVL;

	UINT iFlag = 0;
#ifdef _DEBUG
	iFlag = D3D10_SHADER_DEBUG;
#endif

	//iFlag = D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;

	// 버텍스 쉐이더(HLSL) 컴파일 버전
	char szBuffer[256] = {};
	wsprintfA(szBuffer, "vs_%d_%d", m_VH, m_VL);

	if (S_OK != D3DCompileFromFile(GetPath().c_str(), nullptr
		, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _Desc.FuncName.c_str(), szBuffer, iFlag, 0, &m_pBlob, &m_pErrBlob))
	{
		std::string ErrorText = (char*)m_pErrBlob->GetBufferPointer();
		TASSERT(true);
		return FALSE;
	}  

	if (S_OK != DXDEVICE->CreateVertexShader(
		m_pBlob->GetBufferPointer(),
		m_pBlob->GetBufferSize(),
		nullptr, &m_pVtxShader))
	{
		TASSERT(true);
		return false;
	}

	return true;
}

void CVtxShader::Update()
{
	if (nullptr != m_pInputLayout)
		m_pInputLayout->Update();

	DXCONTEXT->VSSetShader(m_pVtxShader, 0, 0);
}

