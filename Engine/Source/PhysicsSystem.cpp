#include "PhysicsSystem.h"
#include "CollisionCallback.h"
#include "GameObject.h"
#include "Collider.h"
#include "Transform.h"

USING(ENGINE)
IMPLEMENT_SINGLETON(PhysicsSystem)

#ifdef _DEBUG
#define PVD_HOST "127.0.0.1"
#endif // _DEBUG

physx::PxFilterFlags contactReportFilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
{
	PX_UNUSED(attributes0);
	PX_UNUSED(attributes1);
	PX_UNUSED(filterData0);
	PX_UNUSED(filterData1);
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(constantBlock);

	if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = physx::PxPairFlag::eNOTIFY_TOUCH_FOUND | physx::PxPairFlag::eNOTIFY_TOUCH_LOST | physx::PxPairFlag::eSOLVE_CONTACT | physx::PxPairFlag::eDETECT_DISCRETE_CONTACT;
		return physx::PxFilterFlag::eDEFAULT;
	}

	// all initial and persisting reports for everything, with per-point data
	pairFlags = physx::PxPairFlag::eSOLVE_CONTACT | physx::PxPairFlag::eDETECT_DISCRETE_CONTACT
		| physx::PxPairFlag::eNOTIFY_TOUCH_FOUND
		| physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS
		| physx::PxPairFlag::eNOTIFY_TOUCH_LOST
		| physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;
	return physx::PxFilterFlag::eDEFAULT;
}

PhysicsSystem::PhysicsSystem()
	: m_pFoundation(nullptr)
	, m_pPhysics(nullptr)
	, m_pDispatcher(nullptr)
	, m_pDefaultMaterial(nullptr)
	, m_pPVD(nullptr)
	, m_pScene(nullptr)
	, m_pCollisionCallback(nullptr)
	, m_bSimulate(false)
{
}

void PhysicsSystem::Free()
{
	Object::Free();
	PX_RELEASE(m_pScene);

	SafeDelete(m_pCollisionCallback);

	PX_RELEASE(m_pDispatcher);
	PX_RELEASE(m_pPhysics);

	if (m_pPVD)
	{
		physx::PxPvdTransport* pPVDTransport = m_pPVD->getTransport();
		PX_RELEASE(m_pPVD);
		PX_RELEASE(pPVDTransport);
	}

	PX_RELEASE(m_pFoundation);
}

HRESULT PhysicsSystem::ReadyPhysicsSystem()
{
	//Foundation
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);

	if (nullptr == m_pFoundation)
	{
		PRINT_LOG(TEXT("Error"), TEXT("Failed to PxCreateFoundation"));
		return E_FAIL;
	}

	//PhysX Visual Debugger
#ifdef _DEBUG
	m_pPVD = physx::PxCreatePvd(*m_pFoundation);
	if (nullptr == m_pPVD)
	{
		PRINT_LOG(TEXT("Error"), TEXT("Failed to PxCreatePvd"));
		return E_FAIL;
	}
	physx::PxPvdTransport* pPVDTransport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	m_pPVD->connect(*pPVDTransport, physx::PxPvdInstrumentationFlag::eALL);
#endif

	//Physics
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, physx::PxTolerancesScale(), true, m_pPVD);
	if (nullptr == m_pPhysics)
	{
		PRINT_LOG(TEXT("Error"), TEXT("Failed to PxCreatePhysics"));
		return E_FAIL;
	}

	//Scene Description
	physx::PxSceneDesc sceneDesc(m_pPhysics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.f, 0.f, 0.f);
	m_pDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = m_pDispatcher;
	sceneDesc.filterShader = contactReportFilterShader;

	//Scene
	m_pScene = m_pPhysics->createScene(sceneDesc);
	if (nullptr == m_pScene)
	{
		PRINT_LOG(TEXT("Error"), TEXT("Failed to createScene (PhysicsSystem)"));
		return E_FAIL;
	}
	m_pCollisionCallback = new CollisionCallback;
	m_pScene->setSimulationEventCallback(m_pCollisionCallback);

	//Default Material
	m_pDefaultMaterial = m_pPhysics->createMaterial(0.5f, 0.5f, 0.5f);

	return S_OK;
}

