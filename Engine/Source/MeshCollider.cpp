#include "MeshCollider.h"
#include "PhysicsSystem.h"

USING(ENGINE)

MeshCollider::MeshCollider(std::weak_ptr<GameObject> const _pGameObject)
	: Collider(_pGameObject)
	, m_pTriangleMesh(nullptr)
{
}

void MeshCollider::Free()
{
	PX_RELEASE(m_pTriangleMesh)
		Collider::Free();
}

MeshCollider* MeshCollider::Create(std::weak_ptr<GameObject> const _pGameObject)
{
	MeshCollider* pInstance = new MeshCollider(_pGameObject);

	if (FAILED(pInstance->ReadyCollider()))
	{
		delete pInstance;
		return nullptr;
	}

	return pInstance;
}

HRESULT MeshCollider::ReadyCollider()
{
	return S_OK;
}

HRESULT MeshCollider::DrawCollider(const DrawInfo& _Info)
{
	if (FAILED(Collider::DrawCollider(_Info)))
		return S_OK;
	return S_OK;
}

void MeshCollider::ReadyMeshCollider(D3DXVECTOR3* _pPoints, UINT _nNumPoint, UINT* _pIndices, UINT _nNumFace)
{
	CookTriangleMesh(_pPoints, _nNumPoint, _pIndices, _nNumFace);

	PxTriangleMeshGeometry geometry;

	geometry.triangleMesh = m_pTriangleMesh;
	geometry.scale = PxVec3(0.001f, 0.001f, 0.001f);

	m_pShape = PhysicsSystem::GetInstance()->GetPxPhysics()->createShape(geometry, *PhysicsSystem::GetInstance()->GetDefaultMaterial(), true);

	Collider::ReadyCollider();
}

void MeshCollider::CookTriangleMesh(D3DXVECTOR3* _pPoints, UINT _nNumPoint, UINT* _pIndices, UINT _nNumFace)
{
	PxTriangleMeshDesc triangleMeshDesc;

	triangleMeshDesc.points.count = _nNumPoint;
	triangleMeshDesc.points.stride = sizeof(PxVec3);
	triangleMeshDesc.points.data = _pPoints;

	triangleMeshDesc.triangles.count = _nNumFace / 3;
	triangleMeshDesc.triangles.stride = 3 * sizeof(PxU32);
	triangleMeshDesc.triangles.data = _pIndices;

	triangleMeshDesc.flags = PxMeshFlags(0);

	PxDefaultMemoryOutputStream writeBuffer;

	PxTriangleMeshCookingResult::Enum eResult;

	bool bTemp = PhysicsSystem::GetInstance()->GetCooking()->validateTriangleMesh(triangleMeshDesc);
	bool status = PhysicsSystem::GetInstance()->GetCooking()->cookTriangleMesh(triangleMeshDesc, writeBuffer, &eResult);


	if (false == status)
	{
		return;
	}

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());

	m_pTriangleMesh = PhysicsSystem::GetInstance()->GetPxPhysics()->createTriangleMesh(readBuffer);
}

