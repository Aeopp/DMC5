#include "stdafx.h"
#include "Em100.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include "TestObject.h"
#include <filesystem>
#include "Em100Hand.h"
#include "Nero.h"
#include "RedQueen.h"
#include "NeroFSM.h"
#include "Liquid.h"
#include "AppearGroundMonster.h"

void Em100::Free()
{
	Destroy(m_pBlood);
	Destroy(m_pAppear);

	Monster::Free();
}

std::string Em100::GetName()
{
	return "Em100";
}

Em100* Em100::Create()
{
	return new Em100{};
}


void Em100::Fight(const float _fDeltaTime)
{
	Vector3	 vDir = m_pPlayerTrans.lock()->GetPosition() - m_pTransform.lock()->GetPosition();
	float	 fDir = D3DXVec3Length(&vDir);

	if (m_BattleInfo.iHp <= 0.f && m_bAir == false)
	{
		m_eState = Dead;
		m_bIng = true;
	}



	//몬스터 움직이는 방향 정해주는 놈
	if (fDir >= 0.5f)
	{
		int iRandom = FMath::Random<int>(1, 6);
		if (m_bMove && m_bIng == false)
		{
			m_bIng = true;
			//플레이어 방향으로 돌게 만듬
			m_bInteraction = true;
			Update_Angle();
			////////////////////////////
			if (iRandom == 1)
			{
				m_eState = Walk_Left_Start;
				return;
			}
			else if (iRandom == 2)
			{
				m_eState = Walk_Right_Start;
				return;
			}
			else
			{
				m_eState = Walk_Front_Start;
				return;
			}
		}
	}
	//플레이어랑 어느정도 가까워 졌으면 공격.	
	else
	{
		if (m_bHardAttack && m_bIng == false)
		{
			m_bIng = true;
			m_eState = Attack_Hard;
			for (int i = 0; i < 2; ++i)
				m_pHand[i].lock()->Set_Coll(true);
			return;
		}
		if (m_bAttack && m_bIng == false)
		{
			//체력이50% 이상일땐 Attack_A, Attack_D 둘중 하나 이거 두개는 그냥 넉백 히트
			//50%보다 떨어지면 Attack_Hard 추가해서 플레이어 넘어트리는 공격 추가
			int iRandom = FMath::Random<int>(1, 2);
			m_bIng = true;
			if (iRandom == 1)
			{
				m_eState = Attack_A;
				for (int i = 0; i < 2; ++i)
					m_pHand[i].lock()->Set_Coll(true);
				return;
			}
			else if (iRandom == 2)
			{
				m_eState = Attack_D;
				for (int i = 0; i < 2; ++i)
					m_pHand[i].lock()->Set_Coll(true);
				return;
			}
		}
	}
}

