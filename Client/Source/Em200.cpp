#include "stdafx.h"
#include "Em200.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include "TestObject.h"
#include <filesystem>
#include "Em200Hand.h"
#include "Nero.h"
#include "RedQueen.h"
#include "NeroFSM.h"
#include "Liquid.h"
#include "AppearGroundMonster.h"

void Em200::Free()
{
	Destroy(m_pBlood);
	Destroy(m_pAppear);

	Monster::Free();
}

std::string Em200::GetName()
{
	return "Em200";
}

Em200* Em200::Create()
{
	return new Em200{};
}


void Em200::Fight(const float _fDeltaTime)
{
	Vector3	 vDir = m_pPlayerTrans.lock()->GetPosition() - m_pTransform.lock()->GetPosition();
	float	 fDir = D3DXVec3Length(&vDir);

	if (m_BattleInfo.iHp <= 0.f && m_bAir == false)
	{
		m_eState = Dead;
		m_bIng = true;
		SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Dead", { 1,5 }, 0.3f, false);
	}

	//���� �����̴� ���� �����ִ� ��
	if (fDir >= 0.5f)
	{
		if (m_bMove && m_bIng == false)
		{
			m_bIng = true;
			//�÷��̾� �������� ���� ����
			m_bInteraction = true;
			Update_Angle();
			////////////////////////////
			m_eState = Walk_Front_Start;
			return;
			
		}
	}
	//�÷��̾�� ������� ����� ������ ����.	
	else
	{
		if (m_bHardAttack && m_bIng == false)
		{
			m_bIng = true;
			m_eState = Attack_Hard_Start;
			for (int i = 0; i < 2; ++i)
				m_pHand[i].lock()->Set_Coll(true);
			return;
		}
		if (m_bAttack && m_bIng == false)
		{
			//ü����50% �̻��϶� Attack_A, Attack_D ���� �ϳ� �̰� �ΰ��� �׳� �˹� ��Ʈ
			//50%���� �������� Attack_Hard �߰��ؼ� �÷��̾� �Ѿ�Ʈ���� ���� �߰�
			int iRandom = FMath::Random<int>(1, 2);
			m_bIng = true;
			if (iRandom == 1)
			{
				m_eState = Attack_L;
				for (int i = 0; i < 2; ++i)
					m_pHand[i].lock()->Set_Coll(true);
				return;
			}
			else if (iRandom == 2)
			{
				m_eState = Attack_R;
				for (int i = 0; i < 2; ++i)
					m_pHand[i].lock()->Set_Coll(true);
				return;
			}
		}
	}
}

