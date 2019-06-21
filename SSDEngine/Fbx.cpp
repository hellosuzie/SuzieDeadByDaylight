#include "Fbx.h"
#include <atlstr.h>
#include "DXMacro.h"
#include "Mesh.h"
#include "Material.h"
#include "ResourceMgr.h"
#include "Texture.h"
#include "ReadStream.h"
#include "WriteStream.h"
#include "DXMath.h"


/////////////////////////////////////////////					BoneMesh					/////////////////////////////


const UINT CBoneMesh::GetVtxCount()
{
	return (UINT)VtxDataVec.size();
}

const UINT CBoneMesh::GetIBCount()
{
	return (UINT)IdxDataVec.size();
}

const UINT CBoneMesh::GetIdxCount(const UINT& _Idx)
{
#ifdef _DEBUG
	if (_Idx >= IdxDataVec.size())
	{
		assert(nullptr);
		return 0;
	}
#endif

	return (UINT)IdxDataVec[_Idx].size();
}


const UINT CBoneMesh::GetTriIdxCount(const UINT& _Idx)
{
#ifdef _DEBUG
	if (_Idx >= IdxDataVec.size())
	{
		assert(nullptr);
		return 0;
	}
#endif

	return (UINT)IdxDataVec[_Idx].size() / 3;
}

const UINT CBoneMesh::GetAllTriCount()
{
	UINT Count = 0;

	for (UINT  i = 0; i < GetIBCount(); i++)
	{
		Count += GetTriIdxCount(i);
	}

	return Count;
}
const UINT CBoneMesh::GetMaterialCount()
{
	return (UINT)MaterialVec.size();
}


CFBXMesh* CBoneMesh::CreateFBXMesh()
{
	CFBXMesh* NewMesh = new CFBXMesh();

	NewMesh->CreateVertexBuffer(GetVtxCount(), sizeof(FBXVTX), D3D11_USAGE_DYNAMIC, &VtxDataVec[0]);
	
	UINT MAXIB = (UINT)IdxDataVec.size();

	for (UINT Idx = 0; Idx < MAXIB; )
	{
		if (true == IdxDataVec[Idx].empty())
		{
			IdxDataVec.erase(IdxDataVec.begin() + Idx);
			MaterialVec.erase(MaterialVec.begin() + Idx);
			MAXIB = (UINT)IdxDataVec.size();
			continue;
		}

		NewMesh->CreateIndexBuffer(0, (UINT)GetIdxCount(Idx), IDX32::MemSize()
			, D3D11_USAGE_DYNAMIC, &IdxDataVec[Idx][0], IDX32::Format());

		++Idx;
	}

	return NewMesh;

}

///////////////////////////////////////////				FBX				//////////////////////////////////
FbxAMatrix									CFbx::ReverseXY;
FbxAMatrix									CFbx::ReverseXZ;
FbxAMatrix									CFbx::ReverseYZ;

void CFbx::Init()
{
	CFbx::ReverseXY.mData[0] = { 0,1,0,0 };
	CFbx::ReverseXY.mData[1] = { 1,0,0,0 };
	CFbx::ReverseXY.mData[2] = { 0,0,1,0 };
	CFbx::ReverseXY.mData[3] = { 0,0,0,1 };

	CFbx::ReverseXZ.mData[0] = { 0,0,1,0 };
	CFbx::ReverseXZ.mData[1] = { 0,1,0,0 };
	CFbx::ReverseXZ.mData[2] = { 1,0,0,0 };
	CFbx::ReverseXZ.mData[3] = { 0,0,0,1 };

	CFbx::ReverseYZ.mData[0] = { 1,0,0,0 };
	CFbx::ReverseYZ.mData[1] = { 0,0,1,0 };
	CFbx::ReverseYZ.mData[2] = { 0,1,0,0 };
	CFbx::ReverseYZ.mData[3] = { 0,0,0,1 };
}

CFbx::CFbx()
	:m_FbxMgr(nullptr), m_FbxScene(nullptr), m_IOSet(nullptr), m_Imp(nullptr)
	, m_AllBoneCount(0), m_eAnimationType(MAX_FBX_ANIMATION_TYPE), m_BoneOffsetTex(nullptr)
{
	m_AllBoneVec.clear();
	m_AllBoneFindMap.clear();
	m_AllMeshVec.clear();
}


CFbx::~CFbx()
{
	m_AllBoneVec.clear();
	m_AllBoneFindMap.clear();
	m_BoneMatTex = nullptr;
	m_BoneOffsetTex = nullptr;


	SAFE_DESTROY(m_Imp)
	SAFE_DESTROY(m_FbxScene)
	SAFE_DESTROY(m_IOSet)
	SAFE_DESTROY(m_FbxMgr)
}

//로드할때 SFbx폴더에서 파일이 있나 확인하고 있으면 sfbx파일로 로드하고 없으면 Fbx폴더에서 찾아서 로드하고 sfbx바이너리 파일을 만든다
const bool CFbx::Load(const LOAD_DESC& _Desc)
{
	bool ReturnValue = false;
	std::wstring TempPath = GetPath();
	std::wstring Ext = GetExtension();

	m_eAnimationType = (FBX_ANIMATION_TYPE)(_Desc.eFbxInitFlag &  FBXANI_MASK);

	// 자체 포멧경로를 넣을 경우 자동 Load, Save를 수행하지 않는다.
	if (Ext == L".sfbx")
	{
		ReturnValue = BinaryLoad(m_eAnimationType, TempPath.c_str());
		LoadFbxData();
	}
	else if (Ext == L".fbx")
	{
		FBX_AUTO_TYPE LoadType = (FBX_AUTO_TYPE)(_Desc.eFbxInitFlag &  FBXAUTO_LOAD);
		FBX_AUTO_TYPE SaveType = (FBX_AUTO_TYPE)(_Desc.eFbxInitFlag &  FBXAUTO_SAVE);

		size_t Last_Index = TempPath.find_last_of(L".");
		size_t Count = TempPath.size() - Last_Index;
		TempPath.replace(Last_Index, Count, L".sfbx");

		if (LoadType == FBXAUTO_LOAD && true == CFileStream::FileExistCheck(TempPath.c_str()))
		{
			ReturnValue = BinaryLoad(m_eAnimationType, TempPath.c_str());
			LoadFbxData();
		}
		else
		{
			ReturnValue = FbxLoad(_Desc);
			CheckTriangleTB();
			LoadFbxData();
			if (SaveType == FBXAUTO_SAVE)
				BinarySave(TempPath.c_str());
		}

	}

	if (m_eAnimationType != FBXANI_STATIC)
	{
		// Texture 원소 한개당 float4 를 담기 때문에 
		// 행렬을 저장하기 위해선 Texture 원소를 4개를 써야하고 	본갯수 만큼의 행렬을 담기위해 
		// 텍스쳐의 너비 = 본갯수 * 4 , 높이 = 1로 만든다. 
	
		// 계산된 스키닝 행렬을 담을 텍스쳐(CPU 업데이트 용)
		m_BoneMatTex = CTexture::CreateTexture((UINT)m_AllBoneVec.size() * 4, 1
			, DXGI_FORMAT_R32G32B32A32_FLOAT
			, D3D11_BIND_SHADER_RESOURCE 
			, D3D11_USAGE_DYNAMIC);

		// 컴퓨트 셰이더에서 사용할 리소스를 만든다.(애니메이션 전용)
		CreateFBXSubTexture();

	}

	return ReturnValue;

}

