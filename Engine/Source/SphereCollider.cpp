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

	if (nullptr == m_pMesh)
		D3DXCreateSphere(Graphic::GetDevice(), 0.5f, 10, 10, &m_pMesh, nullptr);
	else
		SafeAddRef(m_pMesh);

	return pInstance;
}

HRESULT SphereCollider::ReadyCollider()
{
	m_pMaterial = PhysicsSystem::GetInstance()->GetDefaultMaterial();
	m_pMaterial->acquireReference();
	//Create BoxShape
	m_pShape = PhysicsSystem::GetInstance()->GetPxPhysics()->createShape(PxSphereGeometry(m_fRadius), *m_pMaterial, true);
	//
	ReadyCollider();
	return S_OK;
}

HRESULT SphereCollider::DrawCollider(const DrawInfo& _Info)
{
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

	D3DXMATRIX matGameObject = m_pGameObject.lock()->GetComponent<Transform>().lock()->GetRenderMatrix();

	D3DXVECTOR3 vObjectScale = m_pGameObject.lock()->GetComponent<Transform>().lock()->GetScale();

	D3DXMATRIX matInvScale;

	D3DXMatrixScaling(&matInvScale, vObjectScale.x, vObjectScale.y, vObjectScale.z);
	D3DXMatrixInverse(&matInvScale, nullptr, &matInvScale);

	matWorld = matScale * matRot * matTrans * matInvScale * m_pGameObject.lock()->GetComponent<Transform>().lock()->GetRenderMatrix();

	_Info.Fx->SetMatrix("World", &matWorld);

	m_pMesh->DrawSubset(0);

	return S_OK;
}

void SphereCollider::Editor()
{
	Collider::Editor();
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
