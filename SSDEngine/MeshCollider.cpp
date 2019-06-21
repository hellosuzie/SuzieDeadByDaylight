#include "MeshCollider.h"
#include "NavigationMesh.h"


CMeshCollider::CMeshCollider()
{
	m_pFi = new MESHCOL_FI();
	reinterpret_cast<MESHCOL_FI*>(m_pFi)->pColTri = &m_vecColTri;
}


CMeshCollider::~CMeshCollider()
{
	m_vecColTri.clear();
}

void CMeshCollider::PostUpdate(const float& _DeltaTime)
{
	SubTransUpdate();
	reinterpret_cast<MESHCOL_FI*>(m_pFi)->ColWorldMat = GetSubTransCRWorld();
}

void CMeshCollider::SetColTri(SPTR<class CNavigationMesh> _NaviMesh)
{
	if (nullptr == _NaviMesh)
		return;

	m_vecColTri.clear();

	size_t MaxTriCount = _NaviMesh->m_vecNaviTri.size();
	m_vecColTri.reserve(MaxTriCount);

	for (size_t i = 0; i < MaxTriCount; i++)
	{
		NaviTri* NTri = _NaviMesh->m_vecNaviTri[i];
		m_vecColTri.push_back(ColTri{ NTri->Tri[0] ,NTri->Tri[1] ,NTri->Tri[2] });
	}

	MESHCOL_FI* ColFi = reinterpret_cast<MESHCOL_FI*>(m_pFi);

	if (ColFi->pColTri == nullptr)
	{
		ColFi->pColTri = &m_vecColTri;
	}

}

const UINT CMeshCollider::GetColTriIdx()
{
	return reinterpret_cast<MESHCOL_FI*>(m_pFi)->ColTriIdx;
}

