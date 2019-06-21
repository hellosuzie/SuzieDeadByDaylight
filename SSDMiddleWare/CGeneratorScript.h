#pragma once
#include <Script.h>

class CGeneratorScript : public CScript
{


public:
	CGeneratorScript();
	~CGeneratorScript();

public:
	const bool Init();
	void Update(const float& _DeltaTime) override;
	void LastUpdate(const float& _DeltaTime) override;
	void DebugRender() override;
};