const bool CFbx::BinaryLoad(FBX_ANIMATION_TYPE _AniType, const wchar_t* _BinaryPath)
{
	CReadStream BR = CReadStream(_BinaryPath);

	UINT MeshSize;
	BR.Read(MeshSize);
	m_AllMeshVec.resize(MeshSize);
	for (size_t Vtx = 0; Vtx < m_AllMeshVec.size(); ++Vtx)
	{
		m_AllMeshVec[Vtx] = new CBoneMesh();
		BR.Read(m_AllMeshVec[Vtx]->Name);
		UINT VTXCOUNT = 0;
		BR.Read(VTXCOUNT);
		if (VTXCOUNT <= 0)
		{
			continue;
		}

		m_AllMeshVec[Vtx]->VtxDataVec.resize(VTXCOUNT);
		BR.Read(&(m_AllMeshVec[Vtx]->VtxDataVec[0]), (UINT)(sizeof(FBXVTX) * VTXCOUNT));

		UINT SUBCOUNT = 0;
		BR.Read(SUBCOUNT);
		m_AllMeshVec[Vtx]->IdxDataVec.resize(SUBCOUNT);
		for (size_t SubSet = 0; SubSet < m_AllMeshVec[Vtx]->IdxDataVec.size(); ++SubSet)
		{
			UINT IDXCOUNT = 0;
			BR.Read(IDXCOUNT);
			if (IDXCOUNT <= 0)
			{
				continue;
			}
			m_AllMeshVec[Vtx]->IdxDataVec[SubSet].resize(IDXCOUNT);
			BR.Read(&(m_AllMeshVec[Vtx]->IdxDataVec[SubSet][0]), (UINT)(sizeof(DWORD) * IDXCOUNT));
		}

		UINT MATCOUNT = 0;
		BR.Read(MATCOUNT);
		m_AllMeshVec[Vtx]->MaterialVec.resize(MATCOUNT);
		std::string FbxPath = CFilePathMgr::GetSTRPath(L"Fbx");

		for (size_t MatSet = 0; MatSet < m_AllMeshVec[Vtx]->MaterialVec.size(); ++MatSet)
		{
			m_AllMeshVec[Vtx]->MaterialVec[MatSet] = new CBoneMaterial();
			BR.Read(m_AllMeshVec[Vtx]->MaterialVec[MatSet]->MaterialColor);
			BR.Read(m_AllMeshVec[Vtx]->MaterialVec[MatSet]->DiffTexName);
			BR.Read(m_AllMeshVec[Vtx]->MaterialVec[MatSet]->BumpTexName);
			BR.Read(m_AllMeshVec[Vtx]->MaterialVec[MatSet]->EmivTexName);
			BR.Read(m_AllMeshVec[Vtx]->MaterialVec[MatSet]->SpecTexName);

			if (false == m_AllMeshVec[Vtx]->MaterialVec[MatSet]->DiffTexName.empty()) 
			{
				m_AllMeshVec[Vtx]->MaterialVec[MatSet]->DiffTexName = FbxPath + m_AllMeshVec[Vtx]->MaterialVec[MatSet]->DiffTexName;
			}

			if (false == m_AllMeshVec[Vtx]->MaterialVec[MatSet]->BumpTexName.empty()) 
			{
				m_AllMeshVec[Vtx]->MaterialVec[MatSet]->BumpTexName = FbxPath + m_AllMeshVec[Vtx]->MaterialVec[MatSet]->BumpTexName;
			}

			if (false == m_AllMeshVec[Vtx]->MaterialVec[MatSet]->EmivTexName.empty()) 
			{
				m_AllMeshVec[Vtx]->MaterialVec[MatSet]->EmivTexName = FbxPath + m_AllMeshVec[Vtx]->MaterialVec[MatSet]->EmivTexName;
			}

			if (false == m_AllMeshVec[Vtx]->MaterialVec[MatSet]->SpecTexName.empty()) 
			{
				m_AllMeshVec[Vtx]->MaterialVec[MatSet]->SpecTexName = FbxPath + m_AllMeshVec[Vtx]->MaterialVec[MatSet]->SpecTexName;
			}
		}
	}

	UINT AniSize = 0;
	BR.Read(AniSize);
	m_AllAniInfoVec.resize(AniSize);
	for (size_t AniIdx = 0; AniIdx < m_AllAniInfoVec.size(); ++AniIdx)
	{
		m_AllAniInfoVec[AniIdx] = new CBoneAniInfo();

		BR.Read(m_AllAniInfoVec[AniIdx]->AnimIdx);
		BR.Read(m_AllAniInfoVec[AniIdx]->Name);
		BR.Read(m_AllAniInfoVec[AniIdx]->StartTime);
		BR.Read(m_AllAniInfoVec[AniIdx]->EndTime);
		BR.Read(m_AllAniInfoVec[AniIdx]->StartFrame);
		BR.Read(m_AllAniInfoVec[AniIdx]->EndFrame);
		BR.Read(m_AllAniInfoVec[AniIdx]->TimeLength);
		BR.Read(m_AllAniInfoVec[AniIdx]->TimeMode);
	}

	UINT BoneSize = 0;
	BR.Read(BoneSize);
	m_AllBoneVec.resize(BoneSize);
	for (size_t BoneIdx = 0; BoneIdx < m_AllBoneVec.size(); ++BoneIdx)
	{
		m_AllBoneVec[BoneIdx] = new CBone();
		BR.Read(m_AllBoneVec[BoneIdx]->Name);
		BR.Read(m_AllBoneVec[BoneIdx]->Depth);
		BR.Read(m_AllBoneVec[BoneIdx]->Index);
		BR.Read(m_AllBoneVec[BoneIdx]->FbxBoneMat);
		BR.Read(m_AllBoneVec[BoneIdx]->BoneMat);
		BR.Read(m_AllBoneVec[BoneIdx]->FbxOffsetMat);
		BR.Read(m_AllBoneVec[BoneIdx]->OffsetMat);

		UINT FrameMapSize;
		BR.Read(FrameMapSize);
		std::wstring AnimKey;
		m_AllBoneVec[BoneIdx]->KeyFrameMap.resize(FrameMapSize);

		for (UINT i = 0; i < FrameMapSize; i++)
		{
			UINT FrameSize = 0;
			BR.Read(FrameSize);

			m_AllBoneVec[BoneIdx]->KeyFrameMap[i].resize(FrameSize);

			if (0 < FrameSize)
			{
				BR.Read(&(m_AllBoneVec[BoneIdx]->KeyFrameMap[i][0]), sizeof(CBoneKeyFrame) * FrameSize);
			}
		}

		m_AllBoneFindMap.insert(std::unordered_multimap<std::string, SPTR<CBone>>::value_type(m_AllBoneVec[BoneIdx]->Name, m_AllBoneVec[BoneIdx]));

	}

	return true;
}

