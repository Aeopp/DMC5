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
	m_pFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, m_Allocator, m_ErrorCallback);

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
	PxTolerancesScale TolerancesScale;
	// TolerancesScale.length = 1000.f;
	TolerancesScale.length = 1.f;
	TolerancesScale.speed = 10.f;
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, TolerancesScale, true, m_pPVD);
	if (nullptr == m_pPhysics)
	{
		PRINT_LOG(TEXT("Error"), TEXT("Failed to PxCreatePhysics"));
		return E_FAIL;
	}

	//Cooking
	m_pCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_pFoundation, m_pPhysics->getTolerancesScale());

	//Default Material
	m_pDefaultMaterial = m_pPhysics->createMaterial(0.3f, 0.3f, 0.5f);
	
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
	{
		if (ppActors[i]->userData)
			((LPPXUSERDATA)(ppActors[i]->userData))->pCollider.lock()->ReadySimulate();
	}
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
	{
		if (ppActors[i]->userData)
			((LPPXUSERDATA)(ppActors[i]->userData))->pCollider.lock()->ReadySimulate();
	}
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

	while (false == (m_pScene->fetchResults(true)));

	//Callback ȣ��
	for (auto& rPair : m_listCallback)
	{
		if (false == rPair.bTrigger)
		{
			switch (rPair.ePairFlag)
			{
			case PxPairFlag::Enum::eNOTIFY_TOUCH_FOUND:
				if (rPair.pFirst.lock() && rPair.pSecond.lock())
				{
					rPair.pFirst.lock()->OnCollisionEnter(rPair.pSecond);
					rPair.pSecond.lock()->OnCollisionEnter(rPair.pFirst);
				}
				break;
			case PxPairFlag::Enum::eNOTIFY_TOUCH_PERSISTS:
				if (rPair.pFirst.lock() && rPair.pSecond.lock())
				{
					rPair.pFirst.lock()->OnCollisionStay(rPair.pSecond);
					rPair.pSecond.lock()->OnCollisionStay(rPair.pFirst);
				}
				break;
			case PxPairFlag::Enum::eNOTIFY_TOUCH_LOST:
				if (rPair.pFirst.lock() && rPair.pSecond.lock())
				{
					rPair.pFirst.lock()->OnCollisionExit(rPair.pSecond);
					rPair.pSecond.lock()->OnCollisionExit(rPair.pFirst);
				}
				break;
			default:
				break;
			}
		}
		else
		{
			switch (rPair.ePairFlag)
			{
			case PxPairFlag::Enum::eNOTIFY_TOUCH_FOUND:
				if (rPair.pFirst.lock() && rPair.pSecond.lock())
				{
					rPair.pFirst.lock()->OnTriggerEnter(rPair.pSecond);
					rPair.pSecond.lock()->OnTriggerEnter(rPair.pFirst);
				}
				break;
			case PxPairFlag::Enum::eNOTIFY_TOUCH_LOST:
				if (rPair.pFirst.lock() && rPair.pSecond.lock())
				{
					rPair.pFirst.lock()->OnTriggerExit(rPair.pSecond);
					rPair.pSecond.lock()->OnTriggerExit(rPair.pFirst);
				}
				break;
			default:
				break;
			}
		}
	}


	m_listCallback.clear();
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
	sceneDesc.gravity = physx::PxVec3(0.f, -4.45f, 0.f);
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

void PhysicsSystem::AddCallbackPair(const COLLISIONPAIR _tCollsionPair)
{
	m_listCallback.emplace_back(_tCollsionPair);
}

bool PhysicsSystem::RayCast(const D3DXVECTOR3& _vOrigin, const D3DXVECTOR3& _vDir, OUT D3DXVECTOR3& _vPoint, float _fDistance)
{
	PxVec3 vOrigin(0.f, 0.f, 0.f);
	memcpy_s(&vOrigin, sizeof(PxVec3), &_vOrigin, sizeof(PxVec3));
	
	PxVec3 vDir(0.f, 0.f, 0.f);
	D3DXVECTOR3 vNormalizedDir = _vDir;
	D3DXVec3Normalize(&vNormalizedDir, &vNormalizedDir);
	memcpy_s(&vDir, sizeof(PxVec3), &vNormalizedDir, sizeof(PxVec3));
	PxRaycastBuffer tRayCastBuff;
	
	if (true == m_pScene->raycast(vOrigin, vDir, (PxReal)_fDistance, tRayCastBuff))
	{
		if (true == tRayCastBuff.hasBlock)
		{
			PxRaycastHit tRayCastHit = tRayCastBuff.block;
			memcpy_s(&_vPoint, sizeof(D3DXVECTOR3), &tRayCastHit.position, sizeof(D3DXVECTOR3));
			return true;
		}
		return false;
	}

	return false;
}