void Em100::State_Change(const float _fDeltaTime)
{
	Vector3	 vDir = m_pPlayerTrans.lock()->GetPosition() - m_pTransform.lock()->GetPosition();
	float	 fDir = D3DXVec3Length(&vDir);
	Vector3	 vLook = m_pTransform.lock()->GetLook();
	float fDot = D3DXVec3Dot(&vDir, &vLook);

	switch (m_eState)
	{
	case Em100::Air_End:
		break;
	case Em100::Air_Loop:
		break;
	case Em100::Air_Start:
		break;
	case Em100::Attack_A:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Attack_A", false, {}, 1.f, 50.f, true);
			Update_Angle();
			m_bInteraction = true;

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_A" && m_pMesh->IsAnimationEnd())
			{
				int iRandom = FMath::Random<int>(1, 4);

				if (iRandom == 1)
					m_eState = Idle;
				else if (iRandom == 2)
					m_eState = Idle2;
				else if (iRandom == 3)
					m_eState = Idle3;
				else if (iRandom == 4)
					m_eState = Idle4;

				m_bIng = false;
				m_bAttack = false;

				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_Coll(false);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
				}
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_A" && m_pMesh->PlayingTime() >= 0.4f)
			{
				for (int i = 0; i < 2; ++i)
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_A" && m_pMesh->PlayingTime() >= 0.2f)
			{
				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_AttackType(Attack_Front);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(true);
				}
			}
		}
		break;
	case Em100::Attack_D:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Attack_D", false, {}, 1.f, 50.f, true);
			Update_Angle();
			m_bInteraction = true;

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_D" && m_pMesh->IsAnimationEnd())
			{
				int iRandom = FMath::Random<int>(1, 4);

				if (iRandom == 1)
					m_eState = Idle;
				else if (iRandom == 2)
					m_eState = Idle2;
				else if (iRandom == 3)
					m_eState = Idle3;
				else if (iRandom == 4)
					m_eState = Idle4;

				m_bIng = false;
				m_bAttack = false;

				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_Coll(false);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
				}
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_D" && m_pMesh->PlayingTime() >= 0.5f)
			{
				for (int i = 0; i < 2; ++i)
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_D" && m_pMesh->PlayingTime() >= 0.2f)
			{
				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_AttackType(Attack_Front);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(true);
				}
			}
		}
		break;
	case Em100::Attack_Hard:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Attack_Hard", false, {}, 1.f, 50.f, true);
			Update_Angle();
			m_bInteraction = true;

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Hard" && m_pMesh->IsAnimationEnd())
			{
				int iRandom = FMath::Random<int>(1, 4);

				if (iRandom == 1)
					m_eState = Idle;
				else if (iRandom == 2)
					m_eState = Idle2;
				else if (iRandom == 3)
					m_eState = Idle3;
				else if (iRandom == 4)
					m_eState = Idle4;

				m_bIng = false;
				m_bHardAttack = false;

				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_Coll(false);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
				}
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Hard" && m_pMesh->PlayingTime() >= 0.5f)
			{
				for (int i = 0; i < 2; ++i)
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Hard" && m_pMesh->PlayingTime() >= 0.2f)
			{
				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_AttackType(Attack_Front);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(true);
				}
			}
		}
		break;
	case Em100::Dead:
		if (m_bIng == true)
			m_pMesh->PlayAnimation("Death_Front", false, {}, 1.f, 20.f, true);
		StoneDebrisPlayStart();
		break;
	case Em100::Hit_Air:
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
	case Em100::Hit_Air_Start:
		if (m_bHit == true)
		{
			m_bAir = true;
			m_pMesh->PlayAnimation("Air_Start", false, {}, 1.f, 20.f, true);

			m_pCollider.lock()->AddForce({ 0.f, 1.8f,0.f });


			if (m_pMesh->CurPlayAnimInfo.Name == "Air_Start" &&
				m_pMesh->PlayingTime() >= 0.1f && m_pCollider.lock()->IsGround())
			{
				m_eState = Hit_Air_Loop;
				m_pCollider.lock()->SetGravity(true);
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Air_Start" && m_pMesh->PlayingTime() >= 0.8f)
			{
				m_eState = Hit_Air_Loop;
				m_pCollider.lock()->SetGravity(true);
			}

		}
		break;
	case Em100::Hit_Air_Snatch_Start:
		if (m_bHit == true)
		{
			m_bAir = true;
			m_pMesh->PlayAnimation("Air_Start", false, {}, 1.f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Air_Start" &&
				m_pMesh->PlayingTime() >= 0.1f && m_pCollider.lock()->IsGround())
			{
				m_eState = Hit_Air_Loop;
				m_pCollider.lock()->SetGravity(true);
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Air_Start" && m_pMesh->PlayingTime() >= 0.8f)
			{
				m_eState = Hit_Air_Loop;
				m_pCollider.lock()->SetGravity(true);
			}

		}
		break;
	case Em100::Hit_Air_Loop:
		if (m_bHit == true)
		{
			m_pMesh->PlayAnimation("Air_Loop", true, {}, 1.f, 20.f, true);

			m_pCollider.lock()->AddForce({ 0.f, 1.8f,0.f });

			if (m_pMesh->CurPlayAnimInfo.Name == "Air_Loop" && m_pCollider.lock()->IsGround())
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
	case Em100::Hit_Back:
		break;
	case Em100::Hit_Finish:
		break;
	case Em100::Hit_Front:
		if (m_bHit == true)
		{
			m_pMesh->PlayAnimation("Hit_Front", false, {}, 1.f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Hit_Front" && m_pMesh->IsAnimationEnd())
			{
				int iRandom = FMath::Random<int>(1, 4);

				if (iRandom == 1)
					m_eState = Idle;
				else if (iRandom == 2)
					m_eState = Idle2;
				else if (iRandom == 3)
					m_eState = Idle3;
				else if (iRandom == 4)
					m_eState = Idle4;

				m_bHit = false;
				m_bIng = false;
			}
		}
		break;
	case Em100::Hit_L:
		if (m_bHit == true)
		{
			m_pMesh->PlayAnimation("Hit_L", false, {}, 0.8f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Hit_L" && m_pMesh->IsAnimationEnd())
			{
				int iRandom = FMath::Random<int>(1, 4);

				if (iRandom == 1)
					m_eState = Idle;
				else if (iRandom == 2)
					m_eState = Idle2;
				else if (iRandom == 3)
					m_eState = Idle3;
				else if (iRandom == 4)
					m_eState = Idle4;

				m_bHit = false;
				m_bIng = false;
			}
		}
		break;
	case Em100::Hit_R:
		if (m_bHit == true)
		{
			m_pMesh->PlayAnimation("Hit_R", false, {}, 0.8f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Hit_R" && m_pMesh->IsAnimationEnd())
			{
				int iRandom = FMath::Random<int>(1, 4);

				if (iRandom == 1)
					m_eState = Idle;
				else if (iRandom == 2)
					m_eState = Idle2;
				else if (iRandom == 3)
					m_eState = Idle3;
				else if (iRandom == 4)
					m_eState = Idle4;

				m_bHit = false;
				m_bIng = false;
			}
		}
		break;
	case Em100::Hit_KnocBack:
		//날라가는거
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
	case Em100::Hit_End_Front:
		if (m_bHit == true)
		{
			m_pMesh->PlayAnimation("Hit_End", false, {}, 1.2f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Hit_End" && m_pMesh->IsAnimationEnd())
				m_eState = Downword_Down_StandUp;
		}
		break;
	case Em100::Hit_End_Back:
		if (m_bHit == true)
		{
			m_pMesh->PlayAnimation("Blown_Front_Landing", false, {}, 1.f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Blown_Front_Landing" && m_pMesh->IsAnimationEnd())
				m_eState = Downword_Down_StandUp;
		}
		break;
	case Em100::Downword_Down_StandUp:
		if (m_bDown == true)
		{
			m_pMesh->PlayAnimation("Downword_Down_StandUp", false, {}, 1.f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Downword_Down_StandUp" && m_pMesh->IsAnimationEnd())
			{
				//쳐맞아서 넘어진다음에 일어나는 모션이 끝남
				//그렇다는건 이제 맞지도않고, 다음 행동 해도 되고(m_bing == false), 넘어진 것도 끝났음.
				m_bHit = false;
				m_bIng = false;
				m_bDown = false;
				int iRandom = FMath::Random<int>(1, 4);

				if (iRandom == 1)
					m_eState = Idle;
				else if (iRandom == 2)
					m_eState = Idle2;
				else if (iRandom == 3)
					m_eState = Idle3;
				else if (iRandom == 4)
					m_eState = Idle4;
			}
		}
		break;
	case Em100::Downword_Damage:
		if (m_bDown && m_bHit == true)
		{
			m_pMesh->PlayAnimation("Downword_Damage", false, {}, 1.f, 10.f);
			m_bHit = false;
		}
		if (m_pMesh->CurPlayAnimInfo.Name == "Downword_Damage" && m_pMesh->IsAnimationEnd())
			m_eState = Downword_Down_StandUp;
		break;
	case Em100::Walk_Front_End:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Move_C_End", false, {}, 1.f, 50.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Move_C_End" && m_pMesh->IsAnimationEnd())
			{
				int iRandom = FMath::Random<int>(1, 4);

				if (iRandom == 1)
					m_eState = Idle;
				else if (iRandom == 2)
					m_eState = Idle2;
				else if (iRandom == 3)
					m_eState = Idle3;
				else if (iRandom == 4)
					m_eState = Idle4;

				m_bMove = false;
				m_bIng = false;
			}
		}
		break;
	case Em100::Walk_Front_Loop:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Move_C_Loop", true, {}, 1.f, 50.f, true);
			Update_Angle();
			m_bInteraction = true;

			if (fDir <= 0.5f)
				m_eState = Walk_Front_End;
		}
		break;
	case Em100::Walk_Front_Start:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Move_C_Start", false, {}, 1.f, 50.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Move_C_Start" && m_pMesh->IsAnimationEnd())
				m_eState = Walk_Front_Loop;
		}
		break;
	case Em100::Walk_Left_End:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Move_L_End", false, {}, 1.f, 50.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Move_L_End" && m_pMesh->IsAnimationEnd())
			{
				int iRandom = FMath::Random<int>(1, 4);

				if (iRandom == 1)
					m_eState = Idle;
				else if (iRandom == 2)
					m_eState = Idle2;
				else if (iRandom == 3)
					m_eState = Idle3;
				else if (iRandom == 4)
					m_eState = Idle4;

				m_bMove = false;
				m_bIng = false;
			}
		}
		break;
	case Em100::Walk_Left_Loop:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Move_L_Loop", false, {}, 1.f, 50.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Move_L_Loop" && m_pMesh->IsAnimationEnd())
				m_eState = Walk_Left_End;
		}
		break;
	case Em100::Walk_Left_Start:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Move_L_Start", false, {}, 1.f, 50.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Move_L_Start" && m_pMesh->IsAnimationEnd())
				m_eState = Walk_Left_Loop;
		}
		break;
	case Em100::Walk_Right_Stop:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Move_R_End", false, {}, 1.f, 50.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Move_R_End" && m_pMesh->IsAnimationEnd())
			{
				int iRandom = FMath::Random<int>(1, 4);

				if (iRandom == 1)
					m_eState = Idle;
				else if (iRandom == 2)
					m_eState = Idle2;
				else if (iRandom == 3)
					m_eState = Idle3;
				else if (iRandom == 4)
					m_eState = Idle4;

				m_bMove = false;
				m_bIng = false;
			}
		}
		break;
	case Em100::Walk_Right_Loop:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Move_R_Loop", false, {}, 1.f, 50.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Move_R_Loop" && m_pMesh->IsAnimationEnd())
				m_eState = Walk_Right_Stop;
		}
		break;
	case Em100::Walk_Right_Start:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Move_R_Start", false, {}, 1.f, 50.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Move_R_Start" && m_pMesh->IsAnimationEnd())
				m_eState = Walk_Right_Loop;
		}
		break;
	case Em100::Idle:
		m_pMesh->PlayAnimation("Idle", true, {}, 1.f, 50.f, true);
		m_BattleInfo.eAttackType = Attack_END;
		m_bHit = false;
		break;
	case Em100::Idle2:
		m_pMesh->PlayAnimation("Idle2", true, {}, 1.f, 50.f, true);
		m_BattleInfo.eAttackType = Attack_END;
		m_bHit = false;
		break;
	case Em100::Idle3:
		m_pMesh->PlayAnimation("Idle3", true, {}, 1.f, 50.f, true);
		m_BattleInfo.eAttackType = Attack_END;
		m_bHit = false;
		break;
	case Em100::Idle4:
		m_pMesh->PlayAnimation("Idle4", true, {}, 1.f, 50.f, true);
		m_BattleInfo.eAttackType = Attack_END;
		m_bHit = false;
		break;

	case Em100::Hit_Buster_Start:
		if (m_bHit)
		{
			m_pMesh->PlayAnimation("Slam_Damage_fall_loop", true, {}, 1.f, 20.f, true);

			if (m_pPlayer.lock()->Get_CurAnimationIndex() == Nero::ANI_BUSTER_STRIKE_COMMON
				&& m_pPlayer.lock()->Get_PlayingTime() >= 0.4f)
			{
				m_eState = Hit_Buster_End;
				Vector3 vRot(0.f, 0.f, 0.f);
				Vector3	vPlayerPos = m_pPlayerTrans.lock()->GetPosition();
				Vector3 vPos = m_pTransform.lock()->GetPosition();

				m_pTransform.lock()->SetRotation(vRot);
				m_pTransform.lock()->SetPosition({ vPos.x, vPlayerPos.y, vPos.z });

				m_pCollider.lock()->SetRigid(true);
			}
		}
		break;
	case Em100::Hit_Buster_End:
		if (m_bHit)
		{
			if (fDot < 0)
				m_pMesh->PlayAnimation("Hit_End", false, {}, 1.f, 20.f, true);
			else
				m_pMesh->PlayAnimation("Blown_Front_Landing", false, {}, 1.f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Hit_End" && m_pMesh->IsAnimationEnd())
				m_eState = Downword_Down_StandUp;
			else if (m_pMesh->CurPlayAnimInfo.Name == "Blown_Front_Landing" && m_pMesh->IsAnimationEnd())
				m_eState = Downword_Down_StandUp;
		}
		break;
	case Em100::Hit_Air_Buster_Start:
		if (m_bHit)
		{
			m_pCollider.lock()->SetGravity(false);
			m_pCollider.lock()->SetTrigger(true);

			Update_Angle();
			Set_Rotate();
			m_pMesh->PlayAnimation("Buster_Start", false, {}, 1.f, 20.f, true);

			if (m_pPlayer.lock()->Get_CurAnimationIndex() == Nero::ANI_BUSTER_STRIKE_COMMON_AIR &&
				m_pPlayer.lock()->Get_PlayingTime() >= 0.5f)
			{
				m_eState = Hit_Air_Buster_End;

				Vector3 vRot(0.f, 0.f, 0.f);
				m_pTransform.lock()->SetRotation(vRot);
				m_pCollider.lock()->SetGravity(true);
			}
		}
		break;
	case Em100::Hit_Air_Buster_End:
		if (m_bHit)
		{
			m_eState = Hit_KnocBack;

			m_pCollider.lock()->SetTrigger(false);
			m_pCollider.lock()->SetRigid(true);
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
	case Em100::Hit_Split_Start:
		if (m_bHit && m_bAir == true)
		{
			m_pMesh->PlayAnimation("Slam_Damage_fall_loop", true, {}, 1.f, 20.f, true);

			if (m_pPlayer.lock()->Get_CurAnimationIndex() == Nero::ANI_SPLIT_LANDING
				&& m_pCollider.lock()->IsGround())
			{
				m_eState = Hit_Buster_End;
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
	case Em100::Hit_Snatch_Start:
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
	case Em100::Hit_Snatch_End:
		if (m_bHit)
		{
			Update_Angle();
			Set_Rotate();
			m_bDown = false;
			m_pMesh->PlayAnimation("Snatch_End", false, {}, 1.f, 20.f, true);

			if (m_pCollider.lock()->IsGround())
				m_bAir = false;

			if (m_pMesh->CurPlayAnimInfo.Name == "Snatch_End" && m_pMesh->IsAnimationEnd())
			{
				m_eState = Idle;
				m_bHit = false;
			}
		}
		break;
	case Em100::Enter_Ground:
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
			m_eState = Idle;
		break;
	}


}

void Em100::Skill_CoolTime(const float _fDeltaTime)
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
	if (m_bHardAttack == false && m_BattleInfo.iHp <= 100)
	{
		m_fHardAttackTime += _fDeltaTime;
		if (m_fHardAttackTime >= 7.f)
		{
			m_bHardAttack = true;
			m_fHardAttackTime = 0.f;
		}
	}
}

HRESULT Em100::Ready()
{
	Unit::Ready();
	//GameObject를 받아오려면 각자 태그가 있어야함.
	m_nTag = Monster100;

	m_BattleInfo.iMaxHp = 150;
	m_BattleInfo.iHp = 150;
	m_BattleInfo.iAttack = 20;

	m_pTransform.lock()->SetPosition({ -4.8f, 1.2f, -4.82f });

	RenderInit();
	// 트랜스폼 초기화하며 Edit 에 정보가 표시되도록 푸시 . 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.001,0.001,0.001 });
	PushEditEntity(InitTransform.lock().get());

	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.
	// 	PushEditEntity(_ShaderInfo.GetShader(RenderProperty::Order::ForwardAlphaBlend).get());

	//몬스터 회전 기본 속도
	m_fAngleSpeed = D3DXToRadian(100.f);


	m_fPower = 100.f;
	m_vPower = D3DXVECTOR3(0.f, 1.f, 0.5f);
	return S_OK;
}

HRESULT Em100::Awake()
{
	Unit::Awake();

	m_pCollider = AddComponent<CapsuleCollider>();
	m_pCollider.lock()->ReadyCollider();
	PushEditEntity(m_pCollider.lock().get());

	for (int i = 0; i < 2; ++i)
	{
		m_pHand[i] = AddGameObject<Em100Hand>();
		m_pHand[i].lock()->m_pEm100 = static_pointer_cast<Em100>(m_pGameObject.lock());
		m_pHand[i].lock()->m_pEm100Mesh = m_pMesh;
		m_pHand[i].lock()->m_bLeft = (bool)i;
	}
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);


	m_pCollider.lock()->SetRigid(true);
	m_pCollider.lock()->SetGravity(true);

	m_pCollider.lock()->SetRadius(0.07f);
	m_pCollider.lock()->SetHeight(0.12f);
	m_pCollider.lock()->SetCenter({ 0.f, 0.122f, 0.f });

	m_pPlayer = std::static_pointer_cast<Nero>(FindGameObjectWithTag(GAMEOBJECTTAG::Player).lock());
	m_pPlayerTrans = m_pPlayer.lock()->GetComponent<ENGINE::Transform>();
	m_pRedQueen = std::static_pointer_cast<RedQueen>(FindGameObjectWithTag(GAMEOBJECTTAG::TAG_RedQueen).lock());

	m_pPlayerBone = m_pPlayer.lock()->Get_BoneMatrixPtr("R_MiddleF1");

	//몬스터 초기상태 Idle
	m_eState = Enter_Ground;


	/*--- 이펙트 ---*/
	m_pBlood = AddGameObject<Liquid>();
	m_pAppear = AddGameObject<AppearGroundMonster>();
	StoneDebrisInit();
	/*----------------*/

	return S_OK;
}

