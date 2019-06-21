#include "UIBase.h"
#include "Actor.h"
#include "Camera.h"
#include "DXStruct.h"
#include "TransForm.h"
#include "Device.h"
#include "MultiRenderTarget.h"
#include "ResourceMgr.h"
#include "Material.h"
#include "Mesh.h"
#include "RectCollider.h"
#include "OBBCollider.h"
#include "SSDCore.h"


CUIBase::CUIBase()
	: m_UIMesh(nullptr), m_UIMaterial(nullptr), m_UIColorFactor(Vec4::One),
	m_CollPivot(Vec3()), m_CollSize(Vec3()), m_bIsColl(false), m_IsTex(true)
	, m_fCurGauge(0.f),m_MaxSize(Vec2()),m_MinSize(Vec2()), m_StartPos(Vec3())
	, m_SizeRatio(Vec3()), m_PosRatio(Vec3())
	, m_MinRatio(Vec3()), m_MaxRatio(Vec3()), m_StartPosRatio(Vec3())
{
}

CUIBase::~CUIBase()
{
}

const bool CUIBase::Init(const UI_DATA& _Data)
{
	m_UIData.Type = _Data.Type;
	m_UIData.BaseTexName = _Data.BaseTexName;
	m_UIData.Layer = _Data.Layer;
	m_UIData.Camptr = _Data.Camptr;

	if (L"" == m_UIData.BaseTexName)
	{
		m_IsTex = false;
	}

	_Data.Camptr->PushUICom(this, m_UIData.Layer);

	m_UIMesh = CResourceMgr<CMesh>::Find(L"2DMesh");
	m_UIMaterial = CResourceMgr<CMaterial>::Find(L"UIBase");
	
	if (m_IsTex != false)
	{
		m_UIMaterial->PushTexture(0, m_UIData.BaseTexName.c_str(), SHADER_PIXEL);
	}
	m_UIMaterial->SetBlend(L"DisableBlend");

	if (UCT_BUTTON == _Data.Type)
	{
		m_bIsColl = true;
	}

	//엑터에서 업데이트 돌리지 않게 하기 위해서 false로 해놓는다
	return false;
}

void CUIBase::PreUpdate(const float & _DeltaTime)
{
}

void CUIBase::Update(const float & _DeltaTime)
{
	if (true == m_bIsColl)
	{
		UICollision();
	}
}

void CUIBase::PostUpdate(const float & _DeltaTime)
{
}

void CUIBase::LastUpdate(const float & _DeltaTime)
{
}

void CUIBase::UIRender()
{
	if (m_IsTex == false)
	{
		return;
	}

	UIRenderUpdate();

	m_UIMaterial->PushTexture(0, m_UIData.BaseTexName.c_str(), SHADER_PIXEL);
	m_UIMaterial->Update();
	m_UIMesh->Update();
	m_UIMesh->Render();

	m_UIMaterial->TexSamRollBack();
	m_UIMaterial->StateRollBack();
}


void CUIBase::UIRenderUpdate()
{
	SyncronizationUI();
	SubTransUpdate();

	m_BaseBuffer.World = GetSubTransCRWorld().GetTransposeMat();
	m_BaseBuffer.InvWorld = m_BaseBuffer.World.GetInverseMat().GetTransposeMat();
	m_BaseBuffer.NormalWorld = GetSubTransCRWorld().GetInverseMat();

	m_BaseBuffer.View = m_UIData.Camptr->GetViewMatrix().GetTransposeMat();
	m_BaseBuffer.Proj = m_UIData.Camptr->GetProjMatrix().GetTransposeMat();
	m_BaseBuffer.WV = m_BaseBuffer.View * m_BaseBuffer.World;
	m_BaseBuffer.NormalWV = m_BaseBuffer.WV.GetInverseMat().GetTransposeMat();

	m_BaseBuffer.VP = m_BaseBuffer.Proj * m_BaseBuffer.View;
	m_BaseBuffer.WVP = m_BaseBuffer.Proj * m_BaseBuffer.WV;

	m_BaseBuffer.ColorFactor = m_UIColorFactor;
	m_BaseBuffer.CamWorldPos = m_UIData.Camptr->GetTrans()->GetWorldPos();

	if (nullptr != CDevice::RenBaseCB)
	{
		CDevice::RenBaseCB->Update(m_BaseBuffer);
		CDevice::RenBaseCB->SupplyShader();
	}
}

void CUIBase::CallEnter()
{
	if (nullptr == m_EnterCallBack)
	{
		return;
	}
	m_EnterCallBack();
}

void CUIBase::CallStay()
{
	if (nullptr == m_StayCallBack)
	{
		return;
	}
	m_StayCallBack();
}

void CUIBase::CallExit()
{
	if (nullptr == m_ExitCallBack)
	{
		return;
	}
	m_ExitCallBack();
}

void CUIBase::UICollision()
{
	if (true == UICollisionCheck())
	{
		if (false == m_bColl)
		{
			CallEnter();
		}
		else
		{
			CallStay();
		}
		CollisionEvent();
	}
	else
	{
		if (true == m_bColl)
		{
			CallExit();
		}
		CollisionEndEvent();
	}
}

bool CUIBase::UICollisionCheck()
{
	Vec2 Pos = Vec2(/*ACTOR->GetPos().x + */m_CollPivot.x,/* ACTOR->GetPos().y + */m_CollPivot.y);
	Vec2 MousePos = Vec2(
		SSDCore::GetScreenMousePos().x - Core::GetClientWidthHalf(),
		(SSDCore::GetScreenMousePos().y - Core::GetClientHeightHalf()) * -1.f);

	Vec4 Rect = Vec4(
		Pos.x - (m_CollSize.x * 0.5f),
		Pos.x + (m_CollSize.x * 0.5f),
		Pos.y - (m_CollSize.y * 0.5f),
		Pos.y + (m_CollSize.y * 0.5f));

	if (Rect.x > MousePos.x)
		return false;

	if (Rect.y < MousePos.x)
		return false;

	if (Rect.z > MousePos.y)
		return false;

	if (Rect.w < MousePos.y)
		return false;

	return true;
}

void CUIBase::SetUITexture(const wchar_t * _TexName)
{
	if (L"" == _TexName)
	{
		return;
	}

	if (false == m_IsTex && nullptr != CResourceMgr<CTexture>::Find(_TexName))
	{
		m_IsTex = true;
	}

	m_UIData.BaseTexName = _TexName;
}


void CUIBase::SyncronizationUI()
{	

	Vec3 SyncSize, SyncPos;
	SyncSize = Vec3(m_SizeRatio.x * Core::GetClientWidth(), m_SizeRatio.y * Core::GetClientHeight(), m_SizeRatio.z);
	SyncPos = Vec3(m_PosRatio.x * Core::GetClientWidth(), m_PosRatio.y * Core::GetClientHeight(), m_PosRatio.z);
	CUIBase::SetSubSize(SyncSize);
	CUIBase::SetSubPivot(SyncPos);

	if (true == m_bIsColl) 
	{
		m_CollSize = SyncSize;
		m_CollPivot = SyncPos;
	}
}