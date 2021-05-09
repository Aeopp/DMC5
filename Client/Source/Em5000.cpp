#include "stdafx.h"
#include "Em5000.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include "TestObject.h"
#include "Car.h"
#include "Nero.h"
#include <filesystem>
#include "Em5000Hand.h"
#include "Liquid.h"


void Em5000::Free()
{
	Unit::Free();
}

std::string Em5000::GetName()
{
	return "Em5000";
}

Em5000* Em5000::Create()
{
	return new Em5000{};
}

void Em5000::Fight(const float _fDeltaTime)
{
	Skill_CoolTime(_fDeltaTime);

	Vector3	 vDir = m_pPlayerTrans.lock()->GetPosition() - m_pTransform.lock()->GetPosition();
	Vector3	 vLook = m_pTransform.lock()->GetLook();
	float	 fDir = D3DXVec3Length(&vDir);

	if (!m_bGroggy)
	{
		if (m_BattleInfo.iHp <= 0.f)
		{
			m_eState = Dead;
			m_bIng = true;
		}
	}
	else
	{
		if (m_BattleInfo.iHp <= 0.f)
		{
			m_eState = Groggy_Dead;
			m_bIng = true;
			m_bHit = true;
		}
	}




	D3DXVec3Normalize(&vDir, &vDir);
	//왼쪽 오른쪽 구분하기 위한 외적 결과 값
	Vector3  vCross;
	D3DXVec3Cross(&vCross, &vLook, &vDir);
	//앞 뒤 구분하기 위한 내적 결과 값
	float fDot = D3DXVec3Dot(&vDir, &vLook);
	float fRadian = acosf(fDot);
	float fDegree = D3DXToDegree(fRadian);

	//거리가 멀때만 이동 or 회전을 함.
	//거리가 가까우면 공격으로 회전을 시킬 수 있음
	if (fDir >= 2.f)
	{
		if (m_bThrow && m_bIng == false)
		{
			Turn_To_Car();
			m_bIng = true;
			return;
		}
		if (m_bTurn && m_bIng == false)
		{
			m_bIng = true;
			Turn();
			return;
		}
		if (m_bMove && m_bIng == false)
		{
			m_bIng = true;
			int iRandom = FMath::Random<int>(1, 7);

			if (iRandom == 1 || iRandom == 2)
				m_eState = Back_Jump;
			else
				m_eState = Move_Start;

			return;
		}
		if (m_bJumpAttack && fDir >= 2.5f && fDir <=3.f)
		{
			if (m_eState == Move_Start || m_eState == Move_Loop)
			{
				m_eState = Attack_Jump_Attack;
				return;
			}
		}
	}
	else
	{
		if (m_bAttack && m_bIng == false)
		{
			m_bIng = true;
			int iRandom = FMath::Random<int>(1, 2);
			//공격 쿨 돌았고 왼쪽 or 오른쪽에 있으면 공격
			if (fDegree >= 65.0f && fDegree <= 115.0f)
			{
				if (vCross.y > 0)
				{
					m_eState = Attack_Side_L;
					m_fMoveTime = 0.f;
					return;
				}
				else
				{
					m_eState = Attack_Side_R;
					m_fMoveTime = 0.f;
					return;
				}
			}
			//공격 쿨 돌았고 뒤에 있으면 백 공격
			if (fDegree <= 65.f && fDegree >= 0.f)
			{
				if (vCross.y > 0)
				{
					m_eState = Attack_Back_L;
					m_fMoveTime = 0.f;
					return;
				}
				else
				{
					m_eState = Attack_Back_R;
					m_fMoveTime = 0.f;
					return;
				}
			}
			if (iRandom == 1)
			{
				m_eState = Attack_Punch_Twice;
				return;
			}
			else
			{
				m_eState = Attack_Hack;
				return;
			}
		}
		if (m_bRushAttack && m_bIng == false)
		{
			int iRush = FMath::Random<int>(1, 3);

			if (iRush == 1)
			{
				m_bIng = true;
				Update_Angle();
				Set_Rotate();
				
				m_eState = Back_Jump;
				return;
			}
		}
	}


}

