#include "BoxCollider.h"
#include "Physics.h"
#include "Graphic.h"
#include "GameObject.h"
#include "Transform.h"

USING(ENGINE)

LPD3DXMESH BoxCollider::m_pMesh = nullptr;

BoxCollider::BoxCollider(std::weak_ptr<GameObject> const _pGameObject)
	: Collider(_pGameObject)
	, m_vSize(1.f, 1.f, 1.f)
{
}

void BoxCollider::Free()
{
	if (nullptr != m_pMesh)
	{
		ULONG nRefCount = m_pMesh->Release();
		if (0 == nRefCount)
			m_pMesh = nullptr;
	}
	Collider::Free();
}

BoxCollider* BoxCollider::Create(std::weak_ptr<GameObject> const _pGameObject)
{
	BoxCollider* pInstance = new BoxCollider(_pGameObject);

	if (FAILED(pInstance->ReadyCollider()))
	{
		delete pInstance;
		return nullptr;
	}

	if (nullptr == m_pMesh)
		D3DXCreateBox(Graphic::GetDevice(), 1.f, 1.f, 1.f, &m_pMesh, nullptr);
	else
		SafeAddRef(m_pMesh);

	return pInstance;
}

HRESULT BoxCollider::ReadyCollider()
{
	physx::PxVec3 vHalfExtent(0.f, 0.f, 0.f);
	memcpy_s(&vHalfExtent, sizeof(D3DXVECTOR3), &m_vSize, sizeof(D3DXVECTOR3));
	vHalfExtent = vHalfExtent * 0.5f;

	m_pMaterial = Physics::GetDefaultMaterial();

	//Create BoxShape
	m_pShape = Physics::GetPxPhysics()->createShape(physx::PxBoxGeometry(vHalfExtent), *m_pMaterial, true);

	Collider::ReadyCollider();

	return S_OK;
}

HRESULT BoxCollider::DrawCollider(const DrawInfo& _Info)
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

	D3DXMatrixScaling(&matScale, m_vSize.x, m_vSize.y, m_vSize.z);
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

void BoxCollider::Editor()
{
	Component::Editor();

	Collider::Editor();

	D3DXVECTOR3 vSize = m_vSize;

	if (ImGui::InputFloat3("Size##BoxCollider", vSize))
		SetSize(vSize);
}

D3DXVECTOR3 BoxCollider::GetSize()
{
	return m_vSize;
}

void BoxCollider::SetSize(const D3DXVECTOR3 _vSize)
{
	m_vSize = _vSize;

	physx::PxBoxGeometry boxGeometry;

	m_pShape->getBoxGeometry(boxGeometry);

	boxGeometry.halfExtents.x = m_vSize.x * 0.5f;
	boxGeometry.halfExtents.y = m_vSize.y * 0.5f;
	boxGeometry.halfExtents.z = m_vSize.z * 0.5f;

	m_pShape->setGeometry(boxGeometry);
}