HRESULT Em100::Start()
{
	Unit::Start();
	return S_OK;
}

UINT Em100::Update(const float _fDeltaTime)
{
	Unit::Update(_fDeltaTime);
	
	
	// 현재 스케일과 회전은 의미가 없음 DeltaPos 로 트랜스폼에서 통제 . 
	auto [DeltaScale, DeltaQuat, DeltaPos] = m_pMesh->Update(_fDeltaTime);
	Vector3 Axis = { 1,0,0 };

	//ENGINE::AnimNotify _Notify{};
	////return true 면 이제 호출 안함, false면 저 루프 돌 계속 호출.
	//_Notify.Event[0.5] = [this]() {  AttackStart();  return false; };

	const float Length = FMath::Length(DeltaPos);
	//D3DXMATRIX matRot;
	//D3DXQUATERNION tQuat = m_pTransform.lock()->GetQuaternion();
	//D3DXMatrixRotationQuaternion(&matRot, &tQuat);

	//D3DXVec3TransformNormal(&DeltaPos, &DeltaPos, &matRot);


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

	Rotate(_fDeltaTime);

	if (m_bHit)
		m_bIng = true;



	if (m_bEnterGround == true)
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
		m_pTransform.lock()->SetWorldMatrix(m_Result);

		m_pCollider.lock()->SetRigid(false);
	}
	if (m_eState == Hit_Split_Start)
	{
		Vector3 vRedQueenPos = m_pRedQueen.lock()->GetRedQueenBoneWorldPos("_001");
		m_pTransform.lock()->SetPosition(vRedQueenPos);
	}


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
			for (int i = 0; i < 2; ++i)
				Destroy(m_pHand[i]);
			Destroy(m_pGameObject);
		}
	}

	return 0;
}