void Em5000::State_Change(const float _fDeltaTime)
{
	Vector3	 vDir = m_pPlayerTrans.lock()->GetPosition() - m_pTransform.lock()->GetPosition();
	Vector3	 vLook = m_pTransform.lock()->GetLook();
	float	 fDir = D3DXVec3Length(&vDir);


	Vector3 vCarDir(1.f,1.f,1.f); //= m_pTransform.lock()->GetPosition() - m_pCarTrans.lock()->GetPosition();
	float   fCarDir = 1.f; //= D3DXVec3Length(&vCarDir);


	switch (m_eState)
	{
	case Em5000::Attack_Back_L:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Attack_Back_L", false, {}, 1.5f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Back_L" && m_pMesh->IsAnimationEnd())
			{
				int iRandom = FMath::Random<int>(1, 4);

				m_eState = Idle;
				m_bIng = false;
				m_bAttack = false;

				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_Coll(false);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
				}
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Back_L" && m_pMesh->PlayingTime() >= 0.4f)
			{
				for (int i = 0; i < 2; ++i)
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Back_L" && m_pMesh->PlayingTime() >= 0.2f)
			{
				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_AttackType(Attack_Front);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(true);
					m_pHand[i].lock()->Set_Coll(true);
				}
			}
		}
		break;
	case Em5000::Attack_Back_R:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Attack_Back_R", false, {}, 1.5f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Back_R" && m_pMesh->IsAnimationEnd())
			{
				int iRandom = FMath::Random<int>(1, 4);

				m_eState = Idle;
				m_bIng = false;
				m_bAttack = false;

				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_Coll(false);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
				}
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Back_R" && m_pMesh->PlayingTime() >= 0.4f)
			{
				for (int i = 0; i < 2; ++i)
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Back_R" && m_pMesh->PlayingTime() >= 0.2f)
			{
				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_AttackType(Attack_KnocBack);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(true);
					m_pHand[i].lock()->Set_Coll(true);
				}
			}
		}
		break;
	case Em5000::Attack_Finish:
		break;
	case Em5000::Attack_Grap_Car:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Attack_Grap_Car", false, {}, 1.f, 10.f, true);


			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Grap_Car" && m_pMesh->PlayingTime() >= 0.65f
				&& m_pMesh->PlayingAccTime() <= 0.8f)
			{
				Update_Angle();
				m_bInteraction = true;
			}

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Grap_Car" && m_pMesh->PlayingTime() >= 0.95f)
				m_eState = Attack_Throw_Car;
		}
		break;
	case Em5000::Attack_Hack:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Attack_Hack", false, {}, 1.5f, 50.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Hack" && m_pMesh->IsAnimationEnd())
			{
				int iRandom = FMath::Random<int>(1, 4);

				m_eState = Idle;
				m_bIng = false;
				m_bAttack = false;

				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_Coll(false);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
				}
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Hack" && m_pMesh->PlayingTime() >= 0.4f)
			{
				for (int i = 0; i < 2; ++i)
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Hack" && m_pMesh->PlayingTime() >= 0.2f)
			{
				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_AttackType(Attack_KnocBack);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(true);
					m_pHand[i].lock()->Set_Coll(true);
				}
			}
		}
		break;
	case Em5000::Attack_Jump_Attack:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Attack_Jump_Attack", false, {}, 1.5f, 10.f, true);

			if (m_pMesh->PlayingTime() >= 0.1f && m_pMesh->PlayingTime() <= 0.4f)
			{
				Update_Angle();
				m_bInteraction = true;
			}

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Jump_Attack" && m_pMesh->IsAnimationEnd())
			{
				int iRandom = FMath::Random<int>(1, 4);

				m_eState = Idle;
				m_bIng = false;
				m_bJumpAttack = false;
				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_Coll(false);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
				}
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Jump_Attack" && m_pMesh->PlayingTime() >= 0.4f)
			{
				for (int i = 0; i < 2; ++i)
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Jump_Attack" && m_pMesh->PlayingTime() >= 0.2f)
			{
				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_AttackType(Attack_KnocBack);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(true);
					m_pHand[i].lock()->Set_Coll(true);
				}
			}
		}
		break;
	case Em5000::Attack_Punch_Twice:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Attack_Punch_Twice", false, {}, 1.5f, 50.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Punch_Twice" && m_pMesh->IsAnimationEnd())
			{
				int iRandom = FMath::Random<int>(1, 4);

				m_eState = Idle;
				m_bIng = false;
				m_bAttack = false;

				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_Coll(false);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
				}
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Punch_Twice" && m_pMesh->PlayingTime() >= 0.5f)
			{
				for (int i = 0; i < 2; ++i)
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Punch_Twice" && m_pMesh->PlayingTime() >= 0.2f)
			{
				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_AttackType(Attack_KnocBack);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(true);
					m_pHand[i].lock()->Set_Coll(true);
				}
			}
		}
		break;
	case Em5000::Attack_Rush_Start:
		if (m_bIng == true)
		{
			Update_Angle();
			m_bInteraction = true;
			m_pMesh->PlayAnimation("Attack_Rush_Start", false, {}, 1.f, 50.f, true);
			
			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Rush_Start" && fDir <= 2.5f)
				m_eState = Attack_Rush_End;


			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Rush_Start" && m_pMesh->PlayingTime() >= 0.95f)
				m_eState = Attack_Rush_Loop;
		}
		break;
	case Em5000::Attack_Rush_Loop:
		if (m_bIng == true)
		{
			Update_Angle();
			m_bInteraction = true;
			m_pMesh->PlayAnimation("Attack_Rush_Loop", false, {}, 1.f, 10.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Rush_Loop" && fDir <= 3.5f)
				m_eState = Attack_Rush_End;
		}
		break;
	case Em5000::Attack_Rush_End:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Attack_Rush_End", false, {}, 1.f, 10.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Rush_End" && m_pMesh->IsAnimationEnd())
			{
				int iRandom = FMath::Random<int>(1, 4);

				m_eState = Idle;
				m_bIng = false;
				m_bMove = false;
				m_bRushAttack = false;

				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_Coll(false);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
				}
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Rush_End" && m_pMesh->PlayingTime() >= 0.3f)
			{
				for (int i = 0; i < 2; ++i)
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Rush_End" && m_pMesh->PlayingTime() >= 0.1f)
			{
				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_AttackType(Attack_KnocBack);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(true);
					m_pHand[i].lock()->Set_Coll(true);
				}
			}
		}
		break;
	case Em5000::Attack_Side_L:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Attack_Side_L", false, {}, 1.f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Side_L" && m_pMesh->IsAnimationEnd())
			{
				int iRandom = FMath::Random<int>(1, 4);

				m_eState = Idle;
				m_bIng = false;
				m_bAttack = false;
				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_Coll(false);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
				}
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Side_L" && m_pMesh->PlayingTime() >= 0.4f)
			{
				for (int i = 0; i < 2; ++i)
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Side_L" && m_pMesh->PlayingTime() >= 0.2f)
			{
				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_AttackType(Attack_KnocBack);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(true);
					m_pHand[i].lock()->Set_Coll(true);
				}
			}
		}
		break;
	case Em5000::Attack_Side_R:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Attack_Side_R", false, {}, 1.f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Side_R" && m_pMesh->IsAnimationEnd())
			{
				int iRandom = FMath::Random<int>(1, 4);

				m_eState = Idle;
				m_bIng = false;
				m_bAttack = false;
				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_Coll(false);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
				}
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Side_R" && m_pMesh->PlayingTime() >= 0.4f)
			{
				for (int i = 0; i < 2; ++i)
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Side_R" && m_pMesh->PlayingTime() >= 0.2f)
			{
				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_AttackType(Attack_KnocBack);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(true);
					m_pHand[i].lock()->Set_Coll(true);
				}
			}
		}
		break;
	case Em5000::Attack_Throw_Car:
		if (m_bIng == true)
		{
			Update_Angle();
			m_bInteraction = true;
			m_pMesh->PlayAnimation("Attack_Throw_Car", false, {}, 1.f, 10.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Throw_Car" && m_pMesh->PlayingTime() >= 0.95f)
			{
				m_bIng = false;
				m_bThrow = false;
				m_eState = Idle;
			}
		}
		break;
	case Em5000::Back_Jump:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Back_Jump", false, {}, 1.f, 10.f, true);

			if (m_pMesh->PlayingTime()>=0.9f && m_pMesh->CurPlayAnimInfo.Name == "Back_Jump")
			{
				if (m_bRushAttack)
				{
					m_eState = Attack_Rush_Start;
					m_bMove = false;
					return;
				}

				if (m_bJumpAttack && fDir >= 2.f && fDir <= 2.5f)
				{
					m_eState = Attack_Jump_Attack;
					m_bMove = false;
					return;
				}
				
				m_bIng = false;
				m_bMove = false;
				m_bBackJump = false;
				m_eState = Idle;
			}
		}

		break;
	case Em5000::Dead:
		if (m_bHit)
		{
			m_pMesh->PlayAnimation("Dead", false, {}, 1.f, 20.f, true);
		}
		break;
	case Em5000::Groggy_Dead:
		if (m_bGroggy)
		{
			m_pMesh->PlayAnimation("Groggy_Dead", false, {}, 1.f, 20.f, true);
		}
		break;
	case Em5000::Groggy_Start:
		if (m_bGroggy)
		{
			m_pMesh->PlayAnimation("Groggy_Start", false, {}, 1.f, 10.f, true);

			for (int i = 0; i < 2; ++i)
			{
				m_pHand[i].lock()->Set_Coll(false);
				m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
			}
			if (m_pMesh->CurPlayAnimInfo.Name == "Groggy_Start" && m_pMesh->IsAnimationEnd())
				m_eState = Groggy_Loop;
		}
		break;
	case Em5000::Groggy_Loop:
		if (m_bGroggy)
		{
			m_pMesh->PlayAnimation("Groggy_Loop", false, {}, 1.f, 10.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Groggy_Loop" && m_pMesh->IsAnimationEnd())
				m_eState = Groggy_End;
		}
		break;
	case Em5000::Groggy_End:
		if (m_bGroggy)
		{
			m_pMesh->PlayAnimation("Groggy_End", false, {}, 1.f, 10.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Groggy_End" && m_pMesh->IsAnimationEnd())
			{
				m_eState = Idle;
				m_bGroggy = false;
			
				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_Coll(false);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
				}
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Groggy_End" && m_pMesh->PlayingTime() >= 0.5f)
			{
				for (int i = 0; i < 2; ++i)
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
			}
			else if (m_pMesh->CurPlayAnimInfo.Name == "Groggy_End" && m_pMesh->PlayingTime() >= 0.2f)
			{
				for (int i = 0; i < 2; ++i)
				{
					m_pHand[i].lock()->Set_AttackType(Attack_KnocBack);
					m_pHand[i].lock()->m_pCollider.lock()->SetActive(true);
					m_pHand[i].lock()->Set_Coll(true);
				}
			}
		}
		break;
	case Em5000::Hit_Buster_Start:
		if (m_bHit)
		{
			if (m_bBuster == false)
			{
				Vector3 PlayerPos = m_pPlayerTrans.lock()->GetPosition();
				Vector3 vLook = m_pPlayerTrans.lock()->GetLook();

				Vector3 vResult = PlayerPos + (-vLook * 0.56f);
				vResult.y = PlayerPos.y;
				m_pTransform.lock()->SetPosition(vResult);

				Update_Angle();
				Set_Rotate();
				m_bBuster = true;
			}
			m_pMesh->PlayAnimation("Hit_Buster_Start", false, {}, 1.f, 20.f, true);
			m_pCollider.lock()->SetTrigger(true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Hit_Buster_Start" && m_pMesh->IsAnimationEnd())
			{
				m_eState = Hit_Buster_Swing_Start;
				m_bBuster = false;
			}
		}
		break;
	case Em5000::Hit_Buster_Swing_Start:
		if (m_bHit == true)
		{
			m_pMesh->PlayAnimation("Hit_Buster_Swing_Start", false, {}, 1.1f, 1.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Hit_Buster_Swing_Start" && m_pMesh->IsAnimationEnd())
			{
				m_eState = Hit_Buster_Swing_Loop;
				m_bBuster = false;
			}
		}
		break;
	case Em5000::Hit_Buster_Swing_Loop:
		if (m_bHit == true)
		{
			Update_Angle();
			m_pMesh->PlayAnimation("Hit_Buster_Swing_Loop", true, {}, 1.3f, 1.f, true);
			if (m_pPlayer.lock()->Get_CurAnimationIndex() == Nero::ANI_EM5000_BUSTER_FINISH
				&&m_pPlayer.lock()->Get_PlayingTime() >= 0.12f)
			{
				Vector3 PlayerPos = m_pPlayerTrans.lock()->GetPosition();
				Vector3 vLook = m_pPlayerTrans.lock()->GetLook();

				Vector3 vResult = PlayerPos + (-vLook * 0.56f);
				vResult.y = m_pTransform.lock()->GetPosition().y;
				m_pTransform.lock()->SetPosition(vResult);

				Update_Angle();
				Set_Rotate();

				m_eState = Hit_Buster_Swing_Throw;
			}
		}
		break;
	case Em5000::Hit_Buster_Swing_Throw:
		if (m_bHit == true)
		{
			m_pCollider.lock()->SetRigid(true);
			m_pCollider.lock()->SetTrigger(false);

			m_pMesh->PlayAnimation("Hit_Buster_Swing_Throw", false, {}, 1.f, 1.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Hit_Buster_Swing_Throw" && m_pMesh->IsAnimationEnd())
				m_eState = Hit_Buster_Swing_End;
				
		}
		break;
	case Em5000::Hit_Buster_Swing_End:
		if (m_bHit)
		{
			m_pMesh->PlayAnimation("Hit_Buster_Swing_End", false, {}, 1.f, 1.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Hit_Buster_Swing_End" && m_pMesh->IsAnimationEnd())
			{
				m_eState = Hit_Buster_Standup;
				m_bBuster = false;
			}
		}
		break;
	case Em5000::Hit_Buster_Standup:
		if (m_bHit)
		{
			m_pMesh->PlayAnimation("Buster_Standup", false, {}, 1.f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Buster_Standup" && m_pMesh->IsAnimationEnd())
			{
				m_eState = Idle;
				m_bIng = false;
				m_bHit = false;
				m_bBuster = false;
			}
		}
		break;
	case Em5000::Howling:
		break;
	case Em5000::Idle:
		m_pMesh->PlayAnimation("Idle", true, {}, 1.f, 50.f, true);
		break;
	case Em5000::Move_Loop:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Move_Loop", true, {}, 1.3f, 50.f, true);
			m_bInteraction = true;
			Update_Angle();

			if (fDir <= 3 && fDir >= 2.5f && m_bJumpAttack)  
			{
				m_eState = Attack_Jump_Attack;
				m_bMove = false;
				return;
			}
			if (fDir <= 1.2f)
			{
				m_bIng = false;
				m_bMove = false;
			}
		}
		break;
	case Em5000::Move_Start:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Move_Start", false, {}, 1.3f, 30.f, true);
			if(m_pMesh->CurPlayAnimInfo.Name == "Move_Start" && m_pMesh->PlayingTime()>= 0.2f)
			{
				Update_Angle();
				m_bInteraction = true;
			}
			if (m_pMesh->CurPlayAnimInfo.Name == "Move_Start" && m_pMesh->PlayingTime() >= 0.9f)
			{
				m_eState = Move_Loop;
			}
		}
		break;
	case Em5000::Tornado_End:
		break;
	case Em5000::Tornado_Start:
		break;
	case Em5000::Move_Turn_L_90:
		break;
	case Em5000::Move_Turn_L_180:
		break;
	case Em5000::Move_Turn_R_90:
		break;
	case Em5000::Move_Turn_R_180:
		break;
	case Em5000::Move_Turn_L:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Move_Turn_L", false, {}, 1.2f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Move_Turn_L" && m_pMesh->PlayingTime() >= 0.2f && m_pMesh->PlayingTime() <= 0.6f)
			{
				m_bInteraction = true;
				Update_Angle();
			}
			if (m_pMesh->CurPlayAnimInfo.Name == "Move_Turn_L" && m_pMesh->PlayingTime() >= 0.95f)
			{
				m_eState = Idle;
				m_bIng = false;
				m_bTurn = false;
			}
		}
		break;
	case Em5000::Move_Turn_R:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Move_Turn_R", false, {}, 1.2f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Move_Turn_R" && m_pMesh->PlayingTime() >= 0.2f && m_pMesh->PlayingTime() <= 0.6f)
			{
				m_bInteraction = true;
				Update_Angle();
			}
			if (m_pMesh->CurPlayAnimInfo.Name == "Move_Turn_R" && m_pMesh->PlayingTime() >= 0.95f)
			{
				m_eState = Idle;
				m_bIng = false;
				m_bTurn = false;
			}
		}
		break;
	case Em5000::Car_Turn_L:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Move_Turn_L", false, {}, 1.2f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Move_Turn_L" && m_pMesh->PlayingTime() >= 0.2f && m_pMesh->PlayingTime() <= 0.6f)
			{
				m_bInteraction = true;
				Update_Angle_ToCar();
			}
			if (m_pMesh->CurPlayAnimInfo.Name == "Move_Turn_L" && m_pMesh->PlayingTime() >= 0.85f)
				m_eState = Car_Rush_Start;
		}
		break;
	case Em5000::Car_Turn_R:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Move_Turn_R", false, {}, 1.2f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Move_Turn_R" && m_pMesh->PlayingTime() >= 0.2f && m_pMesh->PlayingTime() <= 0.6f)
			{
				m_bInteraction = true;
				Update_Angle_ToCar();
			}
			if (m_pMesh->CurPlayAnimInfo.Name == "Move_Turn_R" && m_pMesh->PlayingTime() >= 0.85f)
				m_eState = Car_Rush_Start;
		}
		break;
	case Em5000::Car_Rush_Start:
		if (m_bIng == true)
		{
			Update_Angle_ToCar();
			m_bInteraction = true;

			m_pMesh->PlayAnimation("Attack_Rush_Start", false, {}, 1.f, 50.f, true);
				
			if (fCarDir <= 8.5f)
			{
				m_eState = Attack_Grap_Car;
				return;
			}
			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Rush_Start" && m_pMesh->PlayingTime() >= 0.9f)
				m_eState = Car_Rush_Loop;
		}
		break;
	case Em5000::Car_Rush_Loop:
		if(m_bIng == true)
		{
			m_pMesh->PlayAnimation("Attack_Rush_Loop", true, {}, 1.f, 10.f, true);
			Update_Angle_ToCar();
			m_bInteraction = true;

			if (fCarDir <= 8.5f)
				m_eState = Attack_Grap_Car;

		}
		break;
	default:
		break;
	}

}

