#ifndef __ASSIMP_HELPER_H__
#define __ASSIMP_HELPER_H__
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <optional>

#include "EngineStdafx.h"

#include <iostream>

#include "Resources.h"
#include "Texture.h"

BEGIN(ENGINE)
BEGIN(AssimpHelper)
static inline void DebugPrint(const aiMatrix4x4& AiMatrix)
{
	for (uint32 Row = 0u; Row < 4u; ++Row)
	{
		for (uint32 Col = 0u; Col < 4u; ++Col)
		{
			std::cout << AiMatrix[Row][Col] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

// 열기준 행렬을 전치해 DX의 행기준 행렬로 변환한다.
static inline Matrix ConvertMatrix(const aiMatrix4x4& AiMatrix)
{
	Matrix _Matrix;
	std::memcpy(&_Matrix, &AiMatrix, sizeof(aiMatrix4x4));
	D3DXMatrixTranspose(&_Matrix, &_Matrix);
	return _Matrix;
};

static inline Vector3 ConvertVec3(const aiVector3D& AiVector3D)
{
	return Vector3
	{
		AiVector3D.x ,
		AiVector3D.y ,
		AiVector3D.z
	};
};

static inline Quaternion ConvertQuat(const aiQuaternion& AiQuaternion)
{
	Quaternion _Quat;
	_Quat.x = AiQuaternion.x;
	_Quat.y = AiQuaternion.y;
	_Quat.z = AiQuaternion.z;
	_Quat.w = AiQuaternion.w;
	return _Quat;
};

static inline HRESULT LoadMesh(
	IN	const aiMesh* const				_pAiMesh,
	IN	LPDIRECT3DDEVICE9 const			_pDevice,
	OUT LPVERTEXBUFFERDESC const		_pVBDesc,
	OUT	LPDIRECT3DVERTEXBUFFER9* const	_ppVB,
	OUT LPDIRECT3DINDEXBUFFER9*	 const	_ppIB,
	// 본 이름과 본 (인덱스,오프셋) 매핑 정보 ... 
	IN  OUT std::unordered_map<std::string, 
						std::pair<uint32,Matrix>>* _rBoneTableParsingInfoFromName,
						const bool bLoadMeshStorageVertexLocations)
{
	static const  uint32 SupportMaxBoneNum = 8u;
#pragma region SET VERTEX BUFFER DESCRIPTION
	_pVBDesc->nNumFaces				= _pAiMesh->mNumFaces;
	_pVBDesc->nNumVertices			= _pAiMesh->mNumVertices;
	_pVBDesc->nNumUVChannel			= _pAiMesh->GetNumUVChannels();
	_pVBDesc->bHasPosition			= _pAiMesh->HasPositions();
	_pVBDesc->bHasNormal			= _pAiMesh->HasNormals();
	_pVBDesc->bHasTangentBiNormal	= _pAiMesh->HasTangentsAndBitangents();
	_pVBDesc->bHasBone              = _pAiMesh->HasBones();
	_pVBDesc->vecNumUVComponents.reserve(_pVBDesc->nNumUVChannel);
#pragma endregion
	// 키는 버텍스 인덱스   값은   본 인덱스와 가중치들
	std::map<uint32,
		std::vector< std::pair<uint32, float>> > BoneIdxAndWeights{};
	if (_rBoneTableParsingInfoFromName && _pVBDesc->bHasBone)
	{
		_pVBDesc->nNumBones=_pAiMesh->mNumBones;
		for (uint32 i = 0; i < _pAiMesh->mNumBones; ++i)
		{
			const auto* const CurBone=_pAiMesh->mBones[i];
			const std::string BoneName = CurBone->mName.C_Str();
			
			uint32 CurBoneIdx = 0u;

			auto iter = _rBoneTableParsingInfoFromName->find(BoneName);
			if (iter != std::end(*_rBoneTableParsingInfoFromName))
			{
				CurBoneIdx = iter->second.first;
			}
			else
			{
				 CurBoneIdx = _rBoneTableParsingInfoFromName->size();

				_rBoneTableParsingInfoFromName->insert({ BoneName,
					{
						CurBoneIdx,
						AssimpHelper::ConvertMatrix(CurBone->mOffsetMatrix)
					}});
			}

			for (uint32 j = 0u; j < CurBone->mNumWeights; ++j)
			{
				const auto WeightInfo = CurBone->mWeights[j];
				auto& CurVtxIdBoneIdxAndWeights = BoneIdxAndWeights[WeightInfo.mVertexId];
				CurVtxIdBoneIdxAndWeights.push_back
				(
					{ CurBoneIdx ,WeightInfo.mWeight }
				);
				if (CurVtxIdBoneIdxAndWeights.size() > _pVBDesc->nMaxBonesRefPerVtx)
				{
					_pVBDesc->nMaxBonesRefPerVtx = CurVtxIdBoneIdxAndWeights.size();
				}
			}
		}
	}
#pragma region SET VERTEXELEMENT & CREATE VERTEXDECLARATION
	std::vector<D3DVERTEXELEMENT9> vecElements;
	const bool MaxBonesRefPerVtx4Over = _pVBDesc->nMaxBonesRefPerVtx > 4u;
	if (_pVBDesc->nMaxBonesRefPerVtx > 8u)
	{
		PRINT_LOG(L"Warning!!",
			L"When the number of bones referenced per vertex exceeds 8, there is a chance that skinning will behave unexpectedly.");
	}
	
	if (_pVBDesc->bHasPosition)
	{
		vecElements.push_back
		(
			D3DVERTEXELEMENT9
			{
				0,
				(WORD)_pVBDesc->nStride,
				D3DDECLTYPE_FLOAT3,
				D3DDECLMETHOD_DEFAULT,
				D3DDECLUSAGE_POSITION,
				0
			}
		);
		_pVBDesc->nStride += (sizeof(float) * 3u);
	}

	if (_pVBDesc->bHasNormal)
	{
		vecElements.push_back
		(
			D3DVERTEXELEMENT9
			{
				0,
				(WORD)_pVBDesc->nStride,
				D3DDECLTYPE_FLOAT3,
				D3DDECLMETHOD_DEFAULT,
				D3DDECLUSAGE_NORMAL,
				0
			}
		);
		_pVBDesc->nStride += (sizeof(float) * 3u);
	}

	if (_pVBDesc->bHasTangentBiNormal)
	{
		vecElements.push_back
		(
			D3DVERTEXELEMENT9
			{
				0,
				(WORD)_pVBDesc->nStride,
				D3DDECLTYPE_FLOAT3,
				D3DDECLMETHOD_DEFAULT,
				D3DDECLUSAGE_TANGENT,
				0
			}
		);
		_pVBDesc->nStride += (sizeof(float) * 3u);

		vecElements.push_back
		(
			D3DVERTEXELEMENT9
			{
				0,
				(WORD)_pVBDesc->nStride,
				D3DDECLTYPE_FLOAT3,
				D3DDECLMETHOD_DEFAULT,
				D3DDECLUSAGE_BINORMAL,
				0
			}
		);
		_pVBDesc->nStride += (sizeof(float) * 3u);
	}

	for (BYTE i = 0; i < _pVBDesc->nNumUVChannel; ++i)
	{
		_pVBDesc->vecNumUVComponents.push_back(_pAiMesh->mNumUVComponents[i]);

		D3DDECLTYPE eDeclType = D3DDECLTYPE_FLOAT2;
		//eDeclType = (D3DDECLTYPE)(_pVBDesc->vecNumUVComponents[i] - 1);
		switch (_pVBDesc->vecNumUVComponents[i])
		{
		case 1:
			eDeclType = D3DDECLTYPE_FLOAT1;
			break;
		case 2:
			eDeclType = D3DDECLTYPE_FLOAT2;
			break;
		case 3:
			eDeclType = D3DDECLTYPE_FLOAT3;
			break;
		case 4:
			eDeclType = D3DDECLTYPE_FLOAT4;
			break;
		default:
			break;
		}

		vecElements.push_back
		(
			D3DVERTEXELEMENT9
			{
				0,
				(WORD)_pVBDesc->nStride,
				(BYTE)eDeclType,
				D3DDECLMETHOD_DEFAULT,
				D3DDECLUSAGE_TEXCOORD,
				i
			}
		);
		_pVBDesc->nStride += (sizeof(float) * _pVBDesc->vecNumUVComponents[i]);
	}
	
	if (_pVBDesc->bHasBone && _rBoneTableParsingInfoFromName)
	{
		vecElements.push_back
		(
			D3DVERTEXELEMENT9
			{
				0,
				(WORD)_pVBDesc->nStride,
				D3DDECLTYPE_FLOAT4,
				D3DDECLMETHOD_DEFAULT,
				D3DDECLUSAGE_BLENDINDICES,
				0 
			}
		);

		_pVBDesc->nStride += (sizeof(float) * 4u);

		if (MaxBonesRefPerVtx4Over)
		{
			vecElements.push_back
			(
				D3DVERTEXELEMENT9
				{
					0,
					(WORD)_pVBDesc->nStride,
					D3DDECLTYPE_FLOAT4,
					D3DDECLMETHOD_DEFAULT,
					D3DDECLUSAGE_BLENDINDICES,
					1
				}
			);
			_pVBDesc->nStride += (sizeof(float) * 4u);
		}
		

		vecElements.push_back
		(
			D3DVERTEXELEMENT9
			{
				0,
				(WORD)_pVBDesc->nStride,
				D3DDECLTYPE_FLOAT4,
				D3DDECLMETHOD_DEFAULT,
				D3DDECLUSAGE_BLENDWEIGHT,
				0
			}
		);
		_pVBDesc->nStride += (sizeof(float) * 4u);

		if (MaxBonesRefPerVtx4Over)
		{
			vecElements.push_back
			(
				D3DVERTEXELEMENT9
				{
					0,
					(WORD)_pVBDesc->nStride,
					D3DDECLTYPE_FLOAT4,
					D3DDECLMETHOD_DEFAULT,
					D3DDECLUSAGE_BLENDWEIGHT,
					1
				}
			);
			_pVBDesc->nStride += (sizeof(float) * 4u);
		}
	}

	vecElements.push_back(D3DDECL_END());

	if (FAILED(_pDevice->CreateVertexDeclaration(vecElements.data(), &(_pVBDesc->pVertexDecl))))
	{
		PRINT_LOG(TEXT("Failed to load mesh from AiMesh."), TEXT("Failed to CreateVertexDeclaration"));
		return E_FAIL;
	}
#pragma endregion

#pragma region CREATE VERTEX BUFFER

	//_pAiMesh로 부터 메쉬를 이루는 정점 정보 로드
	std::vector<float>	vecVertices;
	const uint32 RefBoneMaxIdx = MaxBonesRefPerVtx4Over ? 8u : 4u;

	std::optional< std::vector<Vector3>> VertexLocations{};
	if (bLoadMeshStorageVertexLocations)
	{
		VertexLocations = std::vector<Vector3>();
		VertexLocations->resize(_pVBDesc->nNumVertices);
	}

	for (UINT nVertexIdx = 0; nVertexIdx < _pVBDesc->nNumVertices; ++nVertexIdx)
	{
		if (_pVBDesc->bHasPosition)
		{
			vecVertices.push_back(_pAiMesh->mVertices[nVertexIdx].x);
			vecVertices.push_back(_pAiMesh->mVertices[nVertexIdx].y);
			vecVertices.push_back(_pAiMesh->mVertices[nVertexIdx].z);
			if (VertexLocations)
			{
				(*VertexLocations)[nVertexIdx] = AssimpHelper::ConvertVec3(_pAiMesh->mVertices[nVertexIdx]);
			}
			
		}

		if (_pVBDesc->bHasNormal)
		{
			vecVertices.push_back(_pAiMesh->mNormals[nVertexIdx].x);
			vecVertices.push_back(_pAiMesh->mNormals[nVertexIdx].y);
			vecVertices.push_back(_pAiMesh->mNormals[nVertexIdx].z);
		}

		if (_pVBDesc->bHasTangentBiNormal)
		{
			vecVertices.push_back(_pAiMesh->mTangents[nVertexIdx].x);
			vecVertices.push_back(_pAiMesh->mTangents[nVertexIdx].y);
			vecVertices.push_back(_pAiMesh->mTangents[nVertexIdx].z);

			vecVertices.push_back(_pAiMesh->mBitangents[nVertexIdx].x);
			vecVertices.push_back(_pAiMesh->mBitangents[nVertexIdx].y);
			vecVertices.push_back(_pAiMesh->mBitangents[nVertexIdx].z);
		}

		for (uint32 nChannelIdx = 0; nChannelIdx < _pVBDesc->nNumUVChannel; ++nChannelIdx)
		{
			const uint32 nCurNumUVComponents = _pVBDesc->vecNumUVComponents[nChannelIdx];
			const float* TexCoordPtr = reinterpret_cast<const float*>(&(_pAiMesh->mTextureCoords[nChannelIdx][nVertexIdx]));
			
			for (uint32 i = 0; i < nCurNumUVComponents; ++i)
				vecVertices.push_back(TexCoordPtr[i]);
		}

		if (_pVBDesc->bHasBone && _rBoneTableParsingInfoFromName)
		{
			auto iter = BoneIdxAndWeights.find(nVertexIdx);
			const auto& RefBoneInfos =iter->second;
			for (const auto& RefBoneInfo : RefBoneInfos)
			{
				vecVertices.push_back(RefBoneInfo.first);
			}
			for (uint32 i = RefBoneInfos.size(); i < RefBoneMaxIdx; ++i)
			{
				vecVertices.push_back(0.0f);
			}
			for (const auto& RefBoneInfo : RefBoneInfos)
			{
				vecVertices.push_back(RefBoneInfo.second);
			}
			for (uint32 i = RefBoneInfos.size(); i < RefBoneMaxIdx; ++i)
			{
				vecVertices.push_back(0.0f);
			}
		}
	}

	//정점 버퍼 생성
	_pVBDesc->nBufferSize = _pVBDesc->nNumVertices * _pVBDesc->nStride;
		
	if (FAILED(_pDevice->CreateVertexBuffer(_pVBDesc->nBufferSize, D3DUSAGE_WRITEONLY, NULL, D3DPOOL_MANAGED, _ppVB, nullptr)))
	{
		PRINT_LOG(TEXT("Failed to load mesh from AiMesh."), TEXT("Failed to CreateVertexBuffer"));
		return E_FAIL;
	}
	//정점 버퍼 데이터 설정.
	void* pVertices = nullptr;
	(*_ppVB)->Lock(0, 0, &pVertices, 0);
	memcpy(pVertices, vecVertices.data(), (size_t)_pVBDesc->nBufferSize);
	(*_ppVB)->Unlock();

	// 컬링을 위해 메쉬 로컬 공간에서 구체 크기를 설정 한다 .
	D3DXComputeBoundingSphere(
		(Vector3*)vecVertices.data(), _pVBDesc->nNumVertices, _pVBDesc->nStride,
		&_pVBDesc->BoundingSphere.Center, &_pVBDesc->BoundingSphere.Radius);
#pragma endregion

#pragma region CREATE INDEX BUFFER
	//_pAiMesh로 부터 메쉬를 이루는 면의 정점 인덱스 정보 로딩.
	std::vector<uint32> vecIndices;

	if (VertexLocations)
	{
		_pVBDesc->LocalVertexLocation = std::make_shared <std::vector<Vector3>>();
		_pVBDesc->LocalVertexLocation->resize(uint64(_pVBDesc->nNumFaces * 3u));
	}

	for (uint32 nFaceIdx = 0; nFaceIdx < _pVBDesc->nNumFaces; ++nFaceIdx)
	{
		const aiFace CurrFace = _pAiMesh->mFaces[nFaceIdx];
		for (uint32 nIdx = 0; nIdx < CurrFace.mNumIndices; ++nIdx)
		{
			const uint32 VertexIndex = CurrFace.mIndices[nIdx]; 
			vecIndices.push_back(VertexIndex);
			if (VertexLocations)
			{
				_pVBDesc->LocalVertexLocation->push_back( (*VertexLocations)[VertexIndex]);
			}
		}
	}
	//인덱스 버퍼 생성
	const uint32 IdxBufSize = sizeof(uint32) * vecIndices.size();
	
	if (FAILED(_pDevice->CreateIndexBuffer(IdxBufSize, D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_MANAGED, _ppIB, nullptr)))
	{
		SafeRelease(*_ppVB);
		PRINT_LOG(TEXT("Failed to load mesh from AiMesh."), TEXT("Failed to CreateIndexBuffer"));
		return E_FAIL;
	}
	//인덱스 버퍼 데이터 설정
	uint32* pIndices = nullptr;
	(*_ppIB)->Lock(0, 0, (void**)&pIndices, 0);
	memcpy(pIndices, vecIndices.data(), IdxBufSize);
	(*_ppIB)->Unlock();
#pragma endregion
	return S_OK;
}


static inline HRESULT LoadMaterial(
	IN const aiScene* const				_pAiScene,
	IN const aiMesh* const				_pAiMesh,
	IN const std::set<aiTextureType>&	_AiTexTypes,
	IN const std::filesystem::path&		_Path,
	IN const LPDIRECT3DDEVICE9			_pDevice,
	OUT LPMATERIAL						_pMaterial)
{
	if (false == _pAiScene->HasMaterials())
		return S_OK;

	//
	aiMaterial* pAiMaterial = _pAiScene->mMaterials[_pAiMesh->mMaterialIndex];
	//재질 이름 설정.
//	std::filesystem::path sBuf = pAiMaterial->GetName().C_Str();
//#ifdef UNICODE
//	std::wstring sName = sBuf.wstring();
//#else
//	std::string sName = sBuf.string();
//#endif
//	_pMaterial->sName = sName;

	//재질 정보 설정
	aiColor4D color;
	pAiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	_pMaterial->tMaterial.Diffuse = D3DCOLORVALUE(color.r, color.g, color.b, color.a);
	pAiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
	_pMaterial->tMaterial.Specular = D3DCOLORVALUE(color.r, color.g, color.b, color.a);
	pAiMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color);
	_pMaterial->tMaterial.Ambient = D3DCOLORVALUE(color.r, color.g, color.b, color.a);
	pAiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color);
	_pMaterial->tMaterial.Emissive = D3DCOLORVALUE(color.r, color.g, color.b, color.a);
	_pMaterial->tMaterial.Power = 1.f;
	_pMaterial->sName = pAiMaterial->GetName().C_Str();

	//재질 텍스처 로드.
	for (const auto& aiTexType : _AiTexTypes)
	{
		const uint32 nNumTypeTex = pAiMaterial->GetTextureCount(aiTexType);

		if (0 == nNumTypeTex)
			continue;

		//재질 텍스처를 저장할 컨테이너의 메모리 확보.
		_pMaterial->Textures[(UINT)aiTexType].resize(nNumTypeTex);

		for (uint32 nTexIdx = 0; nTexIdx < nNumTypeTex; ++nTexIdx)
		{
			aiString sAiPath;

			TEXTUREDESC			tTextureDesc;

			aiTextureMapping	eAiTextureMapping;
			unsigned int		nUVIdx;
			ai_real				fBlendFactor;
			aiTextureOp			eAiTextureOp;
			aiTextureMapMode	eAiTextureMapMode;

			if (AI_SUCCESS == pAiMaterial->GetTexture(
				aiTexType,
				nTexIdx,
				&sAiPath,
				&eAiTextureMapping,
				&nUVIdx,
				&fBlendFactor,
				&eAiTextureOp,
				&eAiTextureMapMode))
			{
				tTextureDesc.eMappingMode		= eAiTextureMapMode;
				tTextureDesc.eMappingType		= eAiTextureMapping;
				tTextureDesc.eTextureOperator	= eAiTextureOp;
				tTextureDesc.fBlendFactor		= fBlendFactor;
				tTextureDesc.nUVIdx				= nUVIdx;
				
				const std::filesystem::path FileNamePath   = sAiPath.C_Str();
				 std::filesystem::path	CurTexFilePath = _Path / FileNamePath.filename();
				 
				 if (std::filesystem::exists(CurTexFilePath)
					 && std::filesystem::is_regular_file(CurTexFilePath))
				 {
					 std::shared_ptr<Texture> pTexture = Resources::Load<Texture>(CurTexFilePath.wstring());

					 if (pTexture)
					 {
						 pTexture->SetDesc(tTextureDesc);
						 pTexture->ResourcePath = CurTexFilePath;
						 if (nullptr != pTexture)
							 _pMaterial->Textures[aiTexType][nTexIdx] = pTexture;
					 }
				 }
				 else
				 {
					 CurTexFilePath.replace_extension("tga");

					 if (std::filesystem::exists(CurTexFilePath)
						 && std::filesystem::is_regular_file(CurTexFilePath))
					 {
						 std::shared_ptr<Texture> pTexture = Resources::Load<Texture>(CurTexFilePath.wstring());

						 if (pTexture)
						 {
							 pTexture->SetDesc(tTextureDesc);
							 pTexture->ResourcePath = CurTexFilePath;
							 if (nullptr != pTexture)
								 _pMaterial->Textures[aiTexType][nTexIdx] = pTexture;
						 }
					 }
				 }
			}
		}
	}
	return S_OK;
}

END
END

#endif // !__ASSIMP_HELPER_H__