#pragma once
#include"DXEnum.h"
#include"DXHeader.h"
#include"SubTrans.h"
#include<unordered_set>
#include<functional>

struct COLDATA
{
	std::wstring			CollisionChannel;
	SCENECOLLISION			eSceneCollision;
};

struct BASE_FI 
{
public:
	COLTYPE						m_eColType;
public:
	BASE_FI() :m_eColType(MAXCOLTYPE){}
	BASE_FI(const COLTYPE& _Type) :m_eColType(_Type) {}
	virtual ~BASE_FI() = 0 {}
};


struct SPHERE_FI :public BASE_FI
{
public:
	DirectX::BoundingSphere					SphereFi;

public:
	SPHERE_FI() : BASE_FI(CT_3DSPHERE) {}
	~SPHERE_FI() {}
};

struct FRUSTUM_FI :public BASE_FI
{
public:
	// x : Fovy , y : Aspect , z = near , w : far 
	Vec4										FrustumParameter;
	DirectX::BoundingFrustum					FrustumFi;

public:
	FRUSTUM_FI() : BASE_FI(CT_FRUSTUM) {}
	~FRUSTUM_FI() {}
};

struct OBB_FI :public BASE_FI
{
public:
	DirectX::BoundingOrientedBox			ObbFi;
	float									CollisionLength;

public:
	OBB_FI() : BASE_FI(CT_3DOBB), CollisionLength(0.f){}
	~OBB_FI() {}
};

struct RAY_FI :public BASE_FI
{
public:
	Vec4									RayOrigin;
	Vec4									RayDir;
	float									RayLength;
	bool									bCollsion;

public:
	RAY_FI() 
		: BASE_FI(CT_RAY) ,RayOrigin(Vec4()), RayDir(Vec4()),RayLength(0.f)
	{}
	~RAY_FI() 
	{}
};

typedef struct _tagCOLMESHTRI
{
	Vec3		Tri[3];

}ColTri;

struct MESHCOL_FI :public BASE_FI
{
public:
	std::vector<ColTri>*			pColTri;
	Mat								ColWorldMat;
	UINT							ColTriIdx;

public:
	MESHCOL_FI()
		: BASE_FI(CT_3DMESH), pColTri(nullptr), ColTriIdx(-1)
	{}
	~MESHCOL_FI()
	{}
};

class CCollider : public CSubTrans
{
	friend class Engine;
	friend class CActor;
	friend class CSceneCollisionMgr;
	friend class CCollisionChannel;
	friend class SSDCore;

protected:
	static SPTR<class CMesh>				Collider2D_Mesh;
	static SPTR<class CMaterial>			Collider2D_Material;

public:
	static void ColliderInit();

private:
	std::unordered_set<CCollider*>									m_setCollider;
	std::unordered_set<CCollider*>::iterator						m_setFindIter;
	std::unordered_set<CCollider*>::iterator						m_setEndIter;

private:
	std::unordered_set<CCollider*>									m_setIgnoreCollider;


private:
	std::function<void(SPTR<CCollider>, SPTR<CCollider>)>			m_EnterCallBack;
	std::function<void(SPTR<CCollider>, SPTR<CCollider>)>			m_StayCallBack;
	std::function<void(SPTR<CCollider>, SPTR<CCollider>)>			m_ExitCallBack;

public:
	template<typename T>
	void SetEnterCallBack(T* _Obj,void(T::*_pFunc)(SPTR<CCollider>, SPTR<CCollider>))
	{
		if (nullptr == _Obj)
			return;

		m_EnterCallBack = std::bind(_pFunc, _Obj, std::placeholders::_1, std::placeholders::_2);
	}

	template<typename T>
	void SetStayCallBack(T* _Obj, void(T::*_pFunc)(SPTR<CCollider>, SPTR<CCollider>))
	{
		if (nullptr == _Obj)
			return;

		m_StayCallBack = std::bind(_pFunc, _Obj, std::placeholders::_1, std::placeholders::_2);
	}

