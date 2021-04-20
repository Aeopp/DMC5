#include "framework.h"
#include "Physics.h"
#include "GameObject.h"

Physics Physics::Instance;

#ifdef _DEBUG
#define PVD_HOST "127.0.0.1"
#endif // _DEBUG

#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}


Physics::Physics()
	: m_pFoundation(nullptr)
	, m_pPhysics(nullptr)
	, m_pDispatcher(nullptr)
	, m_pDefaultMaterial(nullptr)
	, m_pPVD(nullptr)
	, m_pScene(nullptr)
	, m_pCooking(nullptr)
{
}

Physics::~Physics()
{
	PX_RELEASE(m_pScene);

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

HRESULT Physics::Ready()
{
	//Foundation
	Instance.m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, Instance.m_Allocator, Instance.m_ErrorCallback);

	if (nullptr == Instance.m_pFoundation)
	{		
		return E_FAIL;
	}

	//PhysX Visual Debugger
#ifdef _DEBUG
	Instance.m_pPVD = physx::PxCreatePvd(*Instance.m_pFoundation);
	if (nullptr == Instance.m_pPVD)
	{
		return E_FAIL;
	}
	physx::PxPvdTransport* pPVDTransport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	Instance.m_pPVD->connect(*pPVDTransport, physx::PxPvdInstrumentationFlag::eALL);
#endif
	physx::PxTolerancesScale pTolerance = physx::PxTolerancesScale();
	//Physics
	Instance.m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *Instance.m_pFoundation, pTolerance, true, Instance.m_pPVD);
	if (nullptr == Instance.m_pPhysics)
	{
		return E_FAIL;
	}


	//Scene Description
	physx::PxSceneDesc sceneDesc(Instance.m_pPhysics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.f, 0.f, 0.f);
	Instance.m_pDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = Instance.m_pDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVE_ACTORS;

	//Scene
	Instance.m_pScene = Instance.m_pPhysics->createScene(sceneDesc);
	if (nullptr == Instance.m_pScene)
	{
		return E_FAIL;
	}

	//Default Material
	Instance.m_pDefaultMaterial = Instance.m_pPhysics->createMaterial(0.5f, 0.5f, 0.5f);
	
	Instance.m_pCooking = PxCreateCooking(PX_PHYSICS_VERSION, *Instance.m_pFoundation, Instance.m_pPhysics->getTolerancesScale());

	return S_OK;
}

GameObject* Physics::RayCast()
{
	POINT tCursor;
	GetCursorPos(&tCursor);
	ScreenToClient(g_hWnd, &tCursor);

	D3DVIEWPORT9 tViewPort;
	g_pDevice->GetViewport(&tViewPort);

	D3DXVECTOR3 vCursorPos(0.f, 0.f, 0.f);

	vCursorPos.x = tCursor.x / (tViewPort.Width * 0.5f) - 1.f;
	vCursorPos.y = tCursor.y / -(tViewPort.Height * 0.5f) + 1.f;
	vCursorPos.z = 1.f;

	D3DXMATRIX matProj;
	g_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	D3DXMatrixInverse(&matProj, nullptr, &matProj);
	
	D3DXVec3TransformCoord(&vCursorPos, &vCursorPos, &matProj);

	D3DXVECTOR3 vRayOrigin(0.f, 0.f, 0.f);
	D3DXVECTOR3 vRayDir = vCursorPos - vRayOrigin;

	D3DXMATRIX matView;
	g_pDevice->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, nullptr, &matView);

	D3DXVec3TransformCoord(&vRayOrigin, &vRayOrigin, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);

	D3DXVec3Normalize(&vRayDir, &vRayDir);

	PxVec3 vOrigin(vRayOrigin.x, vRayOrigin.y, vRayOrigin.z);
	PxVec3 vDir(vRayDir.x, vRayDir.y, vRayDir.z);
	PxRaycastBuffer rayCastBuf;
	bool bResult = Physics::Instance.m_pScene->raycast(vOrigin, vDir, 2000.f, rayCastBuf);

	if (bResult)
	{
		return ((GameObject*)(rayCastBuf.block.actor->userData));
	}
	else
		return nullptr;
}
