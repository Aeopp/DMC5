#include "Collider.h"
#include "Physics.h"
#include "GameObject.h"
#include "Transform.h"

USING(ENGINE)

Collider::Collider(std::weak_ptr<GameObject> const _pGameObject)
	: Component(_pGameObject)
	, m_pShape(nullptr)
	, m_pMaterial(nullptr)
	, m_pRigidActor(nullptr)
	, m_bRigid(false)
	, m_bTrigger(false)
	, m_vCenter(0.f, 0.f, 0.f)
	, m_bLock{ false,false,false,false,false,false }
	, m_bGravity(true)
{
}

void Collider::Free()
{
	//Scene에서 Actor 제거

	//
	PX_RELEASE(m_pRigidActor);
	PX_RELEASE(m_pMaterial);
	PX_RELEASE(m_pShape);
	Component::Free();
}

HRESULT Collider::ReadyCollider()
{
	CreateRigidActor();

	//Scene에 Actor 추가.
	Physics::AddActor(*m_pRigidActor);
	return S_OK;
}

HRESULT Collider::DrawCollider(LPD3DXEFFECT _pEffect)
{
	return S_OK;
}

void Collider::ReadySimulate()
{
	std::weak_ptr<Transform> pTransform = m_pGameObject.lock()->GetComponent<Transform>();
	//Transform에 변화가 있었는지 확인
	if (true == pTransform.expired() || false == pTransform.lock()->IsUpdated())
		return;
	if (nullptr == m_pRigidActor)
		return;

	D3DXVECTOR3		vPosition = pTransform.lock()->GetPosition();
	D3DXQUATERNION	tQuaternion = pTransform.lock()->GetQuaternion();

	PxVec3 pxPos;
	PxQuat pxQuat;

	memcpy_s(&pxPos, sizeof(D3DXVECTOR3), &vPosition, sizeof(D3DXVECTOR3));
	memcpy_s(&pxQuat, sizeof(D3DXQUATERNION), &tQuaternion, sizeof(D3DXQUATERNION));

	m_pRigidActor->setGlobalPose(physx::PxTransform(pxPos, pxQuat));
}

void Collider::CreateRigidActor()
{
	//기존에 사용중인 Actor가 있다면 해제. ==> Scene에서 제거 추가 수정 필요.
	if (nullptr != m_pRigidActor)
		PX_RELEASE(m_pRigidActor);

	//RigidActor 생성
	if (m_bRigid)
		m_pRigidActor = Physics::GetPxPhysics()->createRigidDynamic(physx::PxTransform(0.f, 0.f, 0.f));
	else
		m_pRigidActor = Physics::GetPxPhysics()->createRigidStatic(physx::PxTransform(0.f, 0.f, 0.f));

	//UserData 설정.
	m_UserData.pCollider = std::static_pointer_cast<Collider>(m_pThis.lock());

	m_pRigidActor->userData = (void*)&m_UserData;

	//Shape 연결
	m_pRigidActor->attachShape(*m_pShape);
}

bool Collider::IsRigid()
{
	return m_bRigid;
}

void Collider::SetRigid(const bool _bRigid)
{
	if (_bRigid == m_bRigid)
		return;

	m_bRigid = _bRigid;

	//Scene에서 Actor제거
	Physics::RemoveActor(*m_pRigidActor);

	//RigidActor 생성
	CreateRigidActor();

	//Scene에 새로운 Actor추가
	Physics::AddActor(*m_pRigidActor);

	if (true == _bRigid)
	{
		PxRigidDynamic* pRigidDynamic = m_pRigidActor->is<PxRigidDynamic>();
		//LockFlag 설정
		for (UINT i = 0; i < 6; ++i)
		{
			pRigidDynamic->setRigidDynamicLockFlag((PxRigidDynamicLockFlag::Enum)pow(2, i), m_bLock[i]);
		}
		//중력 설정
		pRigidDynamic->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !m_bGravity);
	}
}

bool Collider::IsTrigger()
{
	return m_bTrigger;
}

void Collider::SetTrigger(const bool _bTrigger)
{
	if (_bTrigger == m_bTrigger)
		return;

	m_bTrigger = _bTrigger;

	if (m_bTrigger)
	{
		m_pShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
		m_pShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
	}
	else
	{
		m_pShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);
		m_pShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
	}
}

D3DXVECTOR3 Collider::GetCenter()
{
	return m_vCenter;
}

void Collider::SetCenter(const D3DXVECTOR3& _vCenter)
{
	if (_vCenter == m_vCenter)
		return;

	m_vCenter = _vCenter;

	physx::PxTransform localPose = m_pShape->getLocalPose();

	localPose.p.x = _vCenter.x;
	localPose.p.y = _vCenter.y;
	localPose.p.z = _vCenter.z;

	m_pShape->setLocalPose(localPose);
}

bool Collider::IsLock(const PxRigidDynamicLockFlag::Enum _eFlag)
{
	int nBitPos = 0;

	int nFlag = (int)_eFlag;

	while (nFlag > 0)
	{
		if ((nFlag & 1) == 1)
			break;

		++nBitPos;

		nFlag = nFlag >> 1;

	}

	return m_bLock[nBitPos];
}

void Collider::SetLockFlag(const PxRigidDynamicLockFlag::Enum _eFlag, const bool _bLock)
{
	int nBitPos = 0;

	int nFlag = (int)_eFlag;

	while (nFlag > 0)
	{
		if ((nFlag & 1) == 1)
			break;

		++nBitPos;

		nFlag = nFlag >> 1;

	}

	m_bLock[nBitPos] = _bLock;

	if (false == m_bRigid)
		return;

	m_pRigidActor->is<PxRigidDynamic>()->setRigidDynamicLockFlag(_eFlag, _bLock);
}

bool Collider::IsGravity()
{
	return m_bGravity;
}

void Collider::SetGravity(const bool _bActive)
{
	if (_bActive == m_bGravity)
		return;

	m_bGravity = _bActive;

	if (false == m_bRigid)
		return;

	m_pRigidActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !_bActive);
}
