#ifndef __STATIC_MESH_H__
#define __STATIC_MESH_H__
#include "PxPhysicsAPI.h"
using namespace physx;
class Subset;
class StaticMesh
{
public:
	std::vector<Subset*> m_vecSubset;
	std::vector<PxVec3>	m_vecVertices;
	std::vector<UINT>			m_vecIndices;

	PxTriangleMesh* m_pTriangleMesh;
	PxTriangleMeshDesc* m_pTriangleMeshDesc;

public:
	StaticMesh();
	~StaticMesh();

public:
	HRESULT Load(std::filesystem::path _Path);
	HRESULT CreateTriangleMesh();
public:
	void Render();
};
#endif // !__STATIC_MESH_H__