UINT Em100::LateUpdate(const float _fDeltaTime)
{
	Unit::LateUpdate(_fDeltaTime);
	return 0;
}

void Em100::Editor()
{
	Unit::Editor();
	if (bEdit)
	{
		m_pDissolve.DissolveEditor();
	}
}

void Em100::OnEnable()
{
	Unit::OnEnable();
	_RenderProperty.bRender = true;
}

void Em100::OnDisable()
{
	Unit::OnDisable();

	_RenderProperty.bRender = false;

}

void Em100::Hit(BT_INFO _BattleInfo, void* pArg)
{
	AddRankScore(_BattleInfo.iAttack);
	m_BattleInfo.iHp -= _BattleInfo.iAttack;

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
		case ATTACKTYPE::Attack_Air_Start:
		{
			m_eState = Hit_Air_Start;
			m_bHit = true;

			Vector3 vLook = -m_pPlayerTrans.lock()->GetLook();
			D3DXVec3Normalize(&vLook, &vLook);
			Vector3	vDir(vLook.x * 0.08f, 1.f, vLook.z * 0.08f);

			m_pCollider.lock()->AddForce(vDir * m_fPower);
			break;
		}
		case ATTACKTYPE::Attack_Air:
			break;
		case ATTACKTYPE::Attack_Homerun:
		{
			m_eState = Hit_KnocBack;
			m_bHit = true;

			Vector3 vLook = m_pPlayerTrans.lock()->GetLook();

			m_vPower += -vLook;
			m_vPower.y = 2.f;


			D3DXVec3Normalize(&m_vPower, &m_vPower);
			m_fPower = 120.f;
			m_pCollider.lock()->AddForce(m_vPower * m_fPower);

			m_vPower.x = 0.f;
			m_vPower.z = 0.f;
			m_fPower = 100.f;

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
			m_pCollider.lock()->AddForce(m_vPower * m_fPower);

			m_fPower = 100.f;
			m_vPower.x = 0.f;
			m_vPower.z = 0.f;

			break;
		}
		case ATTACKTYPE::Attack_Buster_Start:
			m_eState = Hit_Buster_Start;
			m_bHit = true;
			break;
		case ATTACKTYPE::Attack_Air_Start:
		{
			m_eState = Hit_Air_Start;
			m_bHit = true;

			Vector3 vLook = -m_pPlayerTrans.lock()->GetLook();
			D3DXVec3Normalize(&vLook, &vLook);
			Vector3	vDir(vLook.x * 0.08f, 1.f, vLook.z * 0.08f);

			m_pCollider.lock()->AddForce(vDir * m_fPower);
			break;
		}
		case ATTACKTYPE::Attack_Homerun:
		{
			m_eState = Hit_KnocBack;
			m_bHit = true;

			Vector3 vLook = m_pPlayerTrans.lock()->GetLook();

			m_vPower += -vLook;
			m_vPower.y = 2.f;

			D3DXVec3Normalize(&m_vPower, &m_vPower);
			m_fPower = 120.f;
			m_pCollider.lock()->AddForce(m_vPower * m_fPower);

			m_vPower.x = 0.f;
			m_vPower.z = 0.f;
			m_fPower = 100.f;

			break;
		}
		default:
			m_eState = Downword_Damage;
			m_bHit = true;
			break;
		}
	}

}