const void CFbx::BinarySave(const wchar_t * _Path)
{
	wchar_t ArrDrive[MAXSTRING];
	wchar_t ArrFolder[MAXSTRING];
	wchar_t ArrFile[MAXSTRING];
	wchar_t ArrExt[MAXSTRING];

	_wsplitpath_s(_Path, ArrDrive, ArrFolder, ArrFile, ArrExt);

	std::string FileName = CW2A(ArrFile, CP_UTF8);

	CWriteStream BW = CWriteStream(_Path);
	UINT MeshSize = (UINT)m_AllMeshVec.size();
	BW.Write(MeshSize);
	for (size_t Vtx = 0; Vtx < m_AllMeshVec.size(); Vtx++)
	{
		BW.Write(m_AllMeshVec[Vtx]->Name);
		UINT VTXCOUNT = (UINT)m_AllMeshVec[Vtx]->VtxDataVec.size();
		BW.Write(VTXCOUNT);
		BW.Write((void*)&(m_AllMeshVec[Vtx]->VtxDataVec[0]), (UINT)(sizeof(FBXVTX) * VTXCOUNT));
		wchar_t ARR[100];
		swprintf_s(ARR, L"%d \n", VTXCOUNT);
		OutputDebugString(ARR);

		//for (size_t Idx = 0; Idx < length; ++Idx)
		//{
		//	if (true == m_AllMeshVec[Vtx]->IdxDataVec[Idx].empty())
		//	{
		//		IdxDataVec.erase(IdxDataVec.begin() + Idx);
		//		MaterialVec.erase(MaterialVec.begin() + Idx);
		//		MAXIB = (UINT)IdxDataVec.size();
		//		continue;
		//	}
		//}


		UINT SUBCOUNT = (UINT)m_AllMeshVec[Vtx]->IdxDataVec.size();
		BW.Write(SUBCOUNT);
		for (size_t SubSet = 0; SubSet < m_AllMeshVec[Vtx]->IdxDataVec.size(); SubSet++)
		{
			UINT IDXCOUNT = (UINT)m_AllMeshVec[Vtx]->IdxDataVec[SubSet].size();
			if (0 >= IDXCOUNT)
			{
				continue;
			}
			BW.Write(IDXCOUNT);
			BW.Write((void*)&(m_AllMeshVec[Vtx]->IdxDataVec[SubSet][0]), (UINT)(sizeof(DWORD) * IDXCOUNT));
		}

		UINT MATCOUNT = (UINT)m_AllMeshVec[Vtx]->MaterialVec.size();
		BW.Write(MATCOUNT);
		for (size_t MatSet = 0; MatSet < m_AllMeshVec[Vtx]->MaterialVec.size(); MatSet++)
		{
			BW.Write(m_AllMeshVec[Vtx]->MaterialVec[MatSet]->MaterialColor);
			//std::string SaveTex = GetSaveTexName( m_AllMeshVec[Vtx]->MaterialVec[MatSet]->DiffTexName, FileName);

			BW.Write(GetSaveTexName(m_AllMeshVec[Vtx]->MaterialVec[MatSet]->DiffTexName, FileName));
			BW.Write(GetSaveTexName(m_AllMeshVec[Vtx]->MaterialVec[MatSet]->BumpTexName, FileName));
			BW.Write(GetSaveTexName(m_AllMeshVec[Vtx]->MaterialVec[MatSet]->EmivTexName, FileName));
			BW.Write(GetSaveTexName(m_AllMeshVec[Vtx]->MaterialVec[MatSet]->SpecTexName, FileName));
		}
	}

	UINT AniSize = (UINT)m_AllAniInfoVec.size();
	BW.Write(AniSize);
	for (size_t AniIdx = 0; AniIdx < m_AllAniInfoVec.size(); AniIdx++)
	{
		BW.Write(m_AllAniInfoVec[AniIdx]->AnimIdx);
		BW.Write(m_AllAniInfoVec[AniIdx]->Name);
		BW.Write(m_AllAniInfoVec[AniIdx]->StartTime);
		BW.Write(m_AllAniInfoVec[AniIdx]->EndTime);
		BW.Write(m_AllAniInfoVec[AniIdx]->StartTime);
		BW.Write(m_AllAniInfoVec[AniIdx]->EndFrame);
		BW.Write(m_AllAniInfoVec[AniIdx]->TimeLength);
		BW.Write(m_AllAniInfoVec[AniIdx]->TimeMode);
	}

	UINT BoneSize = (UINT)m_AllBoneVec.size();
	BW.Write(BoneSize);

	for (size_t BoneIdx = 0; BoneIdx < m_AllBoneVec.size(); BoneIdx++)
	{
		BW.Write(m_AllBoneVec[BoneIdx]->Name);
		BW.Write(m_AllBoneVec[BoneIdx]->Depth);
		BW.Write(m_AllBoneVec[BoneIdx]->Index);
		BW.Write(m_AllBoneVec[BoneIdx]->FbxBoneMat);
		BW.Write(m_AllBoneVec[BoneIdx]->BoneMat);
		BW.Write(m_AllBoneVec[BoneIdx]->FbxOffsetMat);
		BW.Write(m_AllBoneVec[BoneIdx]->OffsetMat);

		UINT FrameMapSize = (UINT)m_AllBoneVec[BoneIdx]->KeyFrameMap.size();
		BW.Write(FrameMapSize);

		for (UINT i = 0; i < FrameMapSize; i++)
		{
			UINT FrameSize = (UINT)m_AllBoneVec[BoneIdx]->KeyFrameMap[i].size();
			BW.Write(FrameSize);
			if (0 != FrameSize)
			{
				BW.Write(&(m_AllBoneVec[BoneIdx]->KeyFrameMap[i][0]), sizeof(CBoneKeyFrame) * FrameSize);
			}
		}
	}
}
const std::string CFbx::GetSaveTexName(const std::string& _TexPath, const std::string& _SaveFileName)
{
	std::string SaveTexName = _TexPath;

	if (true == _TexPath.empty())
		return SaveTexName;

	size_t Start = SaveTexName.find_last_of("\\");
	size_t End = SaveTexName.find_last_of("/");

	if (-1 == SaveTexName.find(_SaveFileName + ".fbm"))
	{
		SaveTexName.replace(Start + 1, End - Start - 1, _SaveFileName + ".fbm");
	}

	Start = SaveTexName.find_last_of("\\", Start - 1);
	SaveTexName.erase(0, Start + 1);

	return SaveTexName;
}

void CFbx::CheckTriangleTB()
{
	for (size_t m = 0; m < m_AllMeshVec.size(); ++m)
	{
		SPTR<CBoneMesh> Mesh = m_AllMeshVec[m];

		size_t IdxCount = Mesh->IdxDataVec.size();
		
		for (size_t  i = 0; i < IdxCount; ++i)
		{
			size_t TriCount = (size_t)Mesh->GetTriIdxCount((UINT)i);

			for (size_t Tri = 0; Tri < TriCount; Tri++)
			{
				DWORD Idx[3];
				Idx[0] = Mesh->IdxDataVec[i][Tri * 3];
				Idx[1] = Mesh->IdxDataVec[i][Tri * 3 + 1];
				Idx[2] = Mesh->IdxDataVec[i][Tri * 3 + 2];

				if (Idx[0] == 1353)
					int a = 10;


				Vec3 Tan, BNor;

				for (size_t j = 0; j < 3; ++j)
				{
					if (Mesh->VtxDataVec[Idx[j]].Tangent != Vec3())
					{
						Tan = Mesh->VtxDataVec[Idx[j]].Tangent;
						BNor = Mesh->VtxDataVec[Idx[j]].BiNormal;
						break;
					}
				}

				FBXVTX& Vtx0 = Mesh->VtxDataVec[Idx[0]];
				FBXVTX& Vtx1 = Mesh->VtxDataVec[Idx[1]];
				FBXVTX& Vtx2 = Mesh->VtxDataVec[Idx[2]];

				if (Tan == Vec3())
				{
					Vec3 Tangent = (Vtx1.Pos - Vtx0.Pos).GetNormalizeVec();
					Vec3 BiNormal = DXMath::Vector3Cross(Vtx0.Normal, Tangent).GetNormalizeVec();


					Vtx0.Tangent = Tangent;
					Vtx0.BiNormal = BiNormal;
					Vtx1.Tangent = Tangent;
					Vtx1.BiNormal = BiNormal;
					Vtx2.Tangent = Tangent;
					Vtx2.BiNormal = BiNormal;

					//TASSERT(true);
					continue;
				}

				for (size_t j = 0; j < 3; ++j)
				{
					if (Mesh->VtxDataVec[Idx[j]].Tangent == Vec3())
					{
						Vec3& T = Mesh->VtxDataVec[Idx[j]].Tangent;
						Vec3& B = Mesh->VtxDataVec[Idx[j]].BiNormal;

						T = Tan;
						B = BNor;
					}
				}
			}
		}
	}
}

