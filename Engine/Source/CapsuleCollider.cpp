#include "CapsuleCollider.h"
#include "PhysicsSystem.h"
#include "Graphic.h"
#include "GameObject.h"
#include "Transform.h"

USING(ENGINE)

LPD3DXMESH CapsuleCollider::m_pCylinder = nullptr;
LPD3DXMESH CapsuleCollider::m_pSphere = nullptr;


CapsuleCollider::CapsuleCollider(std::weak_ptr<GameObject> const _pGameObject)
	: Collider(_pGameObject)
	, m_fRadius(0.5f)
	, m_fHeight(1.f)
{
}

void CapsuleCollider::Free()
{
	if (nullptr != m_pCylinder)
	{
		ULONG nRefCount = m_pCylinder->Release();
		if (0 == nRefCount)
			m_pCylinder = nullptr;
	}
	if (nullptr != m_pSphere)
	{
		ULONG nRefCount = m_pSphere->Release();
		if (0 == nRefCount)
			m_pSphere = nullptr;
	}


	Collider::Free();
}

CapsuleCollider* CapsuleCollider::Create(std::weak_ptr<GameObject> const _pGameObject)
{
	CapsuleCollider* pInstance = new CapsuleCollider(_pGameObject);


	if (nullptr == m_pCylinder)
		D3DXCreateCylinder(Graphic::GetDevice(), 0.5f, 0.5f, 1.f, 10, 10, &m_pCylinder, nullptr);
	else
		SafeAddRef(m_pCylinder);

	if (nullptr == m_pSphere)
		D3DXCreateSphere(Graphic::GetDevice(), 0.5f, 10, 10, &m_pSphere, nullptr);
	else
		SafeAddRef(m_pSphere);


	return pInstance;
}

HRESULT CapsuleCollider::ReadyCollider()
{
	m_pMaterial = PhysicsSystem::GetInstance()->GetDefaultMaterial();
	m_pMaterial->acquireReference();
	//Create BoxShape

	m_pShape = PhysicsSystem::GetInstance()->GetPxPhysics()->createShape(PxCapsuleGeometry(m_fRadius, m_fHeight * 0.5f), *m_pMaterial, true);
	//
	PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0.f, 0.f, 1.f)));

	m_pShape->setLocalPose(relativePose);

	Collider::ReadyCollider();
	return S_OK;
}

HRESULT CapsuleCollider::DrawCollider(LPD3DXEFFECT _pEffect)
{
	DrawCylinder(_pEffect);
	DrawSphere(_pEffect);
	DrawSphere(_pEffect, false);

	return S_OK;
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

HRESULT CapsuleCollider::DrawSphere(LPD3DXEFFECT _pEffect, const bool _bUp)
{
	if (nullptr == m_pSphere)
		return S_OK;

	PxTransform localPose = m_pShape->getLocalPose();

	D3DXQUATERNION tQuat(0.f, 0.f, 0.f, 1.f);
	D3DXVECTOR3 vPos(0.f, 0.f, 0.f);

	memcpy_s(&tQuat, sizeof(D3DXQUATERNION), &localPose.q, sizeof(D3DXQUATERNION));
	memcpy_s(&vPos, sizeof(D3DXVECTOR3), &localPose.p, sizeof(D3DXVECTOR3));


	//Draw Cylinder
	D3DXMATRIX matScale, matRot, matTrans, matWorld;

	D3DXMatrixScaling(&matScale, m_fRadius * 2.f, m_fRadius * 2.f, m_fRadius * 2.f);
	D3DXMatrixRotationQuaternion(&matRot, &tQuat);
	if (_bUp)
		D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y + m_fHeight * 0.5f, vPos.z);
	else
		D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y - m_fHeight * 0.5f, vPos.z);


	D3DXMATRIX matGameObject = m_pGameObject.lock()->GetComponent<Transform>().lock()->GetRenderMatrix();

	D3DXVECTOR3 vObjectScale = m_pGameObject.lock()->GetComponent<Transform>().lock()->GetScale();

	D3DXMATRIX matInvScale;

	D3DXMatrixScaling(&matInvScale, vObjectScale.x, vObjectScale.y, vObjectScale.z);
	D3DXMatrixInverse(&matInvScale, nullptr, &matInvScale);

	matWorld = matScale * matRot * matTrans * matInvScale * m_pGameObject.lock()->GetComponent<Transform>().lock()->GetRenderMatrix();

	if (nullptr == _pEffect)
		Graphic::GetDevice()->SetTransform(D3DTS_WORLD, &matWorld);
	else
		_pEffect->SetMatrix("World", &matWorld);

	m_pSphere->DrawSubset(0);

	return S_OK;
}

HRESULT CapsuleCollider::DrawCylinder(LPD3DXEFFECT _pEffect)
{
	if (nullptr == m_pCylinder)
		return S_OK;

	PxTransform localPose = m_pShape->getLocalPose();

	D3DXQUATERNION tQuat(0.f, 0.f, 0.f, 1.f);
	D3DXVECTOR3 vPos(0.f, 0.f, 0.f);

	memcpy_s(&tQuat, sizeof(D3DXQUATERNION), &localPose.q, sizeof(D3DXQUATERNION));
	memcpy_s(&vPos, sizeof(D3DXVECTOR3), &localPose.p, sizeof(D3DXVECTOR3));


	//Draw Cylinder
	D3DXMATRIX matScale, matRot, matTrans, matWorld;

	D3DXMatrixScaling(&matScale, m_fRadius * 2.f, m_fRadius * 2.f, m_fHeight);

	D3DXMatrixScaling(&matScale, m_fRadius * 5.f, m_fRadius * 5.f, m_fHeight);


	D3DXMatrixRotationQuaternion(&matRot, &tQuat);
	D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y, vPos.z);


	D3DXMATRIX matGameObject = m_pGameObject.lock()->GetComponent<Transform>().lock()->GetRenderMatrix();

	D3DXVECTOR3 vObjectScale = m_pGameObject.lock()->GetComponent<Transform>().lock()->GetScale();

	D3DXMATRIX matInvScale;

	D3DXMatrixScaling(&matInvScale, vObjectScale.x, vObjectScale.y, vObjectScale.z);
	D3DXMatrixInverse(&matInvScale, nullptr, &matInvScale);


	D3DXMATRIX matRX;

	D3DXMatrixRotationY(&matRX, D3DXToRadian(90.f));

	matWorld = matScale * matRX * matRot * matTrans * matInvScale * m_pGameObject.lock()->GetComponent<Transform>().lock()->GetRenderMatrix();

	if (nullptr == _pEffect)
		Graphic::GetDevice()->SetTransform(D3DTS_WORLD, &matWorld);
	else
		_pEffect->SetMatrix("World", &matWorld);

	m_pCylinder->DrawSubset(0);

	return S_OK;
}
