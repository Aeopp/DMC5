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
#include "Trigger.h"
#include "BtlPanel.h"
#include "Em5300Ulte.h"
#include "SnatchPoint.h"
#include "ArtemisMissile.h"
#include "LensFlare.h"
#include "ShockWave.h"
#include "Reverberation.h"
#include "ParticleSystem.h"
#include "Renderer.h"
#include "Energism.h"
#include "EnergismReady.h"

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



	if (m_BattleInfo.iHp <= 450.f)
	{
		if (m_bUlte == false)
		{
			for (int i = 0; i < 32; ++i)
				m_pBullet[i].lock()->m_pMissile.lock()->PlayEnd();
			for (int i = 0; i < 8; ++i)
				m_pHoming[i].lock()->m_pMissile.lock()->PlayEnd();
			for (int i = 0; i < 12; ++i)
				m_pRain[i].lock()->m_pMissile.lock()->PlayEnd();


			m_pTrigger.lock()->TriggerEnable();
			m_bIng = true;
			m_bUlte = true;
			m_eState = Attack_Ulte_Move;
			SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Back", { 1,5 }, 5.f, false);
			if (m_fCenterY >= -0.2f)
			{
				m_fCenterY -= 0.01f;
				m_pCollider.lock()->SetCenter({ 0.f, m_fCenterY, 0.f });
			}
		}
	}
	if (m_BattleInfo.iMaxHp <= 0.f && m_bDead == false)
	{
		m_bIng = true;
		m_bDead = true;
		m_bHit = true;
		m_eState = Dead;
		SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Dead", { 1,1 }, 0.5f, false);
	}

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
		int iRandom = FMath::Random<int>(1, 5);
		
		if (m_bMove && m_bIng == false)
		{
			m_eState = Move_Back_Start;
			SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Back", { 1,5 }, 5.f, false);
			m_bIng = true;
		}
		
		/*	else
			{
				if (m_bIng == false)
				{
					m_eState = Attack_Rush_Start;
					SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Attack", { 1,5 }, 0.8f, false);
					m_bIng = true;
				}
			}*/
	
	}

	if (m_bLaser && m_bIng == false)
	{
		m_eState = Attack_Laser_Start;
		SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Attack", { 1,5 }, 0.8f, false);
		m_bIng = true;
	}

	if (m_bHoming && m_bIng == false)
	{
		m_eState = Attack_Homing_Start;
		SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Attack", { 1,5 }, 0.8f, false);
		m_bIng = true;
	}

	if (m_bMissile && m_bIng == false)
	{
		int iRan = FMath::Random<int>(1, 4);
		if (iRan == 1)
		{
			m_eState = Attack_Missile2_Start;
			SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Attack", { 1,5 }, 0.8f, false);
		}
		else
		{
			m_eState = New_Missile_Start;
			SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Attack", { 1,5 }, 0.8f, false);
		}

		m_bIng = true;
	}
	if (m_bRain && m_bIng == false)
	{
		m_eState = Attack_Rain_Start;
		m_bIng = true;
		SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Attack", { 1,5 }, 0.8f, false);
	}
	if (m_bRushAttack && m_bIng == false)
	{
		m_eState = Attack_Rush_Start;
		SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Attack", { 1,5 }, 0.8f, false);
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
				SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Idle", { 1,5 }, 0.8f, false);
				SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Missile3", { 1,1 }, 0.8f, false);
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
					m_pHoming[i].lock()->Set_AttackType(Attack_KnocBack);
					m_pHoming[i].lock()->Set_Coll(true);
				}
				SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Missile3", { 1,1 }, 0.5f, false);

				Matrix Head = *m_pMesh->GetToRootMatrixPtr("Head") * m_pTransform.lock()->GetWorldMatrix();
				Vector3 vHead = { Head._41, Head._42, Head._43 };
				vHead.y -= 0.2f;
				m_pLens.lock()->PlayStart(vHead);
				m_bLens = true;
				m_pShockWave.lock()->PlayStart(vHead, ShockWave::Option::ArtemisLaunch);
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
			{
				m_eState = Attack_Homing_Loop;

				for (int i = 0; i < 8; ++i)
					m_pHoming[i].lock()->m_pMissile.lock()->PlayStart();
			}
		}
		break;
	case Em5300::Attack_Laser_End:
		if (m_bIng == true)
		{
			m_pMesh->PlayAnimation("Attack_Laser_End", false, {}, 0.8f, 20.f, true);

			if (m_fCenterY >= -0.2f)
			{
				m_fCenterY -= 0.01f;
				m_pCollider.lock()->SetCenter({ 0.f, m_fCenterY, 0.f });
			}

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Laser_End" && m_pMesh->IsAnimationEnd())
			{
				SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Idle", { 1,5 }, 0.8f, false);
				m_eState = Idle;
				m_bIng = false;
				m_bLaser = false;
				m_bLaserReady = false;
				m_bLaserStart = false;
				m_fAngleSpeed = D3DXToRadian(100.f);
			}
		}
		break;
	case Em5300::Attack_Laser_Loop:
		if (m_bIng == true)
		{
		
			Update_Angle();
			m_bInteraction = true;
			m_fAngleSpeed = D3DXToRadian(15.f);
			Matrix Head = *m_pMesh->GetToRootMatrixPtr("Head");
			Matrix Result = *Head * m_pTransform.lock()->GetWorldMatrix();

			Vector3 vResult = { Result._41, Result._42, Result._43 };
			Vector3 vLook = m_pTransform.lock()->GetLook();
			m_pMesh->PlayAnimation("Attack_Laser_Loop", false, {}, 0.5f, 20.f, true);
			SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Laser2", { 1,1 }, 0.5f, false);
			const float HeadRotation = FMath::ToDegree(std::atan2f(vLook.x, vLook.z));
			
			if (m_bLaserStart == false)
			{
				const Vector3 EnerPosition = vResult + -vLook * m_fEnergismLookOffset;
				m_pEner.lock()->PlayStart(vResult, HeadRotation);

				m_bLaserStart = true;
			}

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Laser_Loop" && m_pMesh->IsAnimationEnd())
			{
				if(m_iLaserCnt <= 3)
					++m_iLaserCnt;
				else
				{
					m_eState = Attack_Laser_End;
					m_pEner.lock()->Set_Coll(false);
					m_pEner.lock()->m_pCollider.lock()->SetActive(false);
					m_iLaserCnt = 0;
				}
			}

			
		}
		break;
	case Em5300::Attack_Laser_Start:
		if (m_bIng == true)
		{
			if (m_fCenterY < 0.1f)
			{
				m_fCenterY += 0.006f;
				m_pCollider.lock()->SetCenter({ 0.f, m_fCenterY, 0.f });
			}		
			if (m_fCenterY >= 0.1f)
			{
				Update_Angle();
				m_bInteraction = true;
			}
			m_pMesh->PlayAnimation("Attack_Laser_Start", false, {}, 1.f, 20.f, true);

			

			Matrix Result = *m_pHeadBone * m_pTransform.lock()->GetWorldMatrix();

			Vector3 vResult = { Result._41, Result._42, Result._43 };

			if (m_bLaserReady == false)
			{
				Vector3 EnerReadyPosition = vResult + -GetComponent<Transform>().lock()->GetLook() * m_fEnergismLookOffset;
				m_pEnerReady.lock()->PlayStart(vResult);
				m_bLaserReady = true;
				SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Laser", { 1,1 }, 0.5f, false);
			}

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Laser_Start" && m_pMesh->IsAnimationEnd())
			{
				m_eState = Attack_Laser_Loop;
				m_pEner.lock()->Set_Coll(true);
				m_pEner.lock()->m_pCollider.lock()->SetActive(true);
				m_pEner.lock()->Set_AttackType(Attack_KnocBack);
			}
		}
		break;
	case Em5300::Attack_Missile2_Start:
		if (m_bIng)
		{
			Update_Angle();
			m_bInteraction = true;
			m_pMesh->PlayAnimation("Attack_Missile_Start", false, {}, 1.f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Missile_Start" && m_pMesh->IsAnimationEnd())
			{
				m_eState = Attack_Missile2_Loop;
				for (int i = 0; i < 16; ++i)
					m_pBullet[i].lock()->m_pMissile.lock()->PlayStart();
			}
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
			Update_Angle();
			m_bInteraction = true;
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
			if (m_pBullet[3].lock()->Get_StartMissile2() && m_bLens == false)
			{
				Matrix Head = *m_pMesh->GetToRootMatrixPtr("Head") * m_pTransform.lock()->GetWorldMatrix();
				Vector3 vHead = { Head._41, Head._42, Head._43 };
				vHead.y -= 0.2f;
				m_pLens.lock()->PlayStart(vHead);
				m_bLens = true;
				m_pShockWave.lock()->PlayStart(vHead, ShockWave::Option::ArtemisLaunch);
			}

			m_pMesh->PlayAnimation("Attack_Missile_End", false, {}, 1.f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Missile_End" && m_pMesh->IsAnimationEnd())
			{
				m_eState = Idle;
				m_bIng = false;
				m_bMissile = false;
				m_bLens = false;
			}
		}
		break;
	case Em5300::New_Missile_End:
		if (m_bIng)
		{
			if (m_pBullet[0].lock()->Get_StartMissile() && m_bLens == false)
			{
				Matrix Head = *m_pMesh->GetToRootMatrixPtr("Head") * m_pTransform.lock()->GetWorldMatrix();
				Vector3 vHead = { Head._41, Head._42, Head._43 };
				vHead.y -= 0.2f;
				m_pLens.lock()->PlayStart(vHead);
				m_bLens = true;
				m_pShockWave.lock()->PlayStart(vHead, ShockWave::Option::ArtemisLaunch);
			}

			m_pMesh->PlayAnimation("New_Missile_End", false, {}, 0.6f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "New_Missile_End" && m_pMesh->IsAnimationEnd())
			{
				SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Idle", { 1,5 }, 0.8f, false);
				m_bIng = false;
				m_bMissile = false;
				m_bLens = false;
				m_eState = Idle;
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
			{
				m_eState = New_Missile_Loop;

				for (int i = 0; i < 32; ++i)
					m_pBullet[i].lock()->m_pMissile.lock()->PlayStart();

			}
		}
		break;
	case Em5300::Attack_Rain_End:
		if (m_bIng)
		{
			m_pMesh->PlayAnimation("Attack_Rain_End", false, {}, 1.f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Rain_End" && m_pMesh->IsAnimationEnd())
			{
				SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Idle", { 1,5 }, 0.8f, false);
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

					m_pRain[i].lock()->m_pMissile.lock()->PlayStart();
				}

				Matrix Head = *m_pMesh->GetToRootMatrixPtr("Head") * m_pTransform.lock()->GetWorldMatrix();
				Vector3 vHead = { Head._41, Head._42, Head._43 };
				vHead.y -= 0.2f;
				m_pLens.lock()->PlayStart(vHead);
				m_bLens = true;
				m_pShockWave.lock()->PlayStart(vHead, ShockWave::Option::ArtemisLaunch);
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
				m_vRushDir.y = 0.f;
				m_bRushDir = false;
				m_fRushTime = 0.f;
				m_fOuterCricle = 0.f;
				m_fStartScaleAdder = 0.f;
				m_fEndScaleAdder = 0.f;
			}
			m_fRushTime += _fDeltaTime;
			m_fOuterCricle += _fDeltaTime;
			m_pTransform.lock()->Translate(m_vRushDir * 0.1f);

			SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Rush", { 1,1 }, 5.f, false);
			m_pMesh->PlayAnimation("Attack_Rush_Loop", true, {}, 1.f, 20.f, true);
			
			
			Vector3 vResult;
			if (Physics::RayCast(m_pTransform.lock()->GetPosition() + m_pCollider.lock()->GetCenter(), -m_pTransform.lock()->GetLook(),
				vResult))
				vResult -= m_pTransform.lock()->GetPosition() + m_pCollider.lock()->GetCenter();

			float	fDir2 = D3DXVec3Length(&vResult);
			
			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Rush_Loop" && fDir2 <= 1.f )
			{
				SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Idle", { 1,5 }, 0.8f, false);
				m_eState = Move_Front_End;
				m_fRushTime = 0.f;
				m_pRush.lock()->Set_Coll(false);
				m_pRush.lock()->SetActive(false);
				m_bRushLens = false;
			
			}
			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Rush_Loop" && m_pMesh->PlayingTime() >= 0.2f)
			{
				if (m_fCenterY <= 0.2f)
				{
					m_fCenterY += 0.01f;
					m_pCollider.lock()->SetCenter({ 0.f, m_fCenterY, 0.f });
				}
			}
	
			m_pRush.lock()->Set_Coll(true);
			m_pRush.lock()->SetActive(true);
			m_pRush.lock()->Set_AttackType(Attack_KnocBack);




			if (m_fOuterCricle >= 0.1f)
				m_bRushCricle = true;
			if (m_bRushCricle)
			{
				m_iRushIndex = (m_iRushIndex + 1) % 12;
				
				Matrix Head = *m_pMesh->GetToRootMatrixPtr("Head") * m_pTransform.lock()->GetWorldMatrix();
				Vector3 vHead = { Head._41, Head._42, Head._43 };
				m_pRushRever[m_iRushIndex].lock()->PlayStart(vHead, m_vRushDir, m_fStartScale + m_fStartScaleAdder, m_fEndScale + m_fEndScaleAdder);
				m_fOuterCricle = 0.f;
				m_fStartScaleAdder -= 0.0009f;
				m_fEndScaleAdder -= 0.0009f;
				m_bRushCricle = false;
			}
	
		}
		break;
	case Em5300::Attack_Rush_Start:
		if (m_bIng)
		{
			m_pMesh->PlayAnimation("Attack_Rush_Start", false, {}, 1.f, 20.f, true);
			Update_Angle();
			m_bInteraction = true;
			m_pCollider.lock()->SetTrigger(true);
			m_pCollider.lock()->SetRigid(false);

			if (m_fCenterY <= 0.2f)
			{
				m_fCenterY += 0.01f;
				m_pCollider.lock()->SetCenter({ 0.f, m_fCenterY, 0.f });
			}
			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Rush_Start" && m_pMesh->IsAnimationEnd())
			{
				m_eState = Attack_Rush_Loop;
				m_pCollider.lock()->SetTrigger(false);
				m_pCollider.lock()->SetRigid(true);

				Particle();
				
				if (m_bRushLens == false)
				{
					Matrix Head = *m_pMesh->GetToRootMatrixPtr("Head") * m_pTransform.lock()->GetWorldMatrix();
					Vector3 vHead = { Head._41, Head._42, Head._43 };
					m_pLens.lock()->PlayStart(vHead);
					m_bRushLens = true;
					m_pShockWave.lock()->PlayStart(vHead, ShockWave::Option::ArtemisRush);
				}


			}
		}
		break;
	case Em5300::Attack_Ulte_Move:
		if (m_bIng == true)
		{
			Update_Angle();
			m_bInteraction = true;
			Vector3 vCenter = { -37.148f, 0.f, 30.902f };

			Vector3 vUlteDir = vCenter - m_pTransform.lock()->GetPosition();
			D3DXVec3Normalize(&vUlteDir, &vUlteDir);
			float fUlteDir = D3DXVec3Length(&vUlteDir);
			m_pMesh->PlayAnimation("Move_Back_Loop", true, {}, 1.f, 20.f, true);
			m_pTransform.lock()->Translate({ vUlteDir.x * 0.02f, 0.f, vUlteDir.z * 0.02f });

		}
		break;
	case Em5300::Attack_Ulte_Start:
		if (m_bIng)
		{
			m_pMesh->PlayAnimation("Attack_Ulte_Start", false, {}, 1.f, 20.f, true);

			SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Ulte5", { 1,1 }, 0.5f, false);
			for (int i = 0; i < 16; ++i)
				m_pBullet[i].lock()->m_pMissile.lock()->PlayStart(true);
			for (int i = 0; i < 12; ++i)
				m_pRain[i].lock()->m_pMissile.lock()->PlayStart(true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Ulte_Start" && m_pMesh->IsAnimationEnd())
			{
				m_eState = Attack_Ulte_Loop;
				m_pUlte.lock()->Set_Ulte(true);
			}
		}
		break;
	case Em5300::Attack_Ulte_Loop:
		if (m_bIng)
		{
			SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Ulte", { 1,1 }, 0.5f, false);
			m_pMesh->PlayAnimation("Attack_Ulte_Loop", false, {}, 1.f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Ulte_Loop" && m_pMesh->IsAnimationEnd())
				m_eState = Attack_Ulte_Loop2;
		}
		break;
	case Em5300::Attack_Ulte_Loop2:
		if (m_bIng)
		{
			SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Ulte", { 1,1 }, 0.5f, false);
			m_pMesh->PlayAnimation("Attack_Ulte_Loop2", false, {}, 1.f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Ulte_Loop2" && m_pMesh->IsAnimationEnd())
			{
				m_eState = Attack_Ulte_End;

				for (int i = 0; i < 16; ++i)
					m_pBullet[i].lock()->m_pMissile.lock()->PlayEnd();
				for (int i = 0; i < 12; ++i)
					m_pRain[i].lock()->m_pMissile.lock()->PlayEnd();
			}
		}
		break;
	case Em5300::Attack_Ulte_End:
		if (m_bIng)
		{
		
			m_pMesh->PlayAnimation("Attack_Ulte_End", false, {}, 1.f, 20.f, true);
			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Ulte_End" && m_pMesh->IsAnimationEnd())
				m_eState = Attack_Ulte_End_Down;

			if(m_pMesh->CurPlayAnimInfo.Name == "Attack_Ulte_End" && m_pMesh->PlayingTime() <= 0.6f)
				SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Ulte", { 1,1 }, 0.5f, false);
		}
		break;
	case Em5300::Attack_Ulte_End_Down:
		if (m_bIng)
		{
			m_pMesh->PlayAnimation("Attack_Ulte_End_Down", false, {}, 1.f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Ulte_End_Down" && m_pMesh->IsAnimationEnd())
			{
				m_eState = Idle;
				m_pCollider.lock()->SetGravity(true);
				m_pCollider.lock()->SetRigid(true);
				m_pCollider.lock()->SetTrigger(false);
			}
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
	case Em5300::Down_Loop:
		if (m_bGroggy == true)
		{
			m_pMesh->PlayAnimation("Down_Loop", false, {}, 1.f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Down_Loop" && m_pMesh->IsAnimationEnd())
			{
				m_eState = Down_Standup;

				m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, false);
				m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, false);
				m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, false);
			}
		}
		break;
	case Em5300::Down_Standup:
		if (m_bGroggy == true)
		{
			m_pMesh->PlayAnimation("Down_Standup", false, {}, 1.f, 20.f, true);

		
			if (m_fCenterY >= -0.2f)
			{
				m_fCenterY -= 0.01f;
				m_pCollider.lock()->SetCenter({ 0.f, m_fCenterY, 0.f });
			}
			if (m_pMesh->CurPlayAnimInfo.Name == "Down_Standup" && m_pMesh->IsAnimationEnd())
			{
				SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Attack", { 1,5 }, 0.8f, false);
				m_eState = Idle;
				m_bIng = false;
				m_bGroggy = false;
				m_fCenterY = -0.2f;
				m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, false);
				m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, false);
				m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, false);
			}
		}
		break;
	case Em5300::Hit_Falling_End:
		if (m_bGroggy)
		{
			m_pMesh->PlayAnimation("Hit_Falling_End", false, {}, 3.f, 20.f, true);

			if (m_pMesh->CurPlayAnimInfo.Name == "Hit_Falling_End" && m_pMesh->IsAnimationEnd())
				m_eState = Down_Loop;
		}
		break;
	case Em5300::Hit_Falling_Loop:
		if (m_bGroggy)
		{
			m_pMesh->PlayAnimation("Hit_Falling_Loop", false, {}, 3.f, 20.f, true);

			if (m_fCenterY <= 0.35f)
			{
				m_fCenterY += 0.01f;
				m_pCollider.lock()->SetCenter({ 0.f, m_fCenterY, 0.f });
			}


			if (m_pMesh->CurPlayAnimInfo.Name == "Hit_Falling_Loop" && m_pMesh->IsAnimationEnd())
			{
				m_eState = Hit_Falling_End;
				m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, true);
				m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, true);
				m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, true);
			}
		}
		break;
	case Em5300::Hit_Falling_Start:
		if (m_bGroggy)
		{
			m_pMesh->PlayAnimation("Hit_Falling_Start", false, {}, 1.f, 20.f, true);
			m_pRush.lock()->Set_Coll(false);
			m_pRush.lock()->m_pCollider.lock()->SetActive(false);
			SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Fall", { 1,1 }, 0.5f, false);
			if (m_pMesh->CurPlayAnimInfo.Name == "Hit_Falling_Start" && m_pMesh->IsAnimationEnd())
				m_eState = Hit_Falling_Loop;
		}
		break;
	case Em5300::Idle:
		m_pMesh->PlayAnimation("Idle", true, {}, 1.f, 20.f, true);
		if (m_fCenterY >= -0.2f)
		{
			m_fCenterY -= 0.01f;
			m_pCollider.lock()->SetCenter({ 0.f, m_fCenterY, 0.f });
		}

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


			if (m_fCenterY >= -0.2f)
			{
				m_fCenterY -= 0.01f;
				m_pCollider.lock()->SetCenter({ 0.f, m_fCenterY, 0.f });
			}

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
	case Em5300::CutScene_Start:
		if(m_bCutStart == false)
		{
			m_bCutStart = true;
			Skill_CoolTime(_fDeltaTime);
			
			m_pMesh->PlayAnimation("Attack_Rush_Start", false, {}, 1.f, 20.f);
		}
		if (m_fCenterY <= 0.2f)
		{
			m_fCenterY += 0.01f;
			m_pCollider.lock()->SetCenter({ 0.f, m_fCenterY, 0.f });
		}
		if (m_pMesh->CurPlayAnimInfo.Name == "Attack_Rush_Start" && m_pMesh->IsAnimationEnd())
			m_eState = CutScene_End;
		break;
	case CutScene_End:
		if (m_bCutStart == true)
		{
			m_bCutStart = false;
			m_pMesh->PlayAnimation("Attack_Rush_Loop", true, {}, 1.f, 20.f);
		}
		if (m_fCenterY <= 0.2f)
		{
			m_fCenterY += 0.01f;
			m_pCollider.lock()->SetCenter({ 0.f, m_fCenterY, 0.f });
		}
		if (m_pTransform.lock()->GetPosition().x >= -38.7)
			m_pTransform.lock()->Translate({ -0.05f, 0.f, 0.f });
		else
		{
			m_pCollider.lock()->SetTrigger(false);
			m_pCollider.lock()->SetRigid(true);
			m_pCollider.lock()->SetGravity(true);
			m_eState = Idle;
			m_bCutScene = true;
		}
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
	if (m_bRushAttack == false)
	{
		m_fRushAttackTime += _fDeltaTime;
		if (m_fRushAttackTime >= 13.f)
		{
			m_bRushAttack = true;
			m_bRushDir = true;
			m_fRushAttackTime = 0.f;
		}
	}
	//퍼지는 미사일
	if (m_bMissile == false)
	{
		m_fMissileTime += _fDeltaTime;

		if (m_fMissileTime >= 12.f)
		{
			for (int i = 0; i < 32; ++i)
			{
				m_pBullet[i].lock()->Set_StartMissile2(false);
				m_pBullet[i].lock()->Set_StartMissile(false);
				m_pBullet[i].lock()->Set_Missile(false);
				m_pBullet[i].lock()->Set_Missile2(false);
				m_pBullet[i].lock()->Set_Coll(false);
				m_pBullet[i].lock()->m_pCollider.lock()->SetActive(false);
				m_pBullet[i].lock()->m_pMissile.lock()->PlayEnd();
				m_bLens = false;
			}

			m_bMissile = true;
			m_fMissileTime = 0.f;
		}
	}
	
	//// 위에서 떨어지는거
	if (m_bRain == false)
	{
		m_fRainTime += _fDeltaTime;
		if (m_fRainTime >= 15.f)
		{
			for (int i = 0; i < 12; ++i)
			{
				m_pRain[i].lock()->Set_StartRain(false);
				m_pRain[i].lock()->Set_Rain(false);
				m_pRain[i].lock()->Set_Coll(false);
				m_pRain[i].lock()->m_pCollider.lock()->SetActive(false);

				m_pRain[i].lock()->m_pMissile.lock()->PlayEnd();
			}

			m_bRain = true;
			m_fRainTime = 0.f;
		}
	}
	 
	////세로 미사일
	//if (m_bMissile2 == false)
	//{
	//	m_fMissile2Time += _fDeltaTime;

	//	if (m_fMissile2Time >= 7.f)
	//	{

	//		m_bMissile2 = true;
	//		m_fMissile2Time = 0.f;
	//	}
	//}

	////공중에 떠다니는 미사일
	if (m_bHoming == false)
	{
		m_fHomingTime += _fDeltaTime;

		if (m_fHomingTime >= 20.f)
		{
			m_bHoming = true;
			m_fHomingTime = 0.f;

			for (int i = 0; i < 8; ++i)
			{
				m_pHoming[i].lock()->Set_HomingStart(false);
				m_pHoming[i].lock()->Set_Homing(false);
				m_pHoming[i].lock()->m_pCollider.lock()->SetActive(false);
				m_pHoming[i].lock()->Set_Coll(false);

				m_pHoming[i].lock()->m_pMissile.lock()->PlayEnd();
			}
		}
	}

	if (m_bLaser == false)
	{
		m_fLaserTime += _fDeltaTime;

		if (m_fLaserTime >= 30.f)
		{
			m_bLaser = true;
			m_fLaserTime = 0.f;
		}
	}
}

