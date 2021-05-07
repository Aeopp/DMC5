#include "AssimpHelper.hpp"
#include "StaticMesh.h"
#include "Subset.h"

USING(ENGINE)

StaticMesh::StaticMesh(LPDIRECT3DDEVICE9 const _pDevice)
	: Mesh(_pDevice){}

StaticMesh::StaticMesh(const StaticMesh& _rOther)
	: Mesh(_rOther){}

void StaticMesh::Free()
{
	Mesh::Free();
}

StaticMesh* StaticMesh::Create(LPDIRECT3DDEVICE9 const _pDevice, 
				const std::filesystem::path _Path , const std::any& InitParams)
{
	StaticMesh* pInstance = new StaticMesh(_pDevice);
	if (FAILED(pInstance->LoadMeshFromFile(_Path, InitParams)))
	{
		pInstance->Free();
		delete pInstance;
		return nullptr;
	}
	return pInstance;
}

Resource* StaticMesh::Clone()
{
	StaticMesh* pClone = new StaticMesh(*this);
	return pClone;
}
void StaticMesh::Editor()
{
	Mesh::Editor();
	if (bEdit)
	{

	}
}
std::string StaticMesh::GetName()
{
	return "StaticMesh";
};

HRESULT StaticMesh::LoadMeshImplementation(const aiScene* AiScene,
	const std::filesystem::path _Path,
	const std::any& InitParams)
{
	Mesh::InitializeInfo _InitInfo{};
	if (InitParams.has_value())
	{
		const auto& CRefInitParam = std::any_cast<const Mesh::InitializeInfo&>(InitParams);
		_InitInfo = CRefInitParam;
	}

	//Subset�� �����ϴ� vector �޸� ���� Ȯ��.
	m_vecSubset.resize(AiScene->mNumMeshes);

	//FBX�� Scene�� �����ϴ� Mesh(Subset)�� ���� ������ ����.
	for (uint32 MeshIdx = 0u; MeshIdx < AiScene->mNumMeshes; ++MeshIdx)
	{
		//
		const auto* const AiMesh = AiScene->mMeshes[MeshIdx];
		//
		std::shared_ptr<Subset> _CurrentSubset;
		_CurrentSubset.reset(Subset::Create(m_pDevice), Deleter<Object>());

		VERTEXBUFFERDESC tVBDesc;
		LPDIRECT3DVERTEXBUFFER9 pVB = nullptr;
		LPDIRECT3DINDEXBUFFER9	pIB = nullptr;

		if (FAILED(AssimpHelper::LoadMesh(AiMesh, m_pDevice,
			&tVBDesc, &pVB, &pIB,nullptr,_InitInfo.bLocalVertexLocationsStorage)))
			return E_FAIL;

		MATERIAL tMaterial;

		if (FAILED(AssimpHelper::LoadMaterial(
			AiScene,
			AiMesh,
			{
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
			},
			_Path.parent_path(),
			m_pDevice,
			&tMaterial
		)))
		{
			SafeRelease(pVB);
			SafeRelease(pIB);
			return E_FAIL;
		}

		SafeAddRef(tVBDesc.pVertexDecl);

		_CurrentSubset->Initialize(pVB, pIB, tVBDesc, tMaterial);
		m_vecSubset[MeshIdx] = _CurrentSubset;

		//Triangle Mesh Cooking�� �ʿ��� �ε��� ���� ����� -- 2021/04/12 ������
		m_vecIndices.reset(new std::vector<UINT>());
		UINT* pIndices = nullptr;
		pIB->Lock(0, 0, (void**)&pIndices, 0);
		m_vecIndices->resize(tVBDesc.nNumFaces * 3);
		for (UINT i = 0; i < (tVBDesc.nNumFaces * 3); ++i)
		{
			(*m_vecIndices)[i] = pIndices[i];
		}
	};

	if (_InitInfo.bLocalVertexLocationsStorage)
	{
		MakeVertexLocationsFromSubset();
	}
	

	return S_OK;
}