	template<typename T>
	void SetExitCallBack(T* _Obj, void(T::*_pFunc)(SPTR<CCollider>, SPTR<CCollider>))
	{
		if (nullptr == _Obj)
			return;

		m_ExitCallBack = std::bind(_pFunc, _Obj, std::placeholders::_1, std::placeholders::_2);
	}

private:
	void CallEnter(SPTR<CCollider> _This, SPTR<CCollider> _Other);
	void CallStay(SPTR<CCollider> _This, SPTR<CCollider> _Other);
	void CallExit(SPTR<CCollider> _This, SPTR<CCollider> _Other);

protected:
	std::wstring								m_CollisionChannel;
	BASE_FI*									m_pFi;
	bool										m_bCollision;

protected:
	Vec3										m_ColSize;
	Vec3										m_ColPivot;
	Vec3										m_DebugColor;
	float										m_fBorderSize;


public:
	void SetColSize(const Vec2& _Size)
	{ 
		m_ColSize = Vec3(_Size.x, _Size.y, 1.f);
		SetSubSize(Vec3(_Size.x, _Size.y, 1.f));
	}

	virtual void SetColSize(const Vec3& _Size)
	{ 
		m_ColSize = _Size; 
		SetSubSize(_Size); 
	}

	virtual  void SetColPivot(const Vec3& _Pivot)
	{ 
		m_ColPivot = _Pivot;
		SetSubPivot(m_ColPivot);
	
	}
	void SetColFi(const Vec3& _Pivot, const Vec2& _Size)
	{
		m_ColPivot = _Pivot; m_ColSize = Vec3(_Size.x, _Size.y, 1.f);
		SetSubSize(Vec3(_Size.x, _Size.y, 1.f));
		SetSubPivot(m_ColPivot);
	}
	virtual void SetColFi(const Vec3& _Pivot, const Vec3& _Size)
	{
		m_ColPivot = _Pivot; m_ColSize = _Size;
		SetSubSize(_Size);
		SetSubPivot(m_ColPivot);
	}

	virtual void SetDebugColor(const Vec3& _Color) { m_DebugColor = _Color; }
	virtual void SetBorderSize(const float& _Border) { m_fBorderSize = _Border; }


public:
	std::wstring GetChannel() { return m_CollisionChannel; }

protected:
	const bool Init(const COLDATA& _InitData);
	virtual void DebugRender() {}

private:
	void ReleaseSet();
	void EraseSet(SPTR<CCollider> _Other);
	void DumpDeathCollider();
	void ClearSet();

private:
	void Collision(SPTR<CCollider> _Other);

protected:
	virtual void CollisionEvent() 
	{
		m_bCollision = true;
	}
	virtual void CollisionEndEvent() 
	{
		m_bCollision = false;
	}


public:
	const bool CollisionCheck(SPTR<CCollider> _Other);

private:
	const bool PointCollision(BASE_FI* _OtherFi);
	const bool RectCollision(BASE_FI* _OtherFi);
	const bool CircleCollision(BASE_FI* _OtherFi);
	const bool SphereCollision(BASE_FI* _OtherFi);
	const bool OBBCollision(BASE_FI* _OtherFi);
	const bool RayCollision(BASE_FI* _OtherFi);
	const bool FrustumCollision(BASE_FI* _OtherFi);
private :
	virtual void CollisionSyncro() {}
	
public:
	virtual const Vec3 GetColliderPivot() { return Vec3(); }
	virtual const Vec3 GetColliderSize() { return Vec3(); }

public:
	const COLTYPE GetColType();
	
public:
	virtual const Vec3 GetColPos();


public:
	void AddIgnoreCollider(CCollider* _Col);
	const bool IsIgnoreCollider(CCollider* _Col);

public:
	void On()override;
	void Off()override;

public:
	CCollider();
	~CCollider();
};

