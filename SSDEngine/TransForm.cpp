#include "TransForm.h"
#include "Actor.h"
#include "SubTrans.h"


// 현재 엑터에 트랜스폼이 2개를 넣으려 하면 팅겨낸다
// 한 엑터 안에 트랜스폼 컴포넌트는 2개가 들어올 수 없다.
const bool CTransForm::IsMultiComponentCheck(CActor* _pActor)
{
	if (nullptr == _pActor)
		return false;

	// 엑터 안에 트랜스폼이 있으므로 트랜스폼 컴포넌트가 들어갈수 없다.
	if (nullptr != _pActor->FindCom<CTransForm>())
		return false;

	return true;
}

CTransForm::CTransForm()
	: m_bScale(false), m_bRot(false), m_bTrans(false), m_bRevolve(false), m_bWorld(false)
	, m_Scale(Vec3::One3), m_Rot(Vec3()), m_Trans(Vec3()), m_RevolvePivot(Vec3()), m_RevolveRot(Vec3())
	, m_fMoveSpeed(0.f), m_MoveDir(Vec3()), m_bTransUpdate(false),m_pParent(nullptr), CalWorldFunc(nullptr)
{
	m_matScale.SetIdentityMat();
	m_matRotX.SetIdentityMat();
	m_matRotY.SetIdentityMat();
	m_matRotZ.SetIdentityMat();
	m_matRot.SetIdentityMat();
	m_matTrans.SetIdentityMat();
	m_matRevolve.SetIdentityMat();
	m_matWorld.SetIdentityMat();

	SetParent(nullptr);
}

CTransForm::~CTransForm()
{
}

void CTransForm::SetWolrdMat(const Mat& _WorldMat)
{
	DirectX::XMVECTOR S, R, T;

	Mat::DivisionWorldMat(&S, &R, &T, _WorldMat);

	SetRotateQuat(Vec4(R));
	SetPos(Vec3(T));
	
}
void CTransForm::CalWorld() 
{
	m_matWorld = m_matScale * m_matRot * m_matTrans * m_matRevolve;
}
void CTransForm::CalWorldParent()
{
	if (nullptr == m_pParent)
		CalWorld();
	
	// 부모가 있는경우 크기 행렬이 제외된 부모의 월드행렬을 받는다. 
	//m_matWorld = m_matScale * m_matRot * m_matTrans  * m_matRevolve * m_pParent->GetNonScaleWorld();
	m_matWorld = m_matScale * m_matRot * m_matTrans  * m_matRevolve * m_pParent->GetCRWorldMat();
}
const Vec3 CTransForm::GetWorldPos()
{
	if (nullptr != m_pParent && true == m_pParent->m_bTransUpdate) 
	{
		TransformUpdate();
	}
	else if (true == m_bTrans) 
	{
		TransformUpdate();
	}

	return m_matWorld.Row[3].vec3;
}
const Mat	CTransForm::GetNonScaleWorld()
{
	Mat ReturnMat = m_matWorld;
	
	for (size_t i = 0; i < 3; i++)
		ReturnMat.Row[i].vec3.Normalize();
	
	return ReturnMat;
}
void CTransForm::CalSubTransWorld(struct _tagSubTransData* _SubTransData)
{
	if (nullptr == _SubTransData)
	{
		_SubTransData->SubWorldMat = m_matWorld;
		return;
	}

	Mat Scale, Pos, Rot;

	if (_SubTransData->SubSize == Vec3())
		Scale = m_matScale;
	else 
		Scale.SetScalingMat(_SubTransData->SubSize.x * m_Scale.x, _SubTransData->SubSize.y * m_Scale.y, _SubTransData->SubSize.z * m_Scale.z);

	Pos.SetPosMat(m_Trans + _SubTransData->SubPivot * m_matRot);

	if (_SubTransData->SubRot == Vec3()) 
	{
		Rot = m_matRot;
	} 
	else 
	{
		Mat RotX, RotY, RotZ;
		RotX.SetRotateXMat_ForDegree(m_Rot.x + _SubTransData->SubRot.x);
		RotY.SetRotateYMat_ForDegree(m_Rot.y + _SubTransData->SubRot.y);
		RotZ.SetRotateZMat_ForDegree(m_Rot.z + _SubTransData->SubRot.z);
		Rot = RotX *RotY * RotZ;
	}

	if (nullptr == m_pParent)
		_SubTransData->SubWorldMat = Scale * Rot * Pos * m_matRevolve;
	else
		_SubTransData->SubWorldMat = Scale * Rot * Pos * m_matRevolve * m_pParent->GetCRWorldMat();

	//_SubTransData->SubWorldMat = Scale * m_matRot * Pos * m_matRevolve * m_pParent->GetNonScaleWorld();
}

void CTransForm::PostUpdate(const float& _DeltaTime)
{
	TransformUpdate();
}
void CTransForm::LastUpdate(const float& _DeltaTime)
{
	if (false == m_bTransUpdate)
		TransformUpdate();

	m_bScale = false;
	m_bRot = false;
	m_bTrans = false;
	m_bRevolve = false;
	m_bWorld = false;
	m_bTransUpdate = false;
}

void CTransForm::TransformUpdate()
{
	m_bTransUpdate = true;

	if (true == m_bScale)
	{
		m_bWorld = true;
		m_matScale.SetScalingMat(m_Scale);
	}

	if (true == m_bRot)
	{
		m_bWorld = true;
		//m_matRotX.SetRotateXMat(m_Rot.x);
		//m_matRotY.SetRotateYMat(m_Rot.y);
		//m_matRotZ.SetRotateZMat(m_Rot.z);
		m_matRotX.SetRotateXMat_ForDegree(m_Rot.x);
		m_matRotY.SetRotateYMat_ForDegree(m_Rot.y);
		m_matRotZ.SetRotateZMat_ForDegree(m_Rot.z);
		m_matRot = m_matRotX * m_matRotY * m_matRotZ;
	}

	if (true == m_bTrans)
	{
		m_bWorld = true;
		m_matTrans.SetPosMat(m_Trans);
	}

	if (true == m_bRevolve)
	{
		m_bWorld = true;
		Mat Pivot, RevolveX, RevolveY, RevolveZ;
		Pivot.SetPosMat(m_RevolvePivot);
		RevolveX.SetRotateXMat(m_RevolveRot.x);
		RevolveY.SetRotateYMat(m_RevolveRot.y);
		RevolveZ.SetRotateZMat(m_RevolveRot.z);

		m_matRevolve = Pivot * RevolveX * RevolveY * RevolveZ;
	}

	if (nullptr == m_pParent) 
	{
		if(true == m_bWorld)
			(this->*CalWorldFunc)();
	}
	else
	{
		if(true == ChangeTransform())
			(this->*CalWorldFunc)();
	}

}