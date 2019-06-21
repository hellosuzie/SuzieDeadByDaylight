#include "InstancingBuffer.h"
#include "Device.h"

CInstancingBuffer::CInstancingBuffer() : m_iMaxCount(10)
{
}

CInstancingBuffer::~CInstancingBuffer()
{
	SAFE_RELEASE(m_pInstancingBuffer);
}

void CInstancingBuffer::Init()
{
	D3D11_BUFFER_DESC tDesc = {};
	tDesc.ByteWidth = sizeof(tInstancingData) * m_iMaxCount;
	tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tDesc.Usage = D3D11_USAGE_DYNAMIC;
	tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tDesc.MiscFlags = 0;
	tDesc.StructureByteStride = 0;

	HRESULT Res = DXDEVICE->CreateBuffer(&tDesc, NULL, &m_pInstancingBuffer);

	if (S_OK != Res)
	{
		TASSERT(true);
		SAFE_RELEASE(m_pInstancingBuffer);
	}
}

void CInstancingBuffer::SetData()
{
	if (m_vecData.size() > m_iMaxCount)
	{
		Resize((UINT)m_vecData.size());
	}

	D3D11_MAPPED_SUBRESOURCE tMap = {};

	DXCONTEXT->Map(m_pInstancingBuffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &tMap);
	memcpy(tMap.pData, &m_vecData[0], sizeof(tInstancingData) * m_vecData.size());
	DXCONTEXT->Unmap(m_pInstancingBuffer, 0);
}

void CInstancingBuffer::Resize(UINT _iCount)
{
	SAFE_RELEASE(m_pInstancingBuffer);

	m_iMaxCount = _iCount;

	D3D11_BUFFER_DESC tDesc = {};
	tDesc.ByteWidth = sizeof(tInstancingData) * m_iMaxCount;
	tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tDesc.Usage = D3D11_USAGE_DYNAMIC;
	tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tDesc.MiscFlags = 0;
	tDesc.StructureByteStride = 0;

	if (S_OK != DXDEVICE->CreateBuffer(&tDesc, NULL, &m_pInstancingBuffer))
	{
		TASSERT(true);
		SAFE_RELEASE(m_pInstancingBuffer);
	}
}