void Em100::Air_Hit(BT_INFO _BattleInfo, void* pArg)
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
		m_vPower.z *= 1.4f;
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

void Em100::OnCollisionEnter(std::weak_ptr<GameObject> _pOther)
{
	Monster::OnCollisionEnter(_pOther);
}

void Em100::Buster(BT_INFO _BattleInfo, void* pArg)
{
	if((m_BattleInfo.iHp -= _BattleInfo.iAttack) >=0.f)
		m_BattleInfo.iHp -= _BattleInfo.iAttack;

	m_bHit = true;
	m_bDown = true;

	m_pCollider.lock()->SetRigid(false);

	if (m_bAir)
		m_eState = Hit_Air_Buster_Start;
	else
		m_eState = Hit_Buster_Start;
}

void Em100::Snatch(BT_INFO _BattleInfo, void* pArg)
{
	m_bHit = true;
	m_eState = Hit_Snatch_Start;
}

void Em100::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
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

		for (int i = 0; i < 2; ++i)
		{
			m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
			m_pHand[i].lock()->Set_Coll(false);
		}
		break;
	case GAMEOBJECTTAG::Tag_Cbs_Middle:
		if (m_bAir)
			Air_Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		else
			Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());

		for (int i = 0; i < 2; ++i)
		{
			m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
			m_pHand[i].lock()->Set_Coll(false);
		}
		break;
	case GAMEOBJECTTAG::Tag_Cbs_Short:
		if (m_bAir)
			Air_Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		else
			Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());

		for (int i = 0; i < 2; ++i)
		{
			m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
			m_pHand[i].lock()->Set_Coll(false);
		}
		break;
	case GAMEOBJECTTAG::Tag_Cbs_Long:
		if (m_bAir)
			Air_Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		else
			Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());

		for (int i = 0; i < 2; ++i)
		{
			m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
			m_pHand[i].lock()->Set_Coll(false);
		}
		break;
	case GAMEOBJECTTAG::TAG_BusterArm_Right:
		_pOther.lock()->GetComponent<SphereCollider>().lock()->SetActive(false);
		Buster(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());

		for (int i = 0; i < 2; ++i)
		{
			m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
			m_pHand[i].lock()->Set_Coll(false);
		}
		break;
	case GAMEOBJECTTAG::TAG_WireArm:
		Snatch(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());

		for (int i = 0; i < 2; ++i)
		{
			m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
			m_pHand[i].lock()->Set_Coll(false);
		}
		break;
	case GAMEOBJECTTAG::Overture:
		if (m_bAir)
			Air_Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		else
			Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());

		for (int i = 0; i < 2; ++i)
		{
			m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
			m_pHand[i].lock()->Set_Coll(false);
		}
		break;
	default:
		break;
	}
}

