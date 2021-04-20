#include "CapsuleCollider.h"
#include "PhysicsSystem.h"
#include "Graphic.h"
#include "GameObject.h"
#include "Transform.h"

USING(ENGINE)

LPD3DXMESH		CapsuleCollider::m_pMesh		= nullptr;
LPD3DXBUFFER	CapsuleCollider::m_pAdjacency	= nullptr;
LPD3DXBUFFER	CapsuleCollider::m_pSubset		= nullptr;
DWORD			CapsuleCollider::m_nNumSubset	= 0;

CapsuleCollider::CapsuleCollider(std::weak_ptr<GameObject> const _pGameObject)
	: Collider(_pGameObject)
	, m_fRadius(0.5f)
	, m_fHeight(1.f)
{
}

void CapsuleCollider::Free()
{
	if (nullptr != m_pSubset)
	{
		ULONG nRefCount = m_pSubset->Release();
		if (0 == nRefCount)
			m_pSubset = nullptr;
	}
	if (nullptr != m_pAdjacency)
	{
		ULONG nRefCount = m_pAdjacency->Release();
		if (0 == nRefCount)
			m_pAdjacency = nullptr;
	}
	if (nullptr != m_pMesh)
	{
		ULONG nRefCount = m_pMesh->Release();
		if (0 == nRefCount)
			m_pMesh = nullptr;
	}


	Collider::Free();
}

CapsuleCollider* CapsuleCollider::Create(std::weak_ptr<GameObject> const _pGameObject)
{
	CapsuleCollider* pInstance = new CapsuleCollider(_pGameObject);

	if (FAILED(pInstance->ReadyCollider()))
	{
		delete pInstance;
		return nullptr;
	}

	if (nullptr == m_pMesh)
		D3DXLoadMeshFromX(TEXT("../../Resource/Mesh/Static/Capsule.x"), D3DXMESH_MANAGED, Graphic::GetDevice(), &m_pAdjacency, &m_pSubset, nullptr, &m_nNumSubset, &m_pMesh);
	else
	{
		SafeAddRef(m_pSubset);
		SafeAddRef(m_pAdjacency);
		SafeAddRef(m_pMesh);
	}

	return pInstance;
}

HRESULT CapsuleCollider::ReadyCollider()
{
	m_pMaterial = PhysicsSystem::GetInstance()->GetDefaultMaterial();
	//Create BoxShape

	m_pShape = PhysicsSystem::GetInstance()->GetPxPhysics()->createShape(PxCapsuleGeometry(m_fRadius, m_fHeight * 0.5f), *m_pMaterial, true);
	//
	PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0.f, 0.f, 1.f)));

	m_pShape->setLocalPose(relativePose);

	Collider::ReadyCollider();
	return S_OK;
}

HRESULT CapsuleCollider::DrawCollider(const DrawInfo& _Info)
{
	if (nullptr == m_pMesh)
		return S_OK;

	PxTransform localPose = m_pShape->getLocalPose();

	D3DXQUATERNION tQuat(0.f, 0.f, 0.f, 1.f);
	D3DXVECTOR3 vPos(0.f, 0.f, 0.f);

	memcpy_s(&tQuat, sizeof(D3DXQUATERNION), &localPose.q, sizeof(D3DXQUATERNION));
	memcpy_s(&vPos, sizeof(D3DXVECTOR3), &localPose.p, sizeof(D3DXVECTOR3));


	//Draw Cylinder
	D3DXMATRIX matScale, matRot, matTrans, matWorld;

	D3DXMatrixScaling(&matScale, m_fHeight, m_fRadius * 2.f, m_fRadius * 2.f);
	D3DXMatrixRotationQuaternion(&matRot, &tQuat);
	D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y, vPos.z);


	D3DXMATRIX matGameObject = m_pGameObject.lock()->GetComponent<Transform>().lock()->GetRenderMatrix();

	D3DXVECTOR3 vObjectScale = m_pGameObject.lock()->GetComponent<Transform>().lock()->GetScale();

	D3DXMATRIX matInvScale;

	D3DXMatrixScaling(&matInvScale, vObjectScale.x, vObjectScale.y, vObjectScale.z);
	D3DXMatrixInverse(&matInvScale, nullptr, &matInvScale);

	matWorld = matScale * matRot * matTrans * matInvScale * matGameObject;

	_Info.Fx->SetMatrix("World", &matWorld);
	_Info.Fx->CommitChanges();
	for (UINT i = 0; i < m_nNumSubset; ++i)
		m_pMesh->DrawSubset(i);

	return S_OK;
}

void CapsuleCollider::Editor()
{
	Component::Editor();

	if (false == bEdit)
		return;

	Collider::Editor();

	float fRadius = m_fRadius;
	if (ImGui::InputFloat("Radius", &fRadius))
		SetRadius(fRadius);

	float fHeight = m_fHeight;
	if (ImGui::InputFloat("Height", &fHeight))
		SetHeight(fHeight);

}

float CapsuleCollider::GetRadius()
{
	return m_fRadius;
}

void CapsuleCollider::SetRadius(const float _fRadius)
{
	if (fabs(_fRadius - m_fRadius) < FLT_EPSILON)
		return;;

	m_fRadius = _fRadius;

	PxCapsuleGeometry geometry;

	m_pShape->getCapsuleGeometry(geometry);

	geometry.radius = _fRadius;

	m_pShape->setGeometry(geometry);
}

float CapsuleCollider::GetHeight()
{
	return m_fHeight;
}

void CapsuleCollider::SetHeight(const float _fHeight)
{
	if (fabs(_fHeight - m_fHeight) < FLT_EPSILON)
		return;;

	m_fHeight = _fHeight;

	PxCapsuleGeometry geometry;

	m_pShape->getCapsuleGeometry(geometry);

	geometry.halfHeight = _fHeight * 0.5f;

	m_pShape->setGeometry(geometry);
}