void Em5000::Skill_CoolTime(const float _fDeltaTime)
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
	if (m_bSideAttack == false)
	{
		m_fSideAttackTime += _fDeltaTime;
		if (m_fSideAttackTime >= 5.f)
		{
			m_bSideAttack = true;
			m_fSideAttackTime = 0.f;
		}
	}
	if (m_bTurn == false)
	{
		m_fTurnTime += _fDeltaTime;
		if (m_fTurnTime >= 5.f)
		{
			m_bTurn = true;
			m_fTurnTime = 0.f;
		}
	}
	if (m_bRushAttack == false)
	{
		m_fRushAttackTime += _fDeltaTime;
		if (m_fRushAttackTime >= 10.f)
		{
			m_bRushAttack = true;
			m_fRushAttackTime = 0.f;
		}
	}
	if (m_bJumpAttack == false)
	{
		m_fJumpAttackTime += _fDeltaTime;
		if (m_fJumpAttackTime >= 15.f)
		{
			m_bJumpAttack = true;
			m_fJumpAttackTime = 0.f;
		}
	}
	//if (m_bThrow == false)
	//{
	//	m_fThrowTime += _fDeltaTime;
	//	if (m_fThrowTime >= 20.f)
	//	{
	//		m_bThrow = true;
	//		m_fThrowTime = 0.f;
	//	}
	//}
	if (m_bBackJump == false)
	{
		m_fBackJumpTime += _fDeltaTime;
		if (m_fBackJumpTime >= 10.f)
		{
			m_bBackJump = true;
			m_fBackJumpTime = 0.f;
		}
	}

}

