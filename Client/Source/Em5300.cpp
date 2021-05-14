#include "stdafx.h"
#include "Em5300.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include "TestObject.h"
#include "Car.h"
#include "Nero.h"
#include <filesystem>
#include "Em5300Rush.h"
#include "Liquid.h"
#include "Em5300Missile.h"
#include "Em5300Rain.h"
#include "Em5300Homing.h"

void Em5300::Free()
{
	Unit::Free();
}

std::string Em5300::GetName()
{
	return "Em5300";
}

Em5300* Em5300::Create()
{
	return new Em5300{};
}

void Em5300::Fight(const float _fDeltaTime)
{
	Skill_CoolTime(_fDeltaTime);

	Vector3	 vDir = m_pPlayerTrans.lock()->GetPosition() - m_pTransform.lock()->GetPosition();
	Vector3	 vLook = m_pTransform.lock()->GetLook();
	float	 fDir = D3DXVec3Length(&vDir);


	D3DXVec3Normalize(&vDir, &vDir);
	//왼쪽 오른쪽 구분하기 위한 외적 결과 값
	Vector3  vCross;
	D3DXVec3Cross(&vCross, &vLook, &vDir);
	//앞 뒤 구분하기 위한 내적 결과 값
	float fDot = D3DXVec3Dot(&vDir, &vLook);
	if (fDot > 1.f)
		fDot = 1.f - FLT_EPSILON;
	else if (fDot < -1.f)
		fDot = -1.f + FLT_EPSILON;
	float fRadian = acosf(fDot);
	float fDegree = D3DXToDegree(fRadian);

	//거리가 멀때만 이동 or 회전을 함.
	//거리가 가까우면 공격으로 회전을 시킬 수 있음


	if (fDir <= 1.f)
	{
		if (m_bMove && m_bIng == false)
		{
			m_eState = Move_Back_Start;
			m_bIng = true;
		}
	}



	if (m_bRushAttack && m_bIng == false)
	{
		m_eState = Attack_Rush_Start;
		m_bIng = true;
	}
	if (m_bMissile && m_bIng == false)
	{
		m_eState = New_Missile_Start;
		m_bIng = true;
	}
	if (m_bRain && m_bIng == false)
	{
		m_eState = Attack_Rain_Start;
		m_bIng = true;
	}
	if (m_bMissile2 && m_bIng == false)
	{
		m_eState = Attack_Missile2_Start;
		m_bIng = true;
	}
	if (m_bHoming && m_bIng == false)
	{
		m_eState = Attack_Homing_Start;
		m_bIng = true;

	}

}