const bool CFbx::FbxLoad(const LOAD_DESC& _Desc)
{
	//fbx파일의 전체 관리자
	m_FbxMgr = FbxManager::Create();

	//fbx파일의 바이너리 입출력에 대한 세팅
	m_IOSet = FbxIOSettings::Create(m_FbxMgr, IOSROOT);
	m_FbxMgr->SetIOSettings(m_IOSet);

	m_FbxScene = FbxScene::Create(m_FbxMgr, "");
	if (nullptr == m_FbxScene)
	{
		TASSERT(true);
		return false;
	}

	//실제 파일을 로드하기 위한 객체
	m_Imp = FbxImporter::Create(m_FbxMgr, "");
	if (nullptr == m_Imp)
	{
		TASSERT(true);
		return false;
	}

	if (false == m_Imp->Initialize(CW2A(GetPath().c_str(), CP_UTF8).m_psz, -1, m_FbxMgr->GetIOSettings()))
	{
		TASSERT(true);
		return false;
	}
	m_Imp->Import(m_FbxScene);

	if (m_FbxScene->GetGlobalSettings().GetAxisSystem() != FbxAxisSystem::eMax)
	{
		m_FbxScene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::eMax);
		m_eLocalSpace = FbxAxisSystem::eMax;
	}

	CBoneAniInfo* NewInfo = nullptr;
	if (m_eAnimationType != FBXANI_STATIC)
	{
		NewInfo = CheckExistAnimation(m_FbxScene, _Desc.AnimKey);
	}

	BoneDataSetting(m_FbxScene->GetRootNode(), nullptr, 0);
	m_AllBoneVec;


	if (true == IsAni() && m_eAnimationType != FBXANI_STATIC)
	{
		BoneAnimationSetting(NewInfo);
	}

	return true;
}


bool CFbx::BoneDataSetting(FbxNode* _Node, CBone* _Bone, UINT _Depth)
{
	FbxNodeAttribute* NodeAt = _Node->GetNodeAttribute();
	CBone* NewBone = nullptr;

	if (nullptr != NodeAt)
	{
		//본 카운트 증가
		++m_AllBoneCount;

		//삼각화(좌표계 변환)
		FbxGeometryConverter FbxCon(m_FbxMgr);
		FbxCon.Triangulate(NodeAt, true);

		//본 정보 저장
		NewBone = new CBone();
		NewBone->Name = _Node->GetName();
		NewBone->pNode = _Node;
		NewBone->Depth = ++_Depth;
		NewBone->Parent = _Bone;
		NewBone->Index = (UINT)m_AllBoneVec.size();
		m_AllBoneVec.push_back(NewBone);
		m_AllBoneFindMap.insert(std::unordered_multimap<std::string, SPTR<CBone>>::value_type(NewBone->Name, NewBone));

		//매쉬 정보 저장
		UINT MatCount = 0;
		FbxGeometryElementMaterial* TempMat = nullptr;
		if (NodeAt->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			FbxMesh* TempMesh = _Node->GetMesh();
			if (nullptr != TempMesh)
			{
				CBoneMesh* NewBoneMesh = new CBoneMesh();
				NewBoneMesh->pMesh = TempMesh;
				NewBoneMesh->Name = TempMesh->GetName();

				//정점정보 개수만큼 벡터 할당하기
				int VtxCount = TempMesh->GetControlPointsCount();
				NewBoneMesh->VtxDataVec.resize(VtxCount);
				NewBoneMesh->ParetteDataVec.resize(VtxCount);

				//정점정보 넣어주기
				FbxVector4* PosPtr = TempMesh->GetControlPoints();
				for (size_t i = 0; i < VtxCount; ++i)
				{
					NewBoneMesh->VtxDataVec[i].Pos.x = (float)PosPtr[i].mData[0];
					NewBoneMesh->VtxDataVec[i].Pos.y = (float)PosPtr[i].mData[2];
					NewBoneMesh->VtxDataVec[i].Pos.z = (float)PosPtr[i].mData[1];
				}

				//서브셋의 개수
				MatCount = _Node->GetMaterialCount();
				//삼각형의 개수
				UINT TriangleCount = TempMesh->GetPolygonCount();
				//인덱스 버퍼 사이즈 - 3개로 구성되어있는 인덱스 버퍼가 아니면 로드하지 않는다
				UINT IdxSize = TempMesh->GetPolygonSize(0);
				if (3 != IdxSize)
				{
					TASSERT(true);
					return false;
				}

				//서브셋 개수만큼 벡터 할당하기
				NewBoneMesh->IdxDataVec.resize(MatCount);
				//머테리얼 가져오기
				TempMat = TempMesh->GetElementMaterial();

				//인덱스 정보 넣어주기
				IDX32 TempIdx;
				UINT CurVtx = 0;

				for (UINT TriangleNum = 0; TriangleNum < TriangleCount; ++TriangleNum)
				{
					for (UINT SizeNum = 0; SizeNum < IdxSize; ++SizeNum)
					{
						TempIdx.pIdx[SizeNum] = TempMesh->GetPolygonVertex(TriangleNum, SizeNum);
						GetUVValue(TempMesh, NewBoneMesh, TempIdx.pIdx[SizeNum], CurVtx);
						//노말, 탄젠트 등이 추가되어야 한다
						GetNormalValue(TempMesh, NewBoneMesh, TempIdx.pIdx[SizeNum], CurVtx);
						GetTangentValue(TempMesh, NewBoneMesh, TempIdx.pIdx[SizeNum], CurVtx);
						GetBiNormalValue(TempMesh, NewBoneMesh, TempIdx.pIdx[SizeNum], CurVtx);

						++CurVtx;
					}

					int TriangleIdx = TempMat->GetIndexArray().GetAt(TriangleNum);
					NewBoneMesh->IdxDataVec[TriangleIdx].push_back(TempIdx.pIdx[0]);
					NewBoneMesh->IdxDataVec[TriangleIdx].push_back(TempIdx.pIdx[2]);
					NewBoneMesh->IdxDataVec[TriangleIdx].push_back(TempIdx.pIdx[1]);
				}

				SPTR<CBone> BoneFind = FindBone(_Node->GetName());
				if (nullptr == BoneFind)
				{
					TASSERT(true);
					return false;
				}

				BoneFind->MeshData = NewBoneMesh;
				m_AllMeshVec.push_back(NewBoneMesh);
			}

			//메테리얼 정보 저장
			SPTR<CBone> TempBone = FindBone(_Node->GetName());
			if (nullptr == TempBone || nullptr == TempBone->MeshData)
			{
				TASSERT(true);
				return false;
			}

			MatCount = _Node->GetMaterialCount();
			for (size_t i = 0; i < MatCount; ++i)
			{
				CBoneMaterial* NewMat = new CBoneMaterial();
				FbxSurfaceMaterial* Surface = _Node->GetMaterial((int)i);

				NewMat->MaterialColor.Diffuse = GetMatColor(Surface, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor);
				NewMat->MaterialColor.Ambient = GetMatColor(Surface, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor);
				NewMat->MaterialColor.Specular = GetMatColor(Surface, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor);
				//NewMat->MaterialColor.Emissive = GetMatColor(Surface, FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor);

				NewMat->DiffTexName = GetMatTexName(Surface, FbxSurfaceMaterial::sDiffuse);
				NewMat->BumpTexName = GetMatTexName(Surface, FbxSurfaceMaterial::sNormalMap);
				NewMat->SpecTexName = GetMatTexName(Surface, FbxSurfaceMaterial::sSpecular);
				NewMat->EmivTexName = GetMatTexName(Surface, FbxSurfaceMaterial::sEmissive);

				TempBone->MeshData->MaterialVec.push_back(NewMat);
			}
		}
	}

	int MaxChild = _Node->GetChildCount();

	for (int i = 0; i < MaxChild; ++i)
	{
		BoneDataSetting(_Node->GetChild(i), NewBone, _Depth);
	}

	return true;
}

SPTR<CBone> CFbx::FindBone(const char * _Name)
{
	std::unordered_multimap<std::string, SPTR<CBone>>::iterator FindIter = m_AllBoneFindMap.find(_Name);
	if (FindIter == m_AllBoneFindMap.end())
	{
		return nullptr;
	}
	return FindIter->second;
}

const UINT CFbx::FindBoneIdx(const char* _Name)
{
	std::unordered_multimap<std::string, SPTR<CBone>>::iterator FindIter = m_AllBoneFindMap.find(_Name);
	if (FindIter == m_AllBoneFindMap.end())
	{
		return -1;
	}
	return FindIter->second->Index;

}