HRESULT Em5000::Ready()
{
	Unit::Ready();
	//GameObject를 받아오려면 각자 태그가 있어야함.
	m_nTag = Monster5000;

	RenderInit();

	// 트랜스폼 초기화하며 Edit 에 정보가 표시되도록 푸시 . 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.001,0.001,0.001 });
	PushEditEntity(InitTransform.lock().get());

	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.
	// 	PushEditEntity(_ShaderInfo.GetShader(RenderProperty::Order::ForwardAlphaBlend).get());


	//몬스터 회전 기본 속도
	m_fAngleSpeed = D3DXToRadian(100.f);

	m_pTransform.lock()->SetPosition({ 0.3f, 1.f, -1.22f });

	m_pMesh->EnableToRootMatricies();
	return S_OK;
}

HRESULT Em5000::Awake()
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

	for (UINT i = 0; i < 2; ++i)
	{
		m_pHand[i] = AddGameObject<Em5000Hand>();
		m_pHand[i].lock()->m_pEm5000 = static_pointer_cast<Em5000>(m_pGameObject.lock());
		m_pHand[i].lock()->m_pEm5000Mesh = m_pMesh;
		m_pHand[i].lock()->m_bLeft = (bool)i;
	}
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);

	m_pCollider.lock()->SetRigid(true);
	m_pCollider.lock()->SetGravity(true);
	
	m_pCollider.lock()->SetRadius(0.4f);
	m_pCollider.lock()->SetHeight(0.f);
	m_pCollider.lock()->SetCenter({ 0.f,0.4f,-0.1f });

	m_pBlood = AddGameObject<Liquid>();
	return S_OK;
}