void Em100::OnTriggerExit(std::weak_ptr<GameObject> _pOther)
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

		for (int i = 0; i < 2; ++i)
			m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
		break;
	case GAMEOBJECTTAG::TAG_BusterArm_Right:
		_pOther.lock()->GetComponent<SphereCollider>().lock()->SetActive(false);
		Buster(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		break;
	case GAMEOBJECTTAG::TAG_WireArm:
		Snatch(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		break;
	case GAMEOBJECTTAG::Overture:
		m_BattleInfo.iHp -= static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo().iAttack;
		m_bHit = true;
		m_bDown = true;
		m_eState = Hit_KnocBack;
		break;
	default:
		break;
	}
}

void Em100::RenderGBufferSK(const DrawInfo& _Info)
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

void Em100::RenderShadowSK(const DrawInfo& _Info)
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

void Em100::RenderDebugBone(const DrawInfo& _Info)
{
	const Matrix ScaleOffset = FMath::Scale({ 0.01,0.01 ,0.01 });
	m_pMesh->BoneDebugRender(_RenderUpdateInfo.World, _Info.Fx);
}

void Em100::RenderDebugSK(const DrawInfo& _Info)
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

void Em100::RenderInit()
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
	m_pDissolve.Initialize("..\\..\\Resource\\Mesh\\Dynamic\\Monster\\Em100\\Em100.fbx", 
		Vector3{
			1.f,0.f,0.f
		});
	RenderInterface::Initialize(_InitRenderProp);
	Mesh::InitializeInfo _InitInfo{};
	// 버텍스 정점 정보가 CPU 에서도 필요 한가 ? 
	_InitInfo.bLocalVertexLocationsStorage = false;
	m_pMesh = Resources::Load<ENGINE::SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Monster\\Em100\\Em100.fbx", _InitInfo);
	m_pMesh->LoadAnimationFromDirectory(L"..\\..\\Resource\\Mesh\\Dynamic\\Monster\\Em100\\Ani");
	m_pMesh->AnimationDataLoadFromJsonTable(L"..\\..\\Resource\\Mesh\\Dynamic\\Monster\\Em100\\Em100.Animation");

	m_pMesh->EnableToRootMatricies();
	PushEditEntity(m_pMesh.get());

}

void Em100::Rotate(const float _fDeltaTime)
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

void Em100::Update_Angle()
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

void Em100::Set_Rotate()
{
	m_pTransform.lock()->Rotate({ 0.f, -D3DXToDegree(m_fRadian), 0.f });
}

void Em100::SetGravity(bool _bActiveOrNot)
{
	m_pCollider.lock()->SetGravity(_bActiveOrNot);
}