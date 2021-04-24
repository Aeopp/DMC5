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
	, m_pCooking(nullptr)
{
}

void PhysicsSystem::Free()
{
	Object::Free();

	if (m_bSimulate)
		while (false == (m_pScene->fetchResults(false)));
	delete m_pScene->getSimulationEventCallback();
	PX_RELEASE(m_pScene);

	SafeDelete(m_pCollisionCallback);

	PX_RELEASE(m_pDispatcher);
	PX_RELEASE(m_pCooking);
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

	//Cooking
	m_pCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_pFoundation, m_pPhysics->getTolerancesScale());

	//Default Material
	m_pDefaultMaterial = m_pPhysics->createMaterial(1.f, 1.f, 0.f);
	
	m_pDispatcher = PxDefaultCpuDispatcherCreate(2);

	return S_OK;
}

void PhysicsSystem::Simulate(const float _fDeltaTime)
{
	//
	if (true == m_bSimulate || nullptr == m_pScene)
		return;

	//Simulation 시작 전 변경된 Transform을 Actor에 적용
	PxU32		nNumActors = 0;
	PxActor** ppActors = nullptr;

	//Scene에 있는 RigidDynamicActor의 숫자
	nNumActors = m_pScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC);
	//Scene에 있는 RigidDynamicActor 포인터를 받아오기 위한 동적 배열 생성
	ppActors = new PxActor * [nNumActors];
	//Scene에서 RigidDynamicActor들을 가져옴.
	m_pScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC, ppActors, nNumActors);
	//
	for (UINT i = 0; i < nNumActors; ++i)
	{
		if (ppActors[i]->userData)
			((LPPXUSERDATA)(ppActors[i]->userData))->pCollider.lock()->ReadySimulate();
	}
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
	{
		if (ppActors[i]->userData)
			((LPPXUSERDATA)(ppActors[i]->userData))->pCollider.lock()->ReadySimulate();
	}
	//생성한 동적 배열 삭제
	delete[] ppActors;

	//Simulation 시작
	m_pScene->simulate(1.f/60.f);

	m_bSimulate = true;

}

void PhysicsSystem::FetchResults(const bool _bBlock)
{
	if (false == m_bSimulate || nullptr == m_pScene)
		return;

	while (false == (m_pScene->fetchResults(true)));

	//Simulation 결과로 Actor에 설정된 globalPose를 Transform에 적용.
	PxU32		nNumActors = 0;
	PxActor** ppActors = nullptr;

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

	delete[] ppActors;

	m_bSimulate = false;

}

HRESULT PhysicsSystem::CreateScene(const UINT _nSceneID)
{
	PxScene* pScene = nullptr;

	//Scene Description
	physx::PxSceneDesc sceneDesc(m_pPhysics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.f, -4.905f, 0.f);
	sceneDesc.cpuDispatcher = m_pDispatcher;
	sceneDesc.filterShader = contactReportFilterShader;

	//Create Scene
	pScene = m_pPhysics->createScene(sceneDesc);

	if (nullptr == pScene)
	{
		PRINT_LOG(TEXT("Error"), TEXT("Failed to createScene (PhysicsSystem)"));
		return E_FAIL;
	}

	//Simulation Event Callback 등록.
	pScene->setSimulationEventCallback(new CollisionCallback);

	//map container에 저장
	m_mapScene.emplace(_nSceneID, pScene);
	return S_OK;
}

HRESULT PhysicsSystem::ChangeScene(const UINT _nSceneID)
{
	auto iterFind = m_mapScene.find(_nSceneID);

	//씬 아이디에 해당하는 피직스 씬이 없는 경우 피직스 씬 전환 실패.
	if (m_mapScene.end() == iterFind)
		return E_FAIL;

	//이전 피직스 씬이 없는경우 
	//맵 컨테이너에서 찾은 씬을 현재 씬으로 설정.
	if (nullptr == m_pScene)
	{
		m_pScene = iterFind->second;
		return S_OK;
	}

	//이전 피직스 씬이 있는경우 
	
	// 1. 이전 피직스 씬에 남아있는 Actor를 현재 씬으로 옮김
	PxU32		nNumActors = 0;
	PxActor**	ppActors = nullptr;

	//Scene에 있는 RigidDynamicActor의 개수
	nNumActors = m_pScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC);
	//Scene에 있는 RigidDynamicActor 포인터를 받아오기 위한 동적 배열 생성
	ppActors = new PxActor * [nNumActors];
	//Scene에서 RigidDynamicActor들을 가져옴.
	m_pScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC, ppActors, nNumActors);
	//변경할 씬에 남아있는 Actor를 넘겨줌.
	iterFind->second->addActors(ppActors, nNumActors);
	//생성한 동적 배열 삭제
	delete[] ppActors;

	//Scene에 있는 RigidStaticActor의 숫자
	nNumActors = m_pScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC);
	//Scene에 있는 RigidStaticActor 포인터를 받아오기 위한 동적 배열 생성
	ppActors = new PxActor * [nNumActors];
	//Scene에서 RigidStaticActor들을 가져옴.
	m_pScene->getActors(PxActorTypeFlag::eRIGID_STATIC, ppActors, nNumActors);
	//변경할 씬에 남아있는 Actor를 넘겨줌.
	iterFind->second->addActors(ppActors, nNumActors);
	//생성한 동적 배열 삭제
	delete[] ppActors;

	// 2. 이전 피직스 씬을 맵 컨테이너에서 제거
	for (auto iterCurr = m_mapScene.begin(); iterCurr != m_mapScene.end(); ++iterCurr)
	{
		if (iterCurr->second == m_pScene)
		{
			m_mapScene.erase(iterCurr);
			break;
		}
	}

	delete m_pScene->getSimulationEventCallback();

	// 3. 이전 피직스 씬을 해제 한 후 맵 컨테이너에서 찾은 씬을 현재 씬으로 설정.
	m_pScene->release();

	m_pScene = iterFind->second;

	return S_OK;
}

physx::PxPhysics* PhysicsSystem::GetPxPhysics()
{
	return m_pPhysics;
}

physx::PxMaterial* PhysicsSystem::GetDefaultMaterial()
{
	return m_pDefaultMaterial;
}

PxCooking* PhysicsSystem::GetCooking()
{
	return m_pCooking;
}

void PhysicsSystem::SetGravity(PxVec3 _vGravity)
{
	if (nullptr == m_pScene)
		return;

	m_pScene->setGravity(_vGravity);
}

D3DXVECTOR3 PhysicsSystem::GetGravity()
{
	if (nullptr == m_pScene)
		return D3DXVECTOR3(0.f, 0.f, 0.f);
	PxVec3 vGravity = m_pScene->getGravity();
	return D3DXVECTOR3(vGravity.x, vGravity.y, vGravity.z);
}

void PhysicsSystem::AddActor(const UINT _nSceneID, physx::PxActor& _rActor)
{
	auto iterFind = m_mapScene.find(_nSceneID);

	if (m_mapScene.end() == iterFind)
	{
		PRINT_LOG(TEXT("Failed to AddActor"), TEXT("Failed to Find PxScene."));
		return;
	}

	iterFind->second->addActor(_rActor);
}

void PhysicsSystem::RemoveActor(const UINT _nSceneID, physx::PxActor& _rActor)
{
	auto iterFind = m_mapScene.find(_nSceneID);

	if (m_mapScene.end() == iterFind)
	{
		PRINT_LOG(TEXT("Failed to RemoveActor"), TEXT("Failed to Find PxScene."));
		return;
	}

	iterFind->second->removeActor(_rActor);
}