HRESULT Em5300::Ready()
{
	Monster::Ready();
	//GameObject를 받아오려면 각자 태그가 있어야함.
	m_nTag = Monster5300;

	RenderInit();

	m_BattleInfo.iMaxHp = 1500;
	m_BattleInfo.iHp = m_BattleInfo.iMaxHp;


	// 트랜스폼 초기화하며 Edit 에 정보가 표시되도록 푸시 . 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.001,0.001,0.001 });
	PushEditEntity(InitTransform.lock().get());

	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.
	// 	PushEditEntity(_ShaderInfo.GetShader(RenderProperty::Order::ForwardAlphaBlend).get());


	//몬스터 회전 기본 속도
	m_fAngleSpeed = D3DXToRadian(100.f);


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

	m_pCollider = AddComponent<BoxCollider>();
	m_pRush	 = AddGameObject<Em5300Rush>();
	m_pRush.lock()->m_pEm5300 = static_pointer_cast<Em5300>(m_pGameObject.lock());
	m_pRush.lock()->m_pEm5300Mesh = m_pMesh;

	m_pLens = AddGameObject<LensFlare>();
	m_pShockWave = AddGameObject<ShockWave>();
	
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);

	m_pCollider.lock()->SetRigid(true);
	m_pCollider.lock()->SetGravity(true);
	 
	m_pCollider.lock()->SetSize({ 0.3f, m_fHeight, 0.3f });
	m_pCollider.lock()->SetCenter({ 0.f,m_fCenterY,0.f });
	//m_pCollider.lock()->SetCenter({ 0.f,f,0.f });

	m_pCollider.lock()->SetOffsetRadius(0.6f);

	PushEditEntity(m_pCollider.lock().get());
	m_pBlood = AddGameObject<Liquid>();
	
	for (int i = 0; i < 32; ++i)
	{
		m_pBullet[i] = AddGameObject<Em5300Missile>();
		m_pBullet[i].lock()->m_pEm5300 = static_pointer_cast<Em5300>(m_pGameObject.lock());
		m_pBullet[i].lock()->m_pEm5300Mesh = m_pMesh;
		m_pBullet[i].lock()->Set_MissilePos(i);
	}
	for (int i = 0; i < 10; ++i)
		m_pRever[i] = AddGameObject<Reverberation>();
	for (int i = 0; i < 12; ++i)
		m_pRushRever[i] = AddGameObject<Reverberation>();

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
	m_pUlte = AddGameObject<Em5300Ulte>();
	m_pUlte.lock()->m_pEm5300 = static_pointer_cast<Em5300>(m_pGameObject.lock());
	m_pUlte.lock()->m_pEm5300Mesh = m_pMesh;

	for (int i = 0; i < 4; ++i)
	{
		m_pSnatchPoint[i] = AddGameObject<SnatchPoint>();
		m_pSnatchPoint[i].lock()->m_pEm5300 = static_pointer_cast<Em5300>(m_pGameObject.lock());
		m_pSnatchPoint[i].lock()->m_pEm5300Mesh = m_pMesh;
		m_pSnatchPoint[i].lock()->Set_SnatchPos(i);
	}

	m_pEnerReady = AddGameObject<EnergismReady>();
	m_pEner = AddGameObject<Energism>();

	m_eState = Idle;
	m_pTransform.lock()->SetRotation({ 0.f,D3DXToDegree(45.5f) ,0.f });



	m_pHeadBone = m_pMesh->GetToRootMatrixPtr("Head");
	return S_OK;
}

