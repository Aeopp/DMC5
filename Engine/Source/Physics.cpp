#include "Physics.h"
#include "PhysicsSystem.h"

USING(ENGINE)

std::weak_ptr<PhysicsSystem> Physics::m_pPhysicsSystem = PhysicsSystem::GetInstance();

physx::PxPhysics* Physics::GetPxPhysics()
{
	if (nullptr == m_pPhysicsSystem.lock() || m_pPhysicsSystem.expired())
		return nullptr;
	return m_pPhysicsSystem.lock()->GetPxPhysics();
}

physx::PxMaterial* Physics::GetDefaultMaterial()
{
	if (nullptr == m_pPhysicsSystem.lock() || m_pPhysicsSystem.expired())
		return nullptr;
	return m_pPhysicsSystem.lock()->GetDefaultMaterial();
}

void Physics::AddActor(const UINT _nSceneID, physx::PxActor& _rActor)
{
	if (nullptr == m_pPhysicsSystem.lock() || m_pPhysicsSystem.expired())
		return;
	m_pPhysicsSystem.lock()->AddActor(_nSceneID, _rActor);
}

void Physics::RemoveActor(const UINT _nSceneID, physx::PxActor& _rActor)
{
	if (nullptr == m_pPhysicsSystem.lock() || m_pPhysicsSystem.expired())
		return;
	m_pPhysicsSystem.lock()->RemoveActor(_nSceneID, _rActor);
}

bool Physics::RayCast(const D3DXVECTOR3& _vOrigin, const D3DXVECTOR3& _vDir, OUT D3DXVECTOR3& _vPoint, float _fDistance)
{
	if (nullptr == m_pPhysicsSystem.lock() || m_pPhysicsSystem.expired())
		return false;
	return m_pPhysicsSystem.lock()->RayCast(_vOrigin, _vDir, _vPoint, _fDistance);
}
