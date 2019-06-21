#pragma once
#include "Shader.h"

class CComShader : public CShader
{
	ID3D11ComputeShader*									m_pComShader;



public:
	ID3D11ComputeShader* GetShader() { return m_pComShader; }

public:
	const bool Load(const LOAD_DESC& _Desc) override;
	void Update() override;



public:
	CComShader();
	~CComShader();
};

