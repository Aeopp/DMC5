#include "Transform.h"
#include "FMath.hpp"
#include <numeric>

USING(ENGINE)

Transform::Transform(std::weak_ptr<GameObject> const _pGameObject)
	: Component(_pGameObject)
	, m_vWorldScale(1.f, 1.f, 1.f)
	, m_vWorldRotation(0.f, 0.f, 0.f)
	, m_vWorldPosition(0.f, 0.f, 0.f)
	, m_bSimulation(false)
	, m_bUpdated(false)
{
	m_matBillboard = m_matScale = m_matRotation = m_matTranlation = *D3DXMatrixIdentity(&m_matWorld);
	D3DXQuaternionRotationYawPitchRoll(&m_tWorldQuaternion, D3DXToRadian(m_vWorldRotation.x), D3DXToRadian(m_vWorldRotation.y), D3DXToRadian(m_vWorldRotation.z));
}

void Transform::Free()
{
	Component::Free();
}

void Transform::Editor()
{
	Component::Editor();

	if (bEdit)
	{
		if (ImGui::TreeNode("Transform Edit"))
		{
			const Vector3 CurScale = GetScale(); 
			const Vector3 CurRotation = m_vWorldRotation;
			const Vector3 CurPosition = GetPosition();

			ImGui::Text("Scale : X : %3.5f , Y : %3.5f , Z : %3.5f", CurScale.x, CurScale.y, CurScale.z);
			ImGui::Text("Rotation : X : %3.5f , Y : %3.5f , Z : %3.5f", CurRotation.x, CurRotation.y, CurRotation.z);
			ImGui::Text("Position : X : %3.5f , Y : %3.5f , Z : %3.5f", CurPosition.x, CurPosition.y, CurPosition.z);

			{
				float InputScale = 0.0f;
				if (ImGui::InputFloat("InputScale", &InputScale))
				{
					SetScale({ InputScale , InputScale,InputScale });
				}

				Vector3 InputRotation = { 0,0,0 };
				if (ImGui::InputFloat3("InputRotation", InputRotation))
				{
					SetRotation(InputRotation);
				}

				Vector3 InputPosition = { 0,0,0 };
				if (ImGui::InputFloat3("InputPosition", InputPosition))
				{
					SetPosition(InputPosition);
				}
			}

			static float ScaleSensitivy = 0.000001f;
			ImGui::InputFloat("ScaleSensitivy", &ScaleSensitivy);
			float Scale = 0.0f; 
			if (ImGui::SliderFloat("Scale", &Scale,-10.f,10.f))
			{
				const float ResultScale = Scale* ScaleSensitivy;
				Vector3 Result = CurScale + Vector3{ ResultScale ,ResultScale ,ResultScale };
				if (Result.x < FLT_MIN)
				{
					Result = { FLT_MIN ,FLT_MIN ,FLT_MIN };
				}
				SetScale(Result);
			}
			static float RotationSensitivy = 0.0001f;
			ImGui::InputFloat("RotationSensitivy", &RotationSensitivy);
			Vector3 Rotation{ 0,0,0 };
			if (ImGui::SliderFloat3("Rotation", Rotation,-360.f,+360.f))
			{
				SetRotation(CurRotation +Rotation * RotationSensitivy);
			}
			static float PositionSensitivy = 0.005f;
			ImGui::InputFloat("PositionSensitivy", &PositionSensitivy);
			Vector3 Position{ 0,0,0 };
			if (ImGui::SliderFloat3("Position", Position, -10.f, +10.f))
			{
				SetPosition(CurPosition + Position* PositionSensitivy);
			}
			ImGui::TreePop();
		}
	}
}

Transform* Transform::Create(std::weak_ptr<GameObject> const _pGameObject)
{
	Transform* pInstance = new Transform(_pGameObject);
	return pInstance;
}

void Transform::UpdateTransform()
{
	if (false == m_bSimulation)
		m_matRender = GetWorldMatrix();
	else
		m_bSimulation = false;

	m_bUpdated = false;
}

void Transform::SetSimulationResult(const D3DXQUATERNION& _tQuaternion, const D3DXVECTOR3& _vPosition)
{
	m_tWorldQuaternion = _tQuaternion;

	m_vWorldPosition = _vPosition;

	D3DXMATRIX matScale, matRot, matTrans;

	D3DXMatrixScaling(&matScale, m_vWorldScale.x, m_vWorldScale.y, m_vWorldScale.z);
	D3DXMatrixRotationQuaternion(&matRot, &_tQuaternion);
	D3DXMatrixTranslation(&matTrans, _vPosition.x, _vPosition.y, _vPosition.z);

	m_matRender = matScale * matRot * matTrans;

	m_bSimulation = true;
}

std::string Transform::GetName()
{
	return "Transform";
}

D3DXVECTOR3 Transform::GetScale(const SPACE& _eSpace)
{
	return m_vWorldScale;
}

void Transform::SetScale(const D3DXVECTOR3& _vScale, const SPACE& _eSpace)
{
	m_vWorldScale = _vScale;
	D3DXMatrixScaling(&m_matScale, m_vWorldScale.x, m_vWorldScale.y, m_vWorldScale.z);
	m_bUpdated = true;
}

