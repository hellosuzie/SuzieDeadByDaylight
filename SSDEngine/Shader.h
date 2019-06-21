#pragma once
#include"Resource.h"
#include"DXHeader.h"
#include"DXEnum.h"
#include"DXTemplate.h"
#include"ConstantBuffer.h"
#include<vector>
#include<unordered_map>

class CShader :public CResource
{
public:
	SHADER_TYPE										m_eShaderType;


public:
	static UINT SizeofFmt(const DXGI_FORMAT& _Fmt);

protected:
	ID3DBlob*					m_pBlob;			// 쉐이더 코드를 컴파일해서 디바이스에게 넘겼을때 디바이스가 주는 포인터
	ID3DBlob*					m_pErrBlob;			// 쉐이더 코드 컴파일이 실패했을때 디바이스가 에러의 내용을 넘겨주는 포인터
	UINT						m_VH;				// 버택스 쉐이더 HLSL 상위 버전 (5)
	UINT						m_VL;				// 버택스 쉐이더 HLSL 하위 버전 (0)	-> 5.0

public:
	ID3DBlob* GetBIob() { return m_pBlob; }
	ID3DBlob* GetErrBIob() { return m_pErrBlob; }


	// Constant Buffer 
protected:
	std::unordered_map<UINT, SPTR<CConstantBuffer>>				m_mapConstBuffer;

public:
	const BOOL PushConstantBuffer(const UINT& _RegisterNum, const std::wstring& _BufferKey);

	template<typename T>
	void BufferUpdate(const UINT& _SlotIndex, T* _BuffData)
	{
		SPTR<CConstantBuffer> Buffer = UMAPFINDSPTR<SPTR<CConstantBuffer>>(m_mapConstBuffer, _SlotIndex);

		if (nullptr != Buffer)
			Buffer->Update<T>(_SlotIndex, _BuffData, m_eShaderType);
	}



public:
	CShader(const SHADER_TYPE& _eType);
	~CShader();

public:
	NONE_CREATE_RESOURCE(CShader)

	LOAD_DESC_BODY( UINT	iVH;
		UINT	iVL;
		std::string   FuncName;
		bool		bCreateShader;
	)

	virtual const bool Load(const LOAD_DESC& _Desc) = 0;
	virtual void Update() = 0;

};

