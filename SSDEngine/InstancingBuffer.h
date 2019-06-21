#pragma once
#include "ReferenceBase.h"
#include "DXHeader.h"
#include "DXMacro.h"
#include "Device.h"

struct tInstancingData
{
	Mat matWorld;
	Mat matWV;
	Mat matWVP;
};

class CInstancingBuffer
{
public:
	static CInstancingBuffer* Inst()
	{
		static CInstancingBuffer mgr;
		return &mgr;
	}

private:
	ID3D11Buffer*						m_pInstancingBuffer;
	UINT								m_iMaxCount;
	std::vector<tInstancingData>		m_vecData;
	

public:
	void Init();
	void SetData();
	void Clear()
	{
		m_vecData.clear();
	}

	void AddInstancingData(tInstancingData& _tData)
	{
		m_vecData.push_back(_tData);
	}
	UINT GetInstancingCount()
	{
		return (UINT)m_vecData.size();
	}
	ID3D11Buffer* GetInstancingBuffer()
	{
		return m_pInstancingBuffer;
	}

	void Resize(UINT _iCount);

private:
	CInstancingBuffer();
	~CInstancingBuffer();

};

//template<typename DataType>
//class COldInstancingBuffer : public CReferenceBase
//{
//public:
//	DataType *										m_pDataStream;
//	int												m_DataCount;	//	 CPU 데이터 스트림의 데이터 갯수
//	const type_info*								m_InstDataType;
//	D3D11_BUFFER_DESC								m_BufferDesc;
//
//	ID3D11Buffer*									m_pInstanceBuffer;
//	int												m_BuffCount;	//	 GPU 버퍼의 데이터 갯수
//
//public:
//	DataType* CreateInstancingStream(const int& _DataCount) 
//	{
//		if (_DataCount <= 0)
//			return nullptr;
//
//		m_InstDataType = &typeid(DataType);
//		m_DataCount = _DataCount;
//		m_pDataStream = new DataType[m_DataCount]();
//		return m_pDataStream;
//	}
//
//	ID3D11Buffer* CreateInstancingBuffer()
//	{
//		if (nullptr == m_pDataStream)
//			return nullptr;
//
//		m_BuffCount = m_DataCount;
//		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//		m_BufferDesc.ByteWidth = sizeof(DataType) * m_DataCount;
//		m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//		m_BufferDesc.MiscFlags = 0;
//		m_BufferDesc.StructureByteStride = 0;
//		m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
//
//		D3D11_SUBRESOURCE_DATA InstSub = D3D11_SUBRESOURCE_DATA();
//		InstSub.pSysMem = m_pDataStream;
//
//		if (S_OK != DXDEVICE->CreateBuffer(&m_BufferDesc, &InstSub, &(m_pInstanceBuffer)))
//		{
//			TASSERT(true)
//				SAFE_DELETE(m_pDataStream)
//				return nullptr;
//		}
//
//		return m_pInstanceBuffer;
//	}
//
//	ID3D11Buffer* CreateInstancingBuffer(const int& _BufferCount)
//	{
//		if (nullptr == m_pDataStream || m_DataCount < _BufferCount)
//			return nullptr;
//
//		m_BuffCount = _BufferCount;
//		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//		m_BufferDesc.ByteWidth = sizeof(DataType) * m_BuffCount;
//		m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//		m_BufferDesc.MiscFlags = 0;
//		m_BufferDesc.StructureByteStride = 0;
//		m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
//
//		D3D11_SUBRESOURCE_DATA InstSub = D3D11_SUBRESOURCE_DATA();
//		InstSub.pSysMem = m_pDataStream;
//
//		if (S_OK != DXDEVICE->CreateBuffer(&m_BufferDesc, &InstSub, &(m_pInstanceBuffer)))
//		{
//			TASSERT(true)
//			SAFE_DELETE(m_pDataStream)
//			return nullptr;
//		}
//
//		return m_pInstanceBuffer;
//	}
//
//
//	void Release() 
//	{
//		if (nullptr != m_pDataStream)
//			delete[] m_pDataStream;
//
//		SAFE_RELEASE(m_pInstanceBuffer);
//		m_DataCount = 0;
//		m_BuffCount = 0;
//		m_BufferDesc = CD3D11_BUFFER_DESC();
//	}
//
//	void Update() 
//	{
//		if (nullptr == m_pInstanceBuffer || nullptr == m_pDataStream)
//			return;
//
//		D3D11_MAPPED_SUBRESOURCE SubMapped;
//		UINT  BufSize =  sizeof(DataType) * m_BuffCount;
//		CDevice::GetContext()->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubMapped);
//		memcpy_s(SubMapped.pData, BufSize, m_pDataStream , BufSize);
//		CDevice::GetContext()->Unmap(m_pInstanceBuffer, 0);
//	}
//
//	// 인스턴싱 버퍼만 버택스 버퍼로 셋팅하는 함수
//	void IASetVtxBuffer() 
//	{
//		UINT OffSet = 0;
//		// Stride : 보폭,간격
//		UINT Stride = sizeof(DataType);
//
//		DXCONTEXT->IASetVertexBuffers(0, 1, &m_pInstanceBuffer, &Stride , &OffSet);
//	}
//
//
//public:
//	COldInstancingBuffer() 
//		:m_pDataStream(nullptr), m_DataCount(0), m_InstDataType(nullptr)
//		, m_pInstanceBuffer(nullptr), m_BufferDesc(CD3D11_BUFFER_DESC())
//		, m_BuffCount(0)
//	{}
//	~COldInstancingBuffer() 
//	{
//		if (nullptr != m_pDataStream) 
//		{
//			delete[] m_pDataStream;
//			m_pDataStream = nullptr;
//		}
//		SAFE_RELEASE(m_pInstanceBuffer);
//	}
//};