#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "framework.h"
#include "StaticMesh.h"

#include "Subset.h"
#include "Texture.h"

#include "Physics.h"

StaticMesh::StaticMesh()
	: m_pTriangleMesh(nullptr)
	, m_pTriangleMeshDesc(nullptr)
{
}

StaticMesh::~StaticMesh()
{
	for (auto pSubset : m_vecSubset)
	{
		delete pSubset;
		pSubset = nullptr;
	}
	m_vecSubset.clear();
	m_vecSubset.shrink_to_fit();


	if (nullptr != m_pTriangleMesh)
		m_pTriangleMesh->release();
}

HRESULT StaticMesh::Load(std::filesystem::path _Path)
{
	auto AiImporter = Assimp::Importer{};

	_Path = std::filesystem::absolute(_Path);
	std::filesystem::path curPath = std::filesystem::absolute(".\\");
	//_Path = _Path.relative_path();
	const aiScene* const pAiScene = AiImporter.ReadFile(
		_Path.string(),
		aiProcess_MakeLeftHanded |
		aiProcess_FlipUVs |
		aiProcess_FlipWindingOrder |
		aiProcess_Triangulate |
		aiProcess_CalcTangentSpace |
		aiProcess_ValidateDataStructure |
		aiProcess_ImproveCacheLocality |
		aiProcess_RemoveRedundantMaterials |
		aiProcess_GenUVCoords |
		aiProcess_TransformUVCoords |
		aiProcess_FindInstances |
		aiProcess_GenSmoothNormals |
		aiProcess_SortByPType |
		aiProcess_OptimizeMeshes |
		aiProcess_SplitLargeMeshes |
		aiProcess_JoinIdenticalVertices
	); 

	if (nullptr == pAiScene)
		return E_FAIL;

	m_vecSubset.resize(pAiScene->mNumMeshes);

	for (UINT nMeshIdx = 0; nMeshIdx < pAiScene->mNumMeshes; ++nMeshIdx)
	{
		const auto* const pAiMesh = pAiScene->mMeshes[nMeshIdx];

		Subset* pSubset = new Subset;

		pSubset->m_tVBDesc.nNumFaces			= pAiMesh->mNumFaces;
		pSubset->m_tVBDesc.nNumVertices			= pAiMesh->mNumVertices;
		pSubset->m_tVBDesc.nNumUVChannel		= pAiMesh->GetNumUVChannels();
		pSubset->m_tVBDesc.bHasPosition			= pAiMesh->HasPositions();
		pSubset->m_tVBDesc.bHasNormal			= pAiMesh->HasNormals();
		pSubset->m_tVBDesc.bHasTangentBiNormal	= pAiMesh->HasTangentsAndBitangents();
		pSubset->m_tVBDesc.bHasBone				= pAiMesh->HasBones();
		pSubset->m_tVBDesc.vecNumUVComponents.reserve(pSubset->m_tVBDesc.nNumUVChannel);

		std::vector<D3DVERTEXELEMENT9> vecElements;
#pragma region VERTEX DECL

		if (pSubset->m_tVBDesc.bHasPosition)
		{
			vecElements.push_back
			(
				D3DVERTEXELEMENT9
				{
					0,
					(WORD)pSubset->m_tVBDesc.nStride,
					D3DDECLTYPE_FLOAT3,
					D3DDECLMETHOD_DEFAULT,
					D3DDECLUSAGE_POSITION,
					0
				}
			);
			pSubset->m_tVBDesc.nStride += (sizeof(float) * 3u);
		}

		if (pSubset->m_tVBDesc.bHasNormal)
		{
			vecElements.push_back
			(
				D3DVERTEXELEMENT9
				{
					0,
					(WORD)pSubset->m_tVBDesc.nStride,
					D3DDECLTYPE_FLOAT3,
					D3DDECLMETHOD_DEFAULT,
					D3DDECLUSAGE_NORMAL,
					0
				}
			);
			pSubset->m_tVBDesc.nStride += (sizeof(float) * 3u);
		}

		if (pSubset->m_tVBDesc.bHasTangentBiNormal)
		{
			vecElements.push_back
			(
				D3DVERTEXELEMENT9
				{
					0,
					(WORD)pSubset->m_tVBDesc.nStride,
					D3DDECLTYPE_FLOAT3,
					D3DDECLMETHOD_DEFAULT,
					D3DDECLUSAGE_TANGENT,
					0
				}
			);
			pSubset->m_tVBDesc.nStride += (sizeof(float) * 3u);

			vecElements.push_back
			(
				D3DVERTEXELEMENT9
				{
					0,
					(WORD)pSubset->m_tVBDesc.nStride,
					D3DDECLTYPE_FLOAT3,
					D3DDECLMETHOD_DEFAULT,
					D3DDECLUSAGE_BINORMAL,
					0
				}
			);
			pSubset->m_tVBDesc.nStride += (sizeof(float) * 3u);
		}

		for (BYTE i = 0; i < pSubset->m_tVBDesc.nNumUVChannel; ++i)
		{
			pSubset->m_tVBDesc.vecNumUVComponents.push_back(pAiMesh->mNumUVComponents[i]);

			D3DDECLTYPE eDeclType = D3DDECLTYPE_FLOAT2;
			//eDeclType = (D3DDECLTYPE)(pSubset->m_tVBDesc.vecNumUVComponents[i] - 1);
			switch (pSubset->m_tVBDesc.vecNumUVComponents[i])
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
					(WORD)pSubset->m_tVBDesc.nStride,
					(BYTE)eDeclType,
					D3DDECLMETHOD_DEFAULT,
					D3DDECLUSAGE_TEXCOORD,
					i
				}
			);
			pSubset->m_tVBDesc.nStride += (sizeof(float) * pSubset->m_tVBDesc.vecNumUVComponents[i]);
		}

		vecElements.push_back(D3DDECL_END());

		if (FAILED(g_pDevice->CreateVertexDeclaration(vecElements.data(), &(pSubset->m_tVBDesc.pVertexDecl))))
		{
			delete pSubset;
			return E_FAIL;
		}