bool CFbx::GetUVValue(FbxMesh * _Mesh, CBoneMesh * _MeshData, DWORD _CurIdx, UINT _CurVtx)
{
	int Count = _Mesh->GetElementUVCount();

	//if (1 != Count)
	//{
	//	//uv맵이 한장이 아니라면 로드하지 않는다
	//	return false;
	//}

	FbxGeometryElementUV* TempUv = _Mesh->GetElementUV();

	if (nullptr == TempUv)
		return false;

	UINT ValueCount = 0;
	if (TempUv->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (TempUv->GetReferenceMode() == FbxGeometryElement::eDirect)
		{
			ValueCount = _CurVtx;
		}
		else
		{
			ValueCount = TempUv->GetIndexArray().GetAt(_CurVtx);
		}
	}
	else if (TempUv->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (TempUv->GetReferenceMode() == FbxGeometryElement::eDirect)
		{
			ValueCount = _CurIdx;
		}
		else
		{
			ValueCount = TempUv->GetIndexArray().GetAt(_CurIdx);
		}
	}

	FbxVector2 UvData = TempUv->GetDirectArray().GetAt(ValueCount);
	_MeshData->VtxDataVec[_CurIdx].TexCoord.x = (float)UvData.mData[0];
	_MeshData->VtxDataVec[_CurIdx].TexCoord.y = 1.0f - (float)UvData.mData[1];

	return true;
}