void PhysicsSystem::Simulate(const float _fDeltaTime)
{
	//
	if (true == m_bSimulate || nullptr == m_pScene)
		return;

	//Simulation 시작 전 변경된 Transform을 Actor에 적용
	PxU32		nNumActors = 0;
	PxActor**	ppActors = nullptr;

	//Scene에 있는 RigidDynamicActor의 숫자
	nNumActors = m_pScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC);
	//Scene에 있는 RigidDynamicActor 포인터를 받아오기 위한 동적 배열 생성
	ppActors = new PxActor * [nNumActors];
	//Scene에서 RigidDynamicActor들을 가져옴.
	m_pScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC, ppActors, nNumActors);
	//
	for(UINT i = 0; i < nNumActors; ++i)
		((LPPXUSERDATA)(ppActors[i]->userData))->pCollider.lock()->ReadySimulate();
	//생성한 동적 배열 삭제
	delete[] ppActors;

	//Scene에 있는 RigidStaticActor의 숫자
	nNumActors = m_pScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC);
	//Scene에 있는 RigidStaticActor 포인터를 받아오기 위한 동적 배열 생성
	ppActors = new PxActor * [nNumActors];
	//Scene에서 RigidStaticActor들을 가져옴.
	m_pScene->getActors(PxActorTypeFlag::eRIGID_STATIC, ppActors, nNumActors);
	//
	for (UINT i = 0; i < nNumActors; ++i)
		((LPPXUSERDATA)(ppActors[i]->userData))->pCollider.lock()->ReadySimulate();
	//생성한 동적 배열 삭제
	delete[] ppActors;

	//Simulation 시작
	m_pScene->simulate(_fDeltaTime);

	m_bSimulate = true;

}

void PhysicsSystem::FetchResults(const bool _bBlock)
{
	if (false == m_bSimulate || nullptr == m_pScene)
		return;

	while (false == (m_pScene->fetchResults(_bBlock)));

	//Simulation 결과로 Actor에 설정된 globalPose를 Transform에 적용.
	PxU32		nNumActors = 0;
	PxActor**	ppActors = nullptr;

	nNumActors = m_pScene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC);

	ppActors = new PxActor * [nNumActors];

	m_pScene->getActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC, ppActors, nNumActors);

	std::weak_ptr<Collider> pCollider;
	for (UINT i = 0; i < nNumActors; ++i)
	{
		pCollider = ((LPPXUSERDATA)(ppActors[i]->userData))->pCollider;

		if (pCollider.expired())
			continue;

		physx::PxTransform globalPose = ppActors[i]->is<physx::PxRigidActor>()->getGlobalPose();
		pCollider.lock()->GetGameObject().lock()->GetComponent<Transform>().lock()->SetSimulationResult(D3DXQUATERNION(globalPose.q.x, globalPose.q.y, globalPose.q.z, globalPose.q.w), D3DXVECTOR3(globalPose.p.x, globalPose.p.y, globalPose.p.z));
	}

	m_bSimulate = false;
}

physx::PxPhysics* PhysicsSystem::GetPxPhysics()
{
	return m_pPhysics;
}

physx::PxMaterial* PhysicsSystem::GetDefaultMaterial()
{
	return m_pDefaultMaterial;
}

void PhysicsSystem::AddActor(physx::PxActor& _rActor)
{
	if (nullptr == m_pScene)
		return;
	m_pScene->addActor(_rActor);
}

void PhysicsSystem::RemoveActor(physx::PxActor& _rActor)
{
	if (nullptr == m_pScene)
		return;
	m_pScene->removeActor(_rActor);
}

bool PhysicsSystem::IsSimulate()
{
	return m_bSimulate;
}