HRESULT Em5000::Start()
{
	Unit::Start();
	return S_OK;
}

UINT Em5000::Update(const float _fDeltaTime)
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

		 //SpTransform->Translate(DeltaPos * SpTransform->GetScale().x);

		 SpTransform->SetPosition(SpTransform->GetPosition() + DeltaPos * SpTransform->GetScale().x);
	}
	//플레이어가 사라졌는지 판단
	/*if (false == m_pPlayer.expired())
	{
		std::cout << "Player Dead" << std::endl;
	}*/

	if (Input::GetKeyDown(DIK_T))
	{
		if (m_bTest == true)
			m_bTest = false;
		else
			m_bTest = true;
	}
	if (Input::GetKeyDown(DIK_Y))
	{
		m_eState = Groggy_Start;
		m_bGroggy = true;
	}


	if (m_bTest == true)
	{
		Fight(_fDeltaTime);
	}
	State_Change(_fDeltaTime);
	Rotate(_fDeltaTime);


	if (m_eState == Dead
		&& m_pMesh->IsAnimationEnd())
	{
		for (int i = 0; i < 2; ++i)
			Destroy(m_pHand[i]);
		Destroy(m_pGameObject);
	}
	if (m_eState == Groggy_Dead
		&& m_pMesh->IsAnimationEnd())
	{
		for (int i = 0; i < 2; ++i)
			Destroy(m_pHand[i]);
		Destroy(m_pGameObject);
	}


	return 0;
}