bool CFbx::GetNormalValue(FbxMesh* _Mesh, CBoneMesh* _MeshData, DWORD _CurIdx, UINT _CurVtx)
{
	int Count = _Mesh->GetElementNormalCount();

	FbxGeometryElementNormal* pNormal = _Mesh->GetElementNormal();

	UINT ValueCount = 0;

	if (pNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (pNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
		{
			ValueCount = _CurVtx;
		}
		else
		{
			ValueCount = pNormal->GetIndexArray().GetAt(_CurVtx);
		}
	}
	else if (pNormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (pNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
		{
			ValueCount = _CurIdx;
		}
		else
		{
			ValueCount = pNormal->GetIndexArray().GetAt(_CurIdx);
		}
	}

	FbxVector4 NormalData = pNormal->GetDirectArray().GetAt(ValueCount);
	_MeshData->VtxDataVec[_CurIdx].Normal.x = (float)NormalData.mData[0];
	_MeshData->VtxDataVec[_CurIdx].Normal.y = (float)NormalData.mData[2];
	_MeshData->VtxDataVec[_CurIdx].Normal.z = (float)NormalData.mData[1];
	return true;
}

bool CFbx::GetTangentValue(FbxMesh* _Mesh, CBoneMesh* _MeshData, DWORD _CurIdx, UINT _CurVtx)
{
	int Count = _Mesh->GetElementNormalCount();

	FbxGeometryElementTangent* pTangent = _Mesh->GetElementTangent();

	if (nullptr == pTangent)
	{
		return false;
	}

	UINT ValueCount = 0;

	if (pTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (pTangent->GetReferenceMode() == FbxGeometryElement::eDirect)
		{
			ValueCount = _CurVtx;
		}
		else
		{
			ValueCount = pTangent->GetIndexArray().GetAt(_CurVtx);
		}
	}
	else if (pTangent->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (pTangent->GetReferenceMode() == FbxGeometryElement::eDirect)
		{
			ValueCount = _CurIdx;
		}
		else
		{
			ValueCount = pTangent->GetIndexArray().GetAt(_CurIdx);
		}
	}
	int b = pTangent->GetDirectArray().GetCount();
	FbxVector4 TangentData = pTangent->GetDirectArray().GetAt(ValueCount);
	_MeshData->VtxDataVec[_CurIdx].Tangent.x = (float)TangentData.mData[0];
	_MeshData->VtxDataVec[_CurIdx].Tangent.y = (float)TangentData.mData[2];
	_MeshData->VtxDataVec[_CurIdx].Tangent.z = (float)TangentData.mData[1];

	return true;
}

bool CFbx::GetBiNormalValue(FbxMesh* _Mesh, CBoneMesh* _MeshData, DWORD _CurIdx, UINT _CurVtx)
{
	int Count = _Mesh->GetElementNormalCount();

	FbxGeometryElementBinormal* pBiNormal = _Mesh->GetElementBinormal();

	if (nullptr == pBiNormal)
	{
		return false;
	}

	UINT ValueCount = 0;

	if (pBiNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (pBiNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
		{
			ValueCount = _CurVtx;
		}
		else
		{
			ValueCount = pBiNormal->GetIndexArray().GetAt(_CurVtx);
		}
	}
	else if (pBiNormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (pBiNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
		{
			ValueCount = _CurIdx;
		}
		else
		{
			ValueCount = pBiNormal->GetIndexArray().GetAt(_CurIdx);
		}
	}

	FbxVector4 BiNormalData = pBiNormal->GetDirectArray().GetAt(ValueCount);
	_MeshData->VtxDataVec[_CurIdx].BiNormal.x = (float)BiNormalData.mData[0];
	_MeshData->VtxDataVec[_CurIdx].BiNormal.y = (float)BiNormalData.mData[2];
	_MeshData->VtxDataVec[_CurIdx].BiNormal.z = (float)BiNormalData.mData[1];

	return true;
}
Vec4 CFbx::GetMatColor(FbxSurfaceMaterial * _Surf, const char * _ColorName, const char * _FactorName)
{
	FbxDouble3 Color;
	FbxDouble Factor;

	FbxProperty ColorPro = _Surf->FindProperty(_ColorName);
	FbxProperty FactorPro = _Surf->FindProperty(_FactorName);

	Vec4 ReturnColor = Vec4();
	if (true == ColorPro.IsValid())
	{
		Color = ColorPro.Get<FbxDouble3>();
		ReturnColor.r = (float)Color.mData[0];
		ReturnColor.g = (float)Color.mData[1];
		ReturnColor.b = (float)Color.mData[2];
	}
	if (true == FactorPro.IsValid())
	{
		Factor = FactorPro.Get<FbxDouble>();
		ReturnColor.a = (float)Factor;
	}
	return ReturnColor;
}

std::string CFbx::GetMatTexName(FbxSurfaceMaterial * _Surf, const char * _TexName)
{
	std::string ReturnTexName;
	FbxProperty TexName = _Surf->FindProperty(_TexName);

	if (true == TexName.IsValid())
	{
		UINT ObjCount = TexName.GetSrcObjectCount();
		if (0 != ObjCount)
		{
			FbxFileTexture* Tex = TexName.GetSrcObject<FbxFileTexture>(0);
			if (nullptr != Tex)
			{
				ReturnTexName = Tex->GetFileName();
			}
		}
	}

	return ReturnTexName;
}
// 애니메이션이 있는지 없는지만 체크한다.
CBoneAniInfo* CFbx::CheckExistAnimation(FbxScene* _pScene, const std::wstring& _AnimKey /*= L""*/)
{
	if (nullptr == _pScene)
		return nullptr;

	FbxArray<FbxString*>	arrAniName;

	_pScene->FillAnimStackNameArray(arrAniName);

	if (0 == arrAniName.Size())
	{
		return nullptr;
	}

	// 1. FBX로 아에 애니메이션 매쉬를가 따로 있다.
	// 2. 모든 애니메이션 뭉쳐져 놨다. 

	// Take One 애니메이션만 가져온다.
	for (int i = 0; i < 1; i++)
	{
		// 1. 애니메이션 이름으로 애니메이션과 관련된 정보를 가져온다
		FbxAnimStack* pAniStack = _pScene->FindMember<FbxAnimStack>(arrAniName[i]->Buffer());

		if (nullptr == pAniStack)
		{
			continue;
		}

		CBoneAniInfo* NewAniInfo = new CBoneAniInfo();
		if (false == _AnimKey.empty())
		{
			NewAniInfo->Name = _AnimKey;
		}
		else
		{
			NewAniInfo->Name = CA2W(pAniStack->GetName(), CP_UTF8).m_psz;
		}

		// 2. 애님스택의 애니메이션 이름을 이용하여 애니메이션 키 프레임을 가져온다(FbxTakInfo)
		FbxTakeInfo* TakeInfo = _pScene->GetTakeInfo(pAniStack->GetName());

		NewAniInfo->StartTime = TakeInfo->mLocalTimeSpan.GetStart();
		NewAniInfo->EndTime = TakeInfo->mLocalTimeSpan.GetStop();
		NewAniInfo->TimeMode = m_FbxScene->GetGlobalSettings().GetTimeMode();
		NewAniInfo->StartFrame = NewAniInfo->StartTime.GetFrameCount(NewAniInfo->TimeMode);
		NewAniInfo->EndFrame = NewAniInfo->EndTime.GetFrameCount(NewAniInfo->TimeMode);
		NewAniInfo->TimeLength = NewAniInfo->EndFrame - NewAniInfo->StartFrame;
		NewAniInfo->AnimIdx = (UINT)m_AllAniInfoVec.size();

		m_AllAniInfoVec.push_back(NewAniInfo);
	}

	for (int i = 0; i < arrAniName.GetCount(); ++i)
	{
		delete arrAniName[i];
	}

	return m_AllAniInfoVec.back();
}
// 각 본에 행렬을 넣어주는것.
void CFbx::BoneAnimationSetting(CBoneAniInfo* _AniInfo)
{
	for (size_t i = 0; i < m_AllMeshVec.size(); ++i)
	{
		// eSkin 스키닝 정보이다. 스키닝 정보가 몇개있는지 가져온다
		int Count = m_AllMeshVec[i]->pMesh->GetDeformerCount(FbxDeformer::eSkin);

		if (0 >= Count)
		{
			continue;
		}

		// 스키닝 정보 만큼  반복(기본 1)
		for (int SkinIdx = 0; SkinIdx < Count; ++SkinIdx)
		{
			FbxSkin* pSkin = (FbxSkin*)(m_AllMeshVec[i]->pMesh->GetDeformer(SkinIdx, FbxDeformer::eSkin));

			if (nullptr == pSkin)
			{
				continue;
			}

			FbxSkin::EType LerpType = pSkin->GetSkinningType();

			// 스키닝 방법이 Rigid or Linear 인 경우에만 생각하자 
			if (FbxSkin::EType::eRigid == LerpType
				|| FbxSkin::EType::eLinear == LerpType
				|| FbxSkin::EType::eBlend == LerpType)
			{
				// 결합부의 정점을 Cluster 라고하고
				// 클러스트가 모여 조인트(관절)가 형성된다.
				int MaxClusterCount = pSkin->GetClusterCount();

				FbxCluster* pCluster = nullptr;
				CBone* pBone = nullptr;
				for (int CIdx = 0; CIdx < MaxClusterCount; ++CIdx)
				{
					pCluster = pSkin->GetCluster(CIdx);

					if (nullptr == pCluster || nullptr == pCluster->GetLink())
					{
						continue;
					}

					pBone = FindBone(pCluster->GetLink()->GetName());

					if (nullptr == pBone)
					{
#ifdef _DEBUG
						assert(nullptr);
#endif
					}

					pBone->FbxBoneMat = GetBoneFbxMat(m_AllMeshVec[i]->pMesh->GetNode());
					pBone->BoneMat = ConvertFbxMatToMat(pBone->FbxBoneMat);

					FbxToParetteData(pCluster, pBone, m_AllMeshVec[i]);
					GetOffsetMat(pCluster, pBone, m_AllMeshVec[i]);


					GetKeyFrameMat(_AniInfo, m_AllMeshVec[i]->pMesh->GetNode(), pCluster, pBone);
				}
			}
		}

		ParetteDataToVtx(m_AllMeshVec[i]->pMesh, m_AllMeshVec[i]);
	}

	std::vector<SPTR<CBone>>& Check = m_AllBoneVec;
	int a = 0;
}
void CFbx::FbxToParetteData(FbxCluster* _Cluster, CBone* _pBone, CBoneMesh* _MeshData)
{
	int MaxBoneVtx = _Cluster->GetControlPointIndicesCount();

	CParetteData TempParette;
	double* pWeight = _Cluster->GetControlPointWeights();
	int* pIndices = _Cluster->GetControlPointIndices();

	for (size_t i = 0; i < MaxBoneVtx; ++i)
	{
		TempParette.BoneIndex = _pBone->Index;
		TempParette.dWeights = pWeight[i];
		TempParette.Indices = pIndices[i];
		_MeshData->ParetteDataVec[TempParette.Indices].push_back(TempParette);
	}

}
void CFbx::ParetteDataToVtx(FbxMesh* _pMesh, CBoneMesh* _pBoneMesh)
{
	// 가중치 누적을 연산한후 버텍스에 값을 넣어준다.
	std::vector<std::vector<CParetteData>>::iterator Start = _pBoneMesh->ParetteDataVec.begin();
	std::vector<std::vector<CParetteData>>::iterator End = _pBoneMesh->ParetteDataVec.end();

	for (int iVtxCount = 0; Start != End; ++Start, ++iVtxCount)
	{
		if (Start->size() > 1)
		{
			std::sort(Start->begin(), Start->end(), [](CParetteData& _Left, CParetteData& _Right)
			{
				return _Left.dWeights > _Right.dWeights;
			});

			double AccWeight = 0.0;
			double revision = 0.0;

			for (size_t i = 0; i < Start->size(); ++i)
			{
				AccWeight += (*Start)[i].dWeights;
			}

			// 가중치는 1을 넘을수 없지만 1을 초과한 가중치의 남은부분을 음수로 빼서 
			// 선두에 몰린 가중치를 줄여준다.
			if (AccWeight > 1.f)
			{
				revision = 1.f - AccWeight;
				(*Start)[0].dWeights += revision;
			}

			// 가장큰 4개의 가중치를 제외하곤 나머질 제거한다.

			if (Start->size() >= 4)
			{
				Start->erase(Start->begin() + 4, Start->end());
			}
		}

		for (size_t i = 0; i < Start->size(); ++i)
		{
			_pBoneMesh->VtxDataVec[iVtxCount].Weights.pf[i] = (float)(*Start)[i].dWeights;
			_pBoneMesh->VtxDataVec[iVtxCount].Indices.pi[i] = (*Start)[i].BoneIndex;
		}
	}

}
FbxAMatrix CFbx::GetBoneFbxMat(FbxNode* _Node)
{
	const FbxVector4 vTrans = _Node->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 vRot = _Node->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 vScale = _Node->GetGeometricScaling(FbxNode::eSourcePivot);
	return FbxAMatrix(vTrans, vRot, vScale);
}
void CFbx::GetOffsetMat(FbxCluster* _Cluster, CBone* _pBone, CBoneMesh* _Mesh)
{
	// OffsetMat * BoneMat =  애니메이션의 기본 행렬(T Bone 행렬)
	FbxAMatrix MatClusterTrans;
	FbxAMatrix MatClusterParentTrans;
	FbxAMatrix MatClusterLinkTrans;

	_Cluster->GetTransformMatrix(MatClusterTrans);
	_Cluster->GetTransformParentMatrix(MatClusterParentTrans);
	_Cluster->GetTransformLinkMatrix(MatClusterLinkTrans);

	FbxAMatrix Offset;
	FbxAMatrix Temp;
	FbxAMatrix Parent;

	// 링크된 클러스터의 역행렬 * 클러스터의 행렬 * 본의 행렬 = TBone 행렬
	Offset = MatClusterLinkTrans.Inverse() * MatClusterTrans * _pBone->FbxBoneMat;


	if (FbxAxisSystem::EPreDefinedAxisSystem::eMax == m_eLocalSpace)
	{
		_pBone->FbxOffsetMat = ReverseYZ * Offset *ReverseYZ;
	}

	_pBone->OffsetMat = ConvertFbxMatToMat(_pBone->FbxOffsetMat);

	int a = 0;
}
void CFbx::GetKeyFrameMat(CBoneAniInfo* _Info, FbxNode* _Node, FbxCluster* _Cluster, CBone* _pBone)
{
	SPTR<CBone> CurBone = nullptr;

	UINT CurMapIdx = _Info->AnimIdx;
	CurBone = m_AllBoneVec[_pBone->Index];

	if (CurBone->KeyFrameMap.size() <= CurMapIdx)
	{
		CurBone->KeyFrameMap.resize(CurMapIdx + 1);
	}
	CurBone->KeyFrameMap[CurMapIdx].resize(m_AllAniInfoVec[CurMapIdx]->EndFrame);

	for (FbxLongLong Frame = m_AllAniInfoVec[CurMapIdx]->StartFrame; Frame < m_AllAniInfoVec[CurMapIdx]->EndFrame; ++Frame)
	{
		KeyFrame FrameData = {};
		FbxTime tTime = 0;
		tTime.SetFrame(Frame, m_AllAniInfoVec[CurMapIdx]->TimeMode);

		// 본 , 메쉬 의 행렬을 가져온다
		FbxAMatrix MatFromTrans = _Node->EvaluateGlobalTransform(tTime);
		FbxAMatrix MatCurTrans = MatFromTrans.Inverse() * _Cluster->GetLink()->EvaluateGlobalTransform(tTime);

		// DirectX 에서 사용하는 공간으로 변경
		MatCurTrans = ReverseYZ * MatCurTrans * ReverseYZ;

		//FrameData.dTime = tTime.GetSecondDouble();
		FrameData.dTime = tTime.GetSecondDouble();
		FrameData.Trans = ConvertFbxVecToVec(MatCurTrans.GetT());
		FrameData.Scale = ConvertFbxVecToVec(MatCurTrans.GetS());
		FrameData.Quat = ConvertFbxQuatToVec(MatCurTrans.GetQ());

		CurBone->KeyFrameMap[CurMapIdx][Frame] = FrameData;
	}

}
const Mat CFbx::ConvertFbxMatToMat(const FbxAMatrix& _Mat)
{
	Mat ReturnMat;

	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 4; ++x)
		{
			ReturnMat.M[y][x] = (float)_Mat.Get(y, x);
		}
	}

	return ReturnMat;
}
const Vec4 CFbx::ConvertFbxVecToVec(const FbxVector4& _Vector)
{
	Vec4 ReturnVec;

	for (size_t i = 0; i < 4; ++i)
	{
		ReturnVec.pf[i] = (float)_Vector.mData[i];
	}
	return ReturnVec;
}
const Vec4 CFbx::ConvertFbxQuatToVec(const FbxQuaternion& _Quat)
{
	Vec4 ReturnVec;

	for (size_t i = 0; i < 4; ++i)
	{
		ReturnVec.pf[i] = (float)_Quat.mData[i];
	}
	return ReturnVec;
}
const int CFbx::GetAnimFPS(const unsigned int& _AnimTake)
{
	if (m_AllAniInfoVec.size() <= _AnimTake)
	{
#ifdef _DEBUG
		assert(nullptr);
#endif
	}

	switch (m_AllAniInfoVec[_AnimTake]->TimeMode)
	{
	case FbxTime::EMode::eFrames120:
		return 120;
	case FbxTime::EMode::eFrames100:
		return 100;
	case FbxTime::EMode::eFrames60:
		return 60;
	case FbxTime::EMode::eFrames30:
		return 30;
	default:
		break;
	}

	return 0;
}

const float CFbx::GetOneFrameTime(const unsigned int& _AnimTake)
{
	if (m_AllAniInfoVec.size() <= _AnimTake)
	{
#ifdef _DEBUG
		assert(nullptr);
#endif
	}

	switch (m_AllAniInfoVec[_AnimTake]->TimeMode)
	{
	case FbxTime::EMode::eFrames120:
		return ONEFRAME120;
	case FbxTime::EMode::eFrames100:
		return ONEFRAME60;
	case FbxTime::EMode::eFrames60:
		return ONEFRAME45;
	case FbxTime::EMode::eFrames30:
		return ONEFRAME30;
	default:
		break;
	}

	return 0.f;
}

void CFbx::LoadFbxData()
{
	CBoneMesh* BoneMesh = nullptr;
	UINT MaxMesh = (UINT)m_AllMeshVec.size();
	UINT MatCount = 0;

	for (UINT i = 0; i < MaxMesh; ++i)
	{
		BoneMesh = m_AllMeshVec[i];
		MatCount = (UINT)BoneMesh->MaterialVec.size();

		m_AllFBXMeshVec.push_back(BoneMesh->CreateFBXMesh());
		m_AllFBXMeshVec[i]->m_Fbx = this;
		m_AllFBXMeshVec[i]->m_MeshIdx = i;

		for (size_t Idx = 0; Idx < MatCount; ++Idx)
		{
			if (false == BoneMesh->MaterialVec[Idx]->DiffTexName.empty())
			{
				CResourceMgr<CTexture>::Load(CA2W(BoneMesh->MaterialVec[Idx]->DiffTexName.c_str(), CP_UTF8).m_psz);
			}

			if (false == BoneMesh->MaterialVec[Idx]->BumpTexName.empty())
			{
				CResourceMgr<CTexture>::Load(CA2W(BoneMesh->MaterialVec[Idx]->BumpTexName.c_str(), CP_UTF8).m_psz);
			}

			if (false == BoneMesh->MaterialVec[Idx]->SpecTexName.empty())
			{
				CResourceMgr<CTexture>::Load(CA2W(BoneMesh->MaterialVec[Idx]->SpecTexName.c_str(), CP_UTF8).m_psz);
			}

			if (false == BoneMesh->MaterialVec[Idx]->EmivTexName.empty())
			{
				CResourceMgr<CTexture>::Load(CA2W(BoneMesh->MaterialVec[Idx]->EmivTexName.c_str(), CP_UTF8).m_psz);
			}
		}
	}
}

const bool CFbx::LoadFbxAnimation(const std::wstring& _FbxPath, const wchar_t* _AnimKey)
{
	if (nullptr == m_FbxMgr || nullptr == m_Imp)
		return false;

	FbxScene*  pFbxScene = FbxScene::Create(m_FbxMgr, "");

	if (nullptr == pFbxScene)
	{
		TASSERT(true);
		return false;
	}

	if (false == m_Imp->Initialize(CW2A(_FbxPath.c_str(), CP_UTF8).m_psz, -1, m_FbxMgr->GetIOSettings()))
	{
		TASSERT(true);
		return false;
	}

	m_Imp->Import(pFbxScene);

	if (pFbxScene->GetGlobalSettings().GetAxisSystem() != FbxAxisSystem::eMax)
	{
		pFbxScene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::eMax);
		m_eLocalSpace = FbxAxisSystem::eMax;
	}


	CBoneAniInfo* NewInfo = CheckExistAnimation(pFbxScene, _AnimKey);

	if (nullptr != NewInfo)
	{
		// FBX 에서 키 프레임 정보를 본에 넣는다.
		FbxNode* CurNode = pFbxScene->GetRootNode();
		std::vector<FbxMesh*> vecMesh;

		FindFbxMesh(vecMesh, pFbxScene->GetRootNode());
		LoadFbxKeyFrame(NewInfo, vecMesh, pFbxScene->GetRootNode());

		std::vector<CBone*> NoKeyFrameBone;

		for (size_t i = 0; i < m_AllBoneVec.size(); i++)
		{
			if (m_AllBoneVec[i]->KeyFrameMap.size() <= NewInfo->AnimIdx)
			{
				m_AllBoneVec[i]->KeyFrameMap.resize(NewInfo->AnimIdx + 1);
				NoKeyFrameBone.push_back(m_AllBoneVec[i]);
			}
		}

		NoKeyFrameBone;

		vecMesh.clear();
	}

	SAFE_DESTROY(pFbxScene)
		return true;
}
void CFbx::FindFbxMesh(std::vector<FbxMesh*>& _vecMesh, FbxNode* _Node)
{
	if (nullptr == _Node)
		return;

	FbxMesh* Mesh = _Node->GetMesh();

	if (nullptr != Mesh)
		_vecMesh.push_back(Mesh);

	UINT MaxChild = _Node->GetChildCount();

	for (UINT i = 0; i < MaxChild; i++)
	{
		FindFbxMesh(_vecMesh, _Node->GetChild(i));
	}
}

void CFbx::LoadFbxKeyFrame(CBoneAniInfo* _AniInfo, std::vector<FbxMesh*>& _vecMesh, FbxNode* _Node)
{
	SPTR<CBone> pBone = nullptr;

	UINT  MaxMesh = (UINT)_vecMesh.size();

	for (UINT i = 0; i < MaxMesh; ++i)
	{
		int Count = _vecMesh[i]->GetDeformerCount(FbxDeformer::eSkin);

		for (int SkinIdx = 0; SkinIdx < Count; ++SkinIdx)
		{
			FbxSkin* pSkin = (FbxSkin*)(_vecMesh[i]->GetDeformer(SkinIdx, FbxDeformer::eSkin));

			if (nullptr == pSkin)
			{
				continue;
			}

			FbxSkin::EType LerpType = pSkin->GetSkinningType();

			int MaxClusterCount = pSkin->GetClusterCount();
			FbxCluster* pCluster = nullptr;

			for (int CIdx = 0; CIdx < MaxClusterCount; ++CIdx)
			{
				pCluster = pSkin->GetCluster(CIdx);

				if (nullptr == pCluster || nullptr == pCluster->GetLink())
				{
					continue;
				}

				pBone = FindBone(pCluster->GetLink()->GetName());
				std::string Name = pCluster->GetLink()->GetName();

				if (nullptr == pBone)
				{
#ifdef _DEBUG
					assert(nullptr);
#endif
					continue;
				}

				GetKeyFrameMat(_AniInfo, _vecMesh[i]->GetNode(), pCluster, pBone);
			}
		}
	}

}

void CFbx::CreateFBXSubTexture()
{
	// 계산된 스키닝 행렬을 담을 텍스쳐(GPU 업데이트 용)
	m_ComputeBoneMatTex = CTexture::CreateTexture((UINT)m_AllBoneVec.size() * 4, 1
		, DXGI_FORMAT_R32G32B32A32_FLOAT
		, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS
		, D3D11_USAGE_DEFAULT);

	UINT BoneCount = (UINT)m_AllBoneVec.size();
	Mat* pBOMat = new Mat[BoneCount * 2];

	for (UINT i = 0; i < BoneCount; i++)
	{
		pBOMat[i] = m_AllBoneVec[i]->BoneMat;
		pBOMat[i + BoneCount] = m_AllBoneVec[i]->OffsetMat;
	}


	m_BoneOffsetTex = CTexture::CreateTexture((void*)pBOMat
		, (UINT)BoneCount * 4, 2
		, DXGI_FORMAT_R32G32B32A32_FLOAT
		, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS
		, D3D11_USAGE_DEFAULT);


	delete[] pBOMat;

	size_t AnimCount = m_AllAniInfoVec.size();
	UINT KFCount;

	for (size_t Ani = 0; Ani < AnimCount; ++Ani)
	{
		KFCount = (UINT)(m_AllAniInfoVec[Ani]->EndFrame - m_AllAniInfoVec[Ani]->StartFrame);
		Mat* pKFMap = new Mat[BoneCount * KFCount];

		ZeroMemory(pKFMap, sizeof(Mat)*BoneCount * KFCount);

		for (size_t K = 0; K < KFCount; ++K)
		{
			for (size_t B = 0; B < BoneCount; ++B)
			{
				if (true == m_AllBoneVec[B]->KeyFrameMap.empty() 
					|| true == m_AllBoneVec[B]->KeyFrameMap[0].empty())
				{
					continue;
				}

				pKFMap[BoneCount * K + B].Row[0] = m_AllBoneVec[B]->KeyFrameMap[Ani][K].Scale;
				pKFMap[BoneCount * K + B].Row[1] = m_AllBoneVec[B]->KeyFrameMap[Ani][K].Trans;
				pKFMap[BoneCount * K + B].Row[2] = m_AllBoneVec[B]->KeyFrameMap[Ani][K].Quat;
				pKFMap[BoneCount * K + B]._41 = (float)m_AllBoneVec[B]->KeyFrameMap[Ani][K].dTime;
			}
		}

		m_AllAniInfoVec[Ani]->KeyFrameTex = CTexture::CreateTexture((void*)pKFMap
			, BoneCount * 4
			, KFCount
			, DXGI_FORMAT_R32G32B32A32_FLOAT
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS
			, D3D11_USAGE_DEFAULT);

		delete[] pKFMap;

	}
}
SPTR<CFBXMesh> CFbx::GetFBXMesh(const int& _MeshIdx, const wchar_t* _MatKey)
{
	if (m_AllFBXMeshVec.size() <= _MeshIdx)
	{
#ifdef _DEBUG
		assert(nullptr);
#endif
		return nullptr;
	}

	SPTR<CBoneMesh> BoneMesh = m_AllMeshVec[_MeshIdx];
	SPTR<CFBXMesh> FbxMesh = m_AllFBXMeshVec[_MeshIdx];

	SPTR<CMaterial> Mat = CResourceMgr<CMaterial>::Find(_MatKey);

	if (nullptr == Mat)
	{
#ifdef _DEBUG
		assert(nullptr);
#endif
		return nullptr;
	}

	SPTR<CFBXMesh> CloneMesh = FbxMesh->Clone();
	SPTR<CMaterial> CloneMat = nullptr;

	for (size_t Idx = 0; Idx < CloneMesh->m_vecMaterialList.size(); ++Idx)
	{
		CloneMat = Mat->Clone();
		CloneMat->m_DrawFbx = this;

		if (1 < BoneMesh->MaterialVec[Idx]->DiffTexName.size())
		{
			SPTR<CTexture> DiffTex = CResourceMgr<CTexture>::Find(CFilePathMgr::GetFileName(CA2W(BoneMesh->MaterialVec[Idx]->DiffTexName.c_str(), CP_UTF8).m_psz));

			if (nullptr != DiffTex)
				CloneMat->PushTexture(0, DiffTex->GetFileName().c_str(), SHADER_TYPE::SHADER_PIXEL);
		}

		if (1 < BoneMesh->MaterialVec[Idx]->BumpTexName.size())
		{
			SPTR<CTexture> BumpTex = CResourceMgr<CTexture>::Find(CFilePathMgr::GetFileName(CA2W(BoneMesh->MaterialVec[Idx]->BumpTexName.c_str(), CP_UTF8).m_psz));

			if (nullptr != BumpTex)
				CloneMat->PushTexture(1, BumpTex->GetFileName().c_str(), SHADER_TYPE::SHADER_PIXEL);
		}

		if (1 < BoneMesh->MaterialVec[Idx]->SpecTexName.size())
		{
			SPTR<CTexture> SpecTex = CResourceMgr<CTexture>::Find(CFilePathMgr::GetFileName(CA2W(BoneMesh->MaterialVec[Idx]->SpecTexName.c_str(), CP_UTF8).m_psz));

			if (nullptr != SpecTex)
				CloneMat->PushTexture(2, SpecTex->GetFileName().c_str(), SHADER_TYPE::SHADER_PIXEL);
		}

		if (1 < BoneMesh->MaterialVec[Idx]->EmivTexName.size())
		{
			SPTR<CTexture> EmivTex = CResourceMgr<CTexture>::Find(CFilePathMgr::GetFileName(CA2W(BoneMesh->MaterialVec[Idx]->EmivTexName.c_str(), CP_UTF8).m_psz));

			if (nullptr != EmivTex)
			{
				CloneMat->PushTexture(3, EmivTex->GetFileName().c_str(), SHADER_TYPE::SHADER_PIXEL);
			}
		}

		CloneMesh->m_vecMaterialList[Idx].push_back(CloneMat);
	}

	return CloneMesh;
}
