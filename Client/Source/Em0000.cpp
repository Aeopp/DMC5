#include "stdafx.h"
#include "Em0000.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <filesystem>
#include "Em0000_Weapon.h"
#include "Nero.h"
#include "RedQueen.h"
#include "Liquid.h"
#include "AppearGroundMonster.h"

void Em0000::Free()
{
	Destroy(m_pBlood);
	Destroy(m_pAppear);

	Monster::Free();
}

std::string Em0000::GetName()
{
	return "Em0000";
}

Em0000* Em0000::Create()
{
	return new Em0000{};
}




void Em0000::Fight(const float _fDeltaTime)
{
	Vector3	 vDir = m_pPlayerTrans.lock()->GetPosition() - m_pTransform.lock()->GetPosition();
	float	 fDir = D3DXVec3Length(&vDir);
	
	/////이놈은 움직임이 한방향 밖에 없어서. 앞으로 갈지 아니면 백스탭 이후에 강한 공격할지
	//Move가 On일때 1/4 확률로 백스텝을 하고 백스텝을 한 이후에는 바로 달려가면서 강한 공격

	if (m_BattleInfo.iHp <= 0.f && m_bAir == false)
	{
		m_eState = Dead;
		m_bIng = true;
	}


	if (fDir >= 0.6f)
	{
		int iRandom = FMath::Random<int>(1, 4);

		if (m_bMove && m_bIng == false)
		{
			m_bIng = true;
			m_bInteraction = true;
			Update_Angle();
			///////////////////////
			if (iRandom == 1)
				m_eState = Step_Back;
			else
				m_eState = Move_Front_Start;
			//////////////////////
		}
	}
	else
	{
		if (m_bAttack && m_bIng == false)
		{
			int iRandom = FMath::Random<int>(1, 3);
			m_bIng = true;
			if (iRandom == 1)
			{
				m_eState = Attack_1;
				m_pWeapon.lock()->Set_Coll(true);
			}
			else if (iRandom == 2)
			{
				m_eState = Attack_2;
				m_pWeapon.lock()->Set_Coll(true);
			}
			else
				m_eState = Step_Back;
		}
	}

}