#pragma endregion

#pragma region VERTEX BUFFER
		std::vector<float> vecVertices;

		for (UINT nIdx = 0; nIdx < pSubset->m_tVBDesc.nNumVertices; ++nIdx)
		{
			if (pSubset->m_tVBDesc.bHasPosition)
			{

				vecVertices.push_back(pAiMesh->mVertices[nIdx].x);
				vecVertices.push_back(pAiMesh->mVertices[nIdx].y);
				vecVertices.push_back(pAiMesh->mVertices[nIdx].z);
				PxVec3 pxVPos(pAiMesh->mVertices[nIdx].x, pAiMesh->mVertices[nIdx].y, pAiMesh->mVertices[nIdx].z);
				m_vecVertices.push_back(pxVPos);
				//D3DXVECTOR3 vPos(pAiMesh->mVertices[nIdx].x, pAiMesh->mVertices[nIdx].y, pAiMesh->mVertices[nIdx].z);

				//D3DXMATRIX matScale;
				//D3DXMatrixScaling(&matScale, 0.01f, 0.01f, 0.01f);

				//D3DXVec3TransformCoord(&vPos, &vPos, &matScale);

				/*PxVec3 pxVPos(vPos.x, vPos.y, vPos.z);
				m_vecVertices.push_back(pxVPos);*/

				//vecVertices.push_back(vPos.x);
				//vecVertices.push_back(vPos.y);
				//vecVertices.push_back(vPos.z);
			}
			if (pSubset->m_tVBDesc.bHasNormal)
			{
				vecVertices.push_back(pAiMesh->mNormals[nIdx].x);
				vecVertices.push_back(pAiMesh->mNormals[nIdx].y);
				vecVertices.push_back(pAiMesh->mNormals[nIdx].z);
			}

			if (pSubset->m_tVBDesc.bHasTangentBiNormal)
			{
				vecVertices.push_back(pAiMesh->mTangents[nIdx].x);
				vecVertices.push_back(pAiMesh->mTangents[nIdx].y);
				vecVertices.push_back(pAiMesh->mTangents[nIdx].z);

				vecVertices.push_back(pAiMesh->mBitangents[nIdx].x);
				vecVertices.push_back(pAiMesh->mBitangents[nIdx].y);
				vecVertices.push_back(pAiMesh->mBitangents[nIdx].z);
			}

			for (UINT nChannelIdx = 0; nChannelIdx < pSubset->m_tVBDesc.nNumUVChannel; ++nChannelIdx)
			{
				const UINT nCurNumUVComponents = pSubset->m_tVBDesc.vecNumUVComponents[nChannelIdx];
				const float* TexCoordPtr = reinterpret_cast<const float*>(&(pAiMesh->mTextureCoords[nChannelIdx][nIdx]));

				for (UINT i = 0; i < nCurNumUVComponents; ++i)
					vecVertices.push_back(TexCoordPtr[i]);
			}
		}

		//정점 버퍼 생성
		pSubset->m_tVBDesc.nBufferSize = pSubset->m_tVBDesc.nNumVertices * pSubset->m_tVBDesc.nStride;

		if (FAILED(g_pDevice->CreateVertexBuffer(pSubset->m_tVBDesc.nBufferSize, D3DUSAGE_WRITEONLY, NULL, D3DPOOL_MANAGED, &pSubset->m_pVB, nullptr)))
		{
			delete pSubset;
			return E_FAIL;
		}

		//정점 버퍼 데이터 설정.
		void* pVertices = nullptr;
		pSubset->m_pVB->Lock(0, 0, &pVertices, 0);
		memcpy(pVertices, vecVertices.data(), (size_t)pSubset->m_tVBDesc.nBufferSize);
		pSubset->m_pVB->Unlock();