void Transform::SetRotation(const D3DXVECTOR3& _vRotation, const SPACE& _eSpace)
{
	m_vWorldRotation = _vRotation;

	D3DXQuaternionRotationYawPitchRoll(&m_tWorldQuaternion, D3DXToRadian(m_vWorldRotation.y), D3DXToRadian(m_vWorldRotation.x), D3DXToRadian(m_vWorldRotation.z));
	D3DXMatrixRotationQuaternion(&m_matRotation, &m_tWorldQuaternion);
	m_bUpdated = true;
}

D3DXQUATERNION Transform::GetQuaternion(const SPACE& _eSpace)
{
	return m_tWorldQuaternion;
}

void Transform::SetQuaternion(const D3DXQUATERNION& _tQuaternion, const SPACE& _eSpace)
{
	m_tWorldQuaternion = _tQuaternion;
	D3DXMatrixRotationQuaternion(&m_matRotation, &m_tWorldQuaternion);
	m_bUpdated = true;
}

D3DXVECTOR3 Transform::GetPosition(const SPACE& _eSpace)
{
	return m_vWorldPosition;
}

void Transform::SetPosition(const D3DXVECTOR3& _vPosition, const SPACE& _eSpace)
{
	m_vWorldPosition = _vPosition;
	D3DXMatrixTranslation(&m_matTranlation, m_vWorldPosition.x, m_vWorldPosition.y, m_vWorldPosition.z);
	m_bUpdated = true;
}

D3DXMATRIX Transform::GetBiilBoard()
{
	return m_matBillboard;
}

void Transform::SetBillBoard(const D3DXMATRIX& _matBillboard)
{
	m_matBillboard = _matBillboard;
}

D3DXMATRIX Transform::GetWorldMatrix()
{
	m_matWorld = m_matScale * m_matBillboard * m_matRotation * m_matTranlation;
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

D3DXMATRIX Transform::GetRenderMatrix()
{
	return m_matRender;
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

bool Transform::IsUpdated()
{
	return m_bUpdated;
}

void Transform::Translate(const D3DXVECTOR3 _vTranslate)
{
	m_vWorldPosition += _vTranslate;
	D3DXMatrixTranslation(&m_matTranlation, m_vWorldPosition.x, m_vWorldPosition.y, m_vWorldPosition.z);
	m_bUpdated = true;
}

void Transform::Rotate(const D3DXVECTOR3 _vRotate)
{
	D3DXQUATERNION tQuatAdd;
	D3DXQuaternionRotationYawPitchRoll(&tQuatAdd, D3DXToRadian(_vRotate.y), D3DXToRadian(_vRotate.x), D3DXToRadian(_vRotate.z));

	m_tWorldQuaternion = m_tWorldQuaternion * tQuatAdd;

	D3DXMatrixRotationQuaternion(&m_matRotation, &m_tWorldQuaternion);
	m_bUpdated = true;
}

D3DXQUATERNION Transform::EulerToQuaternion(const D3DXVECTOR3 _vEuler)
{
	D3DXQUATERNION tQuat;

	D3DXQuaternionRotationYawPitchRoll(&tQuat, D3DXToRadian(_vEuler.y), D3DXToRadian(_vEuler.x), D3DXToRadian(_vEuler.z));

	return tQuat;
}

void Normalize(float& _fValue)
{
	while (_fValue > 360.f)
	{
		_fValue -= 360.f;
	}
	while (_fValue < 0.f)
	{
		_fValue += 360.f;
	}
}

D3DXVECTOR3 Transform::QuaternionToEuler(const D3DXQUATERNION _vQuat)
{
	float sqw = _vQuat.w * _vQuat.w;

	float sqx = _vQuat.x * _vQuat.x;

	float sqy = _vQuat.y * _vQuat.y;

	float sqz = _vQuat.z * _vQuat.z;

	float unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor

	float test = _vQuat.x * _vQuat.w - _vQuat.y * _vQuat.z;

	Vector3 v;



	if (test > 0.4995f * unit) 
	{ 
		// singularity at north pole
		v.y = 2.f * atan2(_vQuat.y, _vQuat.x);
		v.x = D3DX_PI / 2.f;
		v.z = 0;

	}
	else if (test < -0.4995f * unit) 
	{ 
		// singularity at south pole
		v.y = -2.f * atan2(_vQuat.y, _vQuat.x);
		v.x = -D3DX_PI / 2.f;
		v.z = 0;
	}
	else
	{
		D3DXQUATERNION q(_vQuat.w, _vQuat.z, _vQuat.x, _vQuat.y);

		v.y = (float)atan2(2.f * q.x * q.w + 2.f * q.y * q.z, 1 - 2.f * (q.z * q.z + q.w * q.w));     // Yaw

		v.x = (float)asin(2.f * (q.x * q.z - q.w * q.y));                             // Pitch

		v.z = (float)atan2(2.f * q.x * q.y + 2.f * q.z * q.w, 1 - 2.f * (q.y * q.y + q.z * q.z));      // Roll
	}

	v.x = D3DXToDegree(v.x);
	v.y = D3DXToDegree(v.y);
	v.z = D3DXToDegree(v.z);

	Normalize(v.x);
	Normalize(v.y);
	Normalize(v.z);


	return v;
}

float Transform::NormalizeAngle(float _fAngle)
{
	while (_fAngle > 360.f)
	{
		_fAngle -= 360.f;
	}
	while (_fAngle < 0.f)
	{
		_fAngle += 360.f;
	}
	return _fAngle;
}