HRESULT Em5300::Start()
{
	Unit::Start();
	m_pPanel = static_pointer_cast<BtlPanel>(FindGameObjectWithTag(UI_BtlPanel).lock());

	if (!m_pPanel.expired())
		m_pPanel.lock()->SetBossGaugeActive(true);

	
	return S_OK;
}

UINT Em5300::Update(const float _fDeltaTime)
{
	Unit::Update(_fDeltaTime);
	if (!m_pPanel.expired())
		m_pPanel.lock()->SetBossGaugeHPRatio(float(float(m_BattleInfo.iHp) / float(m_BattleInfo.iMaxHp)));



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

		 if (m_bGroggy)
			 DeltaPos.y = 0.f;

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
	if (Input::GetKeyDown(DIK_Y))
		m_eState = CutScene_Start;
	if (m_bCutScene)
		Fight(_fDeltaTime);
	State_Change(_fDeltaTime);


	if (m_bUlte)
		Renderer::GetInstance()->SkyDistortionEnd();


	if (m_eState == Attack_Laser_Loop)
	{
		const Vector3 vLook = m_pTransform.lock()->GetLook();
		const float LookYaw = FMath::ToDegree(std::atan2f(vLook.x, vLook.y));
		
		Matrix Result = *m_pHeadBone * m_pTransform.lock()->GetWorldMatrix();
		Vector3 vResult = { Result._41, Result._42, Result._43 };
		vResult += (-vLook * m_fEnergismLookOffset);
		m_pEner.lock()->UpdateVariable(vResult , m_fEnergismRotate);
		m_pEnerReady.lock()->UpdatePosition(vResult);
	}

	if (m_eState == Attack_Laser_Start)
	{ 
		Matrix Result = *m_pHeadBone * m_pTransform.lock()->GetWorldMatrix();
		const Vector3 vLook = m_pTransform.lock()->GetLook();
		Vector3 vResult = { Result._41, Result._42, Result._43 };
		vResult += (-vLook * m_fEnergismLookOffset);
		m_pEnerReady.lock()->UpdatePosition(vResult);
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
			Destroy(m_pRush);
			Destroy(m_pUlte);
			for (int i = 0; i < 32; ++i)
				Destroy(m_pBullet[i]);
			for (int i = 0; i < 12; ++i)
				Destroy(m_pRain[i]);
			for (int i = 0; i < 9; ++i)
				Destroy(m_pHoming[i]);
			for (int i = 0; i < 4; ++i)
				Destroy(m_pSnatchPoint[i]);

			Destroy(m_pGameObject);
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
	AddRankScore(_BattleInfo.iAttack);
	m_BattleInfo.iHp -= _BattleInfo.iAttack;
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
		m_fEnergismRotate = -D3DXToDegree(fAdd);

		m_bInteraction = false;

		return;
	}
	if (m_eState == Attack_Laser_Start || m_eState == Attack_Laser_Loop)
	{
		m_pTransform.lock()->Rotate({ 0.f, -D3DXToDegree(m_fAngleSpeed * _fDeltaTime * 0.15f), 0.f });
		m_fEnergismRotate = -D3DXToDegree(m_fAngleSpeed * _fDeltaTime * 0.15f);
	}
	else
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

void Em5300::SetTrigger(weak_ptr<class Trigger> _Trigger)
{
	m_pTrigger = _Trigger;
}

void Em5300::Set_Ulte()
{
	m_pCollider.lock()->SetGravity(false);
	m_pCollider.lock()->SetRigid(false);
	m_pCollider.lock()->SetTrigger(true);

	m_bIng = true;
	m_eState = Attack_Ulte_Start;
	SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Ulte2", { 1,1 }, 0.5f, false);
	

}

void Em5300::Particle()
{
	const uint32 ParticleCnt = 1000u;
	auto _PlayableParticle = ParticleSystem::GetInstance()->
		PlayParticle("ArtemisRushParticle", ParticleCnt, true);

	for (int32 i = 0; i < _PlayableParticle.size();
		++i)
	{
		auto& _PlayInstance = _PlayableParticle[i];
		_PlayInstance->PlayDescBind(_RenderUpdateInfo.World);
	};
}

void Em5300::OnCollisionEnter(std::weak_ptr<GameObject> _pOther)
{
	Monster::OnCollisionEnter(_pOther);

	if (_pOther.lock()->m_nTag == Player && m_eState == Attack_Rush_Loop)
		m_bCollPlayer = true;

}

void Em5300::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
	if (m_eState == Attack_Rush_Loop)
	{
		if (_pOther.lock()->m_nTag == Overture)
		{
			m_bHit = true;
			m_bGroggy = true;
			m_eState = Hit_Falling_Start;
			return;
		}
	}


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

	HitEffectPlay(_pOther);
}

void Em5300::Set_Cut()
{
	m_eState = CutScene_Start;
}

