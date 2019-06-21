#pragma once
#include"DXBuffer.h"
#include"DXHeader.h"
#include"DXMacro.h"
#include"DXEnum.h"
#include"Device.h"


class CConstantBuffer : public CDXBuffer 
{
private:
	const type_info*								m_pBufferType;
	D3D11_MAPPED_SUBRESOURCE						m_MappedSub;
	UINT											m_ShaderFlag;
	UINT											m_RegisterNum;

	void*											m_pOriginData;
	bool											m_bOriginSet;

public:
	void SetShaderFlag(const UINT& _ShaderFlag) { m_ShaderFlag = _ShaderFlag; }
	void SetRegisterNum(const UINT& _RegNum) {	m_RegisterNum = _RegNum; }

public:
	template<typename BuffType>
	void SetBuffDataType() 
	{
		m_pBufferType = &typeid(BuffType);
	}

	template<typename BuffType>
	void SetBuffDataType(const BuffType& _OriginData)
	{
		m_pBufferType = &typeid(BuffType);
		
		if (nullptr == m_pOriginData)
			m_pOriginData = new char[sizeof(BuffType)];
		
		memcpy_s(m_pOriginData, m_BufferDesc.ByteWidth, &_OriginData, sizeof(BuffType));
		Update(m_pOriginData, sizeof(BuffType));
	}

	template<typename BuffType>
	const bool CheckCBufferType() 
	{
		return m_pBufferType == &typeid(BuffType);
	}

	template<typename BuffType>
	void Update(UINT _RegisterNum, BuffType* _pMapping, const SHADER_TYPE& _ShaderType)
	{
		if (&typeid(BuffType) != m_pBufferType)
			return;
		// Map,Unmap 함수의 2번째 인자값 : subresource의 인덱스 번호
		// Map 함수의 4번째 인자값 : GPU가 Busy 할 경우 CPU의 행동에 대한 옵션(0 : D3D11_MAP_FLAG_DO_NOT_WAIT
	
		CDevice::GetContext()->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &m_MappedSub);
		memcpy_s(m_MappedSub.pData, m_BufferDesc.ByteWidth, _pMapping, sizeof(BuffType));
		CDevice::GetContext()->Unmap(m_pBuffer, 0);
		
		switch (_ShaderType)
		{
		case SHADER_VERTEX:
			CDevice::GetContext()->VSSetConstantBuffers(_RegisterNum, 1, &m_pBuffer);
			break;
		case SHADER_HULL:
			CDevice::GetContext()->HSSetConstantBuffers(_RegisterNum, 1, &m_pBuffer);
			break;
		case SHADER_DOMAIN:
			CDevice::GetContext()->DSSetConstantBuffers(_RegisterNum, 1, &m_pBuffer);
			break;
		case SHADER_GEOMETRY:
			CDevice::GetContext()->GSSetConstantBuffers(_RegisterNum, 1, &m_pBuffer);
			break;
		case SHADER_PIXEL:
			CDevice::GetContext()->PSSetConstantBuffers(_RegisterNum, 1, &m_pBuffer);
			break;
		default:
			break;
		}
	}

	template<typename BuffType>
	void Update(const BuffType& _pMapping) 
	{
		if (&typeid(BuffType) != m_pBufferType)
			return;

		Update(&_pMapping, sizeof(BuffType));
	}

	void Update(const void* _pData,const UINT& _Size);
	void Update();

public:
	// 해당 쉐이더에 상수버퍼를 장착
	void SupplyShader(const UINT& _ShaderFlag, const UINT& _RegisterNum);
	void SupplyShader(const SHADER_TYPE& _ShaderFlag, const UINT& _RegisterNum);
	void SupplyShader();

public:
	void* GetSubDataPointer() { return m_MappedSub.pData; }
	const UINT GetByteWidth() { return m_BufferDesc.ByteWidth; }

public:
	CConstantBuffer();
	~CConstantBuffer();


	NONE_LOAD_RESOURCE(CConstantBuffer)
	
	CREATE_DESC_BODY(UINT ByteWidth;
	D3D11_USAGE Usage;
	UINT CPUAccessFlags;
	UINT ShaderFlag;
	UINT RegisterNum;
	)

	const bool Create(const CREATE_DESC& _Desc);
};

