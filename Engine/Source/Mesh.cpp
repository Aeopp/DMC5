#include "AssimpHelper.hpp"
#include "Mesh.h"
#include "Subset.h"

USING(ENGINE)

Mesh::Mesh(LPDIRECT3DDEVICE9 const _pDevice)
	: Resource(_pDevice)
{

}

Mesh::Mesh(const Mesh& _rOther)
	: Resource(_rOther)
	, m_vecSubset(_rOther.m_vecSubset)
	, m_spVertexLocations(_rOther.m_spVertexLocations)
	, m_vecIndices(_rOther.m_vecIndices)
{

}

void Mesh::MakeVertexLocationsFromSubset()&
{
	m_spVertexLocations = std::make_shared<std::vector<Vector3>>();

	for (const auto& CurSubset : m_vecSubset)
	{
		if (CurSubset)
		{
			const auto& CurLocalVertexLocations = CurSubset->GetVertexBufferDesc().LocalVertexLocation;
			if (CurLocalVertexLocations)
			{
				for (const Vector3 LocalVertexLocation : *CurLocalVertexLocations)
				{
					m_spVertexLocations->push_back(LocalVertexLocation);
				}
			}
		}
	}
}
HRESULT Mesh::LoadMeshFromFile(const std::filesystem::path _Path, 
							  const std::any& InitParams)&
{
	//Assimp Importer 생성.
	auto AiImporter = Assimp::Importer{};
	ResourcePath = _Path;
	//FBX파일을 읽어서 Scene 생성.

	unsigned int LoadFlag =
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
		aiProcess_SortByPType;

	if (InitParams.has_value())
	{
		const auto& LoadInfo = std::any_cast<const Mesh::InitializeInfo&>(InitParams);

		if (LoadInfo.bJoinIdenticalVertices)
		{
			LoadFlag |= aiProcess_JoinIdenticalVertices;
		}

		if (LoadInfo.bOptimizeMeshes)
		{
			LoadFlag |= aiProcess_OptimizeMeshes;
		}

		if (LoadInfo.bSplitLargeMeshes)
		{
			LoadFlag |= aiProcess_SplitLargeMeshes;
		}
	}

	const aiScene* const AiScene = AiImporter.ReadFile(
		ResourcePath.string(),
		LoadFlag);

	return LoadMeshImplementation(AiScene, ResourcePath, InitParams);
};

void Mesh::Free()
{
	m_vecSubset.clear();
	m_vecSubset.shrink_to_fit();
	Resource::Free();
}

HRESULT Mesh::Render(ID3DXEffect* const Fx)
{
	for (auto& pSubset : m_vecSubset)
	{
		if (nullptr == pSubset)
			continue;

		pSubset->Render(Fx);
	}

	return S_OK;
}

const uint64 Mesh::GetNumSubset() const
{
	return m_vecSubset.size();
}

std::weak_ptr<Subset> Mesh::GetSubset(const UINT _nIndex)
{
	if (_nIndex >= m_vecSubset.size())
		return std::weak_ptr<Subset>();

	return m_vecSubset[_nIndex];
}

void Mesh::Editor()
{
	Resource::Editor();
	if (bEdit)
	{
		uint32 FaceCount = 0u;
		uint32 VertexCount = 0u;

		for (const auto& _Subset : m_vecSubset)
		{
			VertexCount += _Subset->m_tVertexBufferDesc.nNumVertices;
			FaceCount += _Subset->m_tVertexBufferDesc.nNumFaces;
		}

		ImGui::Text("Mesh VertexCount : %d", VertexCount);
		ImGui::Text("Mesh Polygon Count: %d", FaceCount);

		for (auto& _Subset : m_vecSubset)
		{
			_Subset->Editor();
		}
	}
}

const VERTEXBUFFERDESC& Mesh::GetVertexBufferDesc(const UINT _nSubsetIdx)
{
	// TODO: 여기에 return 문을 삽입합니다.
	if (_nSubsetIdx >= m_vecSubset.size())
		return VERTEXBUFFERDESC();

	return m_vecSubset[_nSubsetIdx]->GetVertexBufferDesc();
}

const MATERIAL& Mesh::GetMaterial(const UINT _nSubsetIdx)
{
	// TODO: 여기에 return 문을 삽입합니다.
	if (_nSubsetIdx >= m_vecSubset.size())
		return MATERIAL();

	return m_vecSubset[_nSubsetIdx]->GetMaterial();
}

D3DXVECTOR3* Mesh::GetVerticesPointer()
{
	return m_spVertexLocations->data();
}

UINT Mesh::GetNumVertices()
{
	return m_spVertexLocations->size();
}

UINT* Mesh::GetIndicesPointer()
{
	return m_vecIndices->data();
}

UINT Mesh::GetNumIndices()
{
	return m_vecIndices->size();
}