UINT Em5000::LateUpdate(const float _fDeltaTime)
{
	Unit::LateUpdate(_fDeltaTime);
	return 0;
}

void Em5000::Editor()
{
	Unit::Editor();
	if (false == bEdit)
		return;
}

void Em5000::OnEnable()
{
	Unit::OnEnable();
}

void Em5000::OnDisable()
{
	Unit::OnDisable();
}

void Em5000::Hit(BT_INFO _BattleInfo, void* pArg)
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

void Em5000::Buster(BT_INFO _BattleInfo, void* pArg)
{
	m_BattleInfo.iHp -= _BattleInfo.iAttack;

	m_bHit = true;
	m_pCollider.lock()->SetRigid(false);

	m_eState = Hit_Buster_Start;
}

void Em5000::Snatch(BT_INFO _BattleInfo, void* pArg)
{
}

void Em5000::RenderGBufferSK(const DrawInfo& _Info)
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

void Em5000::RenderShadowSK(const DrawInfo& _Info)
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

void Em5000::RenderDebugBone(const DrawInfo& _Info)
{
	const Matrix ScaleOffset = FMath::Scale({ 0.01,0.01 ,0.01 });
	m_pMesh->BoneDebugRender(_RenderUpdateInfo.World, _Info.Fx);
}

