#include "DXBuffer.h"
#include "DXMacro.h"
#include "Device.h"
#include "SSDThread.h"


CDXBuffer::CDXBuffer()
	:m_pBuffer(nullptr), m_bDraw(true)
{
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;
}


CDXBuffer::~CDXBuffer()
{
	SAFE_RELEASE(m_pBuffer)
}



ID3D11Buffer* CDXBuffer::CreateBuffer(void* _pInitData /*= nullptr*/)
{
	if (nullptr == _pInitData) 
		DXDEVICE->CreateBuffer(&m_BufferDesc, 0, &m_pBuffer);
	else 
	{
		D3D11_SUBRESOURCE_DATA BuffSub = D3D11_SUBRESOURCE_DATA();
		BuffSub.pSysMem = _pInitData;
		DXDEVICE->CreateBuffer(&m_BufferDesc, &BuffSub , &m_pBuffer);
	}
	
	return m_pBuffer;
}
void CDXBuffer::MappingBuffer(void* _pData)
{
	CCriSectionObject<CDevice> Lock;

	D3D11_MAPPED_SUBRESOURCE MappedSub;
	CDevice::GetContext()->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSub);
	memcpy_s(MappedSub.pData, m_BufferDesc.ByteWidth, _pData, m_BufferDesc.ByteWidth);
	CDevice::GetContext()->Unmap(m_pBuffer, 0);

}