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

	//Simulation ���� �� ����� Transform�� Actor�� ����
	PxU32		nNumActors = 0;
	PxActor** ppActors = nullptr;

	//Scene�� �ִ� RigidDynamicActor�� ����
	nNumActors = m_pScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC);
	//Scene�� �ִ� RigidDynamicActor �����͸� �޾ƿ��� ���� ���� �迭 ����
	ppActors = new PxActor * [nNumActors];
	//Scene���� RigidDynamicActor���� ������.
	m_pScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC, ppActors, nNumActors);
	//
	for (UINT i = 0; i < nNumActors; ++i)
		((LPPXUSERDATA)(ppActors[i]->userData))->pCollider.lock()->ReadySimulate();
	//������ ���� �迭 ����
	delete[] ppActors;

	//Scene�� �ִ� RigidStaticActor�� ����
	nNumActors = m_pScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC);
	//Scene�� �ִ� RigidStaticActor �����͸� �޾ƿ��� ���� ���� �迭 ����
	ppActors = new PxActor * [nNumActors];
	//Scene���� RigidStaticActor���� ������.
	m_pScene->getActors(PxActorTypeFlag::eRIGID_STATIC, ppActors, nNumActors);
	//
	for (UINT i = 0; i < nNumActors; ++i)
		((LPPXUSERDATA)(ppActors[i]->userData))->pCollider.lock()->ReadySimulate();
	//������ ���� �迭 ����
	delete[] ppActors;

	//Simulation ����
	m_pScene->simulate(1.f/60.f);

	m_bSimulate = true;

}

void PhysicsSystem::FetchResults(const bool _bBlock)
{
	if (false == m_bSimulate || nullptr == m_pScene)
		return;

	while (false == (m_pScene->fetchResults(_bBlock)));

	//Simulation ����� Actor�� ������ globalPose�� Transform�� ����.
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
	sceneDesc.gravity = physx::PxVec3(0.f, -9.81f, 0.f);
	sceneDesc.cpuDispatcher = m_pDispatcher;
	sceneDesc.filterShader = contactReportFilterShader;

	//Create Scene
	pScene = m_pPhysics->createScene(sceneDesc);

	if (nullptr == pScene)
	{
		PRINT_LOG(TEXT("Error"), TEXT("Failed to createScene (PhysicsSystem)"));
		return E_FAIL;
	}

	//Simulation Event Callback ���.
	pScene->setSimulationEventCallback(new CollisionCallback);

	//map container�� ����
	m_mapScene.emplace(_nSceneID, pScene);
	return S_OK;
}

HRESULT PhysicsSystem::ChangeScene(const UINT _nSceneID)
{
	auto iterFind = m_mapScene.find(_nSceneID);

	//�� ���̵� �ش��ϴ� ������ ���� ���� ��� ������ �� ��ȯ ����.
	if (m_mapScene.end() == iterFind)
		return E_FAIL;

	//���� ������ ���� ���°�� 
	//�� �����̳ʿ��� ã�� ���� ���� ������ ����.
	if (nullptr == m_pScene)
	{
		m_pScene = iterFind->second;
		return S_OK;
	}

	//���� ������ ���� �ִ°�� 
	
	// 1. ���� ������ ���� �����ִ� Actor�� ���� ������ �ű�
	PxU32		nNumActors = 0;
	PxActor**	ppActors = nullptr;

	//Scene�� �ִ� RigidDynamicActor�� ����
	nNumActors = m_pScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC);
	//Scene�� �ִ� RigidDynamicActor �����͸� �޾ƿ��� ���� ���� �迭 ����
	ppActors = new PxActor * [nNumActors];
	//Scene���� RigidDynamicActor���� ������.
	m_pScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC, ppActors, nNumActors);
	//������ ���� �����ִ� Actor�� �Ѱ���.
	iterFind->second->addActors(ppActors, nNumActors);
	//������ ���� �迭 ����
	delete[] ppActors;

	//Scene�� �ִ� RigidStaticActor�� ����
	nNumActors = m_pScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC);
	//Scene�� �ִ� RigidStaticActor �����͸� �޾ƿ��� ���� ���� �迭 ����
	ppActors = new PxActor * [nNumActors];
	//Scene���� RigidStaticActor���� ������.
	m_pScene->getActors(PxActorTypeFlag::eRIGID_STATIC, ppActors, nNumActors);
	//������ ���� �����ִ� Actor�� �Ѱ���.
	iterFind->second->addActors(ppActors, nNumActors);
	//������ ���� �迭 ����
	delete[] ppActors;

	// 2. ���� ������ ���� �� �����̳ʿ��� ����
	for (auto iterCurr = m_mapScene.begin(); iterCurr != m_mapScene.end(); ++iterCurr)
	{
		if (iterCurr->second == m_pScene)
		{
			m_mapScene.erase(iterCurr);
			break;
		}
	}

	delete m_pScene->getSimulationEventCallback();

	// 3. ���� ������ ���� ���� �� �� �� �����̳ʿ��� ã�� ���� ���� ������ ����.
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