void Em5000::RenderDebugSK(const DrawInfo& _Info)
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

void Em5000::RenderInit()
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
	m_pMesh = Resources::Load<ENGINE::SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Monster\\Em5000\\Em5000.x", _InitInfo);
	m_pMesh->EnableToRootMatricies();
	// 모션 블러 사용시 켜기 
	// m_pMesh->EnablePrevVTF();
	PushEditEntity(m_pMesh.get());
	//몬스터 초기상태 idle
	m_pMesh->PlayAnimation("Idle", true);

	
}

void Em5000::Rotate(const float _fDeltaTime)
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

void Em5000::Update_Angle()
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

void Em5000::Set_Rotate()
{
	m_pTransform.lock()->Rotate({ 0.f, -D3DXToDegree(m_fRadian), 0.f });
}

void Em5000::OnCollisionEnter(std::weak_ptr<GameObject> _pOther)
{
	Monster::OnCollisionEnter(_pOther);
}

void Em5000::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
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
		for (int i = 0; i < 2; ++i)
			m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
		break;
	case GAMEOBJECTTAG::TAG_BusterArm_Right:
		if (m_bGroggy)
		{
			_pOther.lock()->GetComponent<SphereCollider>().lock()->SetActive(false);
			Buster(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		}
		break;
	case GAMEOBJECTTAG::Overture:
		m_BattleInfo.iHp -= static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo().iAttack;
		m_bHit = true;
		break;
	case GAMEOBJECTTAG::Tag_Cbs_Middle:
		Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		for (int i = 0; i < 2; ++i)
			m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
		break;
	case GAMEOBJECTTAG::Tag_Cbs_Short:
		Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		for (int i = 0; i < 2; ++i)
			m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
		break;
	case GAMEOBJECTTAG::Tag_Cbs_Long:
		Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo());
		for (int i = 0; i < 2; ++i)
			m_pHand[i].lock()->m_pCollider.lock()->SetActive(false);
		break;
	default:
		break;
	}
}

