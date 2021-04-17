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
	PxFoundation* m_pFoundation;
	PxDefaultAllocator		m_Allocator;
	PxDefaultErrorCallback	m_ErrorCallback;
	PxPhysics* m_pPhysics;
	PxDefaultCpuDispatcher* m_pDispatcher;
	PxMaterial* m_pDefaultMaterial;

	//Cooking
	PxCooking* m_pCooking;

	//PhyX Visual Debugger
	PxPvd* m_pPVD;
	//
	PxScene* m_pScene;
	//
	CollisionCallback* m_pCollisionCallback;

	bool					m_bSimulate;

	std::vector<PxActor*>	m_vecAdd;
	std::vector<PxActor*>	m_vecRemove;
	std::vector<PxActor*>	m_vecRelease;

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
	PxPhysics* GetPxPhysics();
	PxMaterial* GetDefaultMaterial();
	PxCooking* GetCooking();
	void AddActor(physx::PxActor& _rActor);
	void RemoveActor(physx::PxActor& _rActor);
	void ReleaseActor(physx::PxActor& _rActor);
};
END
#endif // !__PHYSICS_SYSTEM_H__