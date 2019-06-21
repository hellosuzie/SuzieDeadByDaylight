#pragma once
#include "ReferenceBase.h"
#include "DXHeader.h"
#include "DXStruct.h"



typedef class GlobalDebug
{
private:
	friend class SSDCore;

public:
	static bool												bDebugMode;


private:
	static SPTR<class CMesh>								Debug2D_Mesh;
	static SPTR<class CMaterial>							Debug2D_Material;

	static SPTR<CMesh>										DebugSphere_Mesh;
	static SPTR<CMaterial>									Debug3D_Material;

	static SPTR<CMaterial>									DebugTarget_Material;

	static CBUFFER::RenBaseBuffer							Debug3DCB;
private:
	static void InitGlobalDebug();
	static void ReleaseGlobalDebug();

public:
	static void DrawDebugRect(const Mat& _VP,const Vec3& _Pos, const Vec3& _Size,const Vec3& _Color = Vec3::One3, const float& _BorderSize = 2.f);
	static void DrawDebugCircle(const Mat& _VP, const Vec3& _Pos, const Vec3& _Size, const Vec3& _Color = Vec3::One3, const float& _BorderSize = 2.f);
	
	static void DrawDebugSphere(const Mat& _VP, const Vec3& _Pos, const float& _Radius, const Vec4& _Color = Vec4::White);
	static void DrawDebugTriangle(const Mat& _VP, const Vec3& _Tri1, const Vec3& _Tri2, const Vec3& _Tri3, const Vec3& _Color = Vec3::One3);
	
	static void DrawRenderTarget(SPTR<class RenderTarget> _Target
		, const UINT& _DivRow, const UINT& _DivCol
		, const UINT& _Row, const UINT& _Col);

	static void DrawRenderTarget(const wchar_t* _TargetKey
		, const UINT& _DivRow, const UINT& _DivCol
		, const UINT& _Row, const UINT& _Col);

	static void DrawRenderTarget(SPTR<class CCamera> _Camera
		, const UINT& _DivRow, const UINT& _DivCol
		, const UINT& _Row, const UINT& _Col);
public:
	//static void DrawString(const wchar_t* _FontKey, const wchar_t* _Text, const float& _FontSize, const Vec2& _Pos, const Vec4& Color, const enum FW1_TEXT_FLAG& _Align = FW1_TEXT_FLAG::FW1_TOP);


public:
	static void Update();

private:
	GlobalDebug() {}
	virtual ~GlobalDebug() = 0 {}
}SSDDebug;

