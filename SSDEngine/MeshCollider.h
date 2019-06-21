#pragma once
#include "Base3DCollider.h"
#include <vector>


class CMeshCollider : public CBase3DCollider
{
private:
	std::vector<ColTri>			m_vecColTri;
		
public:
	void SetColTri(SPTR<class CNavigationMesh> _NaviMesh);

public:
	void PostUpdate(const float& _DeltaTime);
public:
	const UINT GetColTriIdx();

public:
	CMeshCollider();
	~CMeshCollider();
};

