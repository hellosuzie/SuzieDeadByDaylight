#pragma once
#include "Shader.h"
#include<list>
#include"Texture.h"

class CPixShader : public CShader
{
private:
	ID3D11PixelShader *								m_pPixShader;

public:
	ID3D11PixelShader * GetShader() { return m_pPixShader; }

public:
	const bool Load(const LOAD_DESC& _Desc) override;
	void Update() override;



public:
	CPixShader();
	~CPixShader();
};

