#include "SphereCollider.h"
#include "PhysicsSystem.h"
#include "Graphic.h"
#include "GameObject.h"
#include "Transform.h"

USING(ENGINE)

LPD3DXMESH SphereCollider::m_pMesh = nullptr;

SphereCollider::SphereCollider(std::weak_ptr<GameObject> const _pGameObject)
	: Collider(_pGameObject)
	, m_fRadius(0.5f)
{
}

void SphereCollider::Free()
{
	if (nullptr != m_pMesh)
	{
		ULONG nRefCount = m_pMesh->Release();

		if (0 == nRefCount)
			m_pMesh = nullptr;
	}
	Collider::Free();
}

SphereCollider* SphereCollider::Create(std::weak_ptr<GameObject> const _pGameObject)
{
	SphereCollider* pInstance = new SphereCollider(_pGameObject);

	if (FAILED(pInstance->ReadyCollider()))
	{
		delete pInstance;
		return nullptr;
	}

	if (nullptr == m_pMesh)
		D3DXCreateSphere(Graphic::GetDevice(), 0.5f, 10, 10, &m_pMesh, nullptr);
	else
		SafeAddRef(m_pMesh);

	return pInstance;
}

HRESULT SphereCollider::ReadyCollider()
{
	m_pMaterial = PhysicsSystem::GetInstance()->GetDefaultMaterial();
	//Create BoxShape
	m_pShape = PhysicsSystem::GetInstance()->GetPxPhysics()->createShape(PxSphereGeometry(m_fRadius), *m_pMaterial, true);
	//
	Collider::ReadyCollider();
	return S_OK;
}

HRESULT SphereCollider::DrawCollider(const DrawInfo& _Info)
{
	if (FAILED(Collider::DrawCollider(_Info)))
		return S_OK;
	if (nullptr == m_pMesh)
		return S_OK;

	PxTransform localPose = m_pShape->getLocalPose();

	D3DXQUATERNION tQuat(0.f, 0.f, 0.f, 1.f);
	D3DXVECTOR3 vPos(0.f, 0.f, 0.f);

	memcpy_s(&tQuat, sizeof(D3DXQUATERNION), &localPose.q, sizeof(D3DXQUATERNION));
	memcpy_s(&vPos, sizeof(D3DXVECTOR3), &localPose.p, sizeof(D3DXVECTOR3));

	D3DXMATRIX matScale, matRot, matTrans, matWorld;

	float m_fSize = m_fRadius * 2.f;

	D3DXMatrixScaling(&matScale, m_fSize, m_fSize, m_fSize);
	D3DXMatrixRotationQuaternion(&matRot, &tQuat);
	D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y, vPos.z);

	PxTransform actorTrans = m_pRigidActor->getGlobalPose();

	D3DXVECTOR3 vActorPos(0.f, 0.f, 0.f);
	D3DXQUATERNION tActorRot(0.f, 0.f, 0.f, 1.f);

	memcpy_s(&vActorPos, sizeof(D3DXVECTOR3), &actorTrans.p, sizeof(D3DXVECTOR3));
	memcpy_s(&tActorRot, sizeof(D3DXQUATERNION), &actorTrans.q, sizeof(D3DXQUATERNION));

	D3DXMATRIX matActorTrans;
	D3DXMATRIX matActorRot;

	D3DXMatrixTranslation(&matActorTrans, vActorPos.x, vActorPos.y, vActorPos.z);
	D3DXMatrixRotationQuaternion(&matActorRot, &tActorRot);

	matWorld = matScale * matRot * matTrans * matActorRot * matActorTrans;

	_Info.Fx->SetMatrix("World", &matWorld);
	_Info.Fx->CommitChanges();
	m_pMesh->DrawSubset(0);

	return S_OK;
}

void SphereCollider::Editor()
{
	Component::Editor();

	Collider::Editor();

	float fRadius = m_fRadius;
	if (ImGui::InputFloat("Radius##SphereCollider", &fRadius))
		SetRadius(fRadius);
}

float SphereCollider::GetRadius()
{
	return m_fRadius;
}

void SphereCollider::SetRadius(const float _fRadius)
{
	if (fabs(m_fRadius - _fRadius) < FLT_EPSILON)
		return;

	m_fRadius = _fRadius;

	PxSphereGeometry sphereGeometry;

	m_pShape->getSphereGeometry(sphereGeometry);

	sphereGeometry.radius = m_fRadius;

	m_pShape->setGeometry(sphereGeometry);
}