void Em200::State_Change(const float _fDeltaTime)
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
	case Em200::Attack_L:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Attack_L", false, {}, 1.f, 50.f, true);
			Update_Angle();
			m_bInteraction = true;

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_L" && m_pMesh->IsAnimationEnd())
			{
				int iRandom = FMath::Random<int>(1, 3);

				if (iRandom == 1)
				{
					m_eState = Idle;
					SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Idle", { 1,5 }, 0.3f, false);
				}
				else if (iRandom == 2)
				{
					m_eState = Idle2;
					SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Idle", { 1,5 }, 0.3f, false);
				}
				else if (iRandom == 3)
				{
					m_eState = Idle3;
					SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Idle", { 1,5 }, 0.3f, false);
				}

				m_bIng = false;
				m_bAttack = false;

				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_Coll(false);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
				}
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_L" && m_pMesh->PlayingTime() >= 0.5f)
			{
				for (int i = 0; i < 2; ++i)
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_L" && m_pMesh->PlayingTime() >= 0.3f)
			{
				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_AttackType(Attack_Front);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(true);
				}
				SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Attack", { 1,2 }, 0.3f, false);
			}
		}
		break;
	case Em200::Attack_R:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Attack_R", false, {}, 1.f, 50.f, true);
			Update_Angle();
			m_bInteraction = true;

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_R" && m_pMesh->IsAnimationEnd())
			{
				int iRandom = FMath::Random<int>(1, 3);

				if (iRandom == 1)
				{
					m_eState = Idle;
					SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Idle", { 1,5 }, 0.3f, false);
				}
				else if (iRandom == 2)
				{
					m_eState = Idle2;
					SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Idle", { 1,5 }, 0.3f, false);
				}
				else if (iRandom == 3)
				{
					m_eState = Idle3;
					SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Idle", { 1,5 }, 0.3f, false);
				}

				m_bIng = false;
				m_bAttack = false;

				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_Coll(false);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
				}
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_R" && m_pMesh->PlayingTime() >= 0.6f)
			{
				for (int i = 0; i < 2; ++i)
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_R" && m_pMesh->PlayingTime() >= 0.3f)
			{
				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_AttackType(Attack_Front);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(true);
				}
			}
			else if(m_pMesh->CurPlayAnimInfo.Name =="Attack_R" && m_pMesh->PlayingTime() >= 0.25f)
				SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Attack", { 1,2 }, 0.3f, false);
		}
		break;
	case Em200::Attack_Hard_Start:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Attack_Hard_Start", false, {}, 1.f, 50.f, true);
			Update_Angle();
			m_bInteraction = true;

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Hard_Start" && m_pMesh->IsAnimationEnd())
				m_eState = Attack_Hard_Loop;
		}
		break;
	case Em200::Attack_Hard_Loop:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Attack_Hard_Loop", false, {}, 1.f, 50.f, true);
			Update_Angle();
			m_bInteraction = true;

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Hard_Loop" && m_pMesh->IsAnimationEnd())
				m_eState = Attack_Hard_End;
		}
		break;
	case Em200::Attack_Hard_End:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Attack_Hard_End", false, {}, 1.f, 50.f, true);
			Update_Angle();
			m_bInteraction = true;

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Hard_End" && m_pMesh->IsAnimationEnd())
			{
				int iRandom = FMath::Random<int>(1, 3);

				if (iRandom == 1)
				{
					m_eState = Idle;
					SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Idle", { 1,5 }, 0.3f, false);
				}
				else if (iRandom == 2)
				{
					m_eState = Idle2;
					SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Idle", { 1,5 }, 0.3f, false);
				}
				else if (iRandom == 3)
				{
					m_eState = Idle3;
					SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Idle", { 1,5 }, 0.3f, false);
				}

				m_bIng = false;
				m_bHardAttack = false;

				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_Coll(false);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
				}
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Hard_End" && m_pMesh->PlayingTime() >= 0.5f)
			{
				for (int i = 0; i < 2; ++i)
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Hard_End" && m_pMesh->PlayingTime() >= 0.2f)
			{
				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_AttackType(Attack_KnocBack);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(true);
					SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Attack", { 1,2 }, 0.3f, false);
				}
			}
		}
		break;
	case Em200::Dead:
		if (m_bIng == true)
			m_pMesh->PlayAnimation("Death_Front", false, {}, 1.5f, 20.f, true);
		m_pCollider.lock()->SetActive(false);
		m_bDead = true;
		StoneDebrisPlayStart();
		break;
	case Em200::Hit_Air:
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
	case Em200::Hit_Air_Start:
		if (m_bHit == true)
		{
			m_bAir = true;
			m_pMesh->PlayAnimation("Hit_Air_Start", false, {}, 1.f, 20.f, true);

			m_pCollider.lock()->AddForce({ 0.f, 1.8f,0.f });


			if (m_pMesh->CurPlayAnimInfo.Name == "Hit_Air_Start" && 
				m_pMesh->PlayingTime()>=0.1f&&m_pCollider.lock()->IsGround() )
			{
				m_eState = Hit_Air_Loop;
				m_pCollider.lock()->SetGravity(true);
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Hit_Air_Start" && m_pMesh->PlayingTime()>=0.8f)
			{
				m_eState = Hit_Air_Loop;
				m_pCollider.lock()->SetGravity(true);
			}

		}
		break;
	case Em200::Hit_Air_Snatch_Start:
		if (m_bHit == true)
		{
			m_bAir = true;
			m_pMesh->PlayAnimation("Hit_Air_Start", false, {}, 1.f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Hit_Air_Start" &&
				m_pMesh->PlayingTime() >= 0.1f && m_pCollider.lock()->IsGround())
			{
				m_eState = Hit_Air_Loop;
				m_pCollider.lock()->SetGravity(true);
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Hit_Air_Start" && m_pMesh->PlayingTime() >= 0.8f)
			{
				m_eState = Hit_Air_Loop;
				m_pCollider.lock()->SetGravity(true);
			}

		}
		break;
	case Em200::Hit_Air_Loop:
		if (m_bHit == true)
		{
			m_pMesh->PlayAnimation("Hit_Air_Loop", true, {}, 1.f, 20.f, true);

			m_pCollider.lock()->AddForce({ 0.f, 1.8f,0.f });

			if (m_pMesh->CurPlayAnimInfo.Name == "Hit_Air_Loop" && m_pCollider.lock()->IsGround())
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
	case Em200::Hit_Back:
		break;
	case Em200::Hit_Finish:
		break;
	case Em200::Hit_Front:
		if (m_bHit == true)
		{
			m_pMesh->PlayAnimation("Hit_Front", false, {}, 0.8f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Hit_Front" && m_pMesh->IsAnimationEnd())
			{
				int iRandom = FMath::Random<int>(1, 3);

				if (iRandom == 1)
				{
					m_eState = Idle;
					SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Idle", { 1,5 }, 0.3f, false);
				}
				else if (iRandom == 2)
				{
					m_eState = Idle2;
					SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Idle", { 1,5 }, 0.3f, false);
				}
				else if (iRandom == 3)
				{
					m_eState = Idle3;
					SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Idle", { 1,5 }, 0.3f, false);
				}

				m_bHit = false;
				m_bIng = false;
			}
		}
		break;
	case Em200::Hit_L:
		if (m_bHit == true)
		{
			m_pMesh->PlayAnimation("Hit_L", false, {}, 0.6f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Hit_L" && m_pMesh->IsAnimationEnd())
			{
				int iRandom = FMath::Random<int>(1, 3);

				if (iRandom == 1)
				{
					m_eState = Idle;
					SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Idle", { 1,5 }, 0.3f, false);
				}
				else if (iRandom == 2)
				{
					m_eState = Idle2;
					SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Idle", { 1,5 }, 0.3f, false);
				}
				else if (iRandom == 3)
				{
					m_eState = Idle3;
					SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Idle", { 1,5 }, 0.3f, false);
				}

				m_bHit = false;
				m_bIng = false;
			}
		}
		break;
	case Em200::Hit_R:
		if (m_bHit == true)
		{
			m_pMesh->PlayAnimation("Hit_R", false, {}, 0.6f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Hit_R" && m_pMesh->IsAnimationEnd())
			{
				int iRandom = FMath::Random<int>(1, 3);

				if (iRandom == 1)
				{
					m_eState = Idle;
					SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Idle", { 1,5 }, 0.3f, false);
				}
				else if (iRandom == 2)
				{
					m_eState = Idle2;
					SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Idle", { 1,5 }, 0.3f, false);
				}
				else if (iRandom == 3)
				{
					m_eState = Idle3;
					SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Idle", { 1,5 }, 0.3f, false);
				}
				m_bHit = false;
				m_bIng = false;
			}
		}
		break;
	case Em200::Hit_KnocBack:
		//���󰡴°�
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
	case Em200::Hit_End_Front:
		if (m_bHit == true)
		{
			m_pMesh->PlayAnimation("Hit_Air_End", false, {}, 1.2f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Hit_Air_End" && m_pMesh->IsAnimationEnd())
				m_eState = Downword_Down_StandUp;
		}
		break;
	case Em200::Hit_End_Back:
		if (m_bHit == true)
		{
			m_pMesh->PlayAnimation("Blown_Front_Landing", false, {}, 1.f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Blown_Front_Landing" && m_pMesh->IsAnimationEnd())
				m_eState = Downword_Down_StandUp;
		}
		break;
	case Em200::Downword_Down_StandUp:
		if (m_bDown == true)
		{
			m_pMesh->PlayAnimation("Downward_Down_StandUp", false, {}, 1.f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Downward_Down_StandUp" && m_pMesh->IsAnimationEnd())
			{
				//�ĸ¾Ƽ� �Ѿ��������� �Ͼ�� ����� ����
				//�׷��ٴ°� ���� �������ʰ�, ���� �ൿ �ص� �ǰ�(m_bing == false), �Ѿ��� �͵� ������.
				m_bHit = false;
				m_bIng = false;
				m_bDown = false;
				int iRandom = FMath::Random<int>(1, 3);

				if (iRandom == 1)
				{
					m_eState = Idle;
					SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Idle", { 1,5 }, 0.3f, false);
				}
				else if (iRandom == 2)
				{
					m_eState = Idle2;
					SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Idle", { 1,5 }, 0.3f, false);
				}
				else if (iRandom == 3)
				{
					m_eState = Idle3;
					SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Idle", { 1,5 }, 0.3f, false);
				}
			}
		}
		break;
	case Em200::Downword_Damage:
		if (m_bDown && m_bHit == true)
		{
			m_pMesh->PlayAnimation("Downward_Damage", false, {}, 1.f, 10.f);
			m_bHit = false;
		}
		if (m_pMesh->CurPlayAnimInfo.Name == "Downward_Damage" && m_pMesh->IsAnimationEnd())
			m_eState = Downword_Down_StandUp;
		break;
	case Em200::Walk_Front_End:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Move_Front_End", false, {}, 1.f, 50.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Move_Front_End" && m_pMesh->IsAnimationEnd())
			{
				int iRandom = FMath::Random<int>(1, 3);

				if (iRandom == 1)
				{
					m_eState = Idle;
					SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Idle", { 1,5 }, 0.3f, false);
				}
				else if (iRandom == 2)
				{
					m_eState = Idle2;
					SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Idle", { 1,5 }, 0.3f, false);
				}
				else if (iRandom == 3)
				{
					m_eState = Idle3;
					SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Idle", { 1,5 }, 0.3f, false);
				}

				m_bMove = false;
				m_bIng = false;
			}
		}
		break;
	case Em200::Walk_Front_Loop:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Move_Front_Loop", true, {}, 1.f, 50.f, true);
			Update_Angle();
			m_bInteraction = true;

			if (fDir <= 0.5f)
				m_eState = Walk_Front_End;
		}
		break;
	case Em200::Walk_Front_Start:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Move_Front_Start", false, {}, 1.f, 50.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Move_Front_Start" && m_pMesh->IsAnimationEnd())
				m_eState = Walk_Front_Loop;
		}
		break;
	case Em200::Idle:
		m_pMesh->PlayAnimation("Idle", true, {}, 1.f, 50.f, true);
		m_BattleInfo.eAttackType = Attack_END;
		if (m_bIng)
			m_bIng = false;
		if (m_bHit)
			m_bHit = false;
		if (m_bBuster)
			m_bBuster = false;
		if (m_bMove)
			m_bMove = false;

		break;
	case Em200::Idle2:
		m_pMesh->PlayAnimation("Idle2", true, {}, 1.f, 50.f, true);
		m_BattleInfo.eAttackType = Attack_END;
		if (m_bIng)
			m_bIng = false;
		if (m_bHit)
			m_bHit = false;
		if (m_bBuster)
			m_bBuster = false;
		if (m_bMove)
			m_bMove = false;
		break;
	case Em200::Idle3:
		m_pMesh->PlayAnimation("Idle3", true, {}, 1.f, 50.f, true);
		m_BattleInfo.eAttackType = Attack_END;
		if (m_bIng)
			m_bIng = false;
		if (m_bHit)
			m_bHit = false;
		if (m_bBuster)
			m_bBuster = false;
		if (m_bMove)
			m_bMove = false;
		break;
	case Em200::Hit_Buster_Start:
		if (m_bHit)
		{
			if (m_bBuster == false)
			{
				Vector3 PlayerPos = m_pPlayerTrans.lock()->GetPosition();
				Vector3 vLook = m_pPlayerTrans.lock()->GetLook();

				Vector3 vResult = PlayerPos + (-vLook * 0.1f);
				vResult.y = m_pTransform.lock()->GetPosition().y;
				m_pTransform.lock()->SetPosition(vResult);

				Update_Angle();
				Set_Rotate();
				SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Hit", { 1,5 }, 0.3f, false);
				m_bBuster = true;
			}

			m_pMesh->PlayAnimation("Buster_Start", false, {}, 1.f, 1.f, true);
			m_pCollider.lock()->SetTrigger(true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Buster_Start" && m_pMesh->IsAnimationEnd())
			{
				m_eState = Hit_Buster_Loop;
				SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Hit", { 1,5 }, 0.3f, false);
			}
		}
		break;
	case Em200::Hit_Buster_Loop:
		if (m_bHit == true)
		{
			m_pMesh->PlayAnimation("Buster_Loop", true, {}, 1.f, 1.f, true);
			
			SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Hit", { 1,5 }, 0.2f, false);
			if (m_pPlayer.lock()->Get_CurAnimationIndex() == Nero::ANI_EM200_BUSTER_FINISH)
			{
				m_pPlayer.lock()->PlayEffect(Eff_Buster);
				m_eState = Hit_Buster_End;
				SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Hit", { 1,5 }, 0.3f, false);
			}
			
		}
		break;
	case Em200::Hit_Buster_End:
		if (m_bHit)
		{
			m_pMesh->PlayAnimation("Buster_Finish", false, {}, 1.f, 1.f, true);
			Vector3 vRot(0.f, 0.f, 0.f);
			SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Hit", { 1,5 }, 0.2f, false);
			if (m_pMesh->CurPlayAnimInfo.Name == "Buster_Finish" && m_pMesh->PlayingTime() >= 0.6f)
			{
				m_pCollider.lock()->SetRigid(true);
				m_pCollider.lock()->SetTrigger(false);
			}
			if (m_pMesh->CurPlayAnimInfo.Name == "Buster_Finish" && m_pMesh->IsAnimationEnd())
			{
				m_eState = Hit_End_Front;
				m_bBuster = false;
				m_bIng = false;
				m_BattleInfo.iHp -= int(m_BattleInfo.iMaxHp / 2);
				SoundSystem::GetInstance()->Play("BusterEnd", 0.5f, false);
				SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Hit", { 1,5 }, 0.2f, false);
			}

		}
		break;
	case Em200::Hit_Air_Buster_Start:
		if (m_bHit)
		{
			m_pCollider.lock()->SetGravity(false);
			m_pCollider.lock()->SetTrigger(true);


			if (m_bBuster == false)
			{
				Vector3 PlayerPos = m_pPlayerTrans.lock()->GetPosition();
				Vector3 vLook = m_pPlayerTrans.lock()->GetLook();

				Vector3 vResult = PlayerPos + (-vLook * 0.1f);
				vResult.y = m_pTransform.lock()->GetPosition().y;
				m_pTransform.lock()->SetPosition(vResult);

				Update_Angle();
				Set_Rotate();
				SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Hit", { 1,5 }, 0.3f, false);
				m_bBuster = true;
			}


			m_pMesh->PlayAnimation("Air_Buster_Start", false, {}, 1.f, 1.f, true);
			m_pCollider.lock()->SetTrigger(true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Air_Buster_Start" && m_pMesh->IsAnimationEnd())
			{
				m_eState = Hit_Air_Buster_Loop;
				SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Hit", { 1,5 }, 0.3f, false);
			}
		}
		break;
	case Em200::Hit_Air_Buster_Loop:
		if (m_bHit == true)
		{
			m_pMesh->PlayAnimation("Air_Buster_Loop", true, {}, 1.f, 1.f, true);
			SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Hit", { 1,5 }, 0.2f, false);
			if (m_pPlayer.lock()->Get_CurAnimationIndex() == Nero::ANI_EM200_BUSTER_AIR_FINISH)
			{
				m_pPlayer.lock()->PlayEffect(Eff_Buster);
				m_eState = Hit_Air_Buster_End;
			}

		}
		break;
	case Em200::Hit_Air_Buster_End:
		if (m_bHit)
		{
			m_pMesh->PlayAnimation("Air_Buster_Finish", false, {}, 1.f, 1.f, true);
			Vector3 vRot(0.f, 0.f, 0.f);
			SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Hit", { 1,5 }, 0.2f, false);
			if (m_pMesh->CurPlayAnimInfo.Name == "Air_Buster_Finish" && m_pMesh->PlayingTime() >= 0.95f)
			{
				m_pCollider.lock()->SetRigid(true);
				m_pCollider.lock()->SetTrigger(false);
				m_pCollider.lock()->SetGravity(true);

				Vector3 vLook = m_pPlayerTrans.lock()->GetLook();

				m_vPower += -vLook;
				m_vPower.y = 0.f;


				D3DXVec3Normalize(&m_vPower, &m_vPower);
				m_fPower = 120.f;
				m_pCollider.lock()->AddForce(m_vPower * m_fPower);

				m_vPower.x = 0.f;
				m_vPower.z = 0.f;
				m_fPower = 100.f;

				m_bBuster = false;
				m_bIng = false;
				m_eState = Hit_KnocBack;
				SoundSystem::GetInstance()->Play("BusterEnd", 0.5f, false);
				SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Hit", { 1,5 }, 0.2f, false);
			}
		}
		break;
	case Em200::Hit_Split_Start:
		if (m_bHit && m_bAir == true)
		{
			m_pMesh->PlayAnimation("Slam_Damage_fall_loop", true, {}, 1.f, 20.f, true);
			Update_Angle();
			Set_Rotate();
			if (m_pPlayer.lock()->Get_CurAnimationIndex() == Nero::ANI_SPLIT_LANDING
				&& m_pCollider.lock()->IsGround())
			{
				m_eState = Hit_End_Front;
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
	case Em200::Hit_Snatch_Start:
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
	case Em200::Hit_Snatch_End:
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
			
				SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Idle", { 1,5 }, 0.3f, false);

			}
		}
		break;
	case Em200::Enter_Ground:
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
		{
			m_eState = Idle;
			SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Idle", { 1,5 }, 0.3f, false);
		}
		break;
	}


}

void Em200::Skill_CoolTime(const float _fDeltaTime)
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

HRESULT Em200::Ready()
{
	Monster::Ready();
	//GameObject�� �޾ƿ����� ���� �±װ� �־����.
	m_nTag = Monster200;

	m_BattleInfo.iMaxHp = 20000;
	m_BattleInfo.iHp = 20000;
	m_BattleInfo.iAttack = 20;

	m_pTransform.lock()->SetPosition({ -4.8f, 1.2f, -4.82f });
		
	RenderInit();
	// Ʈ������ �ʱ�ȭ�ϸ� Edit �� ������ ǥ�õǵ��� Ǫ�� . 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.001,0.001,0.001 });
	PushEditEntity(InitTransform.lock().get());

	// �������� ������ �ް���� ������Ʈ�� Raw �����ͷ� Ǫ��.
	// 	PushEditEntity(_ShaderInfo.GetShader(RenderProperty::Order::ForwardAlphaBlend).get());

	//���� ȸ�� �⺻ �ӵ�
	m_fAngleSpeed = D3DXToRadian(100.f);


	m_fPower = 100.f;
	m_vPower = D3DXVECTOR3(0.f, 1.f, 0.5f);
	return S_OK;
}

HRESULT Em200::Awake()
{
	Unit::Awake();

	m_pCollider = AddComponent<CapsuleCollider>();
	m_pCollider.lock()->ReadyCollider();
	PushEditEntity(m_pCollider.lock().get());

	for (int i = 0; i < 2; ++i)
	{
		m_pHand[i] = AddGameObject<Em200Hand>();
		m_pHand[i].lock()->m_pEm200 = static_pointer_cast<Em200>(m_pGameObject.lock());
		m_pHand[i].lock()->m_pEm200Mesh = m_pMesh;
		m_pHand[i].lock()->m_bLeft = (bool)i;
	}
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);


	m_pCollider.lock()->SetRigid(true);
	m_pCollider.lock()->SetGravity(true);

	m_pCollider.lock()->SetRadius(0.07f);
	m_pCollider.lock()->SetHeight(0.14f);
	m_pCollider.lock()->SetCenter({ 0.f, 0.15f, 0.f });

	m_pPlayer = std::static_pointer_cast<Nero>(FindGameObjectWithTag(GAMEOBJECTTAG::Player).lock());
	m_pPlayerTrans = m_pPlayer.lock()->GetComponent<ENGINE::Transform>();
	m_pRedQueen = std::static_pointer_cast<RedQueen>(FindGameObjectWithTag(GAMEOBJECTTAG::TAG_RedQueen).lock());
	
	m_pPlayerBone = m_pPlayer.lock()->Get_BoneMatrixPtr("CoatCloth_04_03");

	//���� �ʱ���� Idle
	m_eState = Enter_Ground;

	

	/*--- ����Ʈ ---*/
	m_pBlood = AddGameObject<Liquid>();
	m_pAppear = AddGameObject<AppearGroundMonster>();
	StoneDebrisInit();
	/*----------------*/

	return S_OK;
}

HRESULT Em200::Start()
{
	Unit::Start();
	return S_OK;
}

UINT Em200::Update(const float _fDeltaTime)
{
	Unit::Update(_fDeltaTime);

	// ���� �����ϰ� ȸ���� �ǹ̰� ���� DeltaPos �� Ʈ���������� ���� . 
	auto [DeltaScale, DeltaQuat, DeltaPos] = m_pMesh->Update(_fDeltaTime);
	Vector3 Axis = { 1,0,0 };
	  
	//ENGINE::AnimNotify _Notify{};
	////return true �� ���� ȣ�� ����, false�� �� ���� ���� ��� ȣ��.
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


	if (m_eState == Hit_Split_Start)
	{
		Vector3 vRedQueenPos = m_pRedQueen.lock()->GetRedQueenBoneWorldPos("_001");
		m_pTransform.lock()->SetPosition(vRedQueenPos);
	}






	/////////////////////////////
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
	/////////////////////////////

	return 0;
}

UINT Em200::LateUpdate(const float _fDeltaTime)
{
	Unit::LateUpdate(_fDeltaTime);
	return 0;
}

void Em200::Editor()
{
	Unit::Editor();
	if (bEdit)
	{
		m_pDissolve.DissolveEditor();
	}
}

void Em200::OnEnable()
{
	Unit::OnEnable();
	_RenderProperty.bRender = true;
}

void Em200::OnDisable()
{
	Unit::OnDisable();

	_RenderProperty.bRender = false;

}

void Em200::Hit(BT_INFO _BattleInfo, void* pArg)
{
	AddRankScore(_BattleInfo.iAttack);
	m_BattleInfo.iHp -= _BattleInfo.iAttack;

	/*--- �� ����Ʈ ---*/
	if (!m_pBlood.expired())
	{
		int iRandom = FMath::Random<int>(0, 6);
		if (iRandom >= 4)
			++iRandom;

		CalcEffectPos();
		m_vEffectPos += GetMonsterBoneWorldPos("Waist");

		auto pBlood = m_pBlood.lock();
		pBlood->SetVariationIdx(Liquid::VARIATION(iRandom));	// 0 6 7 �� �ڿ��������?
		pBlood->SetPosition(GetMonsterBoneWorldPos("Waist"));
		pBlood->SetScale(0.0085f);
		//pBlood->SetRotation()	// ��Ȳ�� �°� ���� ����
		pBlood->PlayStart(40.f);
	}
	/*----------------*/
	
	if (m_bDown==false)
	{
		switch (_BattleInfo.eAttackType)
		{
		case ATTACKTYPE::Attack_L:
			m_eState = Hit_L;
			m_bHit = true;
			break;
		case ATTACKTYPE::Attack_R:
			m_eState = Hit_Front;
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
			m_fPower = 80.f;

			m_vPower.x *= 1.2f;
			m_vPower.z *= 1.2f;
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
			Vector3	vDir(vLook.x * 0.06f, 0.82f, vLook.z * 0.06f);

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
			m_fPower = 80.f;

			m_vPower.x *= 1.2f;
			m_vPower.z *= 1.2f;
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
			Vector3	vDir(vLook.x * 0.06f, 0.82f, vLook.z * 0.06f);

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
			m_pCollider.lock()->AddForce(m_vPower* m_fPower);

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

void Em200::Air_Hit(BT_INFO _BattleInfo, void* pArg)
{
	AddRankScore(_BattleInfo.iAttack);
	m_BattleInfo.iHp -= _BattleInfo.iAttack;

	m_pCollider.lock()->SetGravity(false);

	/*--- �� ����Ʈ ---*/
	if (!m_pBlood.expired())
	{
		int iRandom = FMath::Random<int>(0, 6);
		if (iRandom >= 4)
			++iRandom;

		auto pBlood = m_pBlood.lock();
		pBlood->SetVariationIdx(Liquid::VARIATION(iRandom));	// 0 6 7 �� �ڿ��������?
		pBlood->SetPosition(GetMonsterBoneWorldPos("Waist"));
		pBlood->SetScale(0.008f);
		//pBlood->SetRotation()	// ��Ȳ�� �°� ���� ����
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

void Em200::OnCollisionEnter(std::weak_ptr<GameObject> _pOther)
{
	Monster::OnCollisionEnter(_pOther);
}

void Em200::Buster(BT_INFO _BattleInfo, void* pArg)
{
	if ((m_BattleInfo.iHp > _BattleInfo.iAttack))
		m_BattleInfo.iHp -= _BattleInfo.iAttack;

	m_bHit = true;
	m_bDown = true;
	m_pCollider.lock()->SetRigid(false);

	if (m_bAir)
		m_eState = Hit_Air_Buster_Start;
	else
		m_eState = Hit_Buster_Start;
}

void Em200::Snatch(BT_INFO _BattleInfo, void* pArg)
{
	m_bHit = true;
	m_eState = Hit_Snatch_Start;
}

void Em200::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
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
			m_pHand[i].lock()->Set_Coll(false);
			m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
		}
		break;
	case GAMEOBJECTTAG::Tag_Cbs_Middle:
		if (m_bAir)
			Air_Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		else
			Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());

		for (int i = 0; i < 2; ++i)
		{
			m_pHand[i].lock()->Set_Coll(false);
			m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
		}
		break;
	case GAMEOBJECTTAG::Tag_Cbs_Short:
		if (m_bAir)
			Air_Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		else
			Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());

		for (int i = 0; i < 2; ++i)
		{
			m_pHand[i].lock()->Set_Coll(false);
			m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
		}
		break;
	case GAMEOBJECTTAG::Tag_Cbs_Long:
		if (m_bAir)
			Air_Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		else
			Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());

		for (int i = 0; i < 2; ++i)
		{
			m_pHand[i].lock()->Set_Coll(false);
			m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
		}
		break;
	case GAMEOBJECTTAG::TAG_BusterArm_Right:
		_pOther.lock()->GetComponent<SphereCollider>().lock()->SetActive(false);
		Buster(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());

		for (int i = 0; i < 2; ++i)
		{
			m_pHand[i].lock()->Set_Coll(false);
			m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
		}
		break;
	case GAMEOBJECTTAG::TAG_WireArm:
		Snatch(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());

		for (int i = 0; i < 2; ++i)
		{
			m_pHand[i].lock()->Set_Coll(false);
			m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
		}
		break;
	case GAMEOBJECTTAG::Overture:
		if (m_bAir)
			Air_Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		else
			Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());

		for (int i = 0; i < 2; ++i)
		{
			m_pHand[i].lock()->Set_Coll(false);
			m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
		}
		break;
	default:
		break;
	}
	HitEffectPlay(_pOther);
	SoundSystem::GetInstance()->RandSoundKeyPlay("Em200Hit", { 1,5 }, 0.3f, false);
}

void Em200::OnTriggerExit(std::weak_ptr<GameObject> _pOther)
{

}

void Em200::RenderGBufferSK(const DrawInfo& _Info)
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

void Em200::RenderShadowSK(const DrawInfo& _Info)
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

void Em200::RenderDebugBone(const DrawInfo& _Info)
{
	const Matrix ScaleOffset = FMath::Scale({ 0.01,0.01 ,0.01 });
	m_pMesh->BoneDebugRender(_RenderUpdateInfo.World, _Info.Fx);
}

void Em200::RenderDebugSK(const DrawInfo& _Info)
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

void Em200::RenderInit()
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

	m_pDissolve.Initialize("..\\..\\Resource\\Mesh\\Dynamic\\Monster\\Em200\\Em200.fbx",
		Vector3{
			1.f,0.f,0.f
		});


	RenderInterface::Initialize(_InitRenderProp);
	Mesh::InitializeInfo _InitInfo{};
	// ���ؽ� ���� ������ CPU ������ �ʿ� �Ѱ� ? 
	_InitInfo.bLocalVertexLocationsStorage = false;
	m_pMesh = Resources::Load<ENGINE::SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Monster\\Em200\\Em200.fbx", _InitInfo);
	m_pMesh->LoadAnimationFromDirectory(L"..\\..\\Resource\\Mesh\\Dynamic\\Monster\\Em200\\Ani");
	m_pMesh->AnimationDataLoadFromJsonTable(L"..\\..\\Resource\\Mesh\\Dynamic\\Monster\\Em200\\Em200.Animation");

	m_pMesh->EnableToRootMatricies();
	PushEditEntity(m_pMesh.get());
	
}

void Em200::Rotate(const float _fDeltaTime)
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

void Em200::Update_Angle()
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

void Em200::Set_Rotate()
{
	m_pTransform.lock()->Rotate({ 0.f, -D3DXToDegree(m_fRadian), 0.f });
}

void Em200::SetGravity(bool _bActiveOrNot)
{
	m_pCollider.lock()->SetGravity(_bActiveOrNot);
}
