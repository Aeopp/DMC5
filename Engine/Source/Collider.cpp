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
	, m_bGravity(false)
{
}

void Collider::Free()
{
	//Scene에서 Actor 제거
	Physics::RemoveActor(m_pGameObject.lock()->GetSceneID(), *m_pRigidActor);
	//
	if (nullptr != m_pRigidActor)
		m_pRigidActor->userData = nullptr;
	//
	PX_RELEASE(m_pRigidActor);
	PX_RELEASE(m_pShape);
	Component::Free();
}

HRESULT Collider::ReadyCollider()
{
	CreateRigidActor();

	//Scene에 Actor 추가.
	Physics::AddActor(m_pGameObject.lock()->GetSceneID(), *m_pRigidActor);
	return S_OK;
}

HRESULT Collider::DrawCollider(const DrawInfo& _Info)
{
	return S_OK;
}

void Collider::Editor()
{
	bool bRigid = m_bRigid;
	if (ImGui::Checkbox("Rigid", &bRigid))
		SetRigid(bRigid);

	bool bGravity = m_bGravity;
	if (ImGui::Checkbox("Gravity", &bGravity))
		SetGravity(bGravity);

	ImGui::Text("Freeze");
	ImGui::Text("Position");
	ImGui::SameLine();

	bool bFreezeX = IsLock(PxRigidDynamicLockFlag::eLOCK_LINEAR_X);
	if (ImGui::Checkbox("X##PosX", &bFreezeX))
		SetLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, bFreezeX);
	
	ImGui::SameLine();
	bool bFreezeY = IsLock(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y);
	if (ImGui::Checkbox("Y##PosX", &bFreezeY))
		SetLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, bFreezeY);

	ImGui::SameLine();
	bool bFreezeZ = IsLock(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z);
	if (ImGui::Checkbox("Z##PosX", &bFreezeZ))
		SetLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, bFreezeZ);

	ImGui::Text("Rotation");
	ImGui::SameLine();

	bool bFreezeRotX = IsLock(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X);
	if (ImGui::Checkbox("X##RotX", &bFreezeRotX))
		SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, bFreezeRotX);

	ImGui::SameLine();
	bool bFreezeRotY = IsLock(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y);
	if (ImGui::Checkbox("Y##RotY", &bFreezeRotY))
		SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, bFreezeRotY);

	ImGui::SameLine();
	bool bFreezeRotZ = IsLock(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z);
	if (ImGui::Checkbox("Z##RotZ", &bFreezeRotZ))
		SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, bFreezeRotZ);

	D3DXVECTOR3 vCenter = m_vCenter;
	if (ImGui::InputFloat3("Center", vCenter))
		SetCenter(vCenter);
}

void Collider::ReadySimulate()
{
	std::weak_ptr<Transform> pTransform = m_pGameObject.lock()->GetComponent<Transform>();
	//Transform에 변화가 있었는지 확인
	if (true == pTransform.expired())
		return;
	if (false == pTransform.lock()->IsUpdated())
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

	//Collider가 부착된 게임오브젝트의 Transform으로 Actor위치 설정.
	std::weak_ptr<Transform> pTransform = m_pGameObject.lock()->GetComponent<Transform>();

	D3DXVECTOR3		vPosition = pTransform.lock()->GetPosition();
	D3DXQUATERNION	tQuaternion = pTransform.lock()->GetQuaternion();

	PxVec3 pxPos;
	PxQuat pxQuat;

	memcpy_s(&pxPos, sizeof(D3DXVECTOR3), &vPosition, sizeof(D3DXVECTOR3));
	memcpy_s(&pxQuat, sizeof(D3DXQUATERNION), &tQuaternion, sizeof(D3DXQUATERNION));

	//RigidActor 생성
	if (m_bRigid)
		m_pRigidActor = Physics::GetPxPhysics()->createRigidDynamic(physx::PxTransform(pxPos, pxQuat));
	else
		m_pRigidActor = Physics::GetPxPhysics()->createRigidStatic(physx::PxTransform(pxPos, pxQuat));

	//UserData 설정.
	//Collider 컴포넌트가 해제되도 userData가 사라지면 안됨 -> FetchResult에서 nullptr 접근으로 터짐.
	//=> Component 해제시 리지드 액터의 userdata를 nullptr로 변경 해서 확인.
	m_UserData.pCollider = std::static_pointer_cast<Collider>(m_pThis);

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
	Physics::RemoveActor(m_pGameObject.lock()->GetSceneID(), *m_pRigidActor);;

	//RigidActor 생성
	CreateRigidActor();

	//Scene에 새로운 Actor추가
	Physics::AddActor(m_pGameObject.lock()->GetSceneID(), *m_pRigidActor);;

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

	if (true == m_bRigid && false == _bActive)
		m_pRigidActor->is<PxRigidDynamic>()->clearForce();

	m_pRigidActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !_bActive);
}

bool Collider::IsGround()
{
	if (false == m_bRigid)
		return;

	PxVec3 vVelocity = m_pRigidActor->is<PxRigidDynamic>()->getLinearVelocity();

	if (fabs(vVelocity.y) > 0.0001)
		return false;

	return true;
}

void Collider::AddForce(const D3DXVECTOR3 _vForce)
{
	if (false == m_bRigid)
		return;

	PxVec3 vForce(_vForce.x, _vForce.y, _vForce.z);

	//m_pRigidActor->is<PxRigidDynamic>()->setLinearVelocity(vForce);
	m_pRigidActor->is<PxRigidDynamic>()->addForce(vForce);
}