#pragma endregion

#pragma region INDEX BUFFER
		std::vector<UINT32> vecIndices;

		for (UINT nFaceIdx = 0; nFaceIdx < pSubset->m_tVBDesc.nNumFaces; ++nFaceIdx)
		{
			const aiFace CurrFace = pAiMesh->mFaces[nFaceIdx];
			for (UINT nIdx = 0; nIdx < CurrFace.mNumIndices; ++nIdx)
			{
				const UINT VertexIndex = CurrFace.mIndices[nIdx];
				vecIndices.push_back(VertexIndex);
				m_vecIndices.push_back(VertexIndex);
			}
		}

		//인덱스 버퍼 생성
		const UINT IdxBufSize = sizeof(UINT) * vecIndices.size();

		if (FAILED(g_pDevice->CreateIndexBuffer(IdxBufSize, D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_MANAGED, &pSubset->m_pIB, nullptr)))
		{
			delete pSubset;
			return E_FAIL;
		}
		//인덱스 버퍼 데이터 설정
		UINT* pIndices = nullptr;
		pSubset->m_pIB->Lock(0, 0, (void**)&pIndices, 0);
		memcpy(pIndices, vecIndices.data(), IdxBufSize);
		pSubset->m_pIB->Unlock();
#pragma endregion

#pragma region MATERIAL
		if (true == pAiScene->HasMaterials())
		{
			aiMaterial* pAiMaterial = pAiScene->mMaterials[pAiMesh->mMaterialIndex];

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
			pSubset->m_tMaterial.tMaterial.Diffuse = D3DCOLORVALUE(color.r, color.g, color.b, color.a);
			pAiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
			pSubset->m_tMaterial.tMaterial.Specular = D3DCOLORVALUE(color.r, color.g, color.b, color.a);
			pAiMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color);
			pSubset->m_tMaterial.tMaterial.Ambient = D3DCOLORVALUE(color.r, color.g, color.b, color.a);
			pAiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color);
			pSubset->m_tMaterial.tMaterial.Emissive = D3DCOLORVALUE(color.r, color.g, color.b, color.a);
			pSubset->m_tMaterial.tMaterial.Power = 1.f;
			pSubset->m_tMaterial.sName = pAiMaterial->GetName().C_Str();

			//

			const std::set<aiTextureType>& setAiTexType{
				aiTextureType::aiTextureType_AMBIENT,
				aiTextureType::aiTextureType_AMBIENT_OCCLUSION,
				aiTextureType::aiTextureType_BASE_COLOR,
				aiTextureType::aiTextureType_DIFFUSE,
				aiTextureType::aiTextureType_DIFFUSE_ROUGHNESS,
				aiTextureType::aiTextureType_DISPLACEMENT,
				aiTextureType::aiTextureType_EMISSION_COLOR,
				aiTextureType::aiTextureType_EMISSIVE,
				aiTextureType::aiTextureType_HEIGHT,
				aiTextureType::aiTextureType_LIGHTMAP,
				aiTextureType::aiTextureType_METALNESS,
				aiTextureType::aiTextureType_NORMALS,
				aiTextureType::aiTextureType_NORMAL_CAMERA,
				aiTextureType::aiTextureType_OPACITY,
				aiTextureType::aiTextureType_REFLECTION,
				aiTextureType::aiTextureType_SHININESS,
				aiTextureType::aiTextureType_SPECULAR,
				aiTextureType::aiTextureType_UNKNOWN
			};

			for (const auto& aiTexType : setAiTexType)
			{
				const UINT nNumTypeTex = pAiMaterial->GetTextureCount(aiTexType);

				if (0 == nNumTypeTex)
					continue;

				pSubset->m_tMaterial.Textures[(UINT)aiTexType].resize(nNumTypeTex);

				for (UINT nTexIdx = 0; nTexIdx < nNumTypeTex; ++nTexIdx)
				{
					aiString sAiPath;

					TEXTUREDESC tTexDesc;

					aiTextureMapping	eAiTexMapping;
					UINT				nUVIdx;
					ai_real				fBlendFactor;
					aiTextureOp			eAiTexOp;
					aiTextureMapMode	eAiTexMapMode;

					if (AI_SUCCESS == pAiMaterial->GetTexture(
						aiTexType,
						nTexIdx,
						&sAiPath,
						&eAiTexMapping,
						&nUVIdx,
						&fBlendFactor,
						&eAiTexOp,
						&eAiTexMapMode))
					{
						tTexDesc.eMappingMode = eAiTexMapMode;
						tTexDesc.eMappingType = eAiTexMapping;
						tTexDesc.eTextureOperator = eAiTexOp;
						tTexDesc.fBlendFactor = fBlendFactor;
						tTexDesc.nUVIdx = nUVIdx;

						const std::filesystem::path FileNamePath = sAiPath.C_Str();
						std::filesystem::path	CurTexFilePath = _Path.parent_path() / FileNamePath.filename();

						if (std::filesystem::exists(CurTexFilePath)
							&& std::filesystem::is_regular_file(CurTexFilePath))
						{
							Texture* pTexture = ResourceManager::Instance.LoadTexture(CurTexFilePath.wstring());

							if (nullptr == pTexture)
							{
								delete pSubset;
								return E_FAIL;
							}
							pTexture->m_tDesc = tTexDesc;
							pSubset->m_tMaterial.Textures[aiTexType][nTexIdx] = pTexture;

						}
						else
						{
							CurTexFilePath.replace_extension("tga");

							if (std::filesystem::exists(CurTexFilePath)
								&& std::filesystem::is_regular_file(CurTexFilePath))
							{
								Texture* pTexture = ResourceManager::Instance.LoadTexture(CurTexFilePath.wstring());

								if (nullptr == pTexture)
								{
									delete pSubset;
									return E_FAIL;
								}
								pTexture->m_tDesc = tTexDesc;
								pSubset->m_tMaterial.Textures[aiTexType][nTexIdx] = pTexture;
							}
						}
					}
				}
			}
		}
