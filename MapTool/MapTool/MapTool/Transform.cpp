#include "framework.h"
#include "Transform.h"

Transform::Transform()
	: m_vWorldScale(1.f, 1.f, 1.f)
	, m_vWorldRotation(0.f, 0.f, 0.f)
	, m_vWorldPosition(0.f, 0.f, 0.f)

{
	m_matScale = m_matRotation = m_matTranlation = *D3DXMatrixIdentity(&m_matWorld);
	D3DXQuaternionRotationYawPitchRoll(&m_tWorldQuaternion, D3DXToRadian(m_vWorldRotation.x), D3DXToRadian(m_vWorldRotation.y), D3DXToRadian(m_vWorldRotation.z));
}

Transform::~Transform()
{
}

D3DXVECTOR3 Transform::GetScale(const SPACE& _eSpace)
{
	return m_vWorldScale;
}

void Transform::SetScale(const D3DXVECTOR3& _vScale, const SPACE& _eSpace)
{
	m_vWorldScale = _vScale;
	D3DXMatrixScaling(&m_matScale, m_vWorldScale.x, m_vWorldScale.y, m_vWorldScale.z);
}

D3DXVECTOR3 Transform::GetRotation(const SPACE& _eSpace)
{
	return m_vWorldRotation;
}

void Transform::SetRotation(const D3DXVECTOR3& _vRotation, const SPACE& _eSpace)
{
	m_vWorldRotation = _vRotation;

	D3DXQuaternionRotationYawPitchRoll(&m_tWorldQuaternion, D3DXToRadian(m_vWorldRotation.y), D3DXToRadian(m_vWorldRotation.x), D3DXToRadian(m_vWorldRotation.z));
	D3DXMatrixRotationQuaternion(&m_matRotation, &m_tWorldQuaternion);

}

D3DXQUATERNION Transform::GetQuaternion(const SPACE& _eSpace)
{
	return m_tWorldQuaternion;
}

void Transform::SetQuaternion(const D3DXQUATERNION& _tQuaternion, const SPACE& _eSpace)
{
	m_tWorldQuaternion = _tQuaternion;
	D3DXMatrixRotationQuaternion(&m_matRotation, &m_tWorldQuaternion);
}

D3DXVECTOR3 Transform::GetPosition(const SPACE& _eSpace)
{
	return m_vWorldPosition;
}

void Transform::SetPosition(const D3DXVECTOR3& _vPosition, const SPACE& _eSpace)
{
	m_vWorldPosition = _vPosition;
	D3DXMatrixTranslation(&m_matTranlation, m_vWorldPosition.x, m_vWorldPosition.y, m_vWorldPosition.z);

}

D3DXMATRIX Transform::GetWorldMatrix()
{
	m_matWorld = m_matScale * m_matRotation * m_matTranlation;
	return m_matWorld;
}

void Transform::SetWorldMatrix(const D3DXMATRIX& _matWorld)
{
	D3DXVECTOR3		vScale(0.f, 0.f, 0.f);
	D3DXVECTOR3		vTrans(0.f, 0.f, 0.f);
	D3DXQUATERNION	tQuaternion(0.f, 0.f, 0.f, 1.f);

	D3DXMatrixDecompose(&vScale, &tQuaternion, &vTrans, &_matWorld);

	SetScale(vScale);
	SetQuaternion(tQuaternion);
	SetPosition(vTrans);
}

D3DXVECTOR3 Transform::GetRight()
{
	D3DXVECTOR3 vRight(0.f, 0.f, 0.f);
	memcpy_s(&vRight, sizeof(D3DXVECTOR3), GetWorldMatrix().m[0], sizeof(D3DXVECTOR3));
	D3DXVec3Normalize(&vRight, &vRight);
	return vRight;
}

D3DXVECTOR3 Transform::GetUp()
{
	D3DXVECTOR3 vUp(0.f, 0.f, 0.f);
	memcpy_s(&vUp, sizeof(D3DXVECTOR3), GetWorldMatrix().m[1], sizeof(D3DXVECTOR3));
	D3DXVec3Normalize(&vUp, &vUp);
	return vUp;
}

D3DXVECTOR3 Transform::GetLook()
{
	D3DXVECTOR3 vLook(0.f, 0.f, 0.f);
	memcpy_s(&vLook, sizeof(D3DXVECTOR3), GetWorldMatrix().m[2], sizeof(D3DXVECTOR3));
	D3DXVec3Normalize(&vLook, &vLook);
	return vLook;
}
