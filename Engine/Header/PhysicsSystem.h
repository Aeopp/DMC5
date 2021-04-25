#ifndef __PHYSICS_SYSTEM_H__
#define __PHYSICS_SYSTEM_H__
#include "Object.h"
#include "PxPhysicsAPI.h"
BEGIN(ENGINE)
USING(physx)
class CollisionCallback;
class GameObject;
class PhysicsSystem final : public Object
{
	DECLARE_SINGLETON(PhysicsSystem)
public:	
	typedef struct tagCollisionPair
	{
		bool						bTrigger;
		physx::PxPairFlag::Enum		ePairFlag;
		std::weak_ptr<GameObject>	pFirst;
		std::weak_ptr<GameObject>	pSecond;
	}COLLISIONPAIR, * LPCOLLISIONPAIR;

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

	std::list<COLLISIONPAIR> m_listCallback;

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
	
	void AddCallbackPair(const COLLISIONPAIR _tCollsionPair);
	PxPhysics*	GetPxPhysics();
	PxMaterial* GetDefaultMaterial();
	PxCooking*	GetCooking();
};
END
#endif // !__PHYSICS_SYSTEM_H__