#pragma once
#include"Shader.h"

#define MAXINPUTSLOT	8


class CVtxShader : public CShader
{

private:
	class CInputLayout : public CRefBase
	{
	private:
		CShader*									m_pParentShader;
		UINT										m_Offset[MAXINPUTSLOT];
		std::vector<D3D11_INPUT_ELEMENT_DESC>		m_vecInputDesc;
		ID3D11InputLayout*							m_pInputLayout;


	public:
		void PushInputDesc(const char* _SemanticName, const UINT& _Index
			, const DXGI_FORMAT& _Fmt, const UINT& _InputSlot
			, const UINT& _IDSR, D3D11_INPUT_CLASSIFICATION _eInput
			= D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA);

		void CreateInputLayout();
		void Update();



		CInputLayout()
			:m_pParentShader(nullptr), m_pInputLayout(nullptr)
		{
			memset(m_Offset, 0, sizeof(m_Offset));
		}
		CInputLayout(CShader* _Shader)
			:m_pParentShader(_Shader), m_pInputLayout(nullptr)
		{
			memset(m_Offset, 0, sizeof(m_Offset));
		}
		~CInputLayout()
		{
			SAFE_RELEASE(m_pInputLayout)
		}

	};



public:
	void PushInputDesc(const char* _SemanticName, const UINT& _Index
		, const DXGI_FORMAT& _Fmt, const UINT& _InputSlot
		, const UINT& _IDSR, D3D11_INPUT_CLASSIFICATION _eInput
		= D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA);
	void PushEndInputDesc(const char* _SemanticName, const UINT& _Index
		, const DXGI_FORMAT& _Fmt, const UINT& _InputSlot
		, const UINT& _IDSR, D3D11_INPUT_CLASSIFICATION _eInput
		= D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA);



private:
	ID3D11VertexShader*				m_pVtxShader;
	SPTR<CInputLayout>				m_pInputLayout;

public:
	ID3D11VertexShader * GetShader() { return m_pVtxShader; }

public:
	const bool Load(const LOAD_DESC& _Desc) override;
	
	void Update() override;

public:
	CVtxShader();
	~CVtxShader();
};

