#pragma once
#include "BaseComponent.h"
#include"DXHeader.h"


typedef struct _tagSubTransData 
{
	Vec3					SubPivot;
	Vec3					SubSize;
	Vec3					SubRot;

	Mat						SubWorldMat;
	_tagSubTransData()
		: SubPivot(Vec3()),SubSize(Vec3(1.f,1.f,1.f)), SubRot(Vec3())
	{
		SubWorldMat.SetIdentityMat();
	}
	_tagSubTransData(const Vec3& _Pivot, const Vec3& _Scale, const Vec3& _Rot)
		: SubPivot(_Pivot), SubSize(_Scale), SubRot(_Rot)
	{
		SubWorldMat.SetIdentityMat();
	}
	~_tagSubTransData() {}

}SubTransData,SubData;

class CSubTrans : public CBaseComponent
{
protected:
	SubTransData*						m_pSubData;
	
	Mat									m_SubMatRotate;

protected:
	void SetSubPivot(const Vec3& _Pivot);
	void SetSubSize(const Vec3& _Size);
	void SetSubRot(const Vec3& _Rot);

public:
	bool IsSubtransExist()
	{
		if (nullptr != m_pSubData)
		{
			return true;
		}
		return false;
	}

	SubTransData* GetSubData()
	{
		return m_pSubData;
	}

public:
	void SubTransUpdate();
	Mat GetSubTransWorld();
	const Mat& GetSubTransCRWorld();

public:
	CSubTrans();
	~CSubTrans();
};


