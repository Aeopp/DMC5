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
	};
protected:
	std::vector<std::shared_ptr<Subset>>  m_vecSubset;
	std::shared_ptr<std::vector<Vector3>> m_spVertexLocations;
protected:
	// 로딩이 끝난 이후 호출해주세요 . 서브셋 정점 정보들로부터 전체 메쉬의 정점 위치를 구합니다.
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
	// Resource을(를) 통해 상속됨
	virtual void Free()			PURE;
public:
	// Resource을(를) 통해 상속됨
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
};

END
#endif // !__MESH_H__