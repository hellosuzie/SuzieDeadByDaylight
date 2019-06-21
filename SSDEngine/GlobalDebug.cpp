#include "GlobalDebug.h"
#include "ResourceMgr.h"
#include "DXStruct.h"
#include "Mesh.h"
#include "VtxShader.h"
#include "PixShader.h"
#include "DXVertex.h"
#include "ConstantBuffer.h"
#include "FilePathMgr.h"
#include "DXFont.h"
#include "Material.h"
#include "MultiRenderTarget.h"
#include "Camera.h"
#include "Texture.h"
#include "KeyMgr.h"



bool					GlobalDebug::bDebugMode = false;
SPTR<CMesh>				GlobalDebug::Debug2D_Mesh = nullptr;
SPTR<CMaterial>			GlobalDebug::Debug2D_Material = nullptr;
SPTR<CMaterial>			GlobalDebug::DebugTarget_Material = nullptr;

SPTR<CMesh>				GlobalDebug::DebugSphere_Mesh = nullptr;
SPTR<CMaterial>			GlobalDebug::Debug3D_Material = nullptr;

CBUFFER::RenBaseBuffer  GlobalDebug::Debug3DCB;

void GlobalDebug::InitGlobalDebug()
{
	Debug2D_Mesh = CResourceMgr<CMesh>::Find(L"2DMesh");
	Debug2D_Material = CResourceMgr<CMaterial>::Find(L"Base2D");

	DebugSphere_Mesh = CResourceMgr<CMesh>::Find(L"Sphere_Lv16");
	Debug3D_Material = CResourceMgr<CMaterial>::Find(L"ColorBase3D")->Clone();
	Debug3D_Material->SetDepthStancilState(L"IgnoreDepth");
	DebugTarget_Material = CResourceMgr<CMaterial>::Find(L"DebugTarget");

}
void GlobalDebug::ReleaseGlobalDebug()
{

}

void GlobalDebug::Update()
{
	if (true == KEYDOWN("DebugKey")) 
	{
		bDebugMode = !bDebugMode;
	}
}
void GlobalDebug::DrawDebugRect(const Mat& _VP, const Vec3& _Pos, const Vec3& _Size, const Vec3& _Color /*= Vec3::One3*/, const float& _BorderSize /*= 2.f*/)
{

	CBUFFER::Base2DBuffer BaseBuffer;

	
	BaseBuffer = CBUFFER::Base2DBuffer(_Color, BASE2D_RECT, Vec4(1.f / _Size.x, 1.f / _Size.y, _BorderSize));
	Mat Pos, Size;
	Pos.SetPosMat(_Pos);
	Size.SetScalingMat(_Size);

	BaseBuffer.WVP = (Size * Pos * _VP).GetTransposeMat();

	
	Debug2D_Mesh->Update();
	Debug2D_Material->SetConstantBufferData(L"MainBuffer", BaseBuffer);
	Debug2D_Material->Update();

	Debug2D_Mesh->Render();
};

void GlobalDebug::DrawDebugCircle(const Mat& _VP, const Vec3& _Pos, const Vec3& _Size, const Vec3& _Color/* = Vec3::One3*/, const float& _BorderSize/* = 2.f*/)
{
	CBUFFER::Base2DBuffer BaseBuffer;

	BaseBuffer = CBUFFER::Base2DBuffer(_Color, BASE2D_CIRCLE, Vec4(1.f / _Size.x, 1.f / _Size.y, _BorderSize));
	Mat Pos, Size;
	Pos.SetPosMat(_Pos);
	Size.SetScalingMat(_Size);

	BaseBuffer.WVP = (Size * Pos * _VP).GetTransposeMat();
	
	Debug2D_Mesh->Update();
	Debug2D_Material->SetConstantBufferData(L"MainBuffer", BaseBuffer);
	DebugTarget_Material->Update();

	Debug2D_Mesh->Render();
	Debug2D_Material->StateRollBack();
	Debug2D_Material->TexSamRollBack();
}

void GlobalDebug::DrawDebugSphere(const Mat& _VP, const Vec3& _Pos, const float& _Radius, const Vec4& _Color/* = Vec4::White*/) 
{
	Mat Pos, Size;
	Pos.SetPosMat(_Pos);
	Size.SetScalingMat(Vec3(_Radius, _Radius, _Radius));

	Debug3DCB.World = (Size * Pos).GetTransposeMat();
	Debug3DCB.WVP = (Size * Pos * _VP).GetTransposeMat();
	Debug3DCB.ColorFactor = _Color;

	DebugSphere_Mesh->Update();

	if (nullptr != CDevice::RenBaseCB) 
	{
		CDevice::RenBaseCB->Update(Debug3DCB);
		CDevice::RenBaseCB->SupplyShader();

	}

	Debug3D_Material->Update();
	DebugSphere_Mesh->Render();
	Debug3D_Material->StateRollBack();
	Debug3D_Material->TexSamRollBack();
}
void GlobalDebug::DrawDebugTriangle(const Mat& _VP, const Vec3& _Tri1, const Vec3& _Tri2, const Vec3& _Tri3, const Vec3& _Color /*= Vec3::One3*/)
{

}


void GlobalDebug::DrawRenderTarget(SPTR<RenderTarget> _Target
	, const UINT& _DivRow, const UINT& _DivCol
	, const UINT& _Row, const UINT& _Col) 
{
	if (nullptr == _Target)
		return;

	if (_DivRow <= _Row || _DivCol <= _Col)
		return;

	CBUFFER::DebugTargetBuffer Buff;
	Buff.DivisionLevel[0] = _DivRow;
	Buff.DivisionLevel[1] = _DivCol;
	Buff.TargetPosition[0] = _Row;
	Buff.TargetPosition[1] = _Col;

	Debug2D_Mesh->Update();
	DebugTarget_Material->SetConstantBufferData(L"DebugBuffer", Buff);
	_Target->GetRTTexture()->Update(0, SHADER_PIXEL);
	DebugTarget_Material->Update();


	Debug2D_Mesh->Render();
	_Target->GetRTTexture()->PopShaderResource(0, SHADER_PIXEL);
}

void GlobalDebug::DrawRenderTarget(const wchar_t* _TargetKey
	, const UINT& _DivRow, const UINT& _DivCol
	, const UINT& _Row, const UINT& _Col) 
{
	return DrawRenderTarget(CResourceMgr<RenderTarget>::Find(_TargetKey), _DivRow, _DivCol, _Row, _Col);
}
void GlobalDebug::DrawRenderTarget(SPTR<CCamera> _Camera
	, const UINT& _DivRow, const UINT& _DivCol
	, const UINT& _Row, const UINT& _Col) 
{
	if (nullptr == _Camera)
		return;

	return DrawRenderTarget(_Camera->GetCamTarget(0), _DivRow, _DivCol, _Row, _Col);
}

