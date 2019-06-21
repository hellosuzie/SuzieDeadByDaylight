#include "ConstantBuffer.h"
#include "DXMacro.h"
#include "Device.h"
#include "SSDThread.h"

CConstantBuffer::CConstantBuffer()
	: m_MappedSub{}, m_pBufferType(nullptr), m_pOriginData(nullptr)
	, m_ShaderFlag(0), m_RegisterNum(0), m_bOriginSet(false)
{
}


CConstantBuffer::~CConstantBuffer()
{
	SAFE_DELETE(m_pOriginData);
}


void CConstantBuffer::Update(const void* _pData, const UINT& _Size)
{
	if (nullptr == _pData)
		return;

	CDevice::GetContext()->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &m_MappedSub);
	memcpy_s(m_MappedSub.pData, m_BufferDesc.ByteWidth, _pData, _Size);
	CDevice::GetContext()->Unmap(m_pBuffer, 0);
	m_bOriginSet = false;

}
void CConstantBuffer::Update()
{
	if (true == m_bOriginSet)
		return;

	if (nullptr != m_pOriginData) 
	{
		CDevice::GetContext()->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &m_MappedSub);
		memcpy_s(m_MappedSub.pData, m_BufferDesc.ByteWidth, m_pOriginData, m_BufferDesc.ByteWidth);
		CDevice::GetContext()->Unmap(m_pBuffer, 0);
		m_bOriginSet = true;
	}
}

void CConstantBuffer::SupplyShader(const UINT& _ShaderFlag, const UINT& _RegisterNum) 
{
	CCriSectionObject<CDevice>();

	if (0 != (_ShaderFlag & SHADER_VERTEX))
		DXCONTEXT->VSSetConstantBuffers(_RegisterNum, 1, &m_pBuffer);

	if (0 != (_ShaderFlag & SHADER_HULL))
		DXCONTEXT->HSSetConstantBuffers(_RegisterNum, 1, &m_pBuffer);

	if (0 != (_ShaderFlag & SHADER_DOMAIN))
		DXCONTEXT->DSSetConstantBuffers(_RegisterNum, 1, &m_pBuffer);

	if (0 != (_ShaderFlag & SHADER_GEOMETRY))
		DXCONTEXT->GSSetConstantBuffers(_RegisterNum, 1, &m_pBuffer);

	if (0 != (_ShaderFlag & SHADER_PIXEL))
		DXCONTEXT->PSSetConstantBuffers(_RegisterNum, 1, &m_pBuffer);



}
void CConstantBuffer::SupplyShader(const SHADER_TYPE& _ShaderFlag, const UINT& _RegisterNum) 
{
	CCriSectionObject<CDevice>();

	switch (_ShaderFlag)
	{
	case SHADER_VERTEX:
		DXCONTEXT->VSSetConstantBuffers(_RegisterNum, 1, &m_pBuffer);
		break;
	case SHADER_HULL:
		DXCONTEXT->HSSetConstantBuffers(_RegisterNum, 1, &m_pBuffer);
		break;
	case SHADER_DOMAIN:
		DXCONTEXT->DSSetConstantBuffers(_RegisterNum, 1, &m_pBuffer);
		break;
	case SHADER_GEOMETRY:
		DXCONTEXT->GSSetConstantBuffers(_RegisterNum, 1, &m_pBuffer);
		break;
	case SHADER_PIXEL:
		DXCONTEXT->PSSetConstantBuffers(_RegisterNum, 1, &m_pBuffer);
		break;
	case SHADER_ALL: 
	{
		DXCONTEXT->VSSetConstantBuffers(_RegisterNum, 1, &m_pBuffer);
		DXCONTEXT->HSSetConstantBuffers(_RegisterNum, 1, &m_pBuffer);
		DXCONTEXT->DSSetConstantBuffers(_RegisterNum, 1, &m_pBuffer);
		DXCONTEXT->GSSetConstantBuffers(_RegisterNum, 1, &m_pBuffer);
		DXCONTEXT->PSSetConstantBuffers(_RegisterNum, 1, &m_pBuffer);
	}
		break;
	default:
		return;
	}
}

void CConstantBuffer::SupplyShader()
{
	SupplyShader(m_ShaderFlag, m_RegisterNum);
}

const bool CConstantBuffer::Create(const CREATE_DESC& _Desc)
{
	m_ShaderFlag = _Desc.ShaderFlag;
	m_RegisterNum = _Desc.RegisterNum;

	m_BufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	m_BufferDesc.ByteWidth = _Desc.ByteWidth;
	m_BufferDesc.Usage = _Desc.Usage;
	m_BufferDesc.CPUAccessFlags = _Desc.CPUAccessFlags;

	if (nullptr == CreateBuffer()) 
	{
		TASSERT(true);
		return false;
	}

	//m_pOriginData = new char[m_BufferDesc.ByteWidth];
	//ZeroMemory(m_pOriginData, m_BufferDesc.ByteWidth);
	return true;
}