void Em5000::Turn()
{
	Vector3	 vDir = m_pPlayerTrans.lock()->GetPosition() - m_pTransform.lock()->GetPosition();
	Vector3	 vLook = m_pTransform.lock()->GetLook();
	float	 fDir = D3DXVec3Length(&vDir);

	D3DXVec3Normalize(&vDir, &vDir);
	//왼쪽 오른쪽 구분하기 위한 외적 결과 값
	Vector3  vCross;
	D3DXVec3Cross(&vCross, &vLook, &vDir);
	//앞 뒤 구분하기 위한 내적 결과 값
	float fDot = D3DXVec3Dot(&vDir, &vLook);


	//
	//fDot> 0 //플레이어가 앞에 있다.
	//fDot< 0 //플레이어가 뒤에 있다.
	//vCross.y> 0 //플레이어가 왼쪽에 있다
	//vCross.y< 0 // 플레이어가 오른쪽에 있다

	//플레이어가 앞에 있어요
	if (fDot > 0)
	{
		//플레이어가 왼쪽에 있어요
		if (vCross.y > 0)
			m_eState = Move_Turn_L;
		//플레이어가 오른쪽에 있어요
		else
			m_eState = Move_Turn_R;
	}
	//플레이어가 뒤에 있어요
	else
	{
		if (vCross.y > 0)
			m_eState = Move_Turn_L;
		//플레이어가 오른쪽에 있어요
		else
			m_eState = Move_Turn_R;
	}


}


void Em5000::Turn_To_Car()
{
	Vector3	 vDir2 = m_pCarTrans.lock()->GetPosition() - m_pTransform.lock()->GetPosition();
	Vector3	 vLook2 = m_pTransform.lock()->GetLook();
	float	 fDir2 = D3DXVec3Length(&vDir2);

	D3DXVec3Normalize(&vDir2, &vDir2);
	Vector3  vCross2;
	D3DXVec3Cross(&vCross2, &vLook2, &vDir2);
	float fDot2 = D3DXVec3Dot(&vDir2, &vLook2);

	if (fDot2 > 0)
	{
		if (vCross2.y > 0)
			m_eState = Car_Turn_L;
		else
			m_eState = Car_Turn_R;
	}
	else
	{
		if (vCross2.y > 0)
			m_eState = Car_Turn_L;
		else
			m_eState = Car_Turn_R;
	}
}

void Em5000::Update_Angle_ToCar()
{
	Vector3 vCarPos = m_pCarTrans.lock()->GetPosition();
	Vector3 vMyPos = m_pTransform.lock()->GetPosition();

	Vector3 vDir = vCarPos - vMyPos;
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


	if (m_fRadian > 0)
		m_fAngleSpeed = fabs(m_fAngleSpeed);
	else
		m_fAngleSpeed = -fabs(m_fAngleSpeed);

}