void Em0000::State_Change(const float _fDeltaTime)
{
	Vector3	 vDir = m_pPlayerTrans.lock()->GetPosition() - m_pTransform.lock()->GetPosition();
	float	 fDir = D3DXVec3Length(&vDir);

	D3DXVec3Normalize(&vDir, &vDir);
	Vector3	 vLook = m_pTransform.lock()->GetLook();
	float    fDot = D3DXVec3Dot(&vDir, &vLook);


	switch (m_eState)
	{
	case Em0000::Attack_1:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Attack_1", false, {}, 1.f, 50.f, true);

			Update_Angle();
			m_bInteraction = true;

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_1" && m_pMesh->PlayingTime() >= 0.9f)
			{
				m_eState = idle;
				m_bIng = false;
				m_bAttack = false;
				m_pWeapon.lock()->Set_Coll(false);
				m_pWeapon.lock()->m_pCollider.lock()->SetActive(false);
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_1" && m_pMesh->PlayingTime() >= 0.5f)
				m_pWeapon.lock()->m_pCollider.lock()->SetActive(false);
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_1" && m_pMesh->PlayingTime() >= 0.2f)
			{
				m_pWeapon.lock()->m_pCollider.lock()->SetActive(true);
				m_pWeapon.lock()->Set_AttackType(Attack_Front);
			}
		}
		break;
	case Em0000::Attack_2:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Attack_2", false, {}, 1.f, 50.f, true);
			Update_Angle();
			m_bInteraction = true;

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_2" && m_pMesh->PlayingTime() >= 0.9f)
			{
				m_eState = idle;
				m_bIng = false;
				m_bAttack = false;
				m_pWeapon.lock()->Set_Coll(false);
				m_pWeapon.lock()->m_pCollider.lock()->SetActive(false);
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_2" && m_pMesh->PlayingTime() >= 0.5f)
				m_pWeapon.lock()->m_pCollider.lock()->SetActive(false);
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_2" && m_pMesh->PlayingTime() >= 0.2f)
			{
				m_pWeapon.lock()->m_pCollider.lock()->SetActive(true);
				m_pWeapon.lock()->Set_AttackType(Attack_Front);
			}

		}
		break;
	case Em0000::Attack_Hard:
		if (m_bIng == true)
		{
			//이건 마지막 피니시 모션까지 계속 플레이어를 향해서 뜀.
			m_bInteraction = true;
			Update_Angle();
			m_pMesh->PlayAnimation("Attack_Hard", false, {}, 1.f, 50.f, true);


			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Hard" && m_pMesh->PlayingTime() >= 0.9f)
			{
				m_eState = idle;
				m_bIng = false;
				m_bAttack = false;
				m_pWeapon.lock()->Set_Coll(false);
				m_pWeapon.lock()->m_pCollider.lock()->SetActive(false);
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Hard" && m_pMesh->PlayingTime() >= 0.35f)
			{
				m_pWeapon.lock()->m_pCollider.lock()->SetActive(false);
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Hard" && m_pMesh->PlayingTime() >= 0.2f)
			{
				m_pWeapon.lock()->m_pCollider.lock()->SetActive(true);
				m_pWeapon.lock()->Set_AttackType(Attack_KnocBack);
			}
		}
		break;
	case Em0000::Dead:
		if (m_bIng == true)
			m_pMesh->PlayAnimation("Death_Front", false, {}, 1.f, 20.f, true);
		StoneDebrisPlayStart();
		break;
		break;
	case Em0000::Guard_End:
		break;
	case Em0000::Guard_Hit1:
		break;
	case Em0000::Guard_Hit2:
		break;
	case Em0000::Guard_Loop:
		break;
	case Em0000::Guard_Start:
		break;
	case Em0000::Hit_Back:
		break;
	case Em0000::Hit_Front:
		if (m_bHit)
		{
			m_pMesh->PlayAnimation("Hit_Front", false, {}, 0.8f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Hit_Front" && m_pMesh->IsAnimationEnd())
			{
				m_eState = idle;
				m_bHit = false;
				m_bIng = false;
			}
		}
		break;
	case Em0000::Hit_L:
		if (m_bHit)
		{
			m_pMesh->PlayAnimation("Hit_L", false, {}, 0.8f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Hit_L" && m_pMesh->IsAnimationEnd())
			{
				m_eState = idle;
				m_bHit = false;
				m_bIng = false;
			}
		}
		break;
	case Em0000::Hit_R:
		if (m_bHit)
		{
			m_pMesh->PlayAnimation("Hit_R", false, {}, 0.8f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Hit_R" && m_pMesh->IsAnimationEnd())
			{
				m_eState = idle;
				m_bHit = false;
				m_bIng = false;
			}
		}
		break;
	case Em0000::Hit_KnocBack:
		if (m_bHit == true)
		{
			m_pMesh->PlayAnimation("Hit_Air", false, {}, 1.f, 20.f, true);
			if (m_pCollider.lock()->IsGround() && m_pMesh->CurPlayAnimInfo.Name == "Hit_Air")
			{
				if (fDot < 0)
					m_eState = Hit_End_Front;
				else
					m_eState = Hit_End_Back;

				m_bIng = false;
				m_bDown = true;
				m_bAir = false;
			}
		}
		break;
	case Em0000::Hit_End_Front:
		if (m_bHit == true)
		{
			m_pMesh->PlayAnimation("Blown_Back_Landing", false, {}, 1.f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Blown_Back_Landing" && m_pMesh->IsAnimationEnd())
				m_eState = Prone_Getup;
		}
		break;
	case Em0000::Hit_End_Back:
		if (m_bHit == true)
		{
			m_pMesh->PlayAnimation("Blown_Front_Landing", false, {}, 1.f, 20.f, true);
			m_pCollider.lock()->SetTrigger(false);
			m_pCollider.lock()->SetRigid(true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Blown_Front_Landing" && m_pMesh->IsAnimationEnd())
				m_eState = Prone_Getup;
		}
		break;
	case Em0000::Hit_Air:
		if (m_bHit && m_bAir == true)
		{
			m_pMesh->PlayAnimation("Hit_Air", false, {}, 1.f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Hit_Air" && m_pMesh->PlayingTime() >= 0.9f)
			{
				m_eState = Hit_Air_Loop;
				m_pCollider.lock()->SetGravity(true);
			}
		}
		break;
		break;
	case Em0000::Prone_Getup:
		if (m_bDown == true)
		{
			m_pMesh->PlayAnimation("Prone_Getup", false, {}, 0.8f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Prone_Getup" && m_pMesh->IsAnimationEnd())
			{
				//쳐맞아서 넘어진다음에 일어나는 모션이 끝남
				//그렇다는건 이제 맞지도않고, 다음 행동 해도 되고(m_bing == false), 넘어진 것도 끝났음.
				m_bHit = false;
				m_bIng = false;
				m_bDown = false;
				m_eState = idle;
			}
		}
		break;
	case Em0000::Downword_Damage:
		if (m_bDown && m_bHit == true)
		{
			m_pMesh->PlayAnimation("Downword_Damage", false, {}, 1.f, 10.f);
			m_bHit = false;
		}
		if (m_pMesh->CurPlayAnimInfo.Name == "Downword_Damage" && m_pMesh->IsAnimationEnd())
			m_eState = Prone_Getup;
		break;
	case Em0000::Hit_Air_Start:
		if (m_bHit == true)
		{
			m_bAir = true;
			m_pMesh->PlayAnimation("Blown_Back", false, {}, 1.f, 20.f, true);

			m_pCollider.lock()->AddForce({ 0.f, 1.8f,0.f });

			if (m_pMesh->CurPlayAnimInfo.Name == "Blown_Back" &&
				m_pMesh->PlayingTime() >= 0.1f && m_pCollider.lock()->IsGround())
			{
				m_eState = Hit_Air_Loop;
				m_pCollider.lock()->SetGravity(true);
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Blown_Back" && m_pMesh->PlayingTime() >= 0.8f)
			{
				m_eState = Hit_Air_Loop;
				m_pCollider.lock()->SetGravity(true);
			}
		}
		break;
	case Em0000::Hit_Air_Snatch_Start:
		if (m_bHit == true)
		{
			m_bAir = true;
			m_pMesh->PlayAnimation("Blown_Back", false, {}, 1.f, 20.f, true);


			if (m_pMesh->CurPlayAnimInfo.Name == "Blown_Back" &&
				m_pMesh->PlayingTime() >= 0.1f && m_pCollider.lock()->IsGround())
			{
				m_eState = Hit_Air_Loop;
				m_pCollider.lock()->SetGravity(true);
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Blown_Back" && m_pMesh->PlayingTime() >= 0.8f)
			{
				m_eState = Hit_Air_Loop;
				m_pCollider.lock()->SetGravity(true);
			}
		}
		break;
	case Em0000::Hit_Air_Loop:
		if (m_bHit == true)
		{
			m_pMesh->PlayAnimation("Blown_Back_Loop", true, {}, 1.f, 20.f, true);

			m_pCollider.lock()->AddForce({ 0.f, 1.8f,0.f });

			if (m_pMesh->CurPlayAnimInfo.Name == "Blown_Back_Loop" && m_pCollider.lock()->IsGround())
			{
				if (fDot < 0)
					m_eState = Hit_End_Front;
				else
					m_eState = Hit_End_Back;
				m_bDown = true;
				m_bAir = false;
			}
		}
		break;
	case Em0000::Hit_Split_Start:
		if (m_bHit && m_bAir == true)
		{
			m_pMesh->PlayAnimation("Slam_Damage_Loop", true, {}, 1.f, 20.f, true);

			if (m_pPlayer.lock()->Get_CurAnimationIndex() == Nero::ANI_SPLIT_LANDING
				&& m_pCollider.lock()->IsGround())
			{
				m_eState = Hit_Split_End;
				Vector3 vRot(0.f, 0.f, 0.f);
				Vector3	vPlayerPos = m_pPlayerTrans.lock()->GetPosition();
				Vector3 vPos = m_pTransform.lock()->GetPosition();

				m_pTransform.lock()->SetRotation(vRot);
				m_pTransform.lock()->SetPosition({ vPos.x, vPlayerPos.y, vPos.z });

				m_pCollider.lock()->SetGravity(true);
				m_bAir = false;
				m_bDown = true;
			}
		}
		break;
	case Em0000::Hit_Split_End:
		if (m_bHit)
		{
			if (m_pCollider.lock()->IsGround() && m_pMesh->CurPlayAnimInfo.Name == "Slam_Damage_Loop")
			{
				if (fDot < 0)
					m_eState = Hit_End_Front;
				else
					m_eState = Hit_End_Back;

				m_bDown = true;
				m_bAir = false;
			}
		}
		break;
	case Em0000::Move_Front_End:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Move_End", false, {}, 1.f, 50.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Move_End" && m_pMesh->PlayingTime() >= 0.9f)
			{
				m_eState = idle;
				m_bIng = false;
				m_bMove = false;
			}
		}
		break;
	case Em0000::Move_Front_Loop:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Move_Loop", true, {}, 1.f, 50.f, true);
			Update_Angle();
			m_bInteraction = true;

			if (fDir <= 0.6f)
				m_eState = Move_Front_End;
		}
		break;
	case Em0000::Move_Front_Start:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Move_Start", false, {}, 1.f, 50.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Move_Start" && m_pMesh->PlayingTime() >= 0.9f)
				m_eState = Move_Front_Loop;
		}
		break;
	case Em0000::Step_Back:
		if (m_bIng == true)
		{
			Update_Angle();
			Set_Rotate();
			m_pMesh->PlayAnimation("Back_Step", false, {}, 1.f, 50.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Back_Step" && m_pMesh->PlayingTime() >= 0.9f)
			{
				m_eState = Attack_Hard;
				m_bMove = false;
				m_pWeapon.lock()->Set_Coll(true);
			}

		}
		break;
	case Em0000::Stun_End:
		break;
	case Em0000::Stun_Start:
		break;
	case Em0000::idle:
		m_pMesh->PlayAnimation("Idle", true, {}, 1.f, 50.f, true);
		break;
	case Em0000::Hit_Snatch_Start:
		if (m_bHit)
		{
			Update_Angle();
			Set_Rotate();
			m_bDown = false;
			m_bAir = false;
			m_pMesh->PlayAnimation("Snatch_Start", false, {}, 1.f, 20.f, true);

			if (m_pPlayer.lock()->GetComponent<ENGINE::CapsuleCollider>().lock()->IsGround() == false
				&& m_bSnatch == false)
			{
				m_bAir = true;
				m_pCollider.lock()->SetGravity(false);
				m_eState = Hit_Air_Snatch_Start;
			}
			else if (m_bSnatch == false)
				m_eState = Hit_Snatch_End;
		}
		break;
	case Em0000::Hit_Snatch_End:
		if (m_bHit)
		{
			m_bDown = false;
			Update_Angle();
			Set_Rotate();
			m_pMesh->PlayAnimation("Snatch_End", false, {}, 1.f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Snatch_End" && m_pMesh->IsAnimationEnd())
				m_eState = idle;
		}
		break;
	case Em0000::Hit_Buster_Start:
		if (m_bHit)
		{
			Update_Angle();
			Set_Rotate();
			m_pMesh->PlayAnimation("Buster_Start", false, {}, 1.f, 20.f, true);

			m_pCollider.lock()->SetTrigger(true);
			if (m_pPlayer.lock()->Get_CurAnimationIndex() == Nero::ANI_EM0000_BUSTER_START
				&& m_pPlayer.lock()->IsAnimationEnd())
			{
				m_eState = Hit_Buster_End;

				Vector3 vRot(0.f, 0.f, 0.f);
				Vector3	vPlayerPos = m_pPlayerTrans.lock()->GetPosition();
				Vector3 vPos = m_pTransform.lock()->GetPosition();

				m_pTransform.lock()->SetRotation(vRot);
				m_pTransform.lock()->SetPosition({ vPos.x, vPlayerPos.y, vPos.z });
			}
		}
		break;
	case Em0000::Hit_Air_Buster_Start:
		if (m_bHit)
		{
			m_pCollider.lock()->SetGravity(false);
			m_pCollider.lock()->SetTrigger(true);

			Update_Angle();
			Set_Rotate();
			m_pMesh->PlayAnimation("Buster_Start", false, {}, 1.f, 20.f, true);

			if (m_pPlayer.lock()->Get_CurAnimationIndex() == Nero::ANI_EM0000_BUSTER_AIR &&
				m_pPlayer.lock()->Get_PlayingTime()>=0.5f)
			{
				m_eState = Hit_Air_Buster_End;

				Vector3 vRot(0.f, 0.f, 0.f);
				m_pTransform.lock()->SetRotation(vRot);
				m_pCollider.lock()->SetGravity(true);
			}
		}
		break;
	case Em0000::Hit_Buster_End:
		if (m_bHit)
		{
			Update_Angle();
			Set_Rotate();
			m_pMesh->PlayAnimation("Buster_End", false, {}, 1.f, 20.f, true);
			m_pCollider.lock()->SetTrigger(false);
			m_pCollider.lock()->SetRigid(true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Buster_End" && m_pMesh->IsAnimationEnd())
				m_eState = Prone_Getup;
		}
		break;
	case Em0000::Hit_Air_Buster_End:
		if (m_bHit)
		{
			Update_Angle();
			Set_Rotate();

			m_pCollider.lock()->SetTrigger(false);
			m_pCollider.lock()->SetRigid(true);

			m_eState = Hit_KnocBack;
			m_bHit = true;

			Vector3 vLook = m_pPlayerTrans.lock()->GetLook();

			m_vPower += -vLook;
			m_vPower.y = -2.f;

			m_fPower = 200.f;

			D3DXVec3Normalize(&m_vPower, &m_vPower);
			m_pCollider.lock()->AddForce(m_vPower * m_fPower);

			m_vPower.x = 0.f;
			m_vPower.z = 0.f;
			m_fPower = 100.f;
		}
		break;
	case Em0000::Enter_Ground:
		if (m_bEnterGround == false)
		{
			m_pMesh->PlayAnimation("Enter_Ground", false, {}, 1.f, 20.f, true);

			Update_Angle();
			Set_Rotate();
			m_pAppear.lock()->PlayStart();
			m_pAppear.lock()->SetPosition(m_pTransform.lock()->GetPosition());

			m_bEnterGround = true;
		}
		if (m_pMesh->CurPlayAnimInfo.Name == "Enter_Ground" && m_pMesh->IsAnimationEnd())
			m_eState = idle;
		break;

	}
}

void Em0000::Skill_CoolTime(const float _fDeltaTime)
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
	if (m_bAttack == false)
	{
		m_fAttackTime += _fDeltaTime;
		if (m_fAttackTime >= 5.f)
		{
			m_bAttack = true;
			m_fAttackTime = 0.f;
		}
	}
}

HRESULT Em0000::Ready()
{
	Unit::Ready();
	m_nTag = Monster0000;

	RenderInit();


	m_BattleInfo.iMaxHp = 200;
	m_BattleInfo.iHp = 200;
	m_BattleInfo.iAttack = 20;

	// 트랜스폼 초기화하며 Edit 에 정보가 표시되도록 푸시 . 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.001f,0.001f,0.001f });
	PushEditEntity(InitTransform.lock().get());

	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.
	// 	PushEditEntity(_ShaderInfo.GetShader(RenderProperty::Order::ForwardAlphaBlend).get());


	//몬스터 회전 기본 속도
	m_fAngleSpeed = D3DXToRadian(100.f);
	m_pTransform.lock()->SetPosition({ -4.8f, 1.2f, -4.82f });

	m_fPower = 100.f;
	m_vPower = D3DXVECTOR3(0.f, 1.f, 0.5f);
	return S_OK;
}

HRESULT Em0000::Awake()
{
	Unit::Awake();
	m_pPlayer = std::static_pointer_cast<Nero>(FindGameObjectWithTag(Player).lock());
	m_pPlayerTrans = m_pPlayer.lock()->GetComponent<ENGINE::Transform>();
	m_pRedQueen = std::static_pointer_cast<RedQueen>(FindGameObjectWithTag(GAMEOBJECTTAG::TAG_RedQueen).lock());

	m_pCollider = AddComponent<CapsuleCollider>();
	m_pCollider.lock()->ReadyCollider();
	PushEditEntity(m_pCollider.lock().get());

	m_pWeapon = AddGameObject<Em0000Weapon>();
	m_pWeapon.lock()->m_pEm0000 = static_pointer_cast<Em0000>(m_pGameObject.lock());
	m_pWeapon.lock()->m_pEm0000Mesh = m_pMesh;

	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);
	m_pCollider.lock()->SetRigid(true);
	m_pCollider.lock()->SetGravity(true);
		
	m_pCollider.lock()->SetRadius(0.06f);
	m_pCollider.lock()->SetHeight(0.13f);
	m_pCollider.lock()->SetCenter({ 0.f,0.13f,-0.03f });

	//m_pPlayerBone = m_pPlayer.lock()->Get_BoneMatrixPtr("Waist");
	//m_pPlayerBone = m_pPlayer.lock()->Get_BoneMatrixPtr("R_Hand");
	m_pPlayerBone = m_pPlayer.lock()->Get_BoneMatrixPtr("WeaponConst");

	//몬스터 초기상태 Enter_Ground
	m_eState = Enter_Ground;

	////////////
	m_pBlood = AddGameObject<Liquid>();
	m_pAppear = AddGameObject<AppearGroundMonster>();
	StoneDebrisInit();
	///////////

	return S_OK;
}

HRESULT Em0000::Start()
{
	Unit::Start();
	return S_OK;
}

UINT Em0000::Update(const float _fDeltaTime)
{
	Unit::Update(_fDeltaTime);
	// 현재 스케일과 회전은 의미가 없음 DeltaPos 로 트랜스폼에서 통제 . 
	auto [DeltaScale, DeltaQuat, DeltaPos] = m_pMesh->Update(_fDeltaTime);
	Vector3 Axis = { 1,0,0 };

	const float Length = FMath::Length(DeltaPos);
	D3DXMATRIX matRot;
	D3DXQUATERNION tQuat = m_pTransform.lock()->GetQuaternion();
	D3DXMatrixRotationQuaternion(&matRot, &tQuat);

	D3DXVec3TransformNormal(&DeltaPos, &DeltaPos, &matRot);

	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		SpTransform->SetPosition(SpTransform->GetPosition() + DeltaPos * SpTransform->GetScale().x);
	}

	if (m_bHit)
		m_bIng = true;

			
	if (m_bEnterGround)
	{
		if (!m_bHit)
			Skill_CoolTime(_fDeltaTime);
		Fight(_fDeltaTime);
	}

	State_Change(_fDeltaTime);



	if (m_eState == Hit_Buster_Start || m_eState == Hit_Air_Buster_Start)
	{
		m_PlayerWorld = m_pPlayerTrans.lock()->GetWorldMatrix();
		m_Result = (*m_pPlayerBone * m_PlayerWorld);

		Matrix Mybone = *m_pMesh->GetToRootMatrixPtr("Hip");
		Matrix BoneResult;

		D3DXMatrixTranslation(&BoneResult, Mybone._41, Mybone._42, Mybone._43);
		D3DXMatrixInverse(&BoneResult, nullptr, &BoneResult);

		m_Result = BoneResult * m_Result;
		
		m_pTransform.lock()->SetWorldMatrix(m_Result);
	}
	if (m_eState == Hit_Split_Start)
	{
		Vector3 vRedQueenPos = m_pRedQueen.lock()->GetRedQueenBoneWorldPos("_001");
		m_pTransform.lock()->SetPosition(vRedQueenPos);
	}

	Rotate(_fDeltaTime);

	if (m_eState == Dead
		&& m_pMesh->IsAnimationEnd())
	{
		if (m_bDissolve == false)
		{
			m_pDissolve.DissolveStart();
			m_bDissolve = true;
		}
		if (m_pDissolve.DissolveUpdate(_fDeltaTime, _RenderUpdateInfo.World))
		{
			Destroy(m_pWeapon);
			Destroy(m_pGameObject);
		}
	}
	return 0;
}

UINT Em0000::LateUpdate(const float _fDeltaTime)
{
	Unit::LateUpdate(_fDeltaTime);
	return 0;

}

void Em0000::Editor()
{
	Unit::Editor();
	if (bEdit)
	{
		ImGui::InputInt("Count", &nCount);
	}

}

void Em0000::OnEnable()
{
	Unit::OnEnable();

	_RenderProperty.bRender = true;
}

void Em0000::OnDisable()
{
	Unit::OnDisable();

	_RenderProperty.bRender = false;
}

void Em0000::Hit(BT_INFO _BattleInfo, void* pArg)
{
	AddRankScore(_BattleInfo.iAttack);
	m_BattleInfo.iHp -= _BattleInfo.iAttack;


	//////////////////////////
	if (!m_pBlood.expired())
	{
		int iRandom = FMath::Random<int>(0, 6);
		if (iRandom >= 4)
			++iRandom;

		auto pBlood = m_pBlood.lock();
		pBlood->SetVariationIdx(Liquid::VARIATION(iRandom));	// 0 6 7 이 자연스러운듯?
		pBlood->SetPosition(GetMonsterBoneWorldPos("Waist"));
		pBlood->SetScale(0.0085f);
		//pBlood->SetRotation()	// 상황에 맞게 각도 조절
		pBlood->PlayStart(40.f);
	}
	///////////////////////////////////////////////////


	if (m_bDown == false)
	{
		switch (_BattleInfo.eAttackType)
		{
		case ATTACKTYPE::Attack_L:
			m_eState = Hit_L;
			m_bHit = true;
			break;
		case ATTACKTYPE::Attack_R:
			m_eState = Hit_R;
			m_bHit = true;
			break;
		case ATTACKTYPE::Attack_Front:
			m_eState = Hit_Front;
			m_bHit = true;
			break;
		case ATTACKTYPE::Attack_KnocBack:
		{
			m_eState = Hit_KnocBack;
			m_bHit = true;

			Vector3 vLook = m_pPlayerTrans.lock()->GetLook();

			m_vPower += -vLook;
			m_vPower.y = 1.f;
			m_fPower = 50.f;

			D3DXVec3Normalize(&m_vPower, &m_vPower);
			m_pCollider.lock()->AddForce(m_vPower * m_fPower);

			m_fPower = 100.f;
			m_vPower.x = 0.f;
			m_vPower.z = 0.f;

			break;
		}
		case ATTACKTYPE::Attack_Back:
			break;
		case ATTACKTYPE::Attack_Buster_Start:
			m_eState = Hit_Buster_Start;
			m_bHit = true;
			break;
		case ATTACKTYPE::Attack_Buster_Loop:
			m_eState = Hit_Buster_Loop;
			m_bHit = true;
			break;
		case ATTACKTYPE::Attack_Buster_End:
			m_eState = Hit_Buster_End;
			m_bHit = true;
			break;
		case ATTACKTYPE::Attack_Homerun:
		{
			m_eState = Hit_KnocBack;
			m_bHit = true;

			Vector3 vLook = m_pPlayerTrans.lock()->GetLook();

			m_vPower += -vLook;
			m_vPower.y = 1.5f;

			D3DXVec3Normalize(&m_vPower, &m_vPower);
			m_pCollider.lock()->AddForce(m_vPower * m_fPower);

			m_vPower.x = 0.f;
			m_vPower.z = 0.f;

			break;
		}
		case ATTACKTYPE::Attack_Air_Start:
		{
			m_eState = Hit_Air_Start;
			m_bHit = true;

			Vector3 vLook = -m_pPlayerTrans.lock()->GetLook();
			D3DXVec3Normalize(&vLook, &vLook);
			Vector3	vDir(vLook.x * 0.06f, 1.f, vLook.z * 0.06f);

			m_pCollider.lock()->AddForce(vDir * m_fPower);
			break;
		}
		default:
			m_bIng = true;
			break;

		}
	}
	else
	{
		switch (_BattleInfo.eAttackType)
		{
		case ATTACKTYPE::Attack_KnocBack:
		{
			m_eState = Hit_KnocBack;
			m_bHit = true;

			Vector3 vLook = m_pPlayerTrans.lock()->GetLook();

			m_vPower += -vLook;
			m_vPower.y = 1.f;
			m_fPower = 50.f;

			D3DXVec3Normalize(&m_vPower, &m_vPower);
			m_pCollider.lock()->AddForce(m_vPower* m_fPower);

			m_fPower = 100.f;
			m_vPower.x = 0.f;
			m_vPower.z = 0.f;

			break;
		}
		case ATTACKTYPE::Attack_Homerun:
		{
			m_eState = Hit_KnocBack;
			m_bHit = true;

			Vector3 vLook = m_pPlayerTrans.lock()->GetLook();

			m_vPower += -vLook;
			m_vPower.y = 1.5f;

			D3DXVec3Normalize(&m_vPower, &m_vPower);
			m_pCollider.lock()->AddForce(m_vPower* m_fPower);

			m_vPower.x = 0.f;
			m_vPower.z = 0.f;

			break;
		}
		case ATTACKTYPE::Attack_Air_Start:
		{
			m_eState = Hit_Air_Start;
			m_bHit = true;

			Vector3 vLook = -m_pPlayerTrans.lock()->GetLook();
			D3DXVec3Normalize(&vLook, &vLook);
			Vector3	vDir(vLook.x * 0.06f, 1.f, vLook.z * 0.06f);

			m_pCollider.lock()->AddForce(vDir* m_fPower);
			break;
		}
		default:
			m_eState = Downword_Damage;
			m_bHit = true;
			break;
		}
	}

}

void Em0000::Buster(BT_INFO _BattleInfo, void* pArg)
{
	if ((m_BattleInfo.iHp -= _BattleInfo.iAttack) >= 0.f)
		m_BattleInfo.iHp -= _BattleInfo.iAttack;

	m_bHit = true;
	m_bDown = true;
	m_pCollider.lock()->SetRigid(false);

	if (m_bAir)
		m_eState = Hit_Air_Buster_Start;
	else
		m_eState = Hit_Buster_Start;
	
}

void Em0000::Snatch(BT_INFO _BattleInfo, void* pArg)
{
	m_bHit = true;
	m_eState = Hit_Snatch_Start;
	
}

void Em0000::Air_Hit(BT_INFO _BattleInfo, void* pArg)
{
	AddRankScore(_BattleInfo.iAttack);
	m_BattleInfo.iHp -= _BattleInfo.iAttack;

	
	m_pCollider.lock()->SetGravity(false);

	/*--- 피 이펙트 ---*/
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
	/*----------------*/

	switch (_BattleInfo.eAttackType)
	{
	case ATTACKTYPE::Attack_Down:
	{
		m_eState = Hit_KnocBack;
		m_bHit = true;

		Vector3 vLook = m_pPlayerTrans.lock()->GetLook();

		m_vPower += -vLook;
		m_vPower.y = -2.f;

		m_fPower = 200.f;

		D3DXVec3Normalize(&m_vPower, &m_vPower);
		m_pCollider.lock()->AddForce(m_vPower * m_fPower);

		m_vPower.x = 0.f;
		m_vPower.z = 0.f;
		m_fPower = 100.f;
		m_pCollider.lock()->SetGravity(true);
	}
	break;
	case ATTACKTYPE::Attack_Air_Start:
	{
		m_eState = Hit_Air_Start;
		m_bHit = true;

		Vector3 vLook = -m_pPlayerTrans.lock()->GetLook();
		D3DXVec3Normalize(&vLook, &vLook);
		Vector3	vDir(vLook.x * 0.03f, 0.32f, vLook.z * 0.03f);

		m_pCollider.lock()->AddForce(vDir * m_fPower);
	}
	break;
	case ATTACKTYPE::Attack_KnocBack:
	{
		m_eState = Hit_KnocBack;
		m_bHit = true;

		Vector3 vLook = m_pPlayerTrans.lock()->GetLook();

		m_vPower += -vLook;

		D3DXVec3Normalize(&m_vPower, &m_vPower);
		m_fPower = 120.f;
		m_vPower.x *= 1.4f;
		m_vPower.z *= 1.4;
		m_vPower.y = 0.1f;

		m_pCollider.lock()->AddForce(m_vPower * m_fPower);

		m_pCollider.lock()->SetGravity(true);
		m_vPower.x = 0.f;
		m_vPower.z = 0.f;
		m_fPower = 100.f;
	}
		break;
	case ATTACKTYPE::Attack_Split:
		m_eState = Hit_Split_Start;
		m_bHit = true;
		break;
	default:
		m_eState = Hit_Air;
		m_bHit = true;
		break;
	}
}

void Em0000::RenderGBufferSK(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = m_pMesh->GetNumSubset();
	if (Numsubset > 0)
	{
		m_pMesh->BindVTF(_Info.Fx);
		m_pDissolve.DissolveVariableBind(_Info.Fx);
	};
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
		{
			continue;
		}

		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->BindProperty(TextureType::DIFFUSE, 0, 0, _Info._Device);
			SpSubset->BindProperty(TextureType::NORMALS, 0, 1, _Info._Device);
			SpSubset->Render(_Info.Fx);
		};
	};
}

void Em0000::RenderShadowSK(const DrawInfo& _Info)
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
		if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
		{
			continue;
		}
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
}

void Em0000::RenderDebugBone(const DrawInfo& _Info)
{
	const Matrix ScaleOffset = FMath::Scale({ 0.01,0.01 ,0.01 });
	m_pMesh->BoneDebugRender(_RenderUpdateInfo.World, _Info.Fx);
}

void Em0000::RenderDebugSK(const DrawInfo& _Info)
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
		if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
		{
			continue;
		}
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
}

void Em0000::RenderInit()
{
	SetRenderEnable(true);
	ENGINE::RenderProperty _InitRenderProp;
	_InitRenderProp.bRender = true;
	_InitRenderProp.RenderOrders[RenderProperty::Order::GBuffer] =
	{
		{DissolveInfo::ShaderSkeletonName,
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
	_InitRenderProp.RenderOrders[RenderProperty::Order::Collider]
		=
	{
		{"Collider" ,
		[this](const DrawInfo& _Info)
		{
			DrawCollider(_Info);
		}
	} };

	m_pDissolve.Initialize("..\\..\\Resource\\Mesh\\Dynamic\\Monster\\Em0000\\Em0000.fbx",
		Vector3{
			1.f,0.f,0.f
		});

	RenderInterface::Initialize(_InitRenderProp);
	Mesh::InitializeInfo _InitInfo{};
	// 버텍스 정점 정보가 CPU 에서도 필요 한가 ? 
	_InitInfo.bLocalVertexLocationsStorage = false;
	m_pMesh = Resources::Load<ENGINE::SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Monster\\Em0000\\Em0000.fbx", _InitInfo);

	m_pMesh->LoadAnimationFromDirectory(L"..\\..\\Resource\\Mesh\\Dynamic\\Monster\\Em0000\\Ani");
	m_pMesh->AnimationDataLoadFromJsonTable(L"..\\..\\Resource\\Mesh\\Dynamic\\Monster\\Em0000\\Em0000.Animation");


	m_pMesh->EnableToRootMatricies();
	PushEditEntity(m_pMesh.get());
	
}

void Em0000::Rotate(const float _fDeltaTime)
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

void Em0000::Update_Angle()
{
	Vector3 vPlayerPos = m_pPlayerTrans.lock()->GetPosition();
	Vector3 vMyPos = m_pTransform.lock()->GetPosition();

	Vector3 vDir = vPlayerPos - vMyPos;
	vDir.y = 0;
	D3DXVec3Normalize(&vDir, &vDir);

	Vector3 vLook = -m_pTransform.lock()->GetLook();

	float fDot = D3DXVec3Dot(&vDir, &vLook);
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

void Em0000::Set_Rotate()
{
	m_pTransform.lock()->Rotate({ 0.f, -D3DXToDegree(m_fRadian), 0.f });
}

void Em0000::Test()
{
	Vector3 vPlayerPos = m_pPlayerTrans.lock()->GetPosition();
	Vector3 vMyPos = m_pTransform.lock()->GetPosition();

	Vector3 vDir = vPlayerPos - vMyPos;
	vDir.y = 0;
	D3DXVec3Normalize(&vDir, &vDir);

	Vector3 vLook = -m_pTransform.lock()->GetLook();

	float fDot = D3DXVec3Dot(&vDir, &vLook);
	float fRadian = acosf(fDot);

	Vector3	vCross;
	D3DXVec3Cross(&vCross, &vLook, &vDir);

	if (vCross.y < 0)
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

void Em0000::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
	if (!m_bCollEnable)
		return;
	if (m_eState == Dead)
		return;

	m_bCollEnable = false;


	switch (_pOther.lock()->m_nTag)
	{
	case GAMEOBJECTTAG::TAG_RedQueen:
		if (m_bAir)
			Air_Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		else
			Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		
		m_pWeapon.lock()->Set_Coll(false);
		m_pWeapon.lock()->m_pCollider.lock()->SetActive(false);
		break;
	case GAMEOBJECTTAG::Tag_Cbs_Middle:
		if (m_bAir)
			Air_Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		else
			Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		
		m_pWeapon.lock()->Set_Coll(false);
		m_pWeapon.lock()->m_pCollider.lock()->SetActive(false);
		break;
	case GAMEOBJECTTAG::Tag_Cbs_Short:
		if (m_bAir)
			Air_Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		else
			Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());

		m_pWeapon.lock()->Set_Coll(false);
		m_pWeapon.lock()->m_pCollider.lock()->SetActive(false);
		break;
	case GAMEOBJECTTAG::Tag_Cbs_Long:
		if (m_bAir)
			Air_Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		else
			Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());

		m_pWeapon.lock()->Set_Coll(false);
		m_pWeapon.lock()->m_pCollider.lock()->SetActive(false);
		break;
	case GAMEOBJECTTAG::TAG_BusterArm_Right:
		_pOther.lock()->GetComponent<SphereCollider>().lock()->SetActive(false);
		Buster(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());

		m_pWeapon.lock()->Set_Coll(false);
		m_pWeapon.lock()->m_pCollider.lock()->SetActive(false);
		break;
	case GAMEOBJECTTAG::TAG_WireArm:
		Snatch(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());

		m_pWeapon.lock()->Set_Coll(false);
		m_pWeapon.lock()->m_pCollider.lock()->SetActive(false);
		break;
	case GAMEOBJECTTAG::Overture:
		if (m_bAir)
			Air_Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		else
			Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());

		m_pWeapon.lock()->Set_Coll(false);
		m_pWeapon.lock()->m_pCollider.lock()->SetActive(false);
		break;
	default:
		break;
	}

}

void Em0000::OnCollisionEnter(std::weak_ptr<GameObject> _pOther)
{
	Monster::OnCollisionEnter(_pOther);
}

void Em0000::SetGravity(bool _bActiveOrNot)
{
	m_pCollider.lock()->SetGravity(_bActiveOrNot);
}
