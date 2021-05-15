#ifndef __MESH_H__
#define __MESH_H__
#include "Resource.h"
#include <any>

class aiScene;

BEGIN(ENGINE)
class Subset;
class ENGINE_DLL Mesh abstract : public Resource
{
public:
	struct InitializeInfo
	{
		bool bLocalVertexLocationsStorage = false;

		bool bJoinIdenticalVertices = true;
		bool bOptimizeMeshes = true;
		bool bSplitLargeMeshes = true;
	};
public:
	std::vector<std::shared_ptr<Subset>>  m_vecSubset;
	std::shared_ptr<std::vector<Vector3>> m_spVertexLocations;

	//Triangle Mesh Cooking�� �ʿ��� �ε��� ���� ����� -- 2021/04/12 ������
	std::shared_ptr<std::vector<UINT>> m_vecIndices;
protected:
	// �ε��� ���� ���� ȣ�����ּ��� . ����� ���� ������κ��� ��ü �޽��� ���� ��ġ�� ���մϴ�.
	void MakeVertexLocationsFromSubset()&;
	HRESULT LoadMeshFromFile(const std::filesystem::path _Path, const std::any& InitParams)&;
	virtual HRESULT LoadMeshImplementation(
		const aiScene* AiScene,
		const std::filesystem::path _Path,
		const std::any& InitParams  )PURE;
protected:
	explicit Mesh(LPDIRECT3DDEVICE9 const _pDevice);
	explicit Mesh(const Mesh& _rOther);
	virtual ~Mesh() = default;
	// Resource��(��) ���� ��ӵ�
	virtual void Free()			PURE;
public:
	// Resource��(��) ���� ��ӵ�
	virtual Resource* Clone()	PURE;
	virtual void Editor()override;
public:
	HRESULT Render(ID3DXEffect* const Fx);
public:
	auto GetVertexLocations() {return m_spVertexLocations;};
	const uint64			GetNumSubset() const; 
	std::weak_ptr<Subset>	GetSubset(const UINT _nIndex);
	const VERTEXBUFFERDESC& GetVertexBufferDesc(const UINT _nSubsetIdx);
	const MATERIAL&			GetMaterial(const UINT _nSubsetIdx);

	//Triangle Mesh Cooking�� �ʿ��� �ε��� ���� ����� -- 2021/04/12 ������
	D3DXVECTOR3*	GetVerticesPointer();
	UINT			GetNumVertices();
	//Triangle Mesh Cooking�� �ʿ��� �ε��� ���� ����� -- 2021/04/12 ������
	UINT*			GetIndicesPointer();
	UINT			GetNumIndices();
};

END
#endif // !__MESH_H__