void Em5300::State_Change(const float _fDeltaTime)
{
	Vector3	 vDir = m_pPlayerTrans.lock()->GetPosition() - m_pTransform.lock()->GetPosition();
	float	 fDir = D3DXVec3Length(&vDir);
	Vector3	 vLook = m_pTransform.lock()->GetLook();
	float fDot = D3DXVec3Dot(&vDir, &vLook);
	if (fDot > 1.f)
		fDot = 1.f - FLT_EPSILON;
	else if (fDot < -1.f)
		fDot = -1.f + FLT_EPSILON;
	
	switch (m_eState)
	{
	case Em5300::Attack_Bug:
		break;
	case Em5300::Attack_Homing_End:
		if (m_bIng)
		{
			m_pMesh->PlayAnimation("Attack_Homing_End", false, {}, 1.f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Homing_End" && m_pMesh->IsAnimationEnd())
			{
				m_eState = Idle;
				m_bIng = false;
				m_bHoming = false;
			}
		}
		break;
	case Em5300::Attack_Homing_Loop:
		if (m_bIng)
		{
			Update_Angle();
			m_bInteraction = true;
			m_pMesh->PlayAnimation("Attack_Homing_Loop", false, {}, 1.f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Homing_Loop" && m_pMesh->IsAnimationEnd())
			{
				m_eState = Attack_Homing_End;

				for (int i = 0; i < 8; ++i)
				{
					m_pHoming[i].lock()->Set_Homing(true);
					m_pHoming[i].lock()->m_pCollider.lock()->SetActive(true);
					m_pHoming[i].lock()->Set_AttackType(Attack_Front);
					m_pHoming[i].lock()->Set_Coll(true);
				}
			}
		}
		break;
	case Em5300::Attack_Homing_Start:
		if (m_bIng)
		{
			Update_Angle();
			m_bInteraction = true;
			m_pMesh->PlayAnimation("Attack_Homing_Start", false, {}, 1.f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Homing_Start" && m_pMesh->IsAnimationEnd())
				m_eState = Attack_Homing_Loop;
		}
		break;
	case Em5300::Attack_Laser_End:
		break;
	case Em5300::Attack_Laser_Loop:
		break;
	case Em5300::Attack_Laser_Start:
		break;
	case Em5300::Attack_Missile2_Start:
		if (m_bIng)
		{
			Update_Angle();
			m_bInteraction = true;
			m_pMesh->PlayAnimation("Attack_Missile_Start", false, {}, 1.f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Missile_Start" && m_pMesh->IsAnimationEnd())
				m_eState = Attack_Missile2_Loop;
		}
		break;
	case Em5300::Attack_Missile2_Loop:
		if (m_bIng)
		{
			Update_Angle();
			m_bInteraction = true;
			m_pMesh->PlayAnimation("Attack_Missile_Loop", false, {}, 1.f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Missile_Loop" && m_pMesh->IsAnimationEnd())
				m_eState = Attack_Missile2_Attack;
		}
		break;
	case Em5300::Attack_Missile2_Attack:
		if (m_bIng)
		{
			m_pMesh->PlayAnimation("Attack_Missile2_Attack", false, {}, 0.4f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Missile2_Attack" && m_pMesh->IsAnimationEnd())
			{
				m_eState = Attack_Missile2_End;
				for (int i = 0; i < 32; ++i)
				{
					m_pBullet[i].lock()->Set_Missile2(true);
					m_pBullet[i].lock()->m_pCollider.lock()->SetActive(true);
					m_pBullet[i].lock()->Set_AttackType(Attack_Front);
					m_pBullet[i].lock()->Set_Coll(true);
				}
			}
		}
		break;
	case Em5300::Attack_Missile2_End:
		if (m_bIng)
		{
			m_pMesh->PlayAnimation("Attack_Missile_End", false, {}, 1.f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Missile_End" && m_pMesh->IsAnimationEnd())
			{
				m_eState = Idle;
				m_bIng = false;
				m_bMissile2 = false;
			}
		}
		break;
	case Em5300::New_Missile_End:
		if (m_bIng)
		{
			m_pMesh->PlayAnimation("New_Missile_End", false, {}, 0.6f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "New_Missile_End" && m_pMesh->IsAnimationEnd())
			{
				m_bIng = false;
				m_bMissile = false;
				m_eState = Idle;
				for (int i = 0; i < 32; ++i)
				{
					//m_pBullet[i].lock()->Set_Missile(false);
					//m_pBullet[i].lock()->Set_StartMissile(false);
					//m_pBullet[i].lock()->m_pCollider.lock()->SetActive(false);
				}
			}
		}
		break;
	case Em5300::New_Missile_Attack:
		if (m_bIng)
		{
			m_pMesh->PlayAnimation("New_Missile_Attack", false, {}, 0.8f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "New_Missile_Attack" && m_pMesh->IsAnimationEnd())
			{
				m_eState = New_Missile_End;
				for (int i = 0; i < 32; ++i)
				{
					m_pBullet[i].lock()->Set_Missile(true);
					m_pBullet[i].lock()->m_pCollider.lock()->SetActive(true);
					m_pBullet[i].lock()->Set_AttackType(Attack_Front);
					m_pBullet[i].lock()->Set_Coll(true);
				}
			}
		}
		break;
	case Em5300::New_Missile_Loop:
		if (m_bIng)
		{
			Update_Angle();
			m_bInteraction = true;
			m_pMesh->PlayAnimation("New_Missile_Loop", false, {}, 1.f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "New_Missile_Loop" && m_pMesh->IsAnimationEnd())
			{
				m_eState = New_Missile_Attack;
				for(int i = 0 ; i< 32 ; ++i)
					m_pBullet[i].lock()->m_pCollider.lock()->SetActive(false);
			}
		}
		break;
	case Em5300::New_Missile_Start:
		if (m_bIng)
		{
			Update_Angle();
			m_bInteraction = true;
			m_pMesh->PlayAnimation("New_Missile_Start", false, {}, 1.f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "New_Missile_Start" && m_pMesh->IsAnimationEnd())
				m_eState = New_Missile_Loop;
		}
		break;
	case Em5300::Attack_Rain_End:
		if (m_bIng)
		{
			m_pMesh->PlayAnimation("Attack_Rain_End", false, {}, 1.f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Rain_End" && m_pMesh->IsAnimationEnd())
			{
				m_eState = Idle;
				m_bRain = false;
				m_bIng = false;
			}
		}
		break;
	case Em5300::Attack_Rain_Loop:
		if (m_bIng)
		{
			Update_Angle();
			m_bInteraction = true;
			m_pMesh->PlayAnimation("Attack_Rain_Loop", false, {}, 1.f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Rain_Loop" && m_pMesh->IsAnimationEnd())
				m_eState = Attack_Rain_End;
		}
		break;
	case Em5300::Attack_Rain_Start:
		if (m_bIng)
		{
			Update_Angle();
			m_bInteraction = true;
			m_pMesh->PlayAnimation("Attack_Rain_Start", false, {}, 1.f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Rain_Start" && m_pMesh->IsAnimationEnd())
				m_eState = Attack_Rain_Loop;
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Rain_Start" && m_pMesh->PlayingTime() >= 0.85f)
			{
				for (int i = 0; i < 12; ++i)
				{
					m_pRain[i].lock()->Set_Rain(true);
					m_pRain[i].lock()->Set_Coll(true);
					m_pRain[i].lock()->m_pCollider.lock()->SetActive(true);
					m_pRain[i].lock()->Set_AttackType(Attack_Front);
				}
			}
		}
		break;
	case Em5300::Attack_Rush_Loop:
		if (m_bIng)
		{	
			if (m_bRushDir)
			{
				m_vRushDir =  m_pPlayerTrans.lock()->GetPosition() - m_pTransform.lock()->GetPosition();
				D3DXVec3Normalize(&m_vRushDir, &m_vRushDir);
				m_bRushDir = false;
				m_fRushTime = 0.f;
			}
			m_fRushTime += _fDeltaTime;
			m_pTransform.lock()->Translate(m_vRushDir * 0.08f);
			m_pMesh->PlayAnimation("Attack_Rush_Loop", true, {}, 1.f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Rush_Loop" && m_fRushTime >= 1.5f)
			{
				m_eState = Move_Front_End;
				m_fRushTime = 0.f;
				m_pRush.lock()->Set_Coll(false);
				m_pRush.lock()->SetActive(false);
			}
			else if (m_fRushTime >= 0.6 && m_pTransform.lock()->GetPosition().y <= 0.5f)
			{
				m_pTransform.lock()->Translate({ 0.f, 0.01f, 0.f });
				m_pRush.lock()->Set_Coll(true);
				m_pRush.lock()->SetActive(true);
				m_pRush.lock()->Set_AttackType(Attack_KnocBack);
			}
		}
		break;
	case Em5300::Attack_Rush_Start:
		if (m_bIng)
		{
			m_pMesh->PlayAnimation("Attack_Rush_Start", false, {}, 1.f, 20.f, true);
			Update_Angle();
			m_bInteraction = true;
			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Rush_Start" && m_pMesh->IsAnimationEnd())
				m_eState = Attack_Rush_Loop;
		}
		break;
	case Em5300::Hit_Buster_Start:
		break;
	case Em5300::Hit_Buster_Stab:
		break;
	case Em5300::Hit_Buster_Stab_Loop:
		break;
	case Em5300::Hit_Buster_Stab_Throw:
		break;
	case Em5300::Hit_Buster_Stab_End:
		break;
	case Em5300::Dead:
		break;
	case Em5300::Down_Dead:
		break;
	case Em5300::Down_Loop:
		break;
	case Em5300::Hit_Falling_End:
		break;
	case Em5300::Hit_Falling_Loop:
		break;
	case Em5300::Hit_Falling_Start:
		break;
	case Em5300::Idle:
		m_pMesh->PlayAnimation("Idle", true, {}, 1.f, 20.f, true);
		m_BattleInfo.eAttackType = Attack_END;

		if (m_pTransform.lock()->GetPosition().y <= 0.5f)
			m_pTransform.lock()->Translate({ 0.f, 0.01f, 0.f });
		m_bHit = false;
		break;
	case Em5300::Move_Back_End:
		if (m_bIng)
		{
			m_pMesh->PlayAnimation("Move_Back_End", false, {}, 1.f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Move_Back_End" && m_pMesh->IsAnimationEnd())
			{
				m_eState = Idle;
				m_bIng = false;
				m_bMove = false;
			}
		}
		break;
	case Em5300::Move_Back_Loop:
		if (m_bIng)
		{
			D3DXVec3Normalize(&vDir, &vDir);
			m_pTransform.lock()->Translate({ -vDir.x * 0.02f, 0.f, -vDir.z * 0.02f });
			m_pMesh->PlayAnimation("Move_Back_Loop", false, {}, 1.f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Move_Back_Loop" && m_pMesh->IsAnimationEnd())
				m_eState = Move_Back_End;
		}
		break;
	case Em5300::Move_Back_Start:
		if (m_bIng)
		{
			Update_Angle();
			Set_Rotate();
			D3DXVec3Normalize(&vDir, &vDir);
			m_pTransform.lock()->Translate({-vDir.x * 0.02f, 0.f, -vDir.z * 0.02f});

			m_pMesh->PlayAnimation("Move_Back_Start", false, {}, 1.f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Move_Back_Start" && m_pMesh->IsAnimationEnd())
				m_eState = Move_Back_Loop;
		}
		break;
	case Em5300::Move_Front_End:
		if (m_bIng)
		{
			m_pMesh->PlayAnimation("Move_Front_End", false, {}, 1.f, 20.f, true);

			if (m_pTransform.lock()->GetPosition().y <= 0.5f)
				m_pTransform.lock()->Translate({ 0.f, 0.01f, 0.f });

			if (m_pMesh->CurPlayAnimInfo.Name == "Move_Front_End" && m_pMesh->IsAnimationEnd())
			{
				m_eState = Idle;	
				m_bRushAttack = false;
				m_bIng = false;
			}

		}
		break;
	case Em5300::Move_Front_Loop:
		break;
	case Em5300::Move_Front_Start:
		break;
	case Em5300::State_END:
		break;
	default:
		break;
	}
}

void Em5300::Skill_CoolTime(const float _fDeltaTime)
{
	if (m_bMove == false)
	{
		m_fMoveTime += _fDeltaTime;
		if (m_fMoveTime >= 5.f)
		{
			m_bMove = true;
			m_fMoveTime = 0.f;
		}
	}
	//if (m_bRushAttack == false)
	//{
	//	m_fRushAttackTime += _fDeltaTime;
	//	if (m_fRushAttackTime >= 5.f)
	//	{
	//		m_bRushAttack = true;
	//		m_bRushDir = true;
	//		m_fRushAttackTime = 0.f;
	//	}
	//}
	//퍼지는 미사일
	/*if (m_bMissile == false)
	{
		m_fMissileTime += _fDeltaTime;
		if (m_fMissileTime >= 1.f)
		{
			m_bMissile = true;
			m_fMissileTime = 0.f;
		}
	}*/
	
	// 위에서 떨어지는거
	//if (m_bRain == false)
	//{
	//	m_fRainTime += _fDeltaTime;
	//	if (m_fRainTime >= 1.f)
	//	{
	//		m_bRain = true;
	//		m_fRainTime = 0.f;
	//	}
	//}
	// 
	//세로 미사일
	/*if (m_bMissile2 == false)
	{
		m_fMissile2Time += _fDeltaTime;
		if (m_fMissile2Time >= 1.f)
		{
			m_bMissile2 = true;
			m_fMissile2Time = 0.f;
		}
	}*/

	//공중에 떠다니는 미사일
	//if (m_bHoming == false)
	//{
	//	m_fHomingTime += _fDeltaTime;
	//	if (m_fHomingTime >= 1.f)
	//	{
	//		m_bHoming = true;
	//		m_fHomingTime = 0.f;
	//	}
	//}
}

HRESULT Em5300::Ready()
{
	Unit::Ready();
	//GameObject를 받아오려면 각자 태그가 있어야함.
	m_nTag = Monster5300;

	RenderInit();

	m_BattleInfo.iMaxHp = 3000;
	m_BattleInfo.iHp = 3000;


	// 트랜스폼 초기화하며 Edit 에 정보가 표시되도록 푸시 . 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.001,0.001,0.001 });
	PushEditEntity(InitTransform.lock().get());

	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.
	// 	PushEditEntity(_ShaderInfo.GetShader(RenderProperty::Order::ForwardAlphaBlend).get());


	//몬스터 회전 기본 속도
	m_fAngleSpeed = D3DXToRadian(100.f);

	m_pTransform.lock()->SetPosition({ -0.2f, 0.5f, -0.5f });

	m_pMesh->EnableToRootMatricies();

	m_fPower = 100.f;
	m_vPower = D3DXVECTOR3(0.f, m_fTest, 0.f);

	return S_OK;
}

HRESULT Em5300::Awake()
{
	Unit::Awake();
	m_pPlayer = std::static_pointer_cast<Nero>(FindGameObjectWithTag(Player).lock());
	m_pPlayerTrans = m_pPlayer.lock()->GetComponent<ENGINE::Transform>();
	//
	//m_pCar = std::static_pointer_cast<Car>(FindGameObjectWithTag(ThrowCar).lock());
	//m_pCarTrans = m_pCar.lock()->GetComponent<ENGINE::Transform>();
	m_pCollider = AddComponent<CapsuleCollider>();
	m_pCollider.lock()->ReadyCollider();
	PushEditEntity(m_pCollider.lock().get());

	m_pRush	 = AddGameObject<Em5300Rush>();
	m_pRush.lock()->m_pEm5300 = static_pointer_cast<Em5300>(m_pGameObject.lock());
	m_pRush.lock()->m_pEm5300Mesh = m_pMesh;
	
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);

	m_pCollider.lock()->SetRigid(true);
	m_pCollider.lock()->SetGravity(false);
	
	m_pCollider.lock()->SetRadius(0.15f);
	m_pCollider.lock()->SetHeight(0.15f);
	m_pCollider.lock()->SetCenter({ 0.f,0.f,0.f });

	m_pBlood = AddGameObject<Liquid>();
	
	for (int i = 0; i < 32; ++i)
	{
		m_pBullet[i] = AddGameObject<Em5300Missile>();
		m_pBullet[i].lock()->m_pEm5300 = static_pointer_cast<Em5300>(m_pGameObject.lock());
		m_pBullet[i].lock()->m_pEm5300Mesh = m_pMesh;
		m_pBullet[i].lock()->Set_MissilePos(i);
	}

	for (int i = 0; i < 12; ++i)
	{
		m_pRain[i] = AddGameObject<Em5300Rain>();
		m_pRain[i].lock()->m_pEm5300 = static_pointer_cast<Em5300>(m_pGameObject.lock());
		m_pRain[i].lock()->m_pEm5300Mesh = m_pMesh;
		m_pRain[i].lock()->Set_RainPos(i);
	}
	for (int i = 0; i < 8; ++i)
	{
		m_pHoming[i] = AddGameObject<Em5300Homing>();
		m_pHoming[i].lock()->m_pEm5300 = static_pointer_cast<Em5300>(m_pGameObject.lock());
		m_pHoming[i].lock()->m_pEm5300Mesh = m_pMesh;
		m_pHoming[i].lock()->Set_HomingPos(i);
	}

	m_eState = Idle;
	return S_OK;
}

HRESULT Em5300::Start()
{
	Unit::Start();
	return S_OK;
}

UINT Em5300::Update(const float _fDeltaTime)
{
	Unit::Update(_fDeltaTime);
	// 현재 스케일과 회전은 의미가 없음 DeltaPos 로 트랜스폼에서 통제 . 
	auto [DeltaScale, DeltaQuat, DeltaPos] = m_pMesh->Update(_fDeltaTime);
	Vector3 Axis = { 1,0,0 };

	//ENGINE::AnimNotify _Notify{};
	////return true 면 이제 호출 안함, false면 저 루프 돌떄 계속 호출.
	//_Notify.Event[0.5] = [this]() {  AttackStart();  return false; };

	const float Length = FMath::Length(DeltaPos);

	//DeltaPos = FMath::RotationVecNormal(DeltaPos, Axis, FMath::ToRadian(90.f)) * Length;
	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{			 
		 D3DXQUATERNION tResult = SpTransform->GetQuaternion() * DeltaQuat;
		 SpTransform->SetQuaternion(tResult);

		 D3DXMATRIX matRot;
		 D3DXQUATERNION tQuat = m_pTransform.lock()->GetQuaternion();
		 D3DXMatrixRotationQuaternion(&matRot, &tQuat);
		 D3DXVec3TransformNormal(&DeltaPos, &DeltaPos, &matRot);


		 SpTransform->SetPosition(SpTransform->GetPosition() + DeltaPos * SpTransform->GetScale().x);
	}
	//플레이어가 사라졌는지 판단
	/*if (false == m_pPlayer.expired())
	{
		std::cout << "Player Dead" << std::endl;
	}*/
	Rotate(_fDeltaTime);

	if (Input::GetKeyDown(DIK_T))
	{
		if (m_bFight)
			m_bFight = false;
		else
			m_bFight = true;
	}

	if (m_bFight)
	{
		Fight(_fDeltaTime);
		State_Change(_fDeltaTime);
	}

	//if (Input::GetKeyDown(DIK_Y))
	//{
	//	for (int i = 0; i < 12; ++i)
	//	{
	//		m_pRain[i].lock()->Set_Rain(false);
	//		m_pRain[i].lock()->Set_StartRain(false);
	//	}
	//}
	if (Input::GetKeyDown(DIK_Y))
	{
		for (int i = 0; i < 32; ++i)
		{
			m_pBullet[i].lock()->Set_Missile2(false);
			m_pBullet[i].lock()->Set_StartMissile2(false);
		}
	}
	return 0;
}

UINT Em5300::LateUpdate(const float _fDeltaTime)
{
	Unit::LateUpdate(_fDeltaTime);
	return 0;
}

void Em5300::Editor()
{
	Unit::Editor();
	if (false == bEdit)
		return;
}

void Em5300::OnEnable()
{
	Unit::OnEnable();
}

void Em5300::OnDisable()
{
	Unit::OnDisable();
}

void Em5300::Hit(BT_INFO _BattleInfo, void* pArg)
{
	if (!m_pBlood.expired())
	{
		int iRandom = FMath::Random<int>(0, 6);
		if (iRandom >= 4)
			++iRandom;

		auto pBlood = m_pBlood.lock();
		pBlood->SetVariationIdx(Liquid::VARIATION(iRandom));	// 0 6 7 이 자연스러운듯?
		pBlood->SetPosition(GetMonsterBoneWorldPos("Waist"));
		pBlood->SetScale(0.008f);
		//pBlood->SetRotation()	// 상황에 맞게 각도 조절
		pBlood->PlayStart(40.f);
	}
}

void Em5300::Buster(BT_INFO _BattleInfo, void* pArg)
{
}

void Em5300::Snatch(BT_INFO _BattleInfo, void* pArg)
{
}


void Em5300::RenderGBufferSK(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = m_pMesh->GetNumSubset();
	if (Numsubset > 0)
	{
		m_pMesh->BindVTF(_Info.Fx);
	};
	for (uint32 i = 0; i < Numsubset; ++i)
	{

		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->BindProperty(TextureType::DIFFUSE, 0, 0, _Info._Device);
			SpSubset->BindProperty(TextureType::NORMALS, 0, 1, _Info._Device);
			SpSubset->Render(_Info.Fx);
		};
	};
}

void Em5300::RenderShadowSK(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = m_pMesh->GetNumSubset();
	if (Numsubset > 0)
	{
		m_pMesh->BindVTF(_Info.Fx);
	};
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
}

void Em5300::RenderDebugBone(const DrawInfo& _Info)
{
	const Matrix ScaleOffset = FMath::Scale({ 0.01,0.01 ,0.01 });
	m_pMesh->BoneDebugRender(_RenderUpdateInfo.World, _Info.Fx);
}

void Em5300::RenderDebugSK(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);
	const uint32 Numsubset = m_pMesh->GetNumSubset();

	if (Numsubset > 0)
	{
		m_pMesh->BindVTF(_Info.Fx);
	};
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
}

void Em5300::RenderInit()
{
	// 렌더를 수행해야하는 오브젝트라고 (렌더러에 등록 가능 ) 알림.
	// 렌더 인터페이스 상속받지 않았다면 키지마세요.
	SetRenderEnable(true);

	// 렌더 속성 전체 초기화 
	ENGINE::RenderProperty _InitRenderProp;
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	_InitRenderProp.bRender = true;
	// 넘겨준 패스에서는 렌더링 호출 보장 . 
	_InitRenderProp.bRender = true;
	_InitRenderProp.RenderOrders[RenderProperty::Order::GBuffer] =
	{
		{"gbuffer_dsSK",
		[this](const DrawInfo& _Info)
			{
				RenderGBufferSK(_Info);
			}
		},
	};
	_InitRenderProp.RenderOrders[RenderProperty::Order::Shadow]
		=
	{
		{"ShadowSK" ,
		[this](const DrawInfo& _Info)
		{
			RenderShadowSK(_Info);
		}
	} };
	_InitRenderProp.RenderOrders[RenderProperty::Order::DebugBone]
		=
	{
		{"DebugBone" ,
		[this](const DrawInfo& _Info)
		{
			RenderDebugBone(_Info);
		}
	} };
	_InitRenderProp.RenderOrders[RenderProperty::Order::Debug]
		=
	{
		{"DebugSK" ,
		[this](const DrawInfo& _Info)
		{
			RenderDebugSK(_Info);
		}
	} };

	//
	_InitRenderProp.RenderOrders[RenderProperty::Order::Collider]
		=
	{
		{"Collider" ,
		[this](const DrawInfo& _Info)
		{
			DrawCollider(_Info);
		}
	} };
	RenderInterface::Initialize(_InitRenderProp);

	// 스켈레톤 메쉬 로딩 ... 
	Mesh::InitializeInfo _InitInfo{};
	// 버텍스 정점 정보가 CPU 에서도 필요 한가 ? 
	_InitInfo.bLocalVertexLocationsStorage = false;
	m_pMesh = Resources::Load<ENGINE::SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Monster\\Em5300\\Em5300.x", _InitInfo);

	m_pMesh->EnableToRootMatricies();
	// 모션 블러 사용시 켜기 
	// m_pMesh->EnablePrevVTF();
	PushEditEntity(m_pMesh.get());
	//몬스터 초기상태 idle
	m_pMesh->PlayAnimation("Idle", true);

	
}

void Em5300::Rotate(const float _fDeltaTime)
{
	if (m_bInteraction == false)
		return;

	if (fabs(m_fRadian) <= fabs(m_fAccuangle))
	{
		float fAdd = m_fRadian - m_fAccuangle;

		m_pTransform.lock()->Rotate({ 0.f, -D3DXToDegree(fAdd), 0.f });

		m_bInteraction = false;

		return;
	}
	m_pTransform.lock()->Rotate({ 0.f, -D3DXToDegree(m_fAngleSpeed * _fDeltaTime), 0.f });

	m_fAccuangle += m_fAngleSpeed * _fDeltaTime;
}

void Em5300::Update_Angle()
{
	Vector3 vPlayerPos = m_pPlayerTrans.lock()->GetPosition();
	Vector3 vMyPos = m_pTransform.lock()->GetPosition();

	Vector3 vDir = vPlayerPos - vMyPos;
	vDir.y = 0;
	D3DXVec3Normalize(&vDir, &vDir);

	Vector3 vLook = -m_pTransform.lock()->GetLook();

	float fDot = D3DXVec3Dot(&vDir, &vLook);
	if (fDot > 1.f)
		fDot = 1.f - FLT_EPSILON;
	else if (fDot < -1.f)
		fDot = -1.f + FLT_EPSILON;
	float fRadian = acosf(fDot);

	Vector3	vCross;
	D3DXVec3Cross(&vCross, &vLook, &vDir);

	if (vCross.y > 0)
		fRadian *= -1;

	m_fRadian = fRadian;
	m_fAccuangle = 0.f;

	if (D3DXToDegree(m_fRadian) > -2.f && D3DXToDegree(m_fRadian) < 2.f)
		m_fRadian = 0.f;

	if (m_fRadian > 0)
		m_fAngleSpeed = fabs(m_fAngleSpeed);
	else
		m_fAngleSpeed = -fabs(m_fAngleSpeed);
}

void Em5300::Set_Rotate()
{
	m_pTransform.lock()->Rotate({ 0.f, -D3DXToDegree(m_fRadian), 0.f });
}

void Em5300::OnCollisionEnter(std::weak_ptr<GameObject> _pOther)
{
	Monster::OnCollisionEnter(_pOther);
}

void Em5300::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
	if (!m_bCollEnable)
		return;
	if (m_eState == Dead)
		return;

	m_bCollEnable = false;
	switch (_pOther.lock()->m_nTag)
	{
	case GAMEOBJECTTAG::TAG_RedQueen:
		Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		m_pRush.lock()->m_pCollider.lock()->SetActive(false);
		break;
	case GAMEOBJECTTAG::TAG_BusterArm_Right:
		//_pOther.lock()->GetComponent<SphereCollider>().lock()->SetActive(false);
		//Buster(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		break;
	case GAMEOBJECTTAG::Overture:
		m_BattleInfo.iHp -= static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo().iAttack;
		m_bHit = true;
		break;
	case GAMEOBJECTTAG::Tag_Cbs_Long:
		Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		m_pRush.lock()->m_pCollider.lock()->SetActive(false);
		break;
	case GAMEOBJECTTAG::Tag_Cbs_Middle:
		Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		m_pRush.lock()->m_pCollider.lock()->SetActive(false);
		break;
	case GAMEOBJECTTAG::Tag_Cbs_Short:
		Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		m_pRush.lock()->m_pCollider.lock()->SetActive(false);
		break;
	default:
		break;
	}
}

