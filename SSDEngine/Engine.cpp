#include "Engine.h"
#include "SSDCore.h"
#include "ResourceMgr.h"
#include "DXStruct.h"
#include "Mesh.h"
#include "Material.h"
#include "VtxShader.h"
#include "GeoShader.h"
#include "PixShader.h"
#include "ComShader.h"
#include "ConstantBuffer.h"
#include "RenderTarget.h"
#include "MultiRenderTarget.h"
#include "DXVertex.h"
#include "SamplerState.h"
#include "BlendState.h"
#include "RasterizerState.h"
#include "DepthStencilState.h"
#include "Device.h"
#include "FilePathMgr.h"
#include "DXFont.h"
#include "DXMath.h"
#include "Renderer.h"
#include "Scene.h"
#include "FilterEffect.h"
#include "SSDRandom.h"
#include "NavigationMesh.h"

std::unordered_map<UINT, CNavigationMesh*>				Engine::mapNavigationMesh;


void Engine::CreateEngineGraphicResource()
{
	Create2DResource();
	Create3DResource();
	CreateBaseState();
	CreateRenderTarget();
	CreateMaterial();
}

void Engine::Create2DResource()
{
	// 기본 메쉬 생성
	CMesh::CREATE_DESC Desc{ DXVTX::ColorVtx ,sizeof(COLORVTX) ,4 ,D3D11_USAGE_DYNAMIC
		, DXVTX::BasicIdx ,IDX16::MemSize() ,6 ,D3D11_USAGE_DYNAMIC ,IDX16::Format() ,D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
	CResourceMgr<CMesh>::Create(L"ColorMesh", Desc);

	Desc = { DXVTX::BasicVtx ,sizeof(BASICVTX) , 4 ,D3D11_USAGE_DYNAMIC
		, DXVTX::BasicIdx ,IDX16::MemSize() ,6 ,D3D11_USAGE_DYNAMIC ,IDX16::Format() ,D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
	CResourceMgr<CMesh>::Create(L"2DMesh", Desc);

	Desc = { DXVTX::TargetViewVtx ,sizeof(POSUVVTX) , 4 ,D3D11_USAGE_DYNAMIC
	, DXVTX::BasicIdx ,IDX16::MemSize() ,6 ,D3D11_USAGE_DYNAMIC ,IDX16::Format() ,D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
	CResourceMgr<CMesh>::Create(L"TargetViewMesh", Desc);


	//상수 버퍼 생성
	SPTR<CConstantBuffer> Buff = CResourceMgr<CConstantBuffer>::Create(L"RenBaseBuffer",
	{ sizeof(CBUFFER::RenBaseBuffer),D3D11_USAGE::D3D11_USAGE_DYNAMIC,D3D11_CPU_ACCESS_WRITE
		,SHADER_ALL ,BASESLOT });
	Buff->SetBuffDataType<CBUFFER::RenBaseBuffer>();
	CDevice::RenBaseCB = Buff;

	Buff = CResourceMgr<CConstantBuffer>::Create(L"Sprite2DBuffer",
	{ sizeof(CBUFFER::Sprite2DBuffer),D3D11_USAGE::D3D11_USAGE_DYNAMIC,D3D11_CPU_ACCESS_WRITE });
	Buff->SetBuffDataType<CBUFFER::Sprite2DBuffer>();

	Buff = CResourceMgr<CConstantBuffer>::Create(L"RectBuffer",
	{ sizeof(IVec4),D3D11_USAGE::D3D11_USAGE_DYNAMIC,D3D11_CPU_ACCESS_WRITE , SHADER_PIXEL , 0 });
	Buff->SetBuffDataType<IVec4>(IVec4(1, 0, 0));

	Buff = CResourceMgr<CConstantBuffer>::Create(L"Base2DBuffer",
	{ sizeof(CBUFFER::Base2DBuffer),D3D11_USAGE::D3D11_USAGE_DYNAMIC,D3D11_CPU_ACCESS_WRITE , SHADER_VERTEX | SHADER_PIXEL , 0 });
	Buff->SetBuffDataType<CBUFFER::Base2DBuffer>();

	Buff = CResourceMgr<CConstantBuffer>::Create(L"GridBuffer",
	{ sizeof(CBUFFER::GridBuffer),D3D11_USAGE::D3D11_USAGE_DYNAMIC,D3D11_CPU_ACCESS_WRITE , SHADER_PIXEL , 0 });
	Buff->SetBuffDataType<CBUFFER::GridBuffer>();

	Buff = CResourceMgr<CConstantBuffer>::Create(L"DebugBuffer",
	{ sizeof(CBUFFER::DebugBuffer),D3D11_USAGE::D3D11_USAGE_DYNAMIC,D3D11_CPU_ACCESS_WRITE , SHADER_VERTEX | SHADER_PIXEL , 0 });
	Buff->SetBuffDataType<CBUFFER::DebugBuffer>();

	Buff = CResourceMgr<CConstantBuffer>::Create(L"DebugTargetBuffer",
	{ sizeof(CBUFFER::DebugTargetBuffer),D3D11_USAGE::D3D11_USAGE_DYNAMIC,D3D11_CPU_ACCESS_WRITE , SHADER_VERTEX , 0 });
	Buff->SetBuffDataType<CBUFFER::DebugTargetBuffer>();

	Buff = CResourceMgr<CConstantBuffer>::Create(L"LightBuffer",
	{ sizeof(CBUFFER::LightBuffer),D3D11_USAGE::D3D11_USAGE_DYNAMIC,D3D11_CPU_ACCESS_WRITE , SHADER_PIXEL , 0 });
	Buff->SetBuffDataType<CBUFFER::LightBuffer>();

	Buff = CResourceMgr<CConstantBuffer>::Create(L"GaussianBuffer",
		{ sizeof(CBUFFER::GaussianBuffer),D3D11_USAGE::D3D11_USAGE_DYNAMIC,D3D11_CPU_ACCESS_WRITE , SHADER_PIXEL , 0 });
	Buff->SetBuffDataType<CBUFFER::GaussianBuffer>();

	Buff = CResourceMgr<CConstantBuffer>::Create(L"MinimizeBuffer",
		{ sizeof(CBUFFER::MinimizeBuffer),D3D11_USAGE::D3D11_USAGE_DYNAMIC,D3D11_CPU_ACCESS_WRITE , SHADER_PIXEL , 0 });
	Buff->SetBuffDataType<CBUFFER::MinimizeBuffer>();

	Buff = CResourceMgr<CConstantBuffer>::Create(L"BloomBuffer",
		{ sizeof(CBUFFER::BloomBuffer),D3D11_USAGE::D3D11_USAGE_DYNAMIC,D3D11_CPU_ACCESS_WRITE , SHADER_PIXEL , 0 });
	Buff->SetBuffDataType<CBUFFER::BloomBuffer>();

	Buff = CResourceMgr<CConstantBuffer>::Create(L"FogBuffer",
		{ sizeof(CBUFFER::FogBuffer),D3D11_USAGE::D3D11_USAGE_DYNAMIC,D3D11_CPU_ACCESS_WRITE , SHADER_PIXEL , 0 });
	Buff->SetBuffDataType<CBUFFER::FogBuffer>();

	Buff = CResourceMgr<CConstantBuffer>::Create(L"TerrainBuffer",
		{ sizeof(CBUFFER::TerrainBuffer),D3D11_USAGE::D3D11_USAGE_DYNAMIC,D3D11_CPU_ACCESS_WRITE , SHADER_PIXEL  | SHADER_GEOMETRY, 0 });
	Buff->SetBuffDataType<CBUFFER::TerrainBuffer>();

	Buff = CResourceMgr<CConstantBuffer>::Create(L"GizmoBuffer",
	{ sizeof(CBUFFER::GizmoBuffer),D3D11_USAGE::D3D11_USAGE_DYNAMIC,D3D11_CPU_ACCESS_WRITE , SHADER_PIXEL| SHADER_VERTEX  , 0 });
	Buff->SetBuffDataType<CBUFFER::GizmoBuffer>();

	Buff = CResourceMgr<CConstantBuffer>::Create(L"SkinningBuffer",
	{ sizeof(CBUFFER::SkinningBuffer),D3D11_USAGE::D3D11_USAGE_DYNAMIC,D3D11_CPU_ACCESS_WRITE , SHADER_PIXEL | SHADER_VERTEX  , 0 });
	Buff->SetBuffDataType<CBUFFER::SkinningBuffer>();

	Buff = CResourceMgr<CConstantBuffer>::Create(L"NaviAreaBuffer",
	{ sizeof(CBUFFER::NaviAreaBuffer),D3D11_USAGE::D3D11_USAGE_DYNAMIC,D3D11_CPU_ACCESS_WRITE , SHADER_PIXEL | SHADER_VERTEX  , 0 });
	Buff->SetBuffDataType<CBUFFER::NaviAreaBuffer>();

	Buff = CResourceMgr<CConstantBuffer>::Create(L"BaseFbxBuffer",
		{ sizeof(CBUFFER::BaseFbxBuffer),D3D11_USAGE::D3D11_USAGE_DYNAMIC,D3D11_CPU_ACCESS_WRITE , SHADER_PIXEL | SHADER_VERTEX  , 0 });
	Buff->SetBuffDataType<CBUFFER::BaseFbxBuffer>();

	Buff = CResourceMgr<CConstantBuffer>::Create(L"MaterialColor",
		{ sizeof(CBUFFER::MaterialColor),D3D11_USAGE::D3D11_USAGE_DYNAMIC,D3D11_CPU_ACCESS_WRITE , SHADER_PIXEL | SHADER_VERTEX  , 0 });
	Buff->SetBuffDataType<CBUFFER::MaterialColor>();

	Buff = CResourceMgr<CConstantBuffer>::Create(L"ForwardLightBuffer",
		{ sizeof(CBUFFER::ForwardLightBuffer),D3D11_USAGE::D3D11_USAGE_DYNAMIC,D3D11_CPU_ACCESS_WRITE , SHADER_PIXEL | SHADER_VERTEX  , 0 });
	Buff->SetBuffDataType<CBUFFER::ForwardLightBuffer>();

	Buff = CResourceMgr<CConstantBuffer>::Create(L"DissolveBuffer",
	{ sizeof(CBUFFER::DissolveBuffer),D3D11_USAGE::D3D11_USAGE_DYNAMIC,D3D11_CPU_ACCESS_WRITE , SHADER_PIXEL | SHADER_VERTEX  , 0 });
	Buff->SetBuffDataType<CBUFFER::DissolveBuffer>();

	Buff = CResourceMgr<CConstantBuffer>::Create(L"ParticleBuffer",
		{ sizeof(CBUFFER::ParticleBuffer),D3D11_USAGE::D3D11_USAGE_DYNAMIC,D3D11_CPU_ACCESS_WRITE , SHADER_ALL , 0 });
	Buff->SetBuffDataType<CBUFFER::ParticleBuffer>();

	Buff = CResourceMgr<CConstantBuffer>::Create(L"DecalBuffer",
		{ sizeof(CBUFFER::DecalBuffer),D3D11_USAGE::D3D11_USAGE_DYNAMIC,D3D11_CPU_ACCESS_WRITE , SHADER_ALL , 0 });
	Buff->SetBuffDataType<CBUFFER::DecalBuffer>();

	// 버텍스 쉐이더 생성
	//2D
	CVtxShader::LOAD_DESC VtxDesc{ 5,0,"VS_Main" ,true };
	SPTR<CVtxShader> NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Sprite2D.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);
	NewVS->PushConstantBuffer(0, L"Sprite2DBuffer");

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Base2D.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);
	NewVS->PushConstantBuffer(0, L"Base2DBuffer");

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"UIBase.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);

	//3D
	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"ColorBase3D.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Debug3D.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"DebugTarget.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"DeferredLight.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"MergeCamera.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"UIMerge.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Base3D.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Skybox.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"SkyboxCube.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Grid.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushInputDesc("COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Gizmo.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushInputDesc("COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"VolumeLight.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"TargetView.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Gaussian.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Minimize.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"DepthOfField.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Bloom.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"BloomMerge.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Fog.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"BaseFBX.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);
	NewVS->PushInputDesc("COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushInputDesc("NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("BINORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"BaseFBXInst.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);
	NewVS->PushInputDesc("COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushInputDesc("NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("BINORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("WORLD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	NewVS->PushInputDesc("WORLD", 1, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	NewVS->PushInputDesc("WORLD", 2, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	NewVS->PushInputDesc("WORLD", 3, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	NewVS->PushInputDesc("WV", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	NewVS->PushInputDesc("WV", 1, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	NewVS->PushInputDesc("WV", 2, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	NewVS->PushInputDesc("WV", 3, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	NewVS->PushInputDesc("WVP", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	NewVS->PushInputDesc("WVP", 1, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	NewVS->PushInputDesc("WVP", 2, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	NewVS->PushEndInputDesc("WVP", 3, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"ForwardBaseFBX.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);
	NewVS->PushInputDesc("COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushInputDesc("NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("BINORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"WireFBX.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);
	NewVS->PushInputDesc("COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushInputDesc("NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("BINORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("WEIGHT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("INDICES", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);


	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Skeletal.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);
	NewVS->PushInputDesc("COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushInputDesc("NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("BINORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("WEIGHT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("INDICES", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_SINT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"DissolveSkeletal.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);
	NewVS->PushInputDesc("COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushInputDesc("NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("BINORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("WEIGHT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("INDICES", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_SINT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Terrain.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);
	NewVS->PushInputDesc("COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushInputDesc("NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("BINORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"DebugTerrain.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);
	NewVS->PushInputDesc("HEIGHT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"NaviArea.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"NaviTri.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION",0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0,0);
	NewVS->PushInputDesc("POSITION", 1, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushInputDesc("POSITION", 2, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Triangle.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushInputDesc("POSITION", 1, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushInputDesc("POSITION", 2, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"GeoTerrain.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);
	NewVS->PushInputDesc("HEIGHT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Particle.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("VIEWPOSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("TEXNUMBER", 0, DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT, 0, 0);
	NewVS->PushInputDesc("SIZEX", 0, DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT, 0, 0);
	NewVS->PushInputDesc("SIZEY", 0, DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT, 0, 0);
	NewVS->PushInputDesc("PARTICLETIME", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);
	NewVS->PushInputDesc("CHECK", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	NewVS->PushInputDesc("AXISX", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("AXISY", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("ANIMUV", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);

	NewVS = CResourceMgr<CVtxShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Decal.fx"), VtxDesc);
	NewVS->PushInputDesc("POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushInputDesc("NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	NewVS->PushEndInputDesc("TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0);

	// 기하 쉐이더 생성
	CGeoShader::LOAD_DESC GeoDesc{ 5,0,"GS_Main" ,true };

	SPTR<CGeoShader> NewGS = CResourceMgr<CGeoShader>::Load(CFilePathMgr::GetPath(L"Shader", L"DebugTerrain.fx"), GeoDesc);
	NewGS = CResourceMgr<CGeoShader>::Load(CFilePathMgr::GetPath(L"Shader", L"GeoTerrain.fx"), GeoDesc);
	NewGS = CResourceMgr<CGeoShader>::Load(CFilePathMgr::GetPath(L"Shader", L"NaviTri.fx"), GeoDesc);
	NewGS = CResourceMgr<CGeoShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Triangle.fx"), GeoDesc);
	NewGS = CResourceMgr<CGeoShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Particle.fx"), GeoDesc);


	// 픽셀 쉐이더 생성
	CPixShader::LOAD_DESC PixDesc = { 5,0,"PS_Main" ,true };
	SPTR<CPixShader> NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Sprite2D.fx"), PixDesc);

	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Base2D.fx"), PixDesc);
	NewPS->PushConstantBuffer(0, L"Base2DBuffer");
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"UIBase.fx"), PixDesc);

	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"ColorBase3D.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Base3D.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Debug3D.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"DeferredLight.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"MergeCamera.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"UIMerge.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"DebugTarget.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Skybox.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"SkyboxCube.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Grid.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Gizmo.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"TargetView.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"BaseFBX.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"BaseFBXInst.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"ForwardBaseFBX.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"WireFBX.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Skeletal.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"DissolveSkeletal.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"VolumeLight.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Gaussian.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Minimize.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Bloom.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"DepthOfField.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"BloomMerge.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Fog.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Terrain.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"GeoTerrain.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"DebugTerrain.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"NaviArea.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"NaviTri.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Triangle.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Particle.fx"), PixDesc);
	NewPS = CResourceMgr<CPixShader>::Load(CFilePathMgr::GetPath(L"Shader", L"Decal.fx"), PixDesc);

	// 기본 폰트 생성
	CResourceMgr<CDXFont>::Create(L"굴림");
	CResourceMgr<CDXFont>::Create(L"바탕");
	CResourceMgr<CDXFont>::Create(L"HYPMokGak 굵게");
	CResourceMgr<CDXFont>::Create(L"나눔바른펜");
	CResourceMgr<CDXFont>::Create(L"나눔스퀘어");
} 

void Engine::Create3DResource()
{
	CMesh::CREATE_DESC Desc = { DXVTX::CubeVtx ,sizeof(BASICVTX) , 24 ,D3D11_USAGE_DYNAMIC
		, DXVTX::ColorCubeIdx ,IDX16::MemSize() ,36 ,D3D11_USAGE_DYNAMIC ,IDX16::Format() ,D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
	CResourceMgr<CMesh>::Create(L"Cube", Desc);
	
	Desc = { DXVTX::FrustumVtx ,sizeof(BASICVTX) , 24 ,D3D11_USAGE_DYNAMIC
		, DXVTX::ColorCubeIdx ,IDX16::MemSize() ,36 ,D3D11_USAGE_DYNAMIC ,IDX16::Format() ,D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
	CResourceMgr<CMesh>::Create(L"FrustumCube", Desc);

	CreateSphereMesh(2);
	CreateSphereMesh(4);
	CreateSphereMesh(8);
	CreateSphereMesh(16);
	CreateSphereMesh(32);
	CreateSphereMesh(64);
	CreateConeMesh(6);
	CreateConeMesh(12);
	CreateConeMesh(360);


	CreateGeosphere(1);
	CreateGeosphere(2);
	CreateGeosphere(3);
	CreateGeosphere(4);

	CreateCylinder(8);
	CreateCylinder(45);
	CreateCylinder(90);
	CreateCylinder(180);
	CreateCylinder(360);


	CreateGizmoMesh(10.f, 200.f);
	
	CreateFrameCube(0.98f, 1.f, 1.f);
	CreateTubeMesh(0.49f, 0.5f, 1.f);
}

void Engine::CreateBaseState()
{
	// 기본 샘플러 스테이트 생성
	CSamplerState::CREATE_DESC Desc;

	Desc.SampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	Desc.SampleDesc.MaxAnisotropy = 1;
	Desc.SampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	Desc.SampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	Desc.SampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	Desc.SampleDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

	for (size_t i = 0; i < 4; i++)
		Desc.SampleDesc.BorderColor[i] = 0.f;

	Desc.SampleDesc.MipLODBias = 0;
	Desc.SampleDesc.MinLOD = -3.402823466e+38f;
	Desc.SampleDesc.MaxLOD = 3.402823466e+38f;

	Desc.SampleDesc.MaxAnisotropy = 0;

	CResourceMgr<CSamplerState>::Create(POINTSAMPLER, Desc);

	Desc.SampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	CResourceMgr<CSamplerState>::Create(LINEARSAMPLER, Desc);

	Desc.SampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	Desc.SampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	Desc.SampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	CResourceMgr<CSamplerState>::Create(LINEARCLAMPSAMPLER, Desc);


	// 기본 블랜드 스테이트 생성
	D3D11_BLEND_DESC	BlendDesc;
	BlendDesc.AlphaToCoverageEnable = false;
	BlendDesc.IndependentBlendEnable = false;
	BlendDesc.RenderTarget[0].BlendEnable = true;
	// RGB 색상 혼합에 관련된 블렌드 펙터를 설정
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	// Alpha 값의 혼합에 관련된 블렌드 펙터를 설정
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	CBlendState::CREATE_DESC BDesc{ BlendDesc };
	CDevice::BaseBS = CResourceMgr<CBlendState>::Create(L"AlphaBlend", BDesc);


	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND::D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND::D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
	
	BlendDesc.RenderTarget[1].BlendEnable = TRUE;
	BlendDesc.RenderTarget[1].SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_COLOR;
	BlendDesc.RenderTarget[1].DestBlend = D3D11_BLEND::D3D11_BLEND_DEST_COLOR;
	BlendDesc.RenderTarget[1].SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[1].DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;

	BDesc.BlendDesc = BlendDesc;
	CResourceMgr<CBlendState>::Create(L"AccBlend", BDesc);

	// RGB 색상 혼합에 관련된 블렌드 펙터를 설정
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_COLOR;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND::D3D11_BLEND_BLEND_FACTOR;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_MIN;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	BDesc.BlendDesc = BlendDesc;
	CResourceMgr<CBlendState>::Create(L"MinBlend", BDesc);


	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_MAX;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND::D3D11_BLEND_DEST_COLOR;
	BDesc.BlendDesc = BlendDesc;
	CResourceMgr<CBlendState>::Create(L"MaxBlend", BDesc);


	BlendDesc.RenderTarget[0].BlendEnable = false;
	BDesc.BlendDesc = BlendDesc;
	CResourceMgr<CBlendState>::Create(L"DisableBlend", BDesc);

	BlendDesc.AlphaToCoverageEnable = true;
	BlendDesc.IndependentBlendEnable = false;
	BlendDesc.RenderTarget[0].BlendEnable = false;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	BDesc.BlendDesc = BlendDesc;
	CResourceMgr<CBlendState>::Create(L"AlphatoCoverageBlend", BDesc);

	// 기본 깊이 - 스탠실 스테이트 생성
	CDepthStencilState::CREATE_DESC DSDesc;


	DSDesc.DepthStencilDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
	DSDesc.DepthStencilDesc.DepthEnable = FALSE;
	DSDesc.DepthStencilDesc.StencilEnable = FALSE;
	DSDesc.DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	DSDesc.DepthStencilDesc.StencilEnable = FALSE;

	CDevice::DisableDS = CResourceMgr<CDepthStencilState>::Create(L"DisableDepth", DSDesc);

	DSDesc.DepthStencilDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
	DSDesc.DepthStencilDesc.DepthEnable = TRUE;
	DSDesc.DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	DSDesc.DepthStencilDesc.StencilEnable = TRUE;
	CDevice::BaseDS = CResourceMgr<CDepthStencilState>::Create(L"BaseDepth", DSDesc);

	DSDesc.DepthStencilDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
	DSDesc.DepthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	CResourceMgr<CDepthStencilState>::Create(L"IgnoreDepth", DSDesc);

	// 후방 깊이 검출용 뎁스 스텐실 스테이트 (볼륨메쉬 의 전면부 메쉬의 뎁스 스탠실 스테이트 )
	DSDesc.DepthStencilDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
	DSDesc.DepthStencilDesc.DepthEnable = TRUE;
	DSDesc.DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	DSDesc.DepthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER;

	DSDesc.DepthStencilDesc.StencilEnable = TRUE;
	DSDesc.DepthStencilDesc.StencilReadMask = 0xff;
	DSDesc.DepthStencilDesc.StencilWriteMask= 0xff;

	D3D11_DEPTH_STENCILOP_DESC LightStencil;
	LightStencil.StencilFunc = D3D11_COMPARISON_ALWAYS;
	LightStencil.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	LightStencil.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	LightStencil.StencilPassOp = D3D11_STENCIL_OP_REPLACE;

	DSDesc.DepthStencilDesc.FrontFace = LightStencil;
	DSDesc.DepthStencilDesc.BackFace = LightStencil;
	CResourceMgr<CDepthStencilState>::Create(L"BackDepthExtraction", DSDesc);

	// 전방 깊이 검출용 뎁스 스텐실 스테이트 (볼륨메쉬 의 후면부 메쉬의 뎁스 스탠실 스테이트 )
	DSDesc.DepthStencilDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
	DSDesc.DepthStencilDesc.DepthEnable = TRUE;
	DSDesc.DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	DSDesc.DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	DSDesc.DepthStencilDesc.StencilEnable = TRUE;
	DSDesc.DepthStencilDesc.StencilReadMask = 0xff;
	DSDesc.DepthStencilDesc.StencilWriteMask = 0xff;

	LightStencil.StencilFunc = D3D11_COMPARISON_EQUAL;
	LightStencil.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
	LightStencil.StencilFailOp = D3D11_STENCIL_OP_ZERO;
	LightStencil.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	DSDesc.DepthStencilDesc.FrontFace = LightStencil;
	DSDesc.DepthStencilDesc.BackFace = LightStencil;
	CResourceMgr<CDepthStencilState>::Create(L"FrontDepthExtraction", DSDesc);

	// 스텐실버퍼가 1로 쓰여져있는 픽셀만 통과시키는 스텐실 스테이트(볼륨메쉬 검출용)
	DSDesc.DepthStencilDesc = CD3D11_DEPTH_STENCIL_DESC();
	DSDesc.DepthStencilDesc.DepthEnable = TRUE;
	DSDesc.DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	DSDesc.DepthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;

	DSDesc.DepthStencilDesc.StencilEnable = TRUE;
	DSDesc.DepthStencilDesc.StencilReadMask = 0xff;
	DSDesc.DepthStencilDesc.StencilWriteMask = 0xff;

	LightStencil.StencilFunc = D3D11_COMPARISON_EQUAL;
	LightStencil.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
	LightStencil.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	LightStencil.StencilPassOp = D3D11_STENCIL_OP_ZERO;

	DSDesc.DepthStencilDesc.FrontFace = LightStencil;
	DSDesc.DepthStencilDesc.BackFace = LightStencil;
	CResourceMgr<CDepthStencilState>::Create(L"PassDepth", DSDesc);

	//깊이버퍼를 저장하지만 모든 픽셀을 통과시키는 스테이트
	DSDesc.DepthStencilDesc = CD3D11_DEPTH_STENCIL_DESC();
	DSDesc.DepthStencilDesc.DepthEnable = TRUE;
	DSDesc.DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	DSDesc.DepthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;

	DSDesc.DepthStencilDesc.StencilEnable = FALSE;
	DSDesc.DepthStencilDesc.StencilReadMask = 0xff;
	DSDesc.DepthStencilDesc.StencilWriteMask = 0xff;

	DSDesc.DepthStencilDesc.FrontFace = LightStencil;
	DSDesc.DepthStencilDesc.BackFace = LightStencil;
	CResourceMgr<CDepthStencilState>::Create(L"PostDepth", DSDesc);

	//레스터라이저 모드
	CRasterizerState::CREATE_DESC RSDest;
	RSDest.ResterizerDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
	RSDest.ResterizerDesc.CullMode = D3D11_CULL_BACK;
	RSDest.ResterizerDesc.FillMode = D3D11_FILL_SOLID;
	SPTR<CRasterizerState> NewRS = CResourceMgr<CRasterizerState>::Create(L"BackCulling", RSDest);
	CDevice::BaseRS = NewRS;

	RSDest.ResterizerDesc.CullMode = D3D11_CULL_NONE;
	RSDest.ResterizerDesc.FillMode = D3D11_FILL_SOLID;
	NewRS = CResourceMgr<CRasterizerState>::Create(L"NoneCulling", RSDest);

	RSDest.ResterizerDesc.CullMode = D3D11_CULL_FRONT;
	RSDest.ResterizerDesc.FillMode = D3D11_FILL_SOLID;
	NewRS = CResourceMgr<CRasterizerState>::Create(L"FrontCulling", RSDest);

	RSDest.ResterizerDesc.CullMode = D3D11_CULL_NONE;
	RSDest.ResterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	NewRS = CResourceMgr<CRasterizerState>::Create(L"WireFrame", RSDest);

	CDevice::SetBaseRS();
	CDevice::SetBaseDS();
	CDevice::SetBaseBlend();
}

void Engine::CreateMaterial()
{
	// 메테리얼 생성
	CMaterial::CREATE_DESC MatDesc;
	SPTR<CMaterial> NewMaterial = CResourceMgr<CMaterial>::Create(L"ColorBase3D", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"ColorBase3D");

	NewMaterial = CResourceMgr<CMaterial>::Create(L"Base2D", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"Base2D");
	NewMaterial->PushConstantBuffer(L"MainBuffer", L"Base2DBuffer");

	NewMaterial = CResourceMgr<CMaterial>::Create(L"UIBase", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"UIBase");
	NewMaterial->PushSampler(0, LINEARSAMPLER);
	NewMaterial->SetBlend(L"AlphaBlend");


	NewMaterial = CResourceMgr<CMaterial>::Create(L"Base3D", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"Base3D");
	//NewMaterial->DeferredOn();

	NewMaterial = CResourceMgr<CMaterial>::Create(L"Gizmo", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"Gizmo");
	NewMaterial->PushConstantBuffer(L"Gizmo", L"GizmoBuffer");

	NewMaterial = CResourceMgr<CMaterial>::Create(L"Debug3D", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"Debug3D");
	NewMaterial->PushConstantBuffer(L"DebugBuffer", L"DebugBuffer");
	NewMaterial->SetRasterizerState(L"WireFrame");

	NewMaterial = CResourceMgr<CMaterial>::Create(L"DebugTarget", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"DebugTarget");
	NewMaterial->SetDepthStancilState(L"IgnoreDepth");
	NewMaterial->PushConstantBuffer(L"DebugBuffer", L"DebugTargetBuffer");
	NewMaterial->PushSampler(0, LINEARSAMPLER);

	NewMaterial = CResourceMgr<CMaterial>::Create(L"Skybox", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"Skybox");
	NewMaterial->SetRasterizerState(L"FrontCulling");

	NewMaterial = CResourceMgr<CMaterial>::Create(L"SkyboxCube", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"SkyboxCube");
	NewMaterial->SetRasterizerState(L"NoneCulling");

	NewMaterial = CResourceMgr<CMaterial>::Create(L"GridMaterial", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"Grid");
	NewMaterial->SetRasterizerState(L"NoneCulling");
	NewMaterial->PushConstantBuffer(L"GridBuffer", L"GridBuffer");

	NewMaterial = CResourceMgr<CMaterial>::Create(L"TargetMaterial", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"TargetView");
	NewMaterial->PushSampler(0, LINEARSAMPLER);

	NewMaterial = CResourceMgr<CMaterial>::Create(L"BaseFBXMaterial", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"BaseFBX");
	NewMaterial->PushConstantBuffer(L"MaterialColor", L"MaterialColor");
	NewMaterial->PushSampler(0, LINEARSAMPLER);
	NewMaterial->DeferredOn();
	NewMaterial->SetBlend(L"AlphatoCoverageBlend");

	NewMaterial = CResourceMgr<CMaterial>::Create(L"BaseFBXInstMaterial", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"BaseFBXInst");
	NewMaterial->PushConstantBuffer(L"MaterialColor", L"MaterialColor");
	NewMaterial->PushSampler(0, LINEARSAMPLER);
	NewMaterial->DeferredOn();
	NewMaterial->SetBlend(L"AlphatoCoverageBlend");

	NewMaterial = CResourceMgr<CMaterial>::Create(L"ForwardFBXMaterial", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"ForwardBaseFBX");
	NewMaterial->PushConstantBuffer(L"ForwardLightBuffer", L"ForwardLightBuffer");
	NewMaterial->PushSampler(0, LINEARSAMPLER);
	NewMaterial->DeferredOff();
	NewMaterial->PushTexture(4, L"Depth", SHADER_PIXEL, TARGET_TEXTURE);
	//NewMaterial->SetBlend(L"AccBlend");

	NewMaterial = CResourceMgr<CMaterial>::Create(L"WireFBXMaterial", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"WireFBX");
	NewMaterial->SetRasterizerState(L"WireFrame");

	NewMaterial = CResourceMgr<CMaterial>::Create(L"NaviAreaMaterial", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"NaviArea");
	NewMaterial->PushConstantBuffer(L"NaviBuffer",L"NaviAreaBuffer");

	NewMaterial = CResourceMgr<CMaterial>::Create(L"NaviTriMaterial", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL | SHADER_GEOMETRY, L"NaviTri");
	

	NewMaterial = CResourceMgr<CMaterial>::Create(L"TriangleMaterial", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL | SHADER_GEOMETRY, L"Triangle");
	NewMaterial->SetRasterizerState(L"NoneCulling");

	NewMaterial = CResourceMgr<CMaterial>::Create(L"DebugTerrain", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL | SHADER_GEOMETRY, L"DebugTerrain");

	NewMaterial = CResourceMgr<CMaterial>::Create(L"SkeletalMaterial", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"Skeletal");
	NewMaterial->PushSampler(0, LINEARSAMPLER);
	NewMaterial->DeferredOn();

	NewMaterial = CResourceMgr<CMaterial>::Create(L"DissolveSkeletal", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"DissolveSkeletal");
	NewMaterial->PushSampler(0, LINEARSAMPLER);
	NewMaterial->PushConstantBuffer(L"DissolveBuffer", L"DissolveBuffer");
	NewMaterial->DeferredOn();

	NewMaterial = CResourceMgr<CMaterial>::Create(L"DeferredLight", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"DeferredLight");
	NewMaterial->PushSampler(0, LINEARSAMPLER);
	NewMaterial->PushTexture(0, L"Position", SHADER_PIXEL, TARGET_TEXTURE);
	NewMaterial->PushTexture(1, L"Normal", SHADER_PIXEL, TARGET_TEXTURE);
	NewMaterial->PushTexture(2, L"Depth", SHADER_PIXEL, TARGET_TEXTURE);
	NewMaterial->PushTexture(3, L"Specular", SHADER_PIXEL, TARGET_TEXTURE);
	NewMaterial->PushTexture(4, L"Emissive", SHADER_PIXEL, TARGET_TEXTURE);
	NewMaterial->PushConstantBuffer(L"LightBuffer", L"LightBuffer");
	NewMaterial->SetBlend(L"AccBlend");

	NewMaterial = CResourceMgr<CMaterial>::Create(L"MergeCamera", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"MergeCamera");
	NewMaterial->PushSampler(0, LINEARSAMPLER);
	NewMaterial->PushTexture(0, L"Diffuse", SHADER_PIXEL, TARGET_TEXTURE);
	NewMaterial->PushTexture(1, L"LightDiffuse", SHADER_PIXEL, TARGET_TEXTURE);
	NewMaterial->PushTexture(2, L"LightSpecular", SHADER_PIXEL, TARGET_TEXTURE);
	NewMaterial->PushTexture(3, L"Emissive", SHADER_PIXEL, TARGET_TEXTURE);
	NewMaterial->PushTexture(4, L"EmvFactor", SHADER_PIXEL, TARGET_TEXTURE);

	NewMaterial = CResourceMgr<CMaterial>::Create(L"UIMerge", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"UIMerge");
	NewMaterial->PushSampler(0, LINEARSAMPLER);
	NewMaterial->SetBlend(L"AlphaBlend");

	NewMaterial = CResourceMgr<CMaterial>::Create(L"VolumeLight", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"VolumeLight");	
	
	NewMaterial = CResourceMgr<CMaterial>::Create(L"Gaussian", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"Gaussian");
	NewMaterial->PushConstantBuffer(L"GaussianBuffer", L"GaussianBuffer");
	NewMaterial->PushSampler(0, LINEARSAMPLER);

	NewMaterial = CResourceMgr<CMaterial>::Create(L"GaussianBloom", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"GaussianBloom");
	NewMaterial->PushConstantBuffer(L"GaussianBloomBuffer", L"GaussianBloomBuffer");
	NewMaterial->PushSampler(0, LINEARSAMPLER);

	NewMaterial = CResourceMgr<CMaterial>::Create(L"Bloom", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"Bloom");
	NewMaterial->PushSampler(0, LINEARCLAMPSAMPLER);
	NewMaterial->PushConstantBuffer(L"BloomBuffer", L"BloomBuffer");

	NewMaterial = CResourceMgr<CMaterial>::Create(L"BloomMerge", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"BloomMerge");
	NewMaterial->PushSampler(0, LINEARCLAMPSAMPLER);

	NewMaterial = CResourceMgr<CMaterial>::Create(L"Fog", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"Fog");
	NewMaterial->PushConstantBuffer(L"FogBuffer", L"FogBuffer");
	NewMaterial->PushSampler(0, LINEARSAMPLER);
	NewMaterial->PushTexture(1, L"Position", SHADER_PIXEL, TARGET_TEXTURE);
	NewMaterial->PushTexture(2, L"Depth", SHADER_PIXEL, TARGET_TEXTURE);

	NewMaterial = CResourceMgr<CMaterial>::Create(L"Terrain", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"Terrain");
	NewMaterial->PushSampler(0, LINEARSAMPLER);
	NewMaterial->PushConstantBuffer(L"TerrainBuffer", L"TerrainBuffer");
	NewMaterial->DeferredOn();

	NewMaterial = CResourceMgr<CMaterial>::Create(L"GeoTerrain", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL | SHADER_GEOMETRY, L"GeoTerrain");
	NewMaterial->PushSampler(0, LINEARSAMPLER);
	NewMaterial->PushConstantBuffer(L"TerrainBuffer", L"TerrainBuffer");
	NewMaterial->DeferredOn();

	NewMaterial = CResourceMgr<CMaterial>::Create(L"Particle", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL | SHADER_GEOMETRY, L"Particle");
	NewMaterial->PushTexture(0, L"Depth", SHADER_PIXEL, TARGET_TEXTURE);
	NewMaterial->PushTexture(0, L"Depth", SHADER_PIXEL, TARGET_TEXTURE);
	//NewMaterial->PushSampler(0, POINTSAMPLER);
	NewMaterial->PushSampler(0, LINEARCLAMPSAMPLER);
	NewMaterial->SetRasterizerState(L"NoneCulling");
	NewMaterial->PushConstantBuffer(L"ParticleBuffer", L"ParticleBuffer");
	NewMaterial->SetBlend(L"AlphaBlend");

	NewMaterial = CResourceMgr<CMaterial>::Create(L"Decal", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"Decal");
	NewMaterial->SetRasterizerState(L"NoneCulling");
	NewMaterial->PushTexture(0, L"Position", SHADER_PIXEL, TARGET_TEXTURE);
	NewMaterial->PushConstantBuffer(L"DecalBuffer", L"DecalBuffer");
	NewMaterial->PushSampler(0, LINEARSAMPLER);
	NewMaterial->DeferredOn();

	//축소버퍼
	NewMaterial = CResourceMgr<CMaterial>::Create(L"Minimize64", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"Minimize");
	NewMaterial->SetRasterizerState(L"NoneCulling");
	NewMaterial->PushSampler(0, LINEARSAMPLER);
	NewMaterial->PushConstantBuffer(L"MinimizeBuffer", L"MinimizeBuffer");
	NewMaterial->CreateViewport(64, 64);

	NewMaterial = CResourceMgr<CMaterial>::Create(L"Minimize128", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"Minimize");
	NewMaterial->SetRasterizerState(L"NoneCulling");
	NewMaterial->PushSampler(0, LINEARSAMPLER);
	NewMaterial->PushConstantBuffer(L"MinimizeBuffer", L"MinimizeBuffer");
	NewMaterial->CreateViewport(128, 128);

	NewMaterial = CResourceMgr<CMaterial>::Create(L"Minimize256", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"Minimize");
	NewMaterial->SetRasterizerState(L"NoneCulling");
	NewMaterial->PushSampler(0, LINEARSAMPLER);
	NewMaterial->PushConstantBuffer(L"MinimizeBuffer", L"MinimizeBuffer");
	NewMaterial->CreateViewport(256, 256);

	NewMaterial = CResourceMgr<CMaterial>::Create(L"Minimize512", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL, L"Minimize");
	NewMaterial->SetRasterizerState(L"NoneCulling");
	NewMaterial->PushSampler(0, LINEARSAMPLER);
	NewMaterial->PushConstantBuffer(L"MinimizeBuffer", L"MinimizeBuffer");
	NewMaterial->CreateViewport(512, 512);

	NewMaterial = CResourceMgr<CMaterial>::Create(L"DepthOfField", MatDesc);
	NewMaterial->SetShader(SHADER_VERTEX | SHADER_PIXEL , L"DepthOfField");
	NewMaterial->PushSampler(0, LINEARSAMPLER);
	NewMaterial->SetDepthStancilState(L"DisableDepth");

	CFilterEffect::MiniBufferInit();
}

void Engine::CreateRenderTarget()
{
	RT::CREATE_DESC RTDesc;
	RTDesc.Width = Core::GetWindowWidth();
	RTDesc.Height = Core::GetWindowHeight();
	RTDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	// 디퍼드에 쓰이는 랜더 타겟
	CResourceMgr<RT>::Create(L"Diffuse", RTDesc);
	CResourceMgr<RT>::Create(L"Position", RTDesc);
	CResourceMgr<RT>::Create(L"Normal", RTDesc);
	CResourceMgr<RT>::Create(L"Depth", RTDesc);
	CResourceMgr<RT>::Create(L"Specular", RTDesc);
	CResourceMgr<RT>::Create(L"Emissive", RTDesc);

	// 라이팅에 쓰이는 랜더 타겟
	CResourceMgr<RT>::Create(L"LightDiffuse", RTDesc);
	CResourceMgr<RT>::Create(L"LightSpecular", RTDesc);
	CResourceMgr<RT>::Create(L"EmvFactor", RTDesc);

	// 멀티 랜더타겟 생성
	//CResourceMgr<RT>::Create(L"Depth", RTDesc);
	CResourceMgr<MRT>::Create(L"Deferred", { 6 , L"Diffuse", L"Position", L"Normal", L"Depth", L"Specular", L"Emissive" });
	CResourceMgr<MRT>::Create(L"Lighting", { 3 , L"LightDiffuse", L"LightSpecular", L"EmvFactor" });
	CResourceMgr<MRT>::Create(L"Decal", { 2 , L"Diffuse", L"Normal"});

	RTDesc.Width = 64;
	RTDesc.Height = 64;
	RTDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	CResourceMgr<RT>::Create(L"Minimize64", RTDesc);
	CResourceMgr<MRT>::Create(L"Minimize64", { 1 , L"Minimize64" });
	RTDesc.Width = 128;
	RTDesc.Height = 128;
	RTDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	CResourceMgr<RT>::Create(L"Minimize128", RTDesc);
	CResourceMgr<MRT>::Create(L"Minimize128", { 1 , L"Minimize128" });
	RTDesc.Width = 256;
	RTDesc.Height = 256;
	RTDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	CResourceMgr<RT>::Create(L"Minimize256", RTDesc);
	CResourceMgr<MRT>::Create(L"Minimize256", { 1 , L"Minimize256" });
	RTDesc.Width = 512;
	RTDesc.Height = 512;
	RTDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	CResourceMgr<RT>::Create(L"Minimize512", RTDesc);
	CResourceMgr<MRT>::Create(L"Minimize512", { 1 , L"Minimize512" });

	CResourceMgr<MRT>::Create(L"Minimize", { 4 , L"Minimize512", L"Minimize256", L"Minimize128", L"Minimize64" });
}

void Engine::CreateSphereMesh(const int& _DetailLv)
{
	if (_DetailLv > 70)
		return;

	UINT Division = _DetailLv + 1;
	UINT SliceCount = _DetailLv + 2;
	float Theta = DXMath::FPI / (float)Division;

	BASICVTX*	AllVtx = new BASICVTX[SliceCount * (Division * 2 + 1)];
	IDX16*		AllIdx = new IDX16[Division * Division * 4];

	Mat Rotate;

	for (size_t Row = 0; Row <= 2 * Division; ++Row)
	{
		Rotate.SetRotateYMat((float)Row * Theta);

		for (size_t Colm = 0; Colm < SliceCount; ++Colm)
		{
			Vec2 Texcoord;
			Texcoord.x = (float)Row / (float)(2 * Division);
			Texcoord.y = (float)Colm / (float)Division;
			Vec3 Pos = Vec3(DXMath::Sin(Theta*(float)Colm), DXMath::Cos(Theta*(float)Colm), 0.f);
			Pos.Normalize();
			Pos *= Rotate;

			AllVtx[Row * SliceCount + Colm] = BASICVTX{ Pos, Pos, Texcoord };
		}
	}

	size_t IdxCount = 0;
	size_t NextRow = 0;

	for (WORD Row = 0; Row < 2 * Division; ++Row)
	{
		NextRow = Row + 1;
		for (WORD Colm = 0; Colm < Division; ++Colm)
		{
			WORD Idx[4];
			Idx[0] = (WORD)(Row * SliceCount + Colm);
			Idx[1] = (WORD)(Row * SliceCount + Colm + 1);
			Idx[2] = (WORD)(NextRow * SliceCount + Colm);
			Idx[3] = (WORD)(NextRow * SliceCount + Colm + 1);

			AllIdx[(Row * 2 * Division) + (2 * Colm)] = IDX16{ Idx[0], Idx[3], Idx[2] };
			AllIdx[(Row * 2 * Division) + (2 * Colm + 1)] = IDX16{ Idx[0], Idx[1], Idx[3] };
		}
	}

	CMesh::CREATE_DESC Desc{ AllVtx ,sizeof(BASICVTX) ,SliceCount * (Division * 2 + 1) ,D3D11_USAGE_DYNAMIC
		, AllIdx ,IDX16::MemSize() ,Division * Division * 12 ,D3D11_USAGE_DYNAMIC ,IDX16::Format() ,D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

	wchar_t MeshKey[256] = { 0, };

	std::wstring SphereKey = L"Sphere_Lv" + std::to_wstring(_DetailLv);
	CResourceMgr<CMesh>::Create(SphereKey.c_str(), Desc);

	delete[] AllVtx;
	delete[] AllIdx;
}

void Engine::CreateConeMesh(const int& _DetailLv)
{
	int EdgeNum = _DetailLv;

	Vec3 Top = VEC3(0.f, 1.f, 0.f);
	Vec3 Center = VEC3(0.f, 0.f, 0.f);

	float Angle = DXMath::DegToRad(30.f);
	float Radius = tanf(Angle);

	float OneAngle = 360.f / EdgeNum;

	std::vector<Vec3> CircleVec;
	CircleVec.push_back(Top);
	CircleVec.push_back(Center);

	for (float Theta = 0.f; Theta <= 360.f; Theta += OneAngle)
	{
		Vec3 TempVec = Vec3::Zero3;
		TempVec.x = cosf(DXMath::DegToRad((float)Theta)) * Radius;
		TempVec.y = 0.f;
		TempVec.z = sinf(DXMath::DegToRad((float)Theta)) * Radius;

		CircleVec.push_back(TempVec);
	}

	for (size_t i = 0; i < CircleVec.size(); ++i)
	{
		CircleVec[i] -= Vec3(0.f, 1.f, 0.f);
	}

	BASICVTX*	AllVtx = new BASICVTX[CircleVec.size()];
	IDX16*		AllIdx = new IDX16[EdgeNum * 2];

	for (size_t i = 0; i < CircleVec.size(); i++)
	{
		Vec2 Texcoord = Vec2(0.f, 1.f);

		Vec3 Pos = Vec3(CircleVec[i].x, CircleVec[i].y, CircleVec[i].z);

		int Snd = (int)i + 1;
		if (i == EdgeNum + 2)
		{
			Snd = 2;
		}
		Vec3 Left = CircleVec[i].GetNormalizeVec() - CircleVec[0];
		Vec3 Right = CircleVec[Snd].GetNormalizeVec() - CircleVec[0];

		Vec3 Normal = DXMath::Vector3Cross(Right, Left);

		AllVtx[i] = BASICVTX{ Pos, Normal, Texcoord };
	}

	std::vector<Vec3> IdxVec;
	for (int i = 0; i < EdgeNum; ++i)
	{
		IdxVec.push_back(Vec3(0, i + 3,  i + 2));
	}
	for (int j = 0; j < EdgeNum; ++j)
	{
		IdxVec.push_back(Vec3(1, j + 2, j + 3));
	}

	for (size_t i = 0; i < EdgeNum * 2; ++i)
	{
		AllIdx[i] = IDX16{ (WORD)IdxVec[i].ix, (WORD)IdxVec[i].iy, (WORD)IdxVec[i].iz };
	}

	CMesh::CREATE_DESC Desc{ AllVtx ,sizeof(BASICVTX) , (UINT)CircleVec.size(), D3D11_USAGE_DYNAMIC
	, AllIdx, IDX16::MemSize(), (UINT)(IdxVec.size() * 3), D3D11_USAGE_DYNAMIC, IDX16::Format() ,D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
	CResourceMgr<CMesh>::Create(L"Cone", Desc);

	wchar_t MeshKey[256] = { 0, };

	std::wstring SphereKey = L"Cone_Lv" + std::to_wstring(_DetailLv);
	CResourceMgr<CMesh>::Create(SphereKey.c_str(), Desc);

	delete[] AllVtx;
	delete[] AllIdx;
}

void Engine::CreateCylinder(const int& _DetailLv)
{
	UINT Theta = (UINT)DXMath::Clamp((360 / _DetailLv), 1, 90);
	UINT Division = 360 / Theta;

	std::wstring CylinderKey = L"Cylinder_Lv" + std::to_wstring(_DetailLv);

	if (nullptr != CResourceMgr<CMesh>::Find(CylinderKey))
		return;

	std::vector<COLORVTX> Vtx;
	std::vector<IDX16> Idx;

	Vtx.reserve((Division + 1) * 2);
	Idx.reserve(Division * 4);

	Vtx.push_back(COLORVTX{ Vec4(0.f,1.f,0.f,1.f),Vec4::White,Vec2() });
	Vtx.push_back(COLORVTX{ Vec4(0.f,-1.f,0.f,1.f),Vec4::White,Vec2() });

	WORD _11, _12, _21, _22;

	for (UINT i = 0; i < Division; ++i)
	{
		Vtx.push_back(COLORVTX{ Vec4(DXMath::CosA((float)i*Theta),1.f, DXMath::SinA((float)i*Theta),1.f),Vec4::White,Vec2() });
		Vtx.push_back(COLORVTX{ Vec4(DXMath::CosA((float)i*Theta),-1.f,  DXMath::SinA((float)i*Theta),1.f),Vec4::White,Vec2() });

		_11 = ((i + 1) * 2);
		_12 = ((i + 2) * 2);
		_21 = (((i + 1) * 2 + 1)) ;
		_22 = (((i + 2) * 2 + 1)) ;

		if (((i + 2) * 2) >= (Division + 1) * 2)
		{
			_12 =  2;
		}

		if (((i + 2) * 2 + 1) >= (Division + 1) * 2)
		{
			_22 =  3;
		}

		Idx.push_back(IDX16{ 0, _11 , _12 });
		Idx.push_back(IDX16{ 1 , _22 , _21 });
		Idx.push_back(IDX16{ _11 , _21 , _12 });
		Idx.push_back(IDX16{ _12 , _21 , _22 });
	}


	UINT VtxCount = (UINT)Vtx.size();
	UINT IdxCount = (UINT)Idx.size() * 3;
	CMesh::CREATE_DESC Desc{ &Vtx[0] ,sizeof(COLORVTX) , VtxCount ,D3D11_USAGE_DYNAMIC
		, &Idx[0] ,IDX16::MemSize() , IdxCount,D3D11_USAGE_DYNAMIC ,IDX16::Format() ,D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

	CResourceMgr<CMesh>::Create(CylinderKey, Desc);

}

void Engine::CreateGeosphere(const unsigned int& _DetailLv)
{
	std::wstring SphereKey = L"SeoulLandSphere_Lv" + std::to_wstring(_DetailLv);

	if (nullptr != CResourceMgr<CMesh>::Find(SphereKey))
		return;

	unsigned int Division = _DetailLv;

	const float X = 0.525731f;
	const float Z = 0.850651f;

	Vec3 Pos[12] =
	{
		 Vec3(-X, 0.f, Z)	, Vec3(X, 0.f, Z)
		,Vec3(-X, 0.f, -Z)	, Vec3(X, 0.f, -Z)
		,Vec3(0.f, Z, X)	, Vec3(0.f, Z, -X)
		,Vec3(0.f, -Z, X)	, Vec3(0.f, -Z, -X)
		,Vec3(Z, X, 0.f)	, Vec3(-Z, X, 0.f)
		,Vec3(Z, -X, 0.f)	, Vec3(-Z, -X, 0.f)
	};

	IDX16 Idx[20] =
	{
		  IDX16(1,4,0),IDX16(4,9,0),IDX16(4,5,9),IDX16(8,5,4),IDX16(1,8,4)
		, IDX16(1,10,8),IDX16(10,3,8),IDX16(8,3,5),IDX16(3,2,5),IDX16(3,7,2)
		, IDX16(3,10,7),IDX16(10,6,7),IDX16(6,11,7),IDX16(6,0,11),IDX16(6,1,0)
		, IDX16(10,1,6),IDX16(11,0,9),IDX16(2,11,9),IDX16(5,2,9),IDX16(11,2,7)
	};

	std::vector<BASICVTX> arrVtx;
	std::vector<IDX16> arrIdx;
	
	arrVtx.resize(12);
	arrIdx.resize(20);
	
	for (size_t i = 0; i < 12; i++)
	{
		arrVtx[i].Pos = Pos[i];
	}

	for (size_t i = 0; i < 20; i++)
	{
		arrIdx[i] = Idx[i];
	}

	for (size_t i = 0; i < _DetailLv; i++)
	{
		Subdivide(arrVtx, arrIdx);
	}
	
	size_t MaxVtx = arrVtx.size();

	for (size_t i = 0; i < MaxVtx; i++)
	{
		Vec3 nPos = arrVtx[i].Pos.GetNormalizeVec();

		arrVtx[i].Pos = nPos;
		arrVtx[i].Normal = nPos;

		float Theta = DXMath::AngleFromXY(arrVtx[i].Pos.x, arrVtx[i].Pos.z);

		float Phi = acosf(arrVtx[i].Pos.y);

		arrVtx[i].TexCoord.x = Theta / DXMath::FPI;
		arrVtx[i].TexCoord.y = Phi / DXMath::FPI;
	}

	UINT VtxCount = (UINT)arrVtx.size();
	UINT IdxCount = (UINT)arrIdx.size() * 3;

	CMesh::CREATE_DESC Desc{ &arrVtx[0] ,sizeof(BASICVTX) , VtxCount ,D3D11_USAGE_DYNAMIC
		, &arrIdx[0] ,IDX16::MemSize() , IdxCount,D3D11_USAGE_DYNAMIC ,IDX16::Format() ,D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

	CResourceMgr<CMesh>::Create(SphereKey.c_str(), Desc);

}

void Engine::CreateGizmoMesh(const float& _Radius, const float& _Height)
{
	std::vector<COLORVTX> GizmoVtx;
	std::vector<IDX16> GizmoIdx;

	// 기즈모의 중심(큐브)
	GizmoVtx.push_back(COLORVTX{ Vec4(_Radius,_Radius,-_Radius,1.f),Vec4::One,Vec2() });
	GizmoVtx.push_back(COLORVTX{ Vec4(_Radius,-_Radius,-_Radius,1.f),Vec4::One,Vec2() });
	GizmoVtx.push_back(COLORVTX{ Vec4(-_Radius,-_Radius,-_Radius,1.f),Vec4::One,Vec2() });
	GizmoVtx.push_back(COLORVTX{ Vec4(-_Radius,_Radius,-_Radius,1.f),Vec4::One,Vec2() });

	GizmoVtx.push_back(COLORVTX{ Vec4(_Radius,_Radius,_Radius,1.f),Vec4::One,Vec2() });
	GizmoVtx.push_back(COLORVTX{ Vec4(_Radius,-_Radius,_Radius,1.f),Vec4::One,Vec2() });
	GizmoVtx.push_back(COLORVTX{ Vec4(-_Radius,-_Radius,_Radius,1.f),Vec4::One,Vec2() });
	GizmoVtx.push_back(COLORVTX{ Vec4(-_Radius,_Radius,_Radius,1.f),Vec4::One,Vec2() });

	GizmoIdx.push_back(IDX16{ 0,1,3 });
	GizmoIdx.push_back(IDX16{ 1,2,3 });
	GizmoIdx.push_back(IDX16{ 4,6,5 });
	GizmoIdx.push_back(IDX16{ 6,4,7 });
	GizmoIdx.push_back(IDX16{ 0,4,5 });
	GizmoIdx.push_back(IDX16{ 0,5,1 });
	GizmoIdx.push_back(IDX16{ 3,6,7 });
	GizmoIdx.push_back(IDX16{ 3,2,6 });
	GizmoIdx.push_back(IDX16{ 1,5,2 });
	GizmoIdx.push_back(IDX16{ 2,5,6 });
	GizmoIdx.push_back(IDX16{ 0,3,4 });
	GizmoIdx.push_back(IDX16{ 3,7,4 });



	// X 축 기즈모 (Red)
	WORD Start = (WORD)GizmoVtx.size();
	std::vector<COLORVTX> Vtx;
	std::vector<IDX16> Idx;

	Vtx.reserve(2000);
	Idx.reserve(2000);

	Vtx.push_back(COLORVTX{ Vec4(_Radius,0.f,0.f,1.f),Vec4::Red,Vec2() });
	Vtx.push_back(COLORVTX{ Vec4(_Radius + _Height ,0.f,0.f,1.f),Vec4::Red,Vec2() });
	
	WORD _11, _12, _21, _22;
	UINT Division = 360;

	for (UINT i = 0; i < Division; ++i)
	{
		Vtx.push_back(COLORVTX{ Vec4(_Radius,_Radius * DXMath::CosA((float)i), _Radius * DXMath::SinA((float)i),1.f),Vec4::Red,Vec2() });
		Vtx.push_back(COLORVTX{ Vec4(_Radius + _Height, _Radius * DXMath::CosA((float)i), _Radius * DXMath::SinA((float)i),1.f),Vec4::Red,Vec2() });
		
		_11 = ((i + 1) * 2 )  + Start;
		_12 = ((i + 2) * 2 ) + Start;
		_21 = (((i + 1) * 2 + 1 )) + Start;
		_22 = (((i + 2) * 2 + 1 )) + Start;

		if ( ((i + 2) * 2) >= (Division + 1) * 2)
		{
			_12 = Start + 2;
		}

		if ( ((i + 2) * 2 + 1) >= (Division + 1) * 2)
		{
			_22 = Start + 3;
		}

		Idx.push_back(IDX16{ Start, _12 , _11 });
		Idx.push_back(IDX16{ (WORD)(Start + 1) , _21 , _22 });
		Idx.push_back(IDX16{ _11 , _12 , _21 });
		Idx.push_back(IDX16{ _12 , _22 , _21 });
	}

	GizmoVtx.insert(GizmoVtx.end(), Vtx.begin(), Vtx.end());
	GizmoIdx.insert(GizmoIdx.end(), Idx.begin(), Idx.end());

	Start = (WORD)GizmoVtx.size();

	Vtx.clear();
	Idx.clear();

	Vtx.push_back(COLORVTX{ Vec4(_Radius + _Height,0.f,0.f,1.f),Vec4::Red,Vec2() });
	Vtx.push_back(COLORVTX{ Vec4(_Radius + 40.f + _Height ,0.f,0.f,1.f),Vec4::Red,Vec2() });

	for (UINT i = 0; i < Division; ++i)
	{
		Vtx.push_back(COLORVTX{ Vec4(_Radius + _Height, (_Radius + 10.f) * DXMath::CosA((float)i),(_Radius + 10.f) * DXMath::SinA((float)i),1.f),Vec4::Red,Vec2() });

		_11 = i + 2 + Start;
		_12 = i + 3 + Start;

		if (i + 3 >= Division )
		{
			_12 = Start + 2;
		}

		Idx.push_back(IDX16{ Start, _12 , _11 });
		Idx.push_back(IDX16{ (WORD)(Start + 1) , _11 , _12 });
	}

	GizmoVtx.insert(GizmoVtx.end(), Vtx.begin(), Vtx.end());
	GizmoIdx.insert(GizmoIdx.end(), Idx.begin(), Idx.end());


	// Y축 기즈모 (Green)
	Start = (WORD)GizmoVtx.size();

	Vtx.clear();
	Idx.clear();

	Vtx.push_back(COLORVTX{ Vec4(0.f,_Radius,0.f,1.f),Vec4::Green,Vec2() });
	Vtx.push_back(COLORVTX{ Vec4(0.f,_Radius + _Height,0.f,1.f),Vec4::Green,Vec2() });

	for (UINT i = 0; i < Division; ++i)
	{
		Vtx.push_back(COLORVTX{ Vec4(_Radius * DXMath::CosA((float)i),_Radius, _Radius * DXMath::SinA((float)i),1.f),Vec4::Green,Vec2() });
		Vtx.push_back(COLORVTX{ Vec4(_Radius * DXMath::CosA((float)i),_Radius + _Height, _Radius * DXMath::SinA((float)i),1.f),Vec4::Green,Vec2() });

		_11 = ((i + 1) * 2) + Start;
		_12 = ((i + 2) * 2) + Start;
		_21 = (((i + 1) * 2 + 1)) + Start;
		_22 = (((i + 2) * 2 + 1)) + Start;

		if ( ((i + 2) * 2) >= (Division + 1) * 2)
		{
			_12 = Start + 2;
		}

		if ( ((i + 2) * 2 + 1) >= (Division + 1) * 2)
		{
			_22 = Start + 3;
		}

		Idx.push_back(IDX16{ Start, _11 , _12 });
		Idx.push_back(IDX16{ (WORD)(Start + 1) , _22 , _21 });
		Idx.push_back(IDX16{ _11 , _21 , _12 });
		Idx.push_back(IDX16{ _12 , _21 , _22 });
	}

	GizmoVtx.insert(GizmoVtx.end(), Vtx.begin(), Vtx.end());
	GizmoIdx.insert(GizmoIdx.end(), Idx.begin(), Idx.end());


	Start = (WORD)GizmoVtx.size();

	Vtx.clear();
	Idx.clear();

	Vtx.push_back(COLORVTX{ Vec4(0.f,_Radius + _Height,0.f,1.f),Vec4::Green,Vec2() });
	Vtx.push_back(COLORVTX{ Vec4(0.f ,_Radius + 40.f + _Height,0.f,1.f),Vec4::Green,Vec2() });

	for (UINT i = 0; i < Division; ++i)
	{
		Vtx.push_back(COLORVTX{ Vec4((_Radius + 10.f) * DXMath::CosA((float)i), _Radius + _Height, (_Radius + 10.f) * DXMath::SinA((float)i),1.f),Vec4::Green,Vec2() });

		_11 = i + 2 + Start;
		_12 = i + 3 + Start;

		if (i + 3 >= Division)
		{
			_12 = Start + 2;
		}

		Idx.push_back(IDX16{ Start, _12 , _11 });
		Idx.push_back(IDX16{ (WORD)(Start + 1) , _11 , _12 });
	}

	GizmoVtx.insert(GizmoVtx.end(), Vtx.begin(), Vtx.end());
	GizmoIdx.insert(GizmoIdx.end(), Idx.begin(), Idx.end());


	// Z축 기즈모 (Green)
	Start = (WORD)GizmoVtx.size();

	Vtx.clear();
	Idx.clear();

	Vtx.push_back(COLORVTX{ Vec4(0.f,0.f,_Radius,1.f),Vec4::Blue,Vec2() });
	Vtx.push_back(COLORVTX{ Vec4(0.f,0.f,_Radius + _Height,1.f),Vec4::Blue,Vec2() });

	for (UINT i = 0; i < Division; ++i)
	{
		Vtx.push_back(COLORVTX{ Vec4(_Radius * DXMath::CosA((float)i), _Radius * DXMath::SinA((float)i) ,_Radius,1.f),Vec4::Blue,Vec2() });
		Vtx.push_back(COLORVTX{ Vec4(_Radius * DXMath::CosA((float)i), _Radius * DXMath::SinA((float)i) ,_Radius + _Height,1.f),Vec4::Blue,Vec2() });

		_11 = ((i + 1) * 2) + Start;
		_12 = ((i + 2) * 2) + Start;
		_21 = (((i + 1) * 2 + 1)) + Start;
		_22 = (((i + 2) * 2 + 1)) + Start;

		if (((i + 2) * 2) >= (Division + 1) * 2)
		{
			_12 = Start + 2;
		}

		if (((i + 2) * 2 + 1) >= (Division + 1) * 2)
		{
			_22 = Start + 3;
		}

		Idx.push_back(IDX16{ Start, _12 , _11 });
		Idx.push_back(IDX16{ (WORD)(Start + 1) , _21 , _22 });
		Idx.push_back(IDX16{ _11 , _12 , _21 });
		Idx.push_back(IDX16{ _12 , _22 , _21 });
	}

	GizmoVtx.insert(GizmoVtx.end(), Vtx.begin(), Vtx.end());
	GizmoIdx.insert(GizmoIdx.end(), Idx.begin(), Idx.end());

	Start = (WORD)GizmoVtx.size();

	Vtx.clear();
	Idx.clear();

	Vtx.push_back(COLORVTX{ Vec4(0.f,0.f,_Radius + _Height,1.f),Vec4::Blue,Vec2() });
	Vtx.push_back(COLORVTX{ Vec4(0.f ,0.f, _Radius + 40.f + _Height,1.f),Vec4::Blue,Vec2() });

	for (UINT i = 0; i < Division; ++i)
	{
		Vtx.push_back(COLORVTX{ Vec4((_Radius + 10.f) * DXMath::CosA((float)i),  (_Radius + 10.f) * DXMath::SinA((float)i), _Radius + _Height,1.f),Vec4::Blue,Vec2() });

		_11 = i + 2 + Start;
		_12 = i + 3 + Start;

		if (i + 3 >= Division)
		{
			_12 = Start + 2;
		}

		Idx.push_back(IDX16{ Start, _12 , _11 });
		Idx.push_back(IDX16{ (WORD)(Start + 1) , _11 , _12 });
	}

	GizmoVtx.insert(GizmoVtx.end(), Vtx.begin(), Vtx.end());
	GizmoIdx.insert(GizmoIdx.end(), Idx.begin(), Idx.end());

	UINT VtxCount = (UINT)GizmoVtx.size();
	UINT IdxCount = (UINT)GizmoIdx.size() * 3;
	CMesh::CREATE_DESC Desc{ &GizmoVtx[0] ,sizeof(COLORVTX) , VtxCount ,D3D11_USAGE_DYNAMIC
		, &GizmoIdx[0] ,IDX16::MemSize() , IdxCount,D3D11_USAGE_DYNAMIC ,IDX16::Format() ,D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

	CResourceMgr<CMesh>::Create(L"GizmoMesh", Desc);
}
void Engine::Subdivide(std::vector<BASICVTX>& _vecVtx, std::vector<IDX16>& _vexIdx)
{
	std::vector<BASICVTX> InputVtx(_vecVtx.begin(), _vecVtx.end());
	std::vector<IDX16> InputIdx(_vexIdx.begin(), _vexIdx.end());


	_vecVtx.resize(0);
	_vexIdx.resize(0);

	UINT numTris = (UINT)InputIdx.size();

	for (unsigned int i = 0; i < numTris; ++i)
	{
		BASICVTX v0 = InputVtx[InputIdx[i]._1];
		BASICVTX v1 = InputVtx[InputIdx[i]._2];
		BASICVTX v2 = InputVtx[InputIdx[i]._3];

		BASICVTX m0, m1, m2;


		m0.Pos = DXMath::Vec3Lerp(v0.Pos, v1.Pos, 0.5f);
		m1.Pos = DXMath::Vec3Lerp(v1.Pos, v2.Pos, 0.5f);
		m2.Pos = DXMath::Vec3Lerp(v2.Pos, v0.Pos, 0.5f);

		_vecVtx.push_back(v0); // 0
		_vecVtx.push_back(v1); // 1
		_vecVtx.push_back(v2); // 2
		_vecVtx.push_back(m0); // 3
		_vecVtx.push_back(m1); // 4
		_vecVtx.push_back(m2); // 5

		_vexIdx.push_back(IDX16(i * 6 + 0, i * 6 + 3, i * 6 + 5));
		_vexIdx.push_back(IDX16(i * 6 + 3, i * 6 + 4, i * 6 + 5));
		_vexIdx.push_back(IDX16(i * 6 + 5, i * 6 + 4, i * 6 + 2));
		_vexIdx.push_back(IDX16(i * 6 + 3, i * 6 + 1, i * 6 + 4));
	}
}

void Engine::CreateFrameCube(const float& _InnerLen, const float& _OutLen, const float& _Height) 
{
	std::vector<COLORVTX> vecVtx;
	std::vector<IDX16> vecIdx;

	vecVtx.reserve(16);

	// 윗면
	vecVtx.push_back(COLORVTX{ Vec4(_OutLen * 0.5f, _Height*0.5f, _OutLen * 0.5f ,1.f),Vec2(),Vec4::White });
	vecVtx.push_back(COLORVTX{ Vec4(_OutLen * -0.5f, _Height*0.5f, _OutLen * 0.5f ,1.f),Vec2(),Vec4::White });
	vecVtx.push_back(COLORVTX{ Vec4(_OutLen * -0.5f, _Height*0.5f, _OutLen * -0.5f ,1.f),Vec2(),Vec4::White });
	vecVtx.push_back(COLORVTX{ Vec4(_OutLen * 0.5f, _Height*0.5f, _OutLen * -0.5f ,1.f),Vec2(),Vec4::White });

	vecVtx.push_back(COLORVTX{ Vec4(_InnerLen * 0.5f, _Height*0.5f, _InnerLen * 0.5f ,1.f),Vec2(),Vec4::White });
	vecVtx.push_back(COLORVTX{ Vec4(_InnerLen * -0.5f, _Height*0.5f, _InnerLen * 0.5f ,1.f),Vec2(),Vec4::White });
	vecVtx.push_back(COLORVTX{ Vec4(_InnerLen * -0.5f, _Height*0.5f, _InnerLen * -0.5f ,1.f),Vec2(),Vec4::White });
	vecVtx.push_back(COLORVTX{ Vec4(_InnerLen * 0.5f, _Height*0.5f, _InnerLen * -0.5f ,1.f),Vec2(),Vec4::White });

	// 아랫면
	vecVtx.push_back(COLORVTX{ Vec4(_OutLen * 0.5f, _Height*-0.5f, _OutLen * 0.5f ,1.f),Vec2(),Vec4::White });
	vecVtx.push_back(COLORVTX{ Vec4(_OutLen * -0.5f, _Height*-0.5f, _OutLen * 0.5f ,1.f),Vec2(),Vec4::White });
	vecVtx.push_back(COLORVTX{ Vec4(_OutLen * -0.5f, _Height*-0.5f, _OutLen * -0.5f ,1.f),Vec2(),Vec4::White });
	vecVtx.push_back(COLORVTX{ Vec4(_OutLen * 0.5f, _Height*-0.5f, _OutLen * -0.5f ,1.f),Vec2(),Vec4::White });

	vecVtx.push_back(COLORVTX{ Vec4(_InnerLen * 0.5f, _Height*-0.5f, _InnerLen * 0.5f ,1.f),Vec2(),Vec4::White });
	vecVtx.push_back(COLORVTX{ Vec4(_InnerLen * -0.5f, _Height*-0.5f, _InnerLen * 0.5f ,1.f),Vec2(),Vec4::White });
	vecVtx.push_back(COLORVTX{ Vec4(_InnerLen * -0.5f, _Height*-0.5f, _InnerLen * -0.5f ,1.f),Vec2(),Vec4::White });
	vecVtx.push_back(COLORVTX{ Vec4(_InnerLen * 0.5f, _Height*-0.5f, _InnerLen * -0.5f ,1.f),Vec2(),Vec4::White });
		
	// 윗면 삼각 리스트 
	vecIdx.push_back(IDX16{ 0,4,1 });
	vecIdx.push_back(IDX16{ 4,5,1 });
	vecIdx.push_back(IDX16{ 1,5,2 });
	vecIdx.push_back(IDX16{ 5,6,2 });
	vecIdx.push_back(IDX16{ 2,6,3 });
	vecIdx.push_back(IDX16{ 6,7,3 });
	vecIdx.push_back(IDX16{ 3,7,0 });
	vecIdx.push_back(IDX16{ 7,4,0 });
	
	// 아랫면 삼각 리스트 
	vecIdx.push_back(IDX16{ 8,9,12 });
	vecIdx.push_back(IDX16{ 12,9,13 });
	vecIdx.push_back(IDX16{ 9,10,13 });
	vecIdx.push_back(IDX16{ 13,10,14 });
	vecIdx.push_back(IDX16{ 10,11,14 });
	vecIdx.push_back(IDX16{ 14,11,15 });
	vecIdx.push_back(IDX16{ 11,8,15 });
	vecIdx.push_back(IDX16{ 15,8,12 });

	// 외부면 삼각 리스트 
	vecIdx.push_back(IDX16{ 0,8,3 });
	vecIdx.push_back(IDX16{ 3,8,11 });
	vecIdx.push_back(IDX16{ 0,1,8 });
	vecIdx.push_back(IDX16{ 1,9,8 });
	vecIdx.push_back(IDX16{ 1,2,10 });
	vecIdx.push_back(IDX16{ 1,10,9 });
	vecIdx.push_back(IDX16{ 2,3,10 });
	vecIdx.push_back(IDX16{ 3,11,10 });

	// 내부면 삼각 리스트 
	vecIdx.push_back(IDX16{ 4,7,12 });
	vecIdx.push_back(IDX16{ 7,15,12 });
	vecIdx.push_back(IDX16{ 4,12,5 });
	vecIdx.push_back(IDX16{ 5,12,13 });
	vecIdx.push_back(IDX16{ 5,13,14 });
	vecIdx.push_back(IDX16{ 5,14,6 });
	vecIdx.push_back(IDX16{ 6,14,7 });
	vecIdx.push_back(IDX16{ 14,15,7 });


	UINT VtxCount = (UINT)vecVtx.size();
	UINT IdxCount = (UINT)vecIdx.size() * 3;
	CMesh::CREATE_DESC Desc{ &vecVtx[0] ,sizeof(COLORVTX) , VtxCount ,D3D11_USAGE_DYNAMIC
		, &vecIdx[0] ,IDX16::MemSize() , IdxCount,D3D11_USAGE_DYNAMIC ,IDX16::Format() ,D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

	CResourceMgr<CMesh>::Create(L"FrameCube", Desc);
}
void Engine::CreateTubeMesh(const float& _InnerRad, const float& _OuterRad, const float& _Height) 
{
	std::vector<COLORVTX> vecVtx;
	std::vector<IDX16> vecIdx;

	const int MaxDiv = 32;
	WORD MaxIdx = (WORD)(4 * MaxDiv);
	vecVtx.reserve(4 * MaxDiv);
	vecVtx.reserve(8 * MaxDiv);
	float Angle; 

	for (int Div = 0; Div < MaxDiv; ++Div)
	{
		Angle = Div * (360.f / MaxDiv);

		vecVtx.push_back(COLORVTX{ Vec4(_OuterRad * DXMath::CosA(Angle), _Height * 0.5f, _OuterRad *  DXMath::SinA(Angle),1.f),Vec2(),Vec4::White });
		vecVtx.push_back(COLORVTX{ Vec4(_InnerRad * DXMath::CosA(Angle), _Height * 0.5f, _InnerRad *  DXMath::SinA(Angle),1.f),Vec2(),Vec4::White });
		vecVtx.push_back(COLORVTX{ Vec4(_OuterRad * DXMath::CosA(Angle), _Height * -0.5f, _OuterRad *  DXMath::SinA(Angle),1.f),Vec2(),Vec4::White });
		vecVtx.push_back(COLORVTX{ Vec4(_InnerRad * DXMath::CosA(Angle), _Height * -0.5f, _InnerRad *  DXMath::SinA(Angle),1.f),Vec2(),Vec4::White });

		vecIdx.push_back(IDX16{ (WORD)(4 * Div ), (WORD)(4 * Div + 1), (WORD)((4 * (Div + 1) + 1 ) % MaxIdx) });
		vecIdx.push_back(IDX16{ (WORD)(4 * Div), (WORD)((4 * (Div + 1) + 1) % MaxIdx), (WORD)((4 * (Div + 1)) % MaxIdx) });
		vecIdx.push_back(IDX16{ (WORD)(4 * Div + 2), (WORD)((4 * (Div + 1) + 3) % MaxIdx), (WORD)(4 * Div  + 3 )});
		vecIdx.push_back(IDX16{ (WORD)(4 * Div + 2), (WORD)((4 * (Div + 1) + 2) % MaxIdx), (WORD)((4 * (Div + 1) + 3 ) % MaxIdx) });
		vecIdx.push_back(IDX16{ (WORD)(4 * Div ), (WORD)((4 * (Div + 1) + 2) % MaxIdx), (WORD)(4 * Div + 2 )});
		vecIdx.push_back(IDX16{ (WORD)(4 * Div ), (WORD)((4 * (Div + 1) ) % MaxIdx), (WORD)((4 * (Div + 1) + 2 ) % MaxIdx) });
		vecIdx.push_back(IDX16{ (WORD)(4 * Div + 1 ), (WORD)(4 * Div + 3), (WORD)((4 * (Div + 1) + 3 ) % MaxIdx) });
		vecIdx.push_back(IDX16{ (WORD)(4 * Div + 1 ), (WORD)((4 * (Div + 1) + 3 ) % MaxIdx), (WORD)((4 * (Div + 1) + 1 ) % MaxIdx) });
	}
	

	UINT VtxCount = (UINT)vecVtx.size();
	UINT IdxCount = (UINT)vecIdx.size() * 3;
	CMesh::CREATE_DESC Desc{ &vecVtx[0] ,sizeof(COLORVTX) , VtxCount ,D3D11_USAGE_DYNAMIC
		, &vecIdx[0] ,IDX16::MemSize() , IdxCount,D3D11_USAGE_DYNAMIC ,IDX16::Format() ,D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

	CResourceMgr<CMesh>::Create(L"Tube", Desc);
}
bool Engine::LoadObjFile(const wchar_t* _FileName, bool _TextureCheck)
{
	std::vector<Vec3> VerticesPositionContainer;
	std::vector<Vec2> VerticesUVContainer;
	std::vector<Vec3> VerticesNormalContainer;

	std::vector<BASICVTX> Vertices;
	std::vector<COLORNORMALVTX> ColoredVertices;
	std::vector<UINT> Indices;

	FILE* pFile = NULL;
	std::wstring Temp = _FileName;
	Temp += L".obj";
	_wfopen_s(&pFile, CFilePathMgr::FilePath(L"Obj", Temp).c_str(), L"rt");

	if (!pFile)
	{
		return false;
	}
	else
	{
		char pLine[512] = {};
		while (true)
		{
			fgets(pLine, 512, pFile);
			int iLength = (int)strlen(pLine);
			if (feof(pFile))
			{
				break;
			}

			if (pLine[0] == '\n' || pLine[0] == '#' || pLine[0] == 's' || pLine[0] == 'g')
				continue;

			if (pLine[0] == 'v' && pLine[1] != 't' && pLine[1] != 'n')
			{
				//Vertex의 위치들을 저장해둔다.
				Vec3 position;
				char* pContext = NULL;
				char* value = strtok_s(&pLine[1], " ", &pContext);
				position.x = std::stof(value);
				value = strtok_s(NULL, " ", &pContext);
				position.y = std::stof(value);
				value = strtok_s(NULL, " ", &pContext);
				position.z = std::stof(value) * -1.0f;

				VerticesPositionContainer.push_back(position);
			}
			else if (pLine[0] == 'v' && pLine[1] == 't' && pLine[1] != 'n')
			{
				Vec2 uv;
				char* pContext = NULL;
				char* value = strtok_s(&pLine[2], " ", &pContext);
				uv.x = std::stof(value);
				value = strtok_s(NULL, " ", &pContext);
				uv.y = 1.0f - std::stof(value);
				VerticesUVContainer.push_back(uv);
			}
			else if (pLine[0] == 'v' && pLine[1] != 't' && pLine[1] == 'n')
			{
				Vec3 normal;
				char* pContext = NULL;
				char* value = strtok_s(&pLine[2], " ", &pContext);
				normal.x = std::stof(value);
				value = strtok_s(NULL, " ", &pContext);
				normal.y = std::stof(value);
				value = strtok_s(NULL, " ", &pContext);
				normal.z = std::stof(value) * -1.0f;

				VerticesNormalContainer.push_back(normal);
			}
			else if (pLine[0] == 'f')
			{
				UINT PositionIndex[3];
				UINT TextureIndex[3];
				UINT NormalIndex[3];

				char* pContext = NULL;
				char* value = strtok_s(&pLine[2], "/", &pContext);
				PositionIndex[0] = std::stoi(value) - 1;
				value = strtok_s(NULL, "/", &pContext);
				TextureIndex[0] = std::stoi(value) - 1;
				value = strtok_s(NULL, " ", &pContext);
				NormalIndex[0] = std::stoi(value) - 1;

				//2
				value = strtok_s(NULL, "/", &pContext);
				PositionIndex[1] = std::stoi(value) - 1;
				value = strtok_s(NULL, "/", &pContext);
				TextureIndex[1] = std::stoi(value) - 1;
				value = strtok_s(NULL, " ", &pContext);
				NormalIndex[1] = std::stoi(value) - 1;
				//3
				value = strtok_s(NULL, "/", &pContext);
				PositionIndex[2] = std::stoi(value) - 1;
				value = strtok_s(NULL, "/", &pContext);
				TextureIndex[2] = std::stoi(value) - 1;
				value = strtok_s(NULL, " ", &pContext);
				NormalIndex[2] = std::stoi(value) - 1;


				//Face 조립
				for (int i = 0; i < 3; ++i)
				{
					if (true == _TextureCheck)
					{
						BASICVTX tVertex;
						tVertex.Pos = VerticesPositionContainer[PositionIndex[2 - i]];
						tVertex.TexCoord = VerticesUVContainer[TextureIndex[2 - i]];
						tVertex.Normal = VerticesNormalContainer[NormalIndex[2 - i]];
						Vertices.push_back(tVertex);
						Indices.push_back((UINT)Indices.size());
					}
					else
					{
						Vec4 vRandomColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);

						COLORNORMALVTX tVertex;
						tVertex.Pos = VerticesPositionContainer[PositionIndex[2 - i]];
						tVertex.Normal = VerticesNormalContainer[NormalIndex[2 - i]];
						tVertex.Color = vRandomColor;
						ColoredVertices.push_back(tVertex);
						Indices.push_back((UINT)Indices.size());
					}
				}
			}
		}

		fclose(pFile);
	}

	BASICVTX*	AllVtx = new BASICVTX[Vertices.size()];
	IDX16*		AllIdx = new IDX16[Indices.size()];

	for (size_t i = 0; i < Vertices.size(); i++)
	{
		AllVtx[i] = Vertices[i];
	}
	for (size_t i = 0; i < (int)Indices.size() / 3; i++)
	{
		IDX16 Temp = IDX16((WORD)i * 3, (WORD)i * 3 + 1, (WORD)i * 3 + 2);
		AllIdx[i] = Temp;
	}
	CMesh::CREATE_DESC Desc = { AllVtx, sizeof(BASICVTX), (UINT)Vertices.size(), D3D11_USAGE_DYNAMIC, AllIdx, IDX16::MemSize(), (UINT)Indices.size(), D3D11_USAGE_DYNAMIC, IDX16::Format(), D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

	CResourceMgr<CMesh>::Create(_FileName, Desc);
	delete[] AllVtx;
	delete[] AllIdx;

	return true;
}


bool Engine::LoadNaviObjFile(const wchar_t* _FileName)
{
	FILE* pFile = NULL;
	std::wstring Temp = _FileName ;
	Temp += L".obj";
	_wfopen_s(&pFile, CFilePathMgr::FilePath(L"Obj", Temp).c_str(), L"rt");

	if (nullptr == pFile)
		return false;

	char Buffer[MAXSTRING];

	std::vector<Vec3> vecVtx;
	std::vector<IDX32> vecIdx;

	char Tag;

	while (!feof(pFile)) 
	{
		Tag = fgetc(pFile);

		if ('v' == Tag)
		{
			Vec3 Pos;

			fscanf_s(pFile, " %f %f %f\n", &Pos.x, &Pos.y, &Pos.z);
			vecVtx.push_back(Pos);
		}
		else if ('f' == Tag) 
		{
			int i,j,k;
			fscanf_s(pFile, "%d %d %d\n", &i, &j, &k);
			vecIdx.push_back(IDX32{ DWORD(i - 1) ,DWORD(k - 1) ,DWORD(j - 1) });
		}
		else 
		{
			fgets(Buffer, sizeof(Buffer), pFile);
		}


	}

	fclose(pFile);


	COLORVTX* TempVtx = new COLORVTX[vecVtx.size()];
	ZeroMemory(TempVtx, sizeof(COLORVTX) *vecVtx.size());

	for (size_t i = 0; i < vecVtx.size(); i++)
	{
		TempVtx[i].Pos = vecVtx[i] * OBJMAG;
		TempVtx[i].Pos.z = -TempVtx[i].Pos.z;
	}

	CMesh::CREATE_DESC Desc = { TempVtx, sizeof(COLORVTX) ,  (UINT)vecVtx.size() ,D3D11_USAGE_DYNAMIC
		, &vecIdx[0] ,IDX32::MemSize() ,(UINT)(3 * vecIdx.size()) ,D3D11_USAGE_DYNAMIC ,IDX32::Format() ,D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
	SPTR<COBJMesh> NewMesh = CResourceMgr<COBJMesh>::Create(_FileName, Desc);

	NewMesh->m_vecColorVtx.resize(vecVtx.size());
	CopyMemory(&NewMesh->m_vecColorVtx[0], TempVtx, sizeof(COLORVTX)*vecVtx.size());
	NewMesh->m_vecIdx.assign(vecIdx.begin(), vecIdx.end());

	delete[] TempVtx;
	return true;

}

void Engine::AddNavigationMesh(CNavigationMesh* _NaviMesh)
{
	UINT ID = _NaviMesh->GetSerialNumber();

	if (-1 == ID)
	{
		ID = (UINT)SSDRandom::RandomInt(INT32_MIN, INT32_MAX);
		std::unordered_map<UINT,CNavigationMesh*>::iterator Find = mapNavigationMesh.find(ID);

		// 기존 맵에 동일한 ID 값이 키로 존재할경우 ID를 재지정
		while (Find != mapNavigationMesh.end())
		{
			ID = (UINT)SSDRandom::RandomInt(INT32_MIN, INT32_MAX);
			std::unordered_map<UINT, CNavigationMesh*>::iterator Find = mapNavigationMesh.find(ID);
		}

		_NaviMesh->SetSerialNumber(ID);
	}

	mapNavigationMesh.insert(std::unordered_map<UINT, CNavigationMesh*>::value_type(ID, _NaviMesh));
}

void Engine::DelNavigationMesh(CNavigationMesh* _NaviMesh)
{
	std::unordered_map<UINT, CNavigationMesh*>::iterator Start = mapNavigationMesh.begin();
	std::unordered_map<UINT, CNavigationMesh*>::iterator End = mapNavigationMesh.end();

	for ( ; Start != End; ++Start)
	{
		if (Start->second == _NaviMesh) 
		{
			mapNavigationMesh.erase(Start);
			return;
		}
	}

}

CNavigationMesh* Engine::FindNavigationMesh(const UINT _ID)
{
	return MAPFINDVALUE<CNavigationMesh*>(mapNavigationMesh, _ID);
}

void Engine::ClearNaviMeshMap()
{
	mapNavigationMesh.clear();
}
void Engine::InitAllNavigationMeshLinkTriList()
{
	std::unordered_map<UINT, CNavigationMesh*>::iterator Start = mapNavigationMesh.begin();
	std::unordered_map<UINT, CNavigationMesh*>::iterator End = mapNavigationMesh.end();

	for (; Start != End; ++Start)
	{
		Start->second->ClearLinkTriList();
	}
}
