#pragma once
#include "BaseComponent.h"
#include"DXHeader.h"
#include"DXEnum.h"


class CTransForm final : public CBaseComponent
{
	friend class CSubTrans;

public:
	static const bool IsMultiComponentCheck(CActor* _pActor);

private:
	// 자기가 소속된 엑터의 부모 엑터의 기하구조
	CTransForm * m_pParent;
	void(CTransForm::*CalWorldFunc)();

private:
	bool			m_bTransUpdate;
	bool			m_bScale;
	bool			m_bRot;
	bool			m_bTrans;
	bool			m_bRevolve;
	bool			m_bWorld;

	float			m_fMoveSpeed;
	Vec3			m_MoveDir;

	Vec3			m_Scale;
	Vec3			m_Rot;
	Vec3			m_Trans;

	Vec3			m_RevolvePivot;
	Vec3			m_RevolveRot;

	Mat				m_matScale;
	Mat				m_matRotX;
	Mat				m_matRotY;
	Mat				m_matRotZ;
	Mat				m_matRot;
	Mat				m_matTrans;
	Mat				m_matRevolve;

	Mat				m_matWorld;

public:
	void SetScale(const Vec3& _Scale)
	{
		m_bTransUpdate = false;
		m_bScale = true;
		m_Scale = _Scale;
	}

	void SetRotate(const Vec3& _Rot)
	{
		if (_Rot == m_Rot)
			return;

		m_bTransUpdate = false;
		m_bRot = true;
		m_Rot = _Rot;
	}

	void AccRotate(const AXIS_TYPE& _Axis,const float& _AccAngle)
	{
		m_bTransUpdate = false;
		m_bRot = true;

		Vec3 Rot = Vec3();

		switch (_Axis)
		{
		case AXIS_X : 
			Rot.x = 1.f;
			break;
		case AXIS_Y:
			Rot.y = 1.f;
			break;
		case AXIS_Z:
			Rot.z = 1.f;
			break;
		default:
			return;
		}

		m_Rot += Rot * _AccAngle;
	}

	void SetRotateQuat(const Vec4& _Quat) 
	{
		m_bTransUpdate = false;
		m_matRot = DirectX::XMMatrixRotationQuaternion(_Quat);
	}

	void AccRotateQuat(const Vec4& _Quat)
	{
		m_bTransUpdate = false;
		m_matRot *= DirectX::XMMatrixRotationQuaternion(_Quat);
	}


	void SetPos(const Vec3& _Pos)
	{
		if (_Pos == m_Trans)
			return;

		m_bTransUpdate = false;
		m_bTrans = true;
		m_Trans = _Pos;
	}

	void SetPos(const Vec2& _Pos)
	{
		if (_Pos == m_Trans.vec2)
			return;

		m_bTransUpdate = false;
		m_bTrans = true;
		m_Trans.vec2 = _Pos;
	}

	void SetPosX(const float& _X)
	{
		m_bTransUpdate = false;
		m_bTrans = true;
		m_Trans.x = _X ;
	}

	void SetPosY(const float& _Y)
	{
		m_bTransUpdate = false;
		m_bTrans = true;
		m_Trans.y = _Y;
	}
	void SetPosZ(const float& _Z)
	{
		m_bTransUpdate = false;
		m_bTrans = true;
		m_Trans.z = _Z;
	}


	void SetRevolve(const Vec3& _Pivot, const Vec3& _Rot)
	{
		m_bTransUpdate = false;
		m_bRevolve = true;
		m_RevolvePivot = _Pivot;
		m_RevolveRot = _Rot;
	}
	void SetRevolve(const Vec3& _Rot)
	{
		m_bTransUpdate = false;
		m_bRevolve = true;
		m_RevolvePivot = Vec3();
		m_RevolveRot = _Rot;
	}

	void SetWolrdMat(const Mat& _WorldMat);


	// 1번쨰 매개변수 : Dir*Speed*DeltaTime
	// 2번째 매개변수 : DeltaTime
	// 이 값들을 통해 Dir과 Speed를 산출
	// 움직인 이후의 위치를 반환
	const Vec3 Move(const Vec3& _Dir, const float& _DeltaTime)
	{
		m_bTransUpdate = false;
		Vec3 Dir = _Dir / _DeltaTime;
		m_fMoveSpeed = Dir.Length();
		m_MoveDir = Dir.GetNormalizeVec();

		m_bTrans = true;
		m_Trans += _Dir;
		return m_Trans;
	}

