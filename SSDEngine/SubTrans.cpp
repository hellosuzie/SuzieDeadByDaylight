#include "SubTrans.h"
#include"DXMacro.h"
#include"TransForm.h"
#include"TimerMgr.h"

CSubTrans::CSubTrans()
	:m_pSubData(nullptr)
{
}


CSubTrans::~CSubTrans()
{
	SAFE_DELETE(m_pSubData)
}

void CSubTrans::SetSubPivot(const Vec3& _Pivot)
{
	if (nullptr == m_pSubData)
		m_pSubData = new SubTransData();

	m_pSubData->SubPivot = _Pivot;
}
void CSubTrans::SetSubSize(const Vec3& _Size)
{
	if (nullptr == m_pSubData)
		m_pSubData = new SubTransData();

	m_pSubData->SubSize = _Size;
}

void CSubTrans::SetSubRot(const Vec3& _Rot)
{
	if (nullptr == m_pSubData)
		m_pSubData = new SubTransData();

	m_pSubData->SubRot = _Rot;
}
void CSubTrans::SubTransUpdate() 
{
	if (nullptr == m_pSubData)
		return;

	//return TRANS->PostUpdate(DELTATIME);
	TRANS->TransformUpdate();
	TRANS->CalSubTransWorld(m_pSubData);
}
Mat CSubTrans::GetSubTransWorld()
{
	if (nullptr == m_pSubData)
		return TRANS->GetWorldMat();

	return m_pSubData->SubWorldMat;
}
const Mat& CSubTrans::GetSubTransCRWorld() 
{
	if (nullptr == m_pSubData)
		return TRANS->GetCRWorldMat();

	return m_pSubData->SubWorldMat;
}