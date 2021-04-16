#ifndef __PHYSICS_SYSTEM_H__
#define __PHYSICS_SYSTEM_H__
#include "Object.h"
#include "PxPhysicsAPI.h"
BEGIN(ENGINE)
USING(physx)
class CollisionCallback;
class PhysicsSystem final : public Object
{
	DECLARE_SINGLETON(PhysicsSystem)
private:
	PxFoundation*			m_pFoundation;
	PxDefaultAllocator		m_Allocator;
	PxDefaultErrorCallback	m_ErrorCallback;
	PxPhysics*				m_pPhysics;
	PxDefaultCpuDispatcher* m_pDispatcher;
	PxMaterial*				m_pDefaultMaterial;

	//PhyX Visual Debugger
	PxPvd*					m_pPVD;
	//
	PxScene*				m_pScene;
	//
	CollisionCallback*		m_pCollisionCallback;

	bool					m_bSimulate;
private:
	explicit PhysicsSystem();
	virtual ~PhysicsSystem() = default;
	// Object을(를) 통해 상속됨
	virtual void Free() override;
public:
	HRESULT ReadyPhysicsSystem();
	void	Simulate(const float _fDeltaTime);
	void	FetchResults(const bool _bBlock = false);

public:
	physx::PxPhysics*	GetPxPhysics();
	physx::PxMaterial*	GetDefaultMaterial();
	void AddActor(physx::PxActor& _rActor);
	void RemoveActor(physx::PxActor& _rActor);

public:
	bool IsSimulate();
};
END
#endif // !__PHYSICS_SYSTEM_H__