	const Vec3 Move(const Vec3& _Dir, const float& _Speed, const float& _DeltaTime)
	{
		m_bTransUpdate = false;

		if (m_fMoveSpeed != _Speed)
			m_fMoveSpeed = _Speed;

		if (m_MoveDir != _Dir)
			m_MoveDir = _Dir;

		m_bTrans = true;
		m_Trans += _Dir * _Speed * _DeltaTime;
		return m_Trans;
	}

	const Vec3 Move(const Vec3& _Dir)
	{
		m_bTransUpdate = false;
		m_MoveDir = _Dir.GetNormalizeVec();

		m_bTrans = true;
		m_Trans += _Dir;
		return m_Trans;
	}

	void AccRotate(const Vec3& _AccRot) 
	{
		m_bTransUpdate = false;
		m_bRot = true;
		m_Rot += _AccRot;
	}


	// 반발에 의한 이동
	const Vec3 Push(const Vec3& _Dir, const float& _Speed)
	{
		m_bTransUpdate = false;
		m_bTrans = true;
		m_Trans += _Dir * _Speed;
		return m_Trans;
	}

	void SetMoveSpeed(const float& _Speed) { m_fMoveSpeed = _Speed; }
	void SetMoveDir(const Vec3& _Dir) { m_MoveDir = _Dir; }
	const float GetMoveSpeed() { return m_fMoveSpeed; }
	const Vec3 GetMoveDir() { return m_MoveDir; }

public:
	const Vec3 GetScale() { return m_Scale; }
	const Vec3 GetLocalPos() { return m_Trans; }
	const Vec3 GetLocalRotate() { return m_Rot; }
	const Vec3 GetLocalRevolve() { return m_RevolveRot; }

	const Vec3 GetWorldPos();
	


	const Mat	GetWorldMat() 
	{
		if (false == m_bTransUpdate)
		{
			TransformUpdate();
		}

		return m_matWorld; 
	}
	const Mat&  GetCRWorldMat() 
	{
		if (false == m_bTransUpdate) 
		{
			TransformUpdate();
		}

		return m_matWorld;
	}

	const Mat	GetNonScaleWorld();
	const Vec3 GetWorldAxis(const Axis& _Axis)
	{
		if (MAXAXIS <= _Axis)
			return Vec3();

		return m_matWorld.Row[_Axis].vec3.GetNormalizeVec();
	}
	const Vec3 GetFront() { return m_matWorld.Row[AXIS_Z].GetNormalize_Vec3(); }
	const Vec3 GetBack() 
	{ 
		return -m_matWorld.Row[AXIS_Z].GetNormalize_Vec3();
	}
	const Vec3 GetUp() { return m_matWorld.Row[AXIS_Y].GetNormalize_Vec3(); }
	const Vec3 GetDown() { return -m_matWorld.Row[AXIS_Y].GetNormalize_Vec3(); }
	const Vec3 GetRight() { return m_matWorld.Row[AXIS_X].GetNormalize_Vec3(); }
	const Vec3 GetLeft() { return -m_matWorld.Row[AXIS_X].GetNormalize_Vec3(); }
	const Vec3 GetRotate() { return m_Rot; }

public:
	const bool IsMove() 
	{
		return m_bTrans; 
	}
	const bool IsRotate() 
	{
		return m_bRot;
	}
	const bool ChangeTransform() 
	{
		//return	m_bWorld;

		// 자신의 월드 행렬이 바꿨다면
		if (true == m_bWorld)
			return true;

		// 부모의 월드 행렬이 바꿨다면
		if (nullptr != m_pParent && true == m_pParent->ChangeTransform())
			return true;

		return false;
	}
	

public:
	// 자기가 소속된 액터의 부모가 셋팅된 경우
	// 부모 액터의 기하구조가 매개 변수로 ㄷㄹ어간다.
	void SetParent(CTransForm* _pParent)
	{
		if (nullptr == _pParent)
			CalWorldFunc = &CTransForm::CalWorld;
		else 
		{
			m_pParent = _pParent;
			CalWorldFunc = &CTransForm::CalWorldParent;
			(this->*CalWorldFunc)();
		}
		
	}

public:
	void CalWorld();
	void CalWorldParent();
	void CalSubTransWorld(struct _tagSubTransData* _SubTransData);

public:
	void PostUpdate(const float& _DeltaTime) override;
	void LastUpdate(const float& _DeltaTime) override;

public:
	void TransformUpdate();


public:
	CTransForm();
	~CTransForm();
};

