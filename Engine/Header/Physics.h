#ifndef __PHYSICS_H__
#define __PHYSICS_H__
#include "EngineStdafx.h"
#include "PxPhysicsAPI.h"
BEGIN(ENGINE)
class PhysicsSystem;
class ENGINE_DLL Physics abstract
{
private:
	static std::weak_ptr<PhysicsSystem> m_pPhysicsSystem;
public:
	static physx::PxPhysics* GetPxPhysics();
	static physx::PxMaterial* GetDefaultMaterial();

	static void AddActor(const UINT _nSceneID, physx::PxActor& _rActor);
	static void RemoveActor(const UINT _nSceneID, physx::PxActor& _rActor);
};
END
#endif // !__PHYSICS_H__