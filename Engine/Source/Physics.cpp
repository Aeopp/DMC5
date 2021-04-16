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

void Physics::AddActor(physx::PxActor& _rActor)
{
	if (nullptr == m_pPhysicsSystem.lock() || m_pPhysicsSystem.expired())
		return;
	m_pPhysicsSystem.lock()->AddActor(_rActor);
}

void Physics::RemoveActor(physx::PxActor& _rActor)
{
	if (nullptr == m_pPhysicsSystem.lock() || m_pPhysicsSystem.expired())
		return;
	m_pPhysicsSystem.lock()->RemoveActor(_rActor);
}
