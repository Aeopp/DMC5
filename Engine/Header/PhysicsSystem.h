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
	std::unordered_map<UINT, PxScene*> m_mapScene;
	//
	CollisionCallback* m_pCollisionCallback;

	bool					m_bSimulate;

	//std::vector<PxActor*>	m_vecAdd;
	//std::vector<PxActor*>	m_vecRemove;
	//std::vector<PxActor*>	m_vecRelease;

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
	HRESULT CreateScene(const UINT _nSceneID);
	HRESULT ChangeScene(const UINT _nSceneID);
public:
	void AddActor(const UINT _nSceneID, physx::PxActor& _rActor);
	void RemoveActor(const UINT _nSceneID, physx::PxActor& _rActor);
	
	PxPhysics*	GetPxPhysics();
	PxMaterial* GetDefaultMaterial();
	PxCooking*	GetCooking();

	//중력값 설정 임시 함수
	void SetGravity(PxVec3 _vGravity);
	D3DXVECTOR3 GetGravity();
};
END
#endif // !__PHYSICS_SYSTEM_H__