#pragma endregion
		m_vecSubset[nMeshIdx] = pSubset;
	}

	CreateTriangleMesh();

	return S_OK;
}

HRESULT StaticMesh::CreateTriangleMesh()
{

	m_pTriangleMeshDesc = new PxTriangleMeshDesc;

	m_pTriangleMeshDesc->points.count = m_vecVertices.size();
	m_pTriangleMeshDesc->points.stride = sizeof(PxVec3);
	m_pTriangleMeshDesc->points.data = m_vecVertices.data();

	m_pTriangleMeshDesc->triangles.count = m_vecIndices.size() / 3.f;
	m_pTriangleMeshDesc->triangles.stride = 3 * sizeof(UINT);
	m_pTriangleMeshDesc->triangles.data = m_vecIndices.data();
	
	m_pTriangleMeshDesc->flags = PxMeshFlags(0);

	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum eResult;

	bool status = Physics::Instance.m_pCooking->cookTriangleMesh(*m_pTriangleMeshDesc, writeBuffer, &eResult);
	
	if (!status)
	{
		cout << "Failed to Create TriangleMesh" << endl;
	}
	cout << eResult << endl;

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());

	m_pTriangleMesh = Physics::Instance.m_pPhysics->createTriangleMesh(readBuffer);
	
	cout << "Succeed to Create TriangleMesh" << endl;

	
	return S_OK;
}

void StaticMesh::Render()
{
	for (auto& pSubset : m_vecSubset)
	{
		pSubset->Render();
	}
}
