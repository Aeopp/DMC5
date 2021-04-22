#include "framework.h"
#include "GameObject.h"
#include "Transform.h"

static int nCount = 0;

GameObject::GameObject()
	: m_pRigidActor(nullptr)
	, m_pShape(nullptr)
{
	m_pTransform = new Transform();
	m_nIndex = nCount++;
}

GameObject::~GameObject()
{
	delete m_pTransform;

	Physics::Instance.m_pScene->removeActor(*m_pRigidActor);

	m_pRigidActor->userData = nullptr;
	if (m_pRigidActor)
		m_pRigidActor->release();
	if (m_pShape)
		m_pShape->release();
	
}

void GameObject::CreateRigidActor(PxTriangleMesh* _pTriangleMesh)
{
	m_pRigidActor = Physics::Instance.m_pPhysics->createRigidStatic(physx::PxTransform(0.f, 0.f, 0.f));

	m_Geometry.triangleMesh = _pTriangleMesh;
	m_Geometry.scale = PxVec3(1.f, 1.f, 1.f);

	m_pShape = Physics::Instance.m_pPhysics->createShape(m_Geometry, *Physics::Instance.m_pDefaultMaterial, true);
	m_pRigidActor->attachShape(*m_pShape);
	m_pRigidActor->userData = this;

	Physics::Instance.m_pScene->addActor(*m_pRigidActor);
}

void GameObject::Update()
{
	PxVec3 p;
	PxQuat q;
	D3DXVECTOR3 vPos = m_pTransform->GetPosition();
	D3DXQUATERNION tQuat = m_pTransform->GetQuaternion();

	D3DXVECTOR3 vScale = m_pTransform->GetScale();
	D3DXMATRIX matScale;
	D3DXMatrixScaling(&matScale, vScale.x, vScale.y, vScale.z);

	PxTriangleMeshGeometry triGeometry;

	m_pShape->getTriangleMeshGeometry(triGeometry);

	triGeometry.scale = PxVec3(vScale.x, vScale.y, vScale.z);

	m_pShape->setGeometry(triGeometry);
	
	memcpy_s(&p, sizeof(D3DXVECTOR3), &vPos, sizeof(D3DXVECTOR3));
	memcpy_s(&q, sizeof(D3DXQUATERNION), &tQuat, sizeof(D3DXQUATERNION));


	PxTransform globalPose(p, q);

	m_pRigidActor->setGlobalPose(globalPose);
}
