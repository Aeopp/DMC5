#include "stdafx.h"
#include "NeroState.h"
#include "Nero.h"
#include "NeroFSM.h"

#pragma region PARENT // 부모

bool NeroState::m_bActive_Gravity = true;
bool NeroState::m_bCbsIdle = false;
bool NeroState::m_bActiveColl_Cbs = false;
bool NeroState::m_bPlayTrail = false;
UINT NeroState::m_iNeroCurWeaponIndex = Nero::NeroCom_RedQueen;
NeroState::NeroState(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:FSMState(_pFSM,_nIndex)
	, m_pNero(_pNero)
{
	//애니메이터 가져올거면 여기서
}

NeroState::~NeroState()
{
}

HRESULT NeroState::StateEnter()
{
	FSMState::StateEnter();
	m_bCbsIdle = false;

	switch (m_iNeroCurWeaponIndex)
	{
	case Nero::NeroCom_RedQueen:
		m_pNero.lock()->ChangeWeapon((Nero::NeroComponentID)m_iNeroCurWeaponIndex);
		break;
	case Nero::NeroCom_Cbs_Short:
		if(Nero::NeroCom_RedQueen == m_pNero.lock()->Get_CurWeaponIndex())
			m_pNero.lock()->ChangeWeapon((Nero::NeroComponentID)m_iNeroCurWeaponIndex);
		break;
	}
	
	return S_OK;
}

HRESULT NeroState::StateExit()
{
	FSMState::StateExit();
	return S_OK;
}

HRESULT NeroState::StateUpdate(const float _fDeltaTime)
{
	FSMState::StateUpdate(_fDeltaTime);

	if (Input::GetKey(DIK_LSHIFT))
		m_pNero.lock()->Locking();
	else
		m_pNero.lock()->SetOffLockOnMonster();

	//테스트
	if (Input::GetKeyDown(DIK_LCONTROL))
	{
		m_iNeroCurWeaponIndex = m_iNeroCurWeaponIndex == Nero::NeroCom_RedQueen ? m_iNeroCurWeaponIndex = Nero::NeroCom_Cbs_Short : m_iNeroCurWeaponIndex = Nero::NeroCom_RedQueen;
		//m_pNero.lock()->ChangeWeapon((Nero::NeroComponentID)m_iNeroCurWeaponIndex);
		m_pNero.lock()->ChangeWeaponUI((Nero::NeroComponentID)m_iNeroCurWeaponIndex);
	}
	return S_OK;
}

void NeroState::ResetAnimation(float fResetTiming, UINT _CheckIndex)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	UINT  CurAnimationIndex = m_pNero.lock()->Get_CurAnimationIndex();


	if (fResetTiming <= fCurrAnimationTime && _CheckIndex == CurAnimationIndex)
	{
		m_pNero.lock()->Set_PlayingTime(0.f);
	}
}

HRESULT NeroState::KeyInput_Idle(const int _nIndex)
{
	UINT Ex_Gauge = m_pNero.lock()->Get_ExGaugeCount();
	UINT Nero_Pre_Dir = m_pNero.lock()->Get_PreDirIndex();

	if (Input::GetKey(DIK_LSHIFT))
	{
		//락온

		if (Input::GetKey(DIK_Q))
		{
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			m_pFSM->ChangeState(NeroFSM::WIRE_SNATCH_PULL);
			return S_OK;
		}
		else if (Input::GetKey(DIK_W))
		{
			if (Input::GetKey(DIK_S) && Input::GetMouse(DIM_L))
			{
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
				if (Ex_Gauge > 0)
					m_pFSM->ChangeState(NeroFSM::SKILL_SHUFFLE_EX);
				else
					m_pFSM->ChangeState(NeroFSM::SKILL_SHUFFLE);
				return S_OK;
			}
			else if (Input::GetMouse(DIM_L))
			{
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
				if (Ex_Gauge > 0)
					m_pFSM->ChangeState(NeroFSM::SKILL_STREAK_EX3);
				else
					m_pFSM->ChangeState(NeroFSM::SKILL_STREAK);
				return S_OK;
			}
			else if (Input::GetMouse(DIM_R))
			{

			}
			else if (Input::GetMouse(DIM_M))
			{
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
				m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT_UP);
				return S_OK;
			}
		}
		else if (Input::GetKey(DIK_S))
		{
			if (Input::GetMouse(DIM_L))
			{
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
				if (Ex_Gauge > 0)
					m_pFSM->ChangeState(NeroFSM::SKILL_FLOAT_GROUND_EX3_START);
				else
					m_pFSM->ChangeState(NeroFSM::SKILL_FLOAT_GROUND);
				return S_OK;
			}
			else if (Input::GetMouse(DIM_R))
			{
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
				m_pFSM->ChangeState(NeroFSM::SKILL_HR_EX_START);
				return S_OK;
			}
			else if (Input::GetMouse(DIM_M))
			{
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
				m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT_DOWN);
				return S_OK;
			}
			else if (Input::GetKey(DIK_SPACE))
			{
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
				m_pNero.lock()->Set_JumpDir(Nero::Back);
				m_pFSM->ChangeState(NeroFSM::JUMP_START);

				return S_OK;
			}
		}
		else if (Input::GetKey(DIK_A))
		{
			if (Input::GetKey(DIK_SPACE))
			{
				//왼쪽 구르기
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
				m_pFSM->ChangeState(NeroFSM::EVADE_L);
				return S_OK;
			}
		}
		else if (Input::GetKey(DIK_D))
		{
			if (Input::GetKey(DIK_SPACE))
			{
				// 오른쪽 구르기
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
				m_pFSM->ChangeState(NeroFSM::EVADE_R);
				return S_OK;
			}
		}

		if (Input::GetMouse(DIM_L))
		{
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			switch (_nIndex)
			{
			case NeroFSM::ATT1:
				break;
			case NeroFSM::ATT2:
				m_pFSM->ChangeState(_nIndex);
				break;
			case NeroFSM::ATT3:
				m_pFSM->ChangeState(_nIndex);
				break;
			case NeroFSM::ATT4:
				m_pFSM->ChangeState(_nIndex);
				break;
			case NeroFSM::ATT_COMBO_C_R:
				m_pFSM->ChangeState(_nIndex);
				break;
			case NeroFSM::ATT_COMBO_C_L:
				m_pFSM->ChangeState(_nIndex);
				break;
			case NeroFSM::ATT_COMBO_C1:
				m_pFSM->ChangeState(_nIndex);
				break;
			case NeroFSM::ATT_COMBO_C2:
				m_pFSM->ChangeState(_nIndex);
				break;
			case NeroFSM::ATT_COMBO_C3:
				m_pFSM->ChangeState(_nIndex);
				break;
			case NeroFSM::ATT_COMBO_C4:
				m_pFSM->ChangeState(_nIndex);
				break;
			case NeroFSM::ATT_COMBO_D1:
				m_pFSM->ChangeState(_nIndex);
				break;
			case NeroFSM::ATT_COMBO_D2:
				m_pFSM->ChangeState(_nIndex);
				break;
			case NeroFSM::ATT_COMBO_D3:
				m_pFSM->ChangeState(_nIndex);
				break;
			case NeroFSM::ATT_COMBO_D4:
				m_pFSM->ChangeState(_nIndex);
				break;
			default:
				m_pFSM->ChangeState(NeroFSM::ATT1);
				break;
			}

			return S_OK;
		}
	}

	else if (Input::GetMouse(DIM_L))
	{
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
		switch (_nIndex)
		{
		case NeroFSM::ATT1:
			break;
		case NeroFSM::ATT2:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::ATT3:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::ATT4:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::ATT_COMBO_C_R:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::ATT_COMBO_C_L:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::ATT_COMBO_C1:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::ATT_COMBO_C2:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::ATT_COMBO_C3:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::ATT_COMBO_C4:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::ATT_COMBO_D1:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::ATT_COMBO_D2:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::ATT_COMBO_D3:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::ATT_COMBO_D4:
			m_pFSM->ChangeState(_nIndex);
			break;
		default:
			m_pFSM->ChangeState(NeroFSM::ATT1);
			break;
		}

		return S_OK;
	}
	else if (Input::GetMouse(DIM_M))
	{
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
		m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT);
		return S_OK;
	}
	else if (Input::GetKey(DIK_SPACE))
	{
		//점프
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
		m_pNero.lock()->Set_JumpDir(Nero::Basic);
		m_pFSM->ChangeState(NeroFSM::JUMP_START);
		return S_OK;
	}
	else if (Input::GetKeyDown(DIK_LCONTROL))
	{
		//m_pNero.lock()->ChangeWeapon(Nero::Cbs);
		//m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
	}

	else if (Input::GetKey(DIK_Q))
	{
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
		m_pFSM->ChangeState(NeroFSM::BUSTER_START);
		return S_OK;
	}

	else if (Input::GetKey(DIK_F))
	{
		if (m_pNero.lock()->Get_IsMajinMode())
			return S_OK;
		//변신게이지 있는지 체크
		float TDTGauge = m_pNero.lock()->Get_TDTGauge();
		if (0.3f <= TDTGauge)
		{
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			m_pFSM->ChangeState(NeroFSM::TO_MAJIN);
			return S_OK;
		}
	}

	else if (Input::GetKey(DIK_W))
	{
		if ((NeroFSM::ATT1 <= _nIndex && _nIndex <= NeroFSM::ATT4))
		{

		}
		else if (Input::GetKey(DIK_SPACE))
		{
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			m_pNero.lock()->Set_JumpDir(Nero::Front);
			m_pFSM->ChangeState(NeroFSM::JUMP_START);

			return S_OK;
		}
		else
		{
			m_pFSM->ChangeState(NeroFSM::RUNSTART);
			return S_OK;
		}
	}
	else if (Input::GetKey(DIK_S))
	{
		//180도 회전
		if ((NeroFSM::ATT1 <= _nIndex && _nIndex <= NeroFSM::ATT4))
		{

		}
		else
		{
			m_pFSM->ChangeState(NeroFSM::RUNSTART_180);
			return S_OK;
		}
	}
	else if (Input::GetKey(DIK_A))
	{
		//왼쪽으로 90도 회전
		if ((NeroFSM::ATT1 <= _nIndex && _nIndex <= NeroFSM::ATT4))
		{

		}
		else
		{
			m_pFSM->ChangeState(NeroFSM::RUNSTART_L);
			return S_OK;
		}
	}
	else if (Input::GetKey(DIK_D))
	{
		//오른쪽으로 90도 회전
		if ((NeroFSM::ATT1 <= _nIndex && _nIndex <= NeroFSM::ATT4))
		{

		}
		else
		{
			m_pFSM->ChangeState(NeroFSM::RUNSTART_R);
			return S_OK;
		}
	}

	if (Input::GetKey(DIK_LSHIFT))
		m_pNero.lock()->Locking();
	else
		m_pNero.lock()->SetOffLockOnMonster();

	return S_OK;
}

HRESULT NeroState::KeyInput_Run(const int _nIndex)
{
	UINT Ex_Gauge = m_pNero.lock()->Get_ExGaugeCount();
	if (Input::GetKey(DIK_LSHIFT))
	{
		//락온
		if (Input::GetKey(DIK_Q))
		{
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			m_pFSM->ChangeState(NeroFSM::WIRE_SNATCH_PULL);
			return S_OK;
		}
		else if (Input::GetKey(DIK_W))
		{
			if (Input::GetKey(DIK_S) && Input::GetMouse(DIM_L))
			{
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
				if (Ex_Gauge > 0)
					m_pFSM->ChangeState(NeroFSM::SKILL_SHUFFLE_EX);
				else
					m_pFSM->ChangeState(NeroFSM::SKILL_SHUFFLE);
				return S_OK;
			}
			else if (Input::GetMouse(DIM_L))
			{
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
				if (Ex_Gauge > 0)
					m_pFSM->ChangeState(NeroFSM::SKILL_STREAK_EX3);
				else
					m_pFSM->ChangeState(NeroFSM::SKILL_STREAK);
				return S_OK;
			}
			else if (Input::GetMouse(DIM_R))
			{

			}
			else if (Input::GetMouse(DIM_M))
			{
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
				m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT_UP);
				return S_OK;
			}
		}
		else if (Input::GetKey(DIK_S))
		{
			if (Input::GetMouse(DIM_L))
			{
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
				m_pFSM->ChangeState(NeroFSM::SKILL_FLOAT_GROUND);
				return S_OK;
			}
			else if (Input::GetMouse(DIM_R))
			{
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
				m_pFSM->ChangeState(NeroFSM::SKILL_HR_EX_START);
				return S_OK;
			}
			else if (Input::GetMouse(DIM_M))
			{
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
				m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT_DOWN);
				return S_OK;
			}
			else if (Input::GetKey(DIK_SPACE))
			{
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
				m_pNero.lock()->Set_JumpDir(Nero::Back);
				m_pFSM->ChangeState(NeroFSM::JUMP_START);

				return S_OK;
			}
		}
		else if (Input::GetKey(DIK_A))
		{
			if (Input::GetKey(DIK_SPACE))
			{
				//왼쪽 구르기
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
				m_pFSM->ChangeState(NeroFSM::EVADE_L);
				return S_OK;
			}
		}
		else if (Input::GetKey(DIK_D))
		{
			if (Input::GetKey(DIK_SPACE))
			{
				// 오른쪽 구르기
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
				m_pFSM->ChangeState(NeroFSM::EVADE_R);
				return S_OK;
			}
		}
	}

	if (Input::GetMouse(DIM_L))
	{
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
		m_pFSM->ChangeState(NeroFSM::ATT1_DASH);
		return S_OK;
	}
	else if (Input::GetMouse(DIM_M))
	{
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
		m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT);
		return S_OK;
	}
	else if (Input::GetKeyDown(DIK_SPACE))
	{
		//점프
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
		m_pNero.lock()->Set_JumpDir(Nero::Front);
		m_pFSM->ChangeState(NeroFSM::JUMP_START);
		return S_OK;
	}
	else if (Input::GetKeyDown(DIK_LCONTROL))
	{
		//m_pNero.lock()->ChangeWeapon(Nero::Cbs);
		//m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
	}

	else if (Input::GetKey(DIK_Q))
	{
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
		m_pFSM->ChangeState(NeroFSM::BUSTER_START);
		return S_OK;
	}

	else if (Input::GetKey(DIK_F))
	{
		if (m_pNero.lock()->Get_IsMajinMode())
			return S_OK;
		//변신게이지 있는지 체크
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
		m_pFSM->ChangeState(NeroFSM::TO_MAJIN);
		return S_OK;
	}

	else if (Input::GetKey(DIK_W))
	{
		m_pNero.lock()->SetAngleFromCamera();
	}
	else if (Input::GetKey(DIK_S))
	{
		//180도 회전
	}
	else if (Input::GetKey(DIK_A))
	{
		//왼쪽으로 90도 회전
	}
	else if (Input::GetKey(DIK_D))
	{
		//오른쪽으로 90도 회전
	}

	if (Input::GetKey(DIK_LSHIFT))
		m_pNero.lock()->Locking();
	else
		m_pNero.lock()->SetOffLockOnMonster();

	return S_OK;
}

HRESULT NeroState::KeyInput_Cbs_Idle(const int _nIndex)
{
	//삼절곤 키입력
	if (Input::GetKey(DIK_1))
	{
		NeroState::SetCbsIdle();
		m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT);
		return S_OK;
	}

	if (Input::GetKey(DIK_2))
	{
		NeroState::SetCbsIdle();
		m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT_UP);
		return S_OK;
	}

	if (Input::GetKey(DIK_3))
	{
		NeroState::SetCbsIdle();
		m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT_DOWN);
		return S_OK;
	}

	if (Input::GetKey(DIK_LSHIFT))
	{
		//락온
		if (Input::GetKey(DIK_Q))
		{
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_Cbs_Short, Nero::WS_Idle);
			m_pFSM->ChangeState(NeroFSM::WIRE_SNATCH_PULL);
			return S_OK;
		}
		else if (Input::GetKey(DIK_W))
		{
			if (Input::GetKey(DIK_S) && Input::GetMouse(DIM_L))
			{
				m_pFSM->ChangeState(NeroFSM::CBS_ICEAGE_START);
				return S_OK;
			}
			else if (Input::GetMouse(DIM_L))
			{
				//앞으로 돌아버리는거
				m_pFSM->ChangeState(NeroFSM::CBS_REVOLVER_START);
				return S_OK;
			}
			else if (Input::GetMouse(DIM_R))
			{
				//땅 꽝 찍는거
				m_pFSM->ChangeState(NeroFSM::MIDDLE_CBS_BLITZATTACK);
				return S_OK;
			}
			else if (Input::GetKey(DIK_S) && Input::GetMouse(DIM_M))
			{
				//앞으로 나가면서 미친연속공격
				m_pFSM->ChangeState(NeroFSM::MIDDLE_CBS_BI_ATTACK);
				return S_OK;
			}
			else if (Input::GetMouse(DIM_M))
			{
				m_pFSM->ChangeState(NeroFSM::MIDDLE_CBS_SATELLITE);
				return S_OK;
			}
			else if (Input::GetKey(DIK_SPACE))
			{
				m_pNero.lock()->Reset_RotationAngle();
				m_pNero.lock()->Reset_RootRotation();
				m_pNero.lock()->SetAngleFromCamera();
				if (NeroFSM::CBS_DASH == m_pFSM->GetCurrentIndex())
					m_pFSM->ChangeState(NeroFSM::STATERESET);
				m_pFSM->ChangeState(NeroFSM::CBS_DASH);
				return S_OK;
			}
		}
		else if (Input::GetKey(DIK_S))
		{
			//락온한 상태로 뒤로
			if (Input::GetMouse(DIM_L))
			{
				m_pFSM->ChangeState(NeroFSM::CBS_CRYSTAL);
				return S_OK;
			}
			else if (Input::GetMouse(DIM_R))
			{
				//총알 발사
				//m_pFSM->ChangeState(NeroFSM::MIDDLE_CBS_THUNDERBULLET);
				return S_OK;
			}
			else if (Input::GetMouse(DIM_M))
			{
				m_pFSM->ChangeState(NeroFSM::POLE_WHIRLWIND_START);
				return S_OK;
			}
			else if (Input::GetKey(DIK_SPACE))
			{
				m_pNero.lock()->SetRotationAngle(180.f);
				if (NeroFSM::CBS_DASH == m_pFSM->GetCurrentIndex())
					m_pFSM->ChangeState(NeroFSM::STATERESET);
				m_pFSM->ChangeState(NeroFSM::CBS_DASH);
				return S_OK;
			}
		}
		else if (Input::GetKey(DIK_A))
		{
			if (Input::GetKey(DIK_SPACE))
			{
				m_pNero.lock()->SetRotationAngle(-90.f);
				if (NeroFSM::CBS_DASH == m_pFSM->GetCurrentIndex())
					m_pFSM->ChangeState(NeroFSM::STATERESET);
				m_pFSM->ChangeState(NeroFSM::CBS_DASH);
				return S_OK;
			}
		}
		else if (Input::GetKey(DIK_D))
		{
			if (Input::GetKey(DIK_SPACE))
			{
				m_pNero.lock()->SetRotationAngle(90.f);
				if (NeroFSM::CBS_DASH == m_pFSM->GetCurrentIndex())
					m_pFSM->ChangeState(NeroFSM::STATERESET);
				m_pFSM->ChangeState(NeroFSM::CBS_DASH);
				return S_OK;
			}
		}
	}
	else if (Input::GetMouse(DIM_L))
	{
		switch (_nIndex)
		{
		case NeroFSM::CBS_COMBOA1:
			break;
		case NeroFSM::CBS_COMBOA2:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::CBS_COMBOA3:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::CBS_COMBOA4:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::CBS_COMBOA5:
			m_pFSM->ChangeState(_nIndex);
			break;
		default:
			m_pFSM->ChangeState(NeroFSM::CBS_COMBOA1);
			break;
		}
		return S_OK;
	}
	else if (Input::GetMouse(DIM_M))
	{
		switch (_nIndex)
		{
		case NeroFSM::POLE_COMBOA1:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::POLE_COMBOA2:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::POLE_COMBOA3:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::POLE_COMBOB1:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::POLE_COMBOB2:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::POLE_COMBOB3:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::POLE_COMBOB4:
			m_pFSM->ChangeState(_nIndex);
			break;
		default:
			m_pFSM->ChangeState(NeroFSM::POLE_COMBOA1);
			break;
		}
		return S_OK;
	}
	else if (Input::GetMouse(DIM_R))
	{
		//몬스터 위치에 따라서 업다운 프론트 결정
		m_pFSM->ChangeState(NeroFSM::MIDDLE_CBS_STRIKE);
		return S_OK;
	}

	else if (Input::GetKeyDown(DIK_SPACE))
	{
	//점프
		m_pNero.lock()->Set_JumpDir(Nero::Basic);
		m_pFSM->ChangeState(NeroFSM::JUMP_START);
		return S_OK;
	}
	else if (Input::GetKey(DIK_Q))
	{
	m_pFSM->ChangeState(NeroFSM::BUSTER_START);
	return S_OK;
	}

	else if (Input::GetKey(DIK_F))
	{
		if (m_pNero.lock()->Get_IsMajinMode())
			return S_OK;
		//변신게이지 있는지 체크
		float TDTGauge = m_pNero.lock()->Get_TDTGauge();
		if (0.3f <= TDTGauge)
		{
			m_pFSM->ChangeState(NeroFSM::TO_MAJIN);
			return S_OK;
		}
	}
	else if (Input::GetKey(DIK_W))
	{
		if (Input::GetKey(DIK_SPACE))
		{
			m_pNero.lock()->Set_JumpDir(Nero::Front);
			m_pFSM->ChangeState(NeroFSM::JUMP_START);

			return S_OK;
		}
		m_pFSM->ChangeState(NeroFSM::RUNSTART);
		return S_OK;
	}
	else if (Input::GetKey(DIK_S))
	{
		m_pFSM->ChangeState(NeroFSM::RUNSTART_180);
		return S_OK;
	}


	else if (Input::GetKey(DIK_A))
	{
		m_pFSM->ChangeState(NeroFSM::RUNSTART_L);
		return S_OK;
	}
	else if (Input::GetKey(DIK_D))
	{
		m_pFSM->ChangeState(NeroFSM::RUNSTART_R);
		return S_OK;
	}
	return S_OK;
}

HRESULT NeroState::KeyInput_Cbs_Run(const int _nIndex)
{
	if (Input::GetKey(DIK_1))
	{
		NeroState::SetCbsIdle();
		m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT);
		return S_OK;
	}

	if (Input::GetKey(DIK_2))
	{
		NeroState::SetCbsIdle();
		m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT_UP);
		return S_OK;
	}

	if (Input::GetKey(DIK_3))
	{
		NeroState::SetCbsIdle();
		m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT_DOWN);
		return S_OK;
	}

	if (Input::GetKey(DIK_LSHIFT))
	{
		//락온
		if (Input::GetKey(DIK_Q))
		{
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_Cbs_Short, Nero::WS_Idle);
			m_pFSM->ChangeState(NeroFSM::WIRE_SNATCH_PULL);
			return S_OK;
		}
		else if (Input::GetKey(DIK_W))
		{
			if (Input::GetKey(DIK_S) && Input::GetMouse(DIM_L))
			{
				m_pFSM->ChangeState(NeroFSM::CBS_ICEAGE_START);
				return S_OK;
			}
			else if (Input::GetMouse(DIM_L))
			{
				//앞으로 돌아버리는거
				m_pFSM->ChangeState(NeroFSM::CBS_REVOLVER_START);
				return S_OK;
			}
			else if (Input::GetMouse(DIM_R))
			{
				//땅 꽝 찍는거
				m_pFSM->ChangeState(NeroFSM::MIDDLE_CBS_BLITZATTACK);
				return S_OK;
			}
			else if (Input::GetKey(DIK_S) && Input::GetMouse(DIM_M))
			{
				//앞으로 나가면서 미친연속공격
				m_pFSM->ChangeState(NeroFSM::MIDDLE_CBS_BI_ATTACK);
				return S_OK;
			}
			else if (Input::GetMouse(DIM_M))
			{
				m_pFSM->ChangeState(NeroFSM::MIDDLE_CBS_SATELLITE);
				return S_OK;
			}
			else if (Input::GetKey(DIK_SPACE))
			{
				m_pNero.lock()->Reset_RotationAngle();
				m_pNero.lock()->Reset_RootRotation();
				m_pNero.lock()->SetAngleFromCamera();
				if(NeroFSM::CBS_DASH == m_pFSM->GetCurrentIndex())
					m_pFSM->ChangeState(NeroFSM::STATERESET);
				m_pFSM->ChangeState(NeroFSM::CBS_DASH);
				return S_OK;
			}
		}
		else if (Input::GetKey(DIK_S))
		{
			//락온한 상태로 뒤로
			if (Input::GetMouse(DIM_L))
			{
				m_pFSM->ChangeState(NeroFSM::CBS_CRYSTAL);
				return S_OK;
			}
			else if (Input::GetMouse(DIM_R))
			{
				//총알 발사
				//m_pFSM->ChangeState(NeroFSM::MIDDLE_CBS_THUNDERBULLET);
				return S_OK;
			}
			else if (Input::GetMouse(DIM_M))
			{
				m_pFSM->ChangeState(NeroFSM::POLE_WHIRLWIND_START);
				return S_OK;
			}
			else if (Input::GetKey(DIK_SPACE))
			{
				m_pNero.lock()->SetRotationAngle(180.f);
				if (NeroFSM::CBS_DASH == m_pFSM->GetCurrentIndex())
					m_pFSM->ChangeState(NeroFSM::STATERESET);
				m_pFSM->ChangeState(NeroFSM::CBS_DASH); 

				return S_OK;
			}
		}
		else if (Input::GetKey(DIK_A))
		{
			if (Input::GetKey(DIK_SPACE))
			{
				m_pNero.lock()->SetRotationAngle(-90.f);
				if (NeroFSM::CBS_DASH == m_pFSM->GetCurrentIndex())
					m_pFSM->ChangeState(NeroFSM::STATERESET);
				m_pFSM->ChangeState(NeroFSM::CBS_DASH);
				return S_OK;
			}
		}
		else if (Input::GetKey(DIK_D))
		{
			if (Input::GetKey(DIK_SPACE))
			{
				m_pNero.lock()->SetRotationAngle(90.f);
				if (NeroFSM::CBS_DASH == m_pFSM->GetCurrentIndex())
					m_pFSM->ChangeState(NeroFSM::STATERESET);
				m_pFSM->ChangeState(NeroFSM::CBS_DASH);
				return S_OK;
			}
		}
	}
	else if (Input::GetMouse(DIM_L))
	{
		switch (_nIndex)
		{
		case NeroFSM::CBS_COMBOA1:
			break;
		case NeroFSM::CBS_COMBOA2:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::CBS_COMBOA3:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::CBS_COMBOA4:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::CBS_COMBOA5:
			m_pFSM->ChangeState(_nIndex);
			break;
		default:
			m_pFSM->ChangeState(NeroFSM::CBS_COMBOA1);
			break;
		}
		return S_OK;
	}
	else if (Input::GetMouse(DIM_M))
	{
		switch (_nIndex)
		{
		case NeroFSM::POLE_COMBOA1:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::POLE_COMBOA2:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::POLE_COMBOA3:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::POLE_COMBOB1:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::POLE_COMBOB2:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::POLE_COMBOB3:
			m_pFSM->ChangeState(_nIndex);
			break;
		case NeroFSM::POLE_COMBOB4:
			m_pFSM->ChangeState(_nIndex);
			break;
		default:
			m_pFSM->ChangeState(NeroFSM::POLE_COMBOA1);
			break;
		}
		return S_OK;
	}
	else if (Input::GetMouse(DIM_R))
	{
		//몬스터 위치에 따라서 업다운 프론트 결정
		m_pFSM->ChangeState(NeroFSM::MIDDLE_CBS_STRIKE);
		return S_OK;
	}

	else if (Input::GetKeyDown(DIK_SPACE))
	{
		//점프
		m_pNero.lock()->Set_JumpDir(Nero::Front);
		m_pFSM->ChangeState(NeroFSM::JUMP_START);
		return S_OK;
	}
	else if (Input::GetKey(DIK_Q))
	{
		m_pFSM->ChangeState(NeroFSM::BUSTER_START);
		return S_OK;
	}

	else if (Input::GetKey(DIK_F))
	{
		if (m_pNero.lock()->Get_IsMajinMode())
			return S_OK;
		//변신게이지 있는지 체크
		float TDTGauge = m_pNero.lock()->Get_TDTGauge();
		if (0.3f <= TDTGauge)
		{
			m_pFSM->ChangeState(NeroFSM::TO_MAJIN);
			return S_OK;
		}
	}
	else if (Input::GetKey(DIK_W))
	{
		m_pNero.lock()->SetAngleFromCamera();
	}
	else if (Input::GetKey(DIK_S))
	{

	}


	else if (Input::GetKey(DIK_A))
	{

	}
	else if (Input::GetKey(DIK_D))
	{

	}
	else if (Input::GetKeyDown(DIK_LCONTROL))
	{
		//삼절곤에서 검으로 변환
		//m_pNero.lock()->ChangeWeapon(Nero::RQ);
		//m_pFSM->ChangeState(NeroFSM::IDLE);
	}
	return S_OK;
}

HRESULT NeroState::KeyInput_Jump(const int _nIndex)
{
	UINT JumpCount = m_pNero.lock()->Get_JumpCount();
	UINT Ex_Gauge = m_pNero.lock()->Get_ExGaugeCount();

	if (Input::GetKey(DIK_LSHIFT))
	{
		//락온
		if (Input::GetKey(DIK_Q))
		{
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			m_pFSM->ChangeState(NeroFSM::WIRE_SNATCH_PULL_AIR);
			return S_OK;
		}
		else if (Input::GetKey(DIK_W))
		{
			if (Input::GetKey(DIK_S) && Input::GetMouse(DIM_L))
			{
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
				m_pFSM->ChangeState(NeroFSM::SKILL_CALIBER_START);
				return S_OK;
			}
			else if (Input::GetMouse(DIM_L))
			{
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
				m_pFSM->ChangeState(NeroFSM::SKILL_AIR_DIVE_SLASH_START);
				return S_OK;
			}
			else if (Input::GetMouse(DIM_R))
			{
				
			}
			else if (Input::GetMouse(DIM_M))
			{
				//오버츄어 공중에서 위로
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
				m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT_AIR_UP);
				return S_OK;
			}
		}
		else if (Input::GetKey(DIK_S))
		{
			if (Input::GetMouse(DIM_L))
			{
				//게이지 따라서 분기
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
				if(Ex_Gauge > 0)
					m_pFSM->ChangeState(NeroFSM::SKILL_SPLIT_EX_START);
				else
					m_pFSM->ChangeState(NeroFSM::SKILL_SPLIT_START);
				return S_OK;
			}
			else if (Input::GetMouse(DIM_R))
			{
				//총알 발사
			}
			else if (Input::GetMouse(DIM_M))
			{
				//오버추어 공중에서 아래로
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
				m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT_AIR_DOWN);
				return S_OK;
			}
			else if (0 < JumpCount && Input::GetKey(DIK_SPACE))
			{
				//뒤로 2단 점프
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
				m_pNero.lock()->Set_JumpDir(Nero::Back);
				m_pFSM->ChangeState(NeroFSM::JUMP_TWICE);
				return S_OK;
			}
		}
	}

	else if (Input::GetMouse(DIM_L))
	{
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
		switch (_nIndex)
		{
		case NeroFSM::AIR_COMBOA1:
			m_pFSM->ChangeState(NeroFSM::AIR_COMBOA2);
			break;
		case NeroFSM::AIR_COMBOA2:
			m_pFSM->ChangeState(NeroFSM::AIR_COMBOA3);
			break;
		case NeroFSM::AIR_COMBOB:
			m_pFSM->ChangeState(NeroFSM::AIR_COMBOB);
			break;
		default:
			m_pFSM->ChangeState(NeroFSM::AIR_COMBOA1);
			break;
		}
		return S_OK;
		
	}
	else if (Input::GetMouse(DIM_M))
	{
		//오버추어 앞으로
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
		m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT_AIR);
		return S_OK;
	}
	else if (Input::GetKey(DIK_Q))
	{
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
		m_pFSM->ChangeState(NeroFSM::BUSTER_AIR_CATCH);
		return S_OK;
	}
	else if (Input::GetKey(DIK_W))
	{
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
		if (0 < JumpCount && Input::GetKeyDown(DIK_SPACE))
		{
			//앞으로 2단 점프
			m_pNero.lock()->Set_JumpDir(Nero::Front);
			m_pFSM->ChangeState(NeroFSM::JUMP_TWICE);
			return S_OK;
		}
	}
	else if (0 < JumpCount && Input::GetKeyDown(DIK_SPACE))
	{
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
		m_pNero.lock()->Set_JumpDir(Nero::Basic);
		m_pFSM->ChangeState(NeroFSM::JUMP_TWICE);
		return S_OK;
	}
	return S_OK;
}

HRESULT NeroState::KeyInput_Cbs_Jump(const int _nIndex)
{
	UINT JumpCount = m_pNero.lock()->Get_JumpCount();
	UINT Ex_Gauge = m_pNero.lock()->Get_ExGaugeCount();

	if (Input::GetKey(DIK_1))
	{
		NeroState::SetCbsIdle();
		m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT_AIR);
		return S_OK;
	}

	if (Input::GetKey(DIK_2))
	{
		NeroState::SetCbsIdle();
		m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT_AIR_UP);
		return S_OK;
	}

	if (Input::GetKey(DIK_3))
	{
		NeroState::SetCbsIdle();
		m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT_AIR_DOWN);
		return S_OK;
	}

	if (Input::GetKey(DIK_LSHIFT))
	{
		//락온
		if (Input::GetKey(DIK_Q))
		{
			m_pFSM->ChangeState(NeroFSM::WIRE_SNATCH_PULL_AIR);
			return S_OK;
		}
		else if (Input::GetKey(DIK_W))
		{
			if (Input::GetMouse(DIM_L))
			{
				m_pFSM->ChangeState(NeroFSM::CBS_REVOLVER_LOOP);
				return S_OK;
			}
			else if (Input::GetMouse(DIM_R))
			{
				m_pFSM->ChangeState(NeroFSM::MIDDLE_CBS_STRIKE_AIR_UP);
				return S_OK;
			}
			else if (Input::GetMouse(DIM_M))
			{
				m_pFSM->ChangeState(NeroFSM::MIDDLE_CBS_SATELLITE_AIR);
				return S_OK;
			}
		}
		else if (Input::GetKey(DIK_S))
		{
			if (Input::GetMouse(DIM_R))
			{
				m_pFSM->ChangeState(NeroFSM::MIDDLE_CBS_STRIKE_AIR_DOWN);
				return S_OK;
			}
			else if (0 < JumpCount && Input::GetKeyDown(DIK_SPACE))
			{
				//뒤로 2단 점프
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
				m_pNero.lock()->Set_JumpDir(Nero::Back);
				m_pFSM->ChangeState(NeroFSM::JUMP_TWICE);
				return S_OK;
			}
		}
	}

	else if (Input::GetMouse(DIM_L))
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
		m_pFSM->ChangeState(NeroFSM::CBS_SWING);
		return S_OK;
	}
	else if (Input::GetMouse(DIM_R))
	{
		m_pFSM->ChangeState(NeroFSM::MIDDLE_CBS_STRIKE_AIR);
		return S_OK;
	}

	else if (Input::GetKey(DIK_Q))
	{
		m_pFSM->ChangeState(NeroFSM::BUSTER_AIR_CATCH);
		return S_OK;
	}
	else if (Input::GetKey(DIK_W))
	{
		if (0 < JumpCount && Input::GetKeyDown(DIK_SPACE))
		{
			//앞으로 2단 점프
			m_pNero.lock()->Set_JumpDir(Nero::Front);
			m_pFSM->ChangeState(NeroFSM::JUMP_TWICE);
			return S_OK;
		}
	}
	else if (0 < JumpCount && Input::GetKeyDown(DIK_SPACE))
	{
		//제자리 2단 점프
		m_pNero.lock()->Set_JumpDir(Nero::Basic);
		m_pFSM->ChangeState(NeroFSM::JUMP_TWICE);
		return S_OK;
	}
	return S_OK;
}

HRESULT NeroState::Move_Jump()
{
	if (Input::GetKey(DIK_W))
	{
		m_pNero.lock()->NeroMove(Nero::Dir_Front,0.006f);
	}

	else if (Input::GetKey(DIK_S))
	{
		m_pNero.lock()->NeroMove(Nero::Dir_Back, 0.006f);
	}

	else if (Input::GetKey(DIK_A))
	{
		m_pNero.lock()->NeroMove(Nero::Dir_Left, 0.006f);
	}

	else if (Input::GetKey(DIK_D))
	{
		m_pNero.lock()->NeroMove(Nero::Dir_Right, 0.006f);
	}


	return S_OK;
}

void NeroState::ActiveColl_Monsters(bool _ActiveOrNot)
{
}

void NeroState::ActiveColl_RedQueen(bool _ActiveOrNot)
{
	if (m_bActiveColl_RedQueen == _ActiveOrNot)
		return;
	m_bActiveColl_RedQueen = _ActiveOrNot;
	m_pNero.lock()->Set_Weapon_Coll(Nero::NeroCom_RedQueen, _ActiveOrNot);
}

void NeroState::ActiveColl_Cbs(bool _ActiveOrNot, const int _nIndex)
{
	if (m_bActiveColl_Cbs == _ActiveOrNot)
		return;
	m_bActiveColl_Cbs = _ActiveOrNot;
	m_pNero.lock()->Set_Weapon_Coll((Nero::NeroComponentID)_nIndex, _ActiveOrNot);
}

void NeroState::ActiveGravity(bool _ActiveOrNot)
{
	if (m_bActive_Gravity == _ActiveOrNot)
		return;
	m_bActive_Gravity = _ActiveOrNot;
	m_pNero.lock()->SetGravity(_ActiveOrNot);
}

void NeroState::SetCbsIdle()
{
	if (m_bCbsIdle == true)
		return;
	m_bCbsIdle = true;
	m_pNero.lock()->SetCbsIdle();
}

void NeroState::IsGround()
{
	if (m_pNero.lock()->CheckIsGround())
	{
		m_pNero.lock()->ChangeNeroDirection(Nero::Dir_Front);
		m_pFSM->ChangeState(NeroFSM::JUMP_LANDING);
	}
}

void NeroState::ActiveTrail(bool _ActvieOrNot)
{
	if (m_bPlayTrail == _ActvieOrNot)
		return;
	m_bPlayTrail = _ActvieOrNot;
	if (m_bPlayTrail)
	{
		switch (m_iNeroCurWeaponIndex)
		{
		case Nero::NeroCom_RedQueen:
			m_pNero.lock()->PlayEffect(Eff_Trail);
			break;
		case Nero::NeroCom_Cbs_Short:
			m_pNero.lock()->PlayEffect(Eff_CbsTrail);
			break;
		case Nero::NeroCom_Cbs_Long:
			break;
		}		
	}
	else
	{
		switch (m_iNeroCurWeaponIndex)
		{
		case Nero::NeroCom_RedQueen:
			m_pNero.lock()->StopEffect(Eff_Trail);
			break;
		case Nero::NeroCom_Cbs_Short:
			m_pNero.lock()->StopEffect(Eff_CbsTrail);
			break;
		case Nero::NeroCom_Cbs_Long:
			break;
		}
	}
}


#pragma endregion 

#pragma region IDLE // 대기



Idle::Idle(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM,_nIndex,_pNero)
{
}

Idle::~Idle()
{
}

Idle* Idle::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Idle(_pFSM,_nIndex,_pNero);
}

HRESULT Idle::StateEnter()
{
	NeroState::StateEnter();
	UINT CurAnimationIndex = m_pNero.lock()->Get_CurAnimationIndex();
	UINT PreAnimationIndex = m_pNero.lock()->Get_PreAnimationIndex();
	m_pNero.lock()->Reset_JumpCount();
	m_pNero.lock()->Set_JumpDir(Nero::Basic);
	switch (CurAnimationIndex)
	{
	case Nero::ANI_IDLE_FROM_COMBOA1:
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA1_Loop", true, Nero::ANI_IDLE_FROM_COMBOA1_LOOP);
		break;
	case Nero::ANI_IDLE_FROM_COMBOA2:
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA2_Loop", true, Nero::ANI_IDLE_FROM_COMBOA2_LOOP);
		break;
	case Nero::ANI_COMBOA3:
		m_pNero.lock()->ChangeAnimation("Idle_Battle", true, Nero::ANI_IDLE_BATTLE);
		break;
	case Nero::ANI_COMBOA4:
		m_pNero.lock()->ChangeAnimation("Idle_Battle", true, Nero::ANI_IDLE_BATTLE);
		break;
	case Nero::ANI_COMBOC1:
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA2_Loop", true, Nero::ANI_IDLE_FROM_COMBOA2_LOOP);
		break;
	case Nero::ANI_COMBOC2:
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA2_Loop", true, Nero::ANI_IDLE_FROM_COMBOA2_LOOP);
		break;
	case Nero::ANI_COMBOC3:
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA2_Loop", true, Nero::ANI_IDLE_FROM_COMBOA2_LOOP);
		break;
	case Nero::ANI_COMBOC4:
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA2_Loop", true, Nero::ANI_IDLE_FROM_COMBOA2_LOOP);
		break;
	case Nero::ANI_COMBOC_R:
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA2_Loop", true, Nero::ANI_IDLE_FROM_COMBOA2_LOOP);
		break;
	case Nero::ANI_COMBOC_L:
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA2_Loop", true, Nero::ANI_IDLE_FROM_COMBOA2_LOOP);
		break;
	case Nero::ANI_COMBOD1:
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA1_Loop", true, Nero::ANI_IDLE_FROM_COMBOA1_LOOP);
		break;
	case Nero::ANI_COMBOD2:
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA1_Loop", true, Nero::ANI_IDLE_FROM_COMBOA1_LOOP);
		break;
	case Nero::ANI_COMBOD3:
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA1_Loop", true, Nero::ANI_IDLE_FROM_COMBOA1_LOOP);
		break;
	case Nero::ANI_COMBOD4:
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA1_Loop", true, Nero::ANI_IDLE_FROM_COMBOA1_LOOP);
		break;
	case Nero::ANI_HR_GROUND:
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA1_Loop", true, Nero::ANI_IDLE_FROM_COMBOA1_LOOP);
		break;
	case Nero::ANI_HR_EX_FINISH:
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA1_Loop", true, Nero::ANI_IDLE_FROM_COMBOA1_LOOP);
		break;
	case Nero::ANI_STREAK_END:
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA1_Loop", true, Nero::ANI_IDLE_FROM_COMBOA1_LOOP);
		break;
	case Nero::ANI_STREAK_EX_ROLL_END:
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA1_Loop", true, Nero::ANI_IDLE_FROM_COMBOA1_LOOP);
		break;
	case Nero::ANI_HITFRONT:
		m_pNero.lock()->ChangeAnimation("Idle_Battle", true, Nero::ANI_IDLE_BATTLE);
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
		break;
	case Nero::ANI_SHUFFLE:
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA2_Loop", true, Nero::ANI_IDLE_FROM_COMBOA2_LOOP);
		break;
	case Nero::ANI_SHUFFLE_EX:
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA2_Loop", true, Nero::ANI_IDLE_FROM_COMBOA2_LOOP);
		break;
	case Nero::ANI_OVERTURE_SHOOT:
		m_pNero.lock()->ChangeAnimation("Idle_Battle", true, Nero::ANI_IDLE_BATTLE);
		break;
	case Nero::ANI_OVERTURE_SHOOT_DOWN:
		m_pNero.lock()->ChangeAnimation("Idle_Battle", true, Nero::ANI_IDLE_BATTLE);
		break;
	case Nero::ANI_OVERTURE_SHOOT_UP:
		m_pNero.lock()->ChangeAnimation("Overture_Idle", true, Nero::ANI_OVERTURE_IDLE);
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
		break;
	case Nero::ANI_JUMP_LANDING:
		m_pNero.lock()->ChangeAnimation("Idle_Battle", true, Nero::ANI_IDLE_BATTLE);
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
		break;
	case Nero::ANI_JUMP_FRONT_LANDING:
		m_pNero.lock()->ChangeAnimation("Idle_Battle", true, Nero::ANI_IDLE_BATTLE);
		break;
	case Nero::ANI_GT_BOMB:
		m_pNero.lock()->ChangeAnimation("Idle_Battle", true, Nero::ANI_IDLE_BATTLE);
		break;
	case Nero::ANI_GT_CRASH_JUST:
		m_pNero.lock()->ChangeAnimation("Idle_Battle", true, Nero::ANI_IDLE_BATTLE);
		break;
	case Nero::ANI_GT_CRASH_RELOAD:
		m_pNero.lock()->ChangeAnimation("Idle_Battle", true, Nero::ANI_IDLE_BATTLE);
		break;
	case Nero::ANI_GT_RELOAD:
		m_pNero.lock()->ChangeAnimation("Idle_Battle", true, Nero::ANI_IDLE_BATTLE);
		break;
	case Nero::ANI_GT_RELOAD_STYLISH:
		m_pNero.lock()->ChangeAnimation("Idle_Battle", true, Nero::ANI_IDLE_BATTLE);
		break;
	case Nero::ANI_WIRE_SNATCH_PULL:
		m_pNero.lock()->ChangeAnimation("Idle_Battle", true, Nero::ANI_IDLE_BATTLE);
		break;
	case Nero::ANI_WIRE_SNATCH_PULL_U:
		m_pNero.lock()->ChangeAnimation("Idle_Battle", true, Nero::ANI_IDLE_BATTLE);
		break;
	case Nero::ANI_WIRE_SNATCH_PULL_D:
		m_pNero.lock()->ChangeAnimation("Idle_Battle", true, Nero::ANI_IDLE_BATTLE);
		break;
	case Nero::ANI_EVADE_L:
		m_pNero.lock()->ChangeAnimation("Idle_Battle", true, Nero::ANI_IDLE_BATTLE);
		break;
	case Nero::ANI_EVADE_R:
		m_pNero.lock()->ChangeAnimation("Idle_Battle", true, Nero::ANI_IDLE_BATTLE);
		break;
	case Nero::ANI_STUN_END:
		m_pNero.lock()->ChangeAnimation("Idle_Battle", true, Nero::ANI_IDLE_BATTLE);
		break;
	case Nero::ANI_RUNSTOP:
		m_pNero.lock()->ChangeAnimation("Idle_Battle", true, Nero::ANI_IDLE_BATTLE);
		break;
	case Nero::ANI_DASHSTOP:
		m_pNero.lock()->ChangeAnimation("Idle_Battle", true, Nero::ANI_IDLE_BATTLE);
		break;
	case Nero::ANI_AIR_DIVE_SLASH_FINISH:
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA1_Loop", true, Nero::ANI_IDLE_FROM_COMBOA1_LOOP);
		break;
	case Nero::ANI_SPLIT_LANDING:
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA1_Loop", true, Nero::ANI_IDLE_FROM_COMBOA1_LOOP);
		break;
	case Nero::ANI_SPLITEX_LANDING:
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA1_Loop", true, Nero::ANI_IDLE_FROM_COMBOA1_LOOP);
		break;
	case Nero::ANI_RUNSTART_FROM_COMBOA1:
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA1_Loop", true, Nero::ANI_IDLE_FROM_COMBOA1_LOOP);
		break;
	case Nero::ANI_RUNSTART_FROM_COMBOA2:
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA2_Loop", true, Nero::ANI_IDLE_FROM_COMBOA2_LOOP);
		break;
	case Nero::ANI_TO_MAJIN:
		m_pNero.lock()->ChangeAnimation("Idle_Battle", true, Nero::ANI_IDLE_BATTLE);
		break;
	case Nero::ANI_BUSTER_START:
		m_pNero.lock()->ChangeAnimation("Idle_Battle", true, Nero::ANI_IDLE_BATTLE);
		break;
	default:
		m_pNero.lock()->ChangeAnimation("Idle_Normal", true,Nero::ANI_IDLE_NORMAL);
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
		break;
	}

	
	return S_OK;
}

HRESULT Idle::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Idle::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);

	if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
	{
		m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}
	NeroState::KeyInput_Idle();

	return S_OK;
}

Idle_Start::Idle_Start(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Idle_Start::~Idle_Start()
{
}

Idle_Start* Idle_Start::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Idle_Start(_pFSM, _nIndex, _pNero);
}

HRESULT Idle_Start::StateEnter()
{
	NeroState::StateEnter();

	UINT CurAnimationIndex = m_pNero.lock()->Get_CurAnimationIndex();
	UINT PreAnimationIndex = m_pNero.lock()->Get_PreAnimationIndex();

	switch (CurAnimationIndex)
	{
	case Nero::ANI_COMBOA1:
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA1", false, Nero::ANI_IDLE_FROM_COMBOA1);
		break;
	case Nero::ANI_COMBOA2:
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA2", false, Nero::ANI_IDLE_FROM_COMBOA2);
		break;
	case Nero::ANI_COMBOA3:
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA1", false, Nero::ANI_IDLE_FROM_COMBOA1);
		break;
	case Nero::ANI_COMBOA4:
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA1", false, Nero::ANI_IDLE_FROM_COMBOA1);
		break;
	default:
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA1", false, Nero::ANI_IDLE_FROM_COMBOA1);
		break;
	}
	return S_OK;
}

HRESULT Idle_Start::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Idle_Start::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	UINT CurAnimationIndex = m_pNero.lock()->Get_CurAnimationIndex();
	UINT PreAnimationIndex = m_pNero.lock()->Get_PreAnimationIndex();

	if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
	{
		ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}

	switch (CurAnimationIndex)
	{
	case Nero::ANI_IDLE_FROM_COMBOA1:
		if (fCurrAnimationTime <= 0.5f)
		{
			// 레드퀸 콤보 땅바닥 찍는거 시작
			if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
				KeyInput_Idle(NeroFSM::ATT_COMBO_C1);
			else
				KeyInput_Cbs_Idle();
		}
		else
		{
			if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
				KeyInput_Idle(NeroFSM::IDLE);
			else
				KeyInput_Cbs_Idle();
		}
		break;
	case Nero::ANI_IDLE_FROM_COMBOA2:
		if (fCurrAnimationTime <= 0.5f)
		{
			if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
				KeyInput_Idle(NeroFSM::ATT_COMBO_D1);
			else
				KeyInput_Cbs_Idle();
		}
		else
		{
			if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
				KeyInput_Idle(NeroFSM::IDLE);
			else
				KeyInput_Cbs_Idle();
		}

		break;
	default:
		break;
	}

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);

		return S_OK;
	}
	//NeroState::KeyInput_Idle();
	return S_OK;
}

#pragma endregion

#pragma region JUMP // 점프

Jump_Basic::Jump_Basic(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Jump_Basic::~Jump_Basic()
{
}

Jump_Basic* Jump_Basic::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Jump_Basic(_pFSM, _nIndex, _pNero);
}

HRESULT Jump_Basic::StateEnter()
{
	NeroState::StateEnter();

	UINT iJumpDir = m_pNero.lock()->Get_JumpDir();
	m_pNero.lock()->SetFly(true);

	switch (iJumpDir)
	{
	case Nero::Basic:

		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pNero.lock()->ChangeAnimation("Jump", false, Nero::ANI_JUMP);
		else
			m_pNero.lock()->ChangeAnimation("Cbs_Jump", false, Nero::ANI_CBS_JUMP);
		m_pNero.lock()->SetAddForce({ 0.f,120.f,0.f });
		break;
	case Nero::Front:
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pNero.lock()->ChangeAnimation("Jump_Front", false, Nero::ANI_JUMP_FRONT);
		else
			m_pNero.lock()->ChangeAnimation("Cbs_JumpFront", false, Nero::ANI_CBS_JUMP_FRONT);
		m_pNero.lock()->SetAddForce({ 0.f,120.f,0.f });
		break;
	case Nero::Back:
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pNero.lock()->ChangeAnimation("Jump_Back", false, Nero::ANI_JUMP_BACK);
		else
			m_pNero.lock()->ChangeAnimation("Cbs_JumpBack", false, Nero::ANI_CBS_JUMP_BACK);
		m_pNero.lock()->SetAddForce({ 0.f,120.f,0.f });
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT Jump_Basic::StateExit()
{
	return S_OK;
}

HRESULT Jump_Basic::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (m_pNero.lock()->IsAnimationEnd() /* || 땅에 닿았다*/)
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
		return S_OK;
	}
	if (0.5f <= fCurAnimationTime && m_pNero.lock()->CheckIsGround())
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LANDING);
		return S_OK;
	}

	if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
		KeyInput_Jump();
	else
		KeyInput_Cbs_Jump();

	Move_Jump();
	return S_OK;
}

Jump_Fly_Loop::Jump_Fly_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Jump_Fly_Loop::~Jump_Fly_Loop()
{
}

Jump_Fly_Loop* Jump_Fly_Loop::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Jump_Fly_Loop(_pFSM, _nIndex, _pNero);
}

HRESULT Jump_Fly_Loop::StateEnter()
{
	m_pNero.lock()->StopAnimation();
	return S_OK;
}

HRESULT Jump_Fly_Loop::StateExit()
{
	m_pNero.lock()->ContinueAnimiation();
	return S_OK;
}

HRESULT Jump_Fly_Loop::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
		NeroState::KeyInput_Jump();
	else
		NeroState::KeyInput_Cbs_Jump();

	//테스트용
	if (m_pNero.lock()->CheckIsGround())
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LANDING);
		return S_OK;
	}

	if (Input::GetKey(DIK_M))
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LANDING);
	}

	Move_Jump();
	return S_OK;
}

Jump_Twice::Jump_Twice(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Jump_Twice::~Jump_Twice()
{
}

Jump_Twice* Jump_Twice::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Jump_Twice(_pFSM, _nIndex, _pNero);
}

HRESULT Jump_Twice::StateEnter()
{
	NeroState::StateEnter();

	m_pNero.lock()->DecreaseJumpCount();
	
	UINT iJumpDir = m_pNero.lock()->Get_JumpDir();

	switch (iJumpDir)
	{
	case Nero::Basic:
		if(Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pNero.lock()->ChangeAnimation("Jump_Twice", false, Nero::ANI_JUMP_TWICE);
		else
			m_pNero.lock()->ChangeAnimation("Cbs_JumpTwice", false, Nero::ANI_CBS_JUMP_TWICE);
		break;
	case Nero::Front:
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pNero.lock()->ChangeAnimation("Jump_Front_Twice", false, Nero::ANI_JUMP_FRONT_TWICE);
		else
			m_pNero.lock()->ChangeAnimation("Cbs_JumpFront_Twice", false, Nero::ANI_CBS_JUMP_FRONT_TWICE);
		break;
	case Nero::Back:
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pNero.lock()->ChangeAnimation("Jump_Back_Twice", false, Nero::ANI_JUMP_BACK_TWICE);
		else
			m_pNero.lock()->ChangeAnimation("Cbs_JumpBack_Twice", false, Nero::ANI_CBS_JUMP_BACK_TWICE);
		break;
	default:
		break;
	}
	m_pNero.lock()->SetLinearVelocity();
	m_pNero.lock()->SetAddForce({ 0.f,120.f,0.f });
	m_pNero.lock()->PlayEffect(Eff_AirHike);
	m_pNero.lock()->PlayEffect(Eff_CircleWave);
	return S_OK;
}

HRESULT Jump_Twice::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Jump_Twice::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
		return S_OK;
	}
	if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
		NeroState::KeyInput_Jump();
	else
		NeroState::KeyInput_Cbs_Jump();


	Move_Jump();
	return S_OK;
}


Jump_Front_Landing::Jump_Front_Landing(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Jump_Front_Landing::~Jump_Front_Landing()
{
}

Jump_Front_Landing* Jump_Front_Landing::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Jump_Front_Landing(_pFSM, _nIndex, _pNero);
}

HRESULT Jump_Front_Landing::StateEnter()
{
	NeroState::StateEnter();

	UINT iJumpDir = m_pNero.lock()->Get_JumpDir();

	switch (iJumpDir)
	{
	case Nero::Basic:
		if(Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pNero.lock()->ChangeAnimation("Jump_Landing", false, Nero::ANI_JUMP_LANDING);
		else
			m_pNero.lock()->ChangeAnimation("Cbs_Landing", false, Nero::ANI_CBS_JUMP_LANDING);
		break;
	case Nero::Front:
		if(Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pNero.lock()->ChangeAnimation("Jump_Front_Landing", false, Nero::ANI_JUMP_FRONT_LANDING);
		else
			m_pNero.lock()->ChangeAnimation("Cbs_Landing", false, Nero::ANI_CBS_JUMP_LANDING);
		break;
	default:		
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
		m_pNero.lock()->ChangeAnimation("Jump_Landing", false, Nero::ANI_JUMP_LANDING);
		   else
		m_pNero.lock()->ChangeAnimation("Cbs_Landing", false, Nero::ANI_CBS_JUMP_LANDING);
		break;
	}
	m_pNero.lock()->Reset_JumpCount();
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
	return S_OK;
}

HRESULT Jump_Front_Landing::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Jump_Front_Landing::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.6f <= fCurAnimationTime)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Idle(NeroFSM::JUMP_LANDING);
		else
			NeroState::KeyInput_Cbs_Idle();
	}

	if (m_pNero.lock()->IsAnimationEnd())
	{
		UINT  iCurWeaponIndex = m_pNero.lock()->Get_CurWeaponIndex();
		switch (iCurWeaponIndex)
		{
		case Nero::NeroCom_RedQueen:
			m_pFSM->ChangeState(NeroFSM::IDLE);
			break;
		case Nero::NeroCom_Cbs_Short:
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
			break;
		case Nero::NeroCom_Cbs_Middle:
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
			break;
		case Nero::NeroCom_Cbs_Long:
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
			break;
		default:
			m_pFSM->ChangeState(NeroFSM::IDLE);
			break;
		}

		return S_OK;
	}
	return S_OK;
}

#pragma endregion

#pragma region RUN // 달리기

RunStartLeft::RunStartLeft(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

RunStartLeft::~RunStartLeft()
{
}

RunStartLeft* RunStartLeft::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new RunStartLeft(_pFSM, _nIndex, _pNero);
}

HRESULT RunStartLeft::StateEnter()
{
	NeroState::StateEnter();
	UINT CurAnimationIndex = m_pNero.lock()->Get_CurAnimationIndex();
	UINT NeroPreDir = m_pNero.lock()->Get_PreDirIndex();
	

	if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
	{
		switch (CurAnimationIndex)
		{
		case Nero::ANI_IDLE_NORMAL:
			if (Nero::Dir_Left == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart270", false, Nero::ANI_RUNSTART270);
				m_pNero.lock()->SetRotationAngle(-90.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_IDLE_BATTLE:
			if (Nero::Dir_Left == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart270", false, Nero::ANI_RUNSTART270);
				m_pNero.lock()->SetRotationAngle(-90.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_IDLE_FROM_COMBOA1:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA1", false, Nero::ANI_RUNSTART_FROM_COMBOA1);
			if (Nero::Dir_Left != NeroPreDir)
				m_pNero.lock()->SetRotationAngle(-90.f);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_IDLE_FROM_COMBOA1_LOOP:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA1", false, Nero::ANI_RUNSTART_FROM_COMBOA1);
			if (Nero::Dir_Left != NeroPreDir)
				m_pNero.lock()->SetRotationAngle(-90.f);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_IDLE_FROM_COMBOA1_END:
			if (Nero::Dir_Left == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart270", false, Nero::ANI_RUNSTART270);
				m_pNero.lock()->SetRotationAngle(-90.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_IDLE_FROM_COMBOA2:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA2", false, Nero::ANI_RUNSTART_FROM_COMBOA2);
			if (Nero::Dir_Left != NeroPreDir)
				m_pNero.lock()->SetRotationAngle(-90.f);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_IDLE_FROM_COMBOA2_LOOP:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA2", false, Nero::ANI_RUNSTART_FROM_COMBOA2);
			if (Nero::Dir_Left != NeroPreDir)
				m_pNero.lock()->SetRotationAngle(-90.f);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_IDLE_FROM_COMBOA2_END:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA2", false, Nero::ANI_RUNSTART_FROM_COMBOA2);
			if (Nero::Dir_Left != NeroPreDir)
				m_pNero.lock()->SetRotationAngle(-90.f);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_SHUFFLE:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA2", false, Nero::ANI_RUNSTART_FROM_COMBOA2);
			if (Nero::Dir_Left != NeroPreDir)
				m_pNero.lock()->SetRotationAngle(-90.f);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_COMBOD4:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA1", false, Nero::ANI_RUNSTART_FROM_COMBOA2);
			if (Nero::Dir_Left != NeroPreDir)
				m_pNero.lock()->SetRotationAngle(-90.f);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_RUNSTOP:
			if (Nero::Dir_Left == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart270", false, Nero::ANI_RUNSTART270);
				m_pNero.lock()->SetRotationAngle(-90.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_OVERTURE_IDLE:
			if (Nero::Dir_Left == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart270", false, Nero::ANI_RUNSTART270);
				m_pNero.lock()->SetRotationAngle(-90.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_OVERTURE_SHOOT:
			if (Nero::Dir_Left == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart270", false, Nero::ANI_RUNSTART270);
				m_pNero.lock()->SetRotationAngle(-90.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_OVERTURE_SHOOT_UP:
			if (Nero::Dir_Left == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart270", false, Nero::ANI_RUNSTART270);
				m_pNero.lock()->SetRotationAngle(-90.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_OVERTURE_SHOOT_DOWN:
			if (Nero::Dir_Left == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart270", false, Nero::ANI_RUNSTART270);
				m_pNero.lock()->SetRotationAngle(-90.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_WIRE_SNATCH_PULL:
			if (Nero::Dir_Left == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart270", false, Nero::ANI_RUNSTART270);
				m_pNero.lock()->SetRotationAngle(-90.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_COMBOA4:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA1", false, Nero::ANI_RUNSTART_FROM_COMBOA1);
			if (Nero::Dir_Left != NeroPreDir)
				m_pNero.lock()->SetRotationAngle(-90.f);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_DASHSTOP:
			if (Nero::Dir_Left == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart270", false, Nero::ANI_RUNSTART270);
				m_pNero.lock()->SetRotationAngle(-90.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_JUMP_FRONT_LANDING:
			if (Nero::Dir_Left == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart270", false, Nero::ANI_RUNSTART270);
				m_pNero.lock()->SetRotationAngle(-90.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_JUMP_LANDING:
			if (Nero::Dir_Left == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart270", false, Nero::ANI_RUNSTART270);
				m_pNero.lock()->SetRotationAngle(-90.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		default:
			if (Nero::Dir_Left == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart270", false, Nero::ANI_RUNSTART270);
				m_pNero.lock()->SetRotationAngle(-90.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		}
	}
	else
	{
		NeroState::SetCbsIdle();
		if (Nero::Dir_Left == NeroPreDir)
		{
			m_pNero.lock()->ChangeAnimation("Cbs_RunStartFront", false, Nero::ANI_CBS_RUNSTART0);
		}
		else
		{
			m_pNero.lock()->ChangeAnimation("Cbs_RunStart270", false, Nero::ANI_CBS_RUNSTART270);
			m_pNero.lock()->SetRotationAngle(-90.f);
		}
	}

	if (Nero::Dir_Left != NeroPreDir)
		m_pNero.lock()->SetAngleFromCamera();
	m_pNero.lock()->ChangeNeroDirection(Nero::Dir_Left);

	return S_OK;
}

HRESULT RunStartLeft::StateExit()
{
	//끝날때 회전
	NeroState::StateExit();
	m_pNero.lock()->ChangeNeroDirection(Nero::Dir_Front);
	return S_OK;
}

HRESULT RunStartLeft::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	m_pNero.lock()->SetAngleFromCamera();

	if (Input::GetKey(DIK_A))
	{
		if (m_pNero.lock()->IsAnimationEnd())
		{
			m_pFSM->ChangeState(NeroFSM::RUNLOOP);
			return S_OK;
		}
	}
	else
	{
		m_pNero.lock()->ChangeNeroDirection(Nero::Dir_Front);
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
		{
			m_pFSM->ChangeState(NeroFSM::IDLE);
			return S_OK;
		}
		else
		{
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
			return S_OK;
		}
	}
	return S_OK;
}

RunStartRight::RunStartRight(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

RunStartRight::~RunStartRight()
{
}

RunStartRight* RunStartRight::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new RunStartRight(_pFSM, _nIndex, _pNero);
}

HRESULT RunStartRight::StateEnter()
{
	NeroState::StateEnter();
	UINT CurAnimationIndex = m_pNero.lock()->Get_CurAnimationIndex();
	UINT NeroPreDir = m_pNero.lock()->Get_PreDirIndex();


	if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
	{
		switch (CurAnimationIndex)
		{
		case Nero::ANI_IDLE_NORMAL:
			if (Nero::Dir_Right == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart90", false, Nero::ANI_RUNSTART90);
				m_pNero.lock()->SetRotationAngle(90.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_IDLE_BATTLE:
			if (Nero::Dir_Right == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart90", false, Nero::ANI_RUNSTART90);
				m_pNero.lock()->SetRotationAngle(90.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_IDLE_FROM_COMBOA1:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA1", false, Nero::ANI_RUNSTART_FROM_COMBOA1);
			if (Nero::Dir_Right != NeroPreDir)
				m_pNero.lock()->SetRotationAngle(90.f);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_IDLE_FROM_COMBOA1_LOOP:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA1", false, Nero::ANI_RUNSTART_FROM_COMBOA1);
			if (Nero::Dir_Right != NeroPreDir)
				m_pNero.lock()->SetRotationAngle(90.f);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_IDLE_FROM_COMBOA1_END:
			if (Nero::Dir_Right == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart90", false, Nero::ANI_RUNSTART90);
				m_pNero.lock()->SetRotationAngle(90.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_IDLE_FROM_COMBOA2:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA2", false, Nero::ANI_RUNSTART_FROM_COMBOA2);
			if (Nero::Dir_Right != NeroPreDir)
				m_pNero.lock()->SetRotationAngle(90.f);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_IDLE_FROM_COMBOA2_LOOP:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA2", false, Nero::ANI_RUNSTART_FROM_COMBOA2);
			if (Nero::Dir_Right != NeroPreDir)
				m_pNero.lock()->SetRotationAngle(90.f);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_IDLE_FROM_COMBOA2_END:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA2", false, Nero::ANI_RUNSTART_FROM_COMBOA2);
			if (Nero::Dir_Right != NeroPreDir)
				m_pNero.lock()->SetRotationAngle(90.f);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_SHUFFLE:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA2", false, Nero::ANI_RUNSTART_FROM_COMBOA2);
			if (Nero::Dir_Right != NeroPreDir)
				m_pNero.lock()->SetRotationAngle(90.f);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_COMBOD4:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA1", false, Nero::ANI_RUNSTART_FROM_COMBOA2);
			if (Nero::Dir_Right != NeroPreDir)
				m_pNero.lock()->SetRotationAngle(90.f);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_RUNSTOP:
			if (Nero::Dir_Right == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart90", false, Nero::ANI_RUNSTART90);
				m_pNero.lock()->SetRotationAngle(90.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_OVERTURE_IDLE:
			if (Nero::Dir_Right == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart90", false, Nero::ANI_RUNSTART90);
				m_pNero.lock()->SetRotationAngle(90.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_OVERTURE_SHOOT:
			if (Nero::Dir_Right == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart90", false, Nero::ANI_RUNSTART90);
				m_pNero.lock()->SetRotationAngle(90.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_OVERTURE_SHOOT_UP:
			if (Nero::Dir_Right == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart90", false, Nero::ANI_RUNSTART90);
				m_pNero.lock()->SetRotationAngle(90.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_OVERTURE_SHOOT_DOWN:
			if (Nero::Dir_Right == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart90", false, Nero::ANI_RUNSTART90);
				m_pNero.lock()->SetRotationAngle(90.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_WIRE_SNATCH_PULL:
			if (Nero::Dir_Right == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart90", false, Nero::ANI_RUNSTART90);
				m_pNero.lock()->SetRotationAngle(90.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_COMBOA4:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA1", false, Nero::ANI_RUNSTART_FROM_COMBOA1);
			if (Nero::Dir_Right != NeroPreDir)
				m_pNero.lock()->SetRotationAngle(90.f);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_DASHSTOP:
			if (Nero::Dir_Right == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart90", false, Nero::ANI_RUNSTART90);
				m_pNero.lock()->SetRotationAngle(90.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_JUMP_FRONT_LANDING:
			if (Nero::Dir_Right == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart90", false, Nero::ANI_RUNSTART90);
				m_pNero.lock()->SetRotationAngle(90.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_JUMP_LANDING:
			if (Nero::Dir_Right == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart90", false, Nero::ANI_RUNSTART90);
				m_pNero.lock()->SetRotationAngle(90.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		default:
			if (Nero::Dir_Right == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart90", false, Nero::ANI_RUNSTART90);
				m_pNero.lock()->SetRotationAngle(90.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		}
	}
	else
	{
		NeroState::SetCbsIdle();
		if (Nero::Dir_Right == NeroPreDir)
		{
			m_pNero.lock()->ChangeAnimation("Cbs_RunStartFront", false, Nero::ANI_CBS_RUNSTART0);
		}
		else
		{
			m_pNero.lock()->ChangeAnimation("Cbs_RunStart90", false, Nero::ANI_CBS_RUNSTART90);
			m_pNero.lock()->SetRotationAngle(90.f);
		}
	}

	if (Nero::Dir_Right != NeroPreDir)
		m_pNero.lock()->SetAngleFromCamera();
	m_pNero.lock()->ChangeNeroDirection(Nero::Dir_Right);
	return S_OK;
}

HRESULT RunStartRight::StateExit()
{
	//끝날때 회전
	NeroState::StateExit();
	m_pNero.lock()->ChangeNeroDirection(Nero::Dir_Front);

	return S_OK;
}

HRESULT RunStartRight::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	m_pNero.lock()->SetAngleFromCamera();

	if (Input::GetKey(DIK_D))
	{
		if (m_pNero.lock()->IsAnimationEnd())
		{
			m_pFSM->ChangeState(NeroFSM::RUNLOOP);
			return S_OK;
		}
	}
	else
	{
		m_pNero.lock()->ChangeNeroDirection(Nero::Dir_Front);
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
		{
			m_pFSM->ChangeState(NeroFSM::IDLE);
			return S_OK;
		}
		else
		{
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
			return S_OK;
		}
	}

	return S_OK;
}

RunStart180::RunStart180(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

RunStart180::~RunStart180()
{
}

RunStart180* RunStart180::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new RunStart180(_pFSM, _nIndex, _pNero);
}

HRESULT RunStart180::StateEnter()
{
	NeroState::StateEnter();
	UINT CurAnimationIndex = m_pNero.lock()->Get_CurAnimationIndex();
	UINT NeroPreDir = m_pNero.lock()->Get_PreDirIndex();


	if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
	{
		switch (CurAnimationIndex)
		{
		case Nero::ANI_IDLE_NORMAL:
			if (Nero::Dir_Back == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart180", false, Nero::ANI_RUNSTART180);
				m_pNero.lock()->SetRotationAngle(180.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_IDLE_BATTLE:
			if (Nero::Dir_Back == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart180", false, Nero::ANI_RUNSTART180);
				m_pNero.lock()->SetRotationAngle(180.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_IDLE_FROM_COMBOA1:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA1", false, Nero::ANI_RUNSTART_FROM_COMBOA1);
			if (Nero::Dir_Back != NeroPreDir)
				m_pNero.lock()->SetRotationAngle(180.f);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_IDLE_FROM_COMBOA1_LOOP:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA1", false, Nero::ANI_RUNSTART_FROM_COMBOA1);
			if (Nero::Dir_Back != NeroPreDir)
				m_pNero.lock()->SetRotationAngle(180.f);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_IDLE_FROM_COMBOA1_END:
			if (Nero::Dir_Back == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart180", false, Nero::ANI_RUNSTART180);
				m_pNero.lock()->SetRotationAngle(180.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_IDLE_FROM_COMBOA2:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA2", false, Nero::ANI_RUNSTART_FROM_COMBOA2);
			if (Nero::Dir_Back != NeroPreDir)
				m_pNero.lock()->SetRotationAngle(180.f);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_IDLE_FROM_COMBOA2_LOOP:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA2", false, Nero::ANI_RUNSTART_FROM_COMBOA2);
			if (Nero::Dir_Back != NeroPreDir)
				m_pNero.lock()->SetRotationAngle(180.f);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_IDLE_FROM_COMBOA2_END:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA2", false, Nero::ANI_RUNSTART_FROM_COMBOA2);
			if (Nero::Dir_Back != NeroPreDir)
				m_pNero.lock()->SetRotationAngle(180.f);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_SHUFFLE:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA2", false, Nero::ANI_RUNSTART_FROM_COMBOA2);
			if (Nero::Dir_Back != NeroPreDir)
				m_pNero.lock()->SetRotationAngle(180.f);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_COMBOD4:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA1", false, Nero::ANI_RUNSTART_FROM_COMBOA2);
			if (Nero::Dir_Back != NeroPreDir)
				m_pNero.lock()->SetRotationAngle(180.f);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_RUNSTOP:
			if (Nero::Dir_Back == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart180", false, Nero::ANI_RUNSTART180);
				m_pNero.lock()->SetRotationAngle(180.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_OVERTURE_IDLE:
			if (Nero::Dir_Back == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart180", false, Nero::ANI_RUNSTART180);
				m_pNero.lock()->SetRotationAngle(180.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_OVERTURE_SHOOT:
			if (Nero::Dir_Back == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart180", false, Nero::ANI_RUNSTART180);
				m_pNero.lock()->SetRotationAngle(180.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_OVERTURE_SHOOT_UP:
			if (Nero::Dir_Back == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart180", false, Nero::ANI_RUNSTART180);
				m_pNero.lock()->SetRotationAngle(180.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_OVERTURE_SHOOT_DOWN:
			if (Nero::Dir_Back == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart180", false, Nero::ANI_RUNSTART180);
				m_pNero.lock()->SetRotationAngle(180.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_WIRE_SNATCH_PULL:
			if (Nero::Dir_Back == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart180", false, Nero::ANI_RUNSTART180);
				m_pNero.lock()->SetRotationAngle(180.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_COMBOA4:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA1", false, Nero::ANI_RUNSTART_FROM_COMBOA1);
			if (Nero::Dir_Back != NeroPreDir)
				m_pNero.lock()->SetRotationAngle(180.f);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_DASHSTOP:
			if (Nero::Dir_Back == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart180", false, Nero::ANI_RUNSTART180);
				m_pNero.lock()->SetRotationAngle(180.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_JUMP_FRONT_LANDING:
			if (Nero::Dir_Back == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart180", false, Nero::ANI_RUNSTART180);
				m_pNero.lock()->SetRotationAngle(180.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_JUMP_LANDING:
			if (Nero::Dir_Back == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart180", false, Nero::ANI_RUNSTART180);
				m_pNero.lock()->SetRotationAngle(180.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		default:
			if (Nero::Dir_Back == NeroPreDir)
			{
				m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			}
			else
			{
				m_pNero.lock()->ChangeAnimation("RunStart180", false, Nero::ANI_RUNSTART180);
				m_pNero.lock()->SetRotationAngle(180.f);
			}
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		}
	}
	else
	{
		NeroState::SetCbsIdle();
		if (Nero::Dir_Back == NeroPreDir)
		{
			m_pNero.lock()->ChangeAnimation("Cbs_RunStartStart", false, Nero::ANI_CBS_RUNSTART0);
		}
		else
		{
			m_pNero.lock()->ChangeAnimation("Cbs_RunStart180", false, Nero::ANI_CBS_RUNSTART180);
			m_pNero.lock()->SetRotationAngle(180.f);
		}
	}

	if (Nero::Dir_Back != NeroPreDir)
		m_pNero.lock()->SetAngleFromCamera();
	m_pNero.lock()->ChangeNeroDirection(Nero::Dir_Back);
	return S_OK;
}

HRESULT RunStart180::StateExit()
{
	NeroState::StateExit();
	m_pNero.lock()->ChangeNeroDirection(Nero::Dir_Front);
	return S_OK;
}

HRESULT RunStart180::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	m_pNero.lock()->SetAngleFromCamera();
	if (Input::GetKey(DIK_S))
	{
		if (m_pNero.lock()->IsAnimationEnd())
		{
			m_pFSM->ChangeState(NeroFSM::RUNLOOP);
			return S_OK;
		}
	}
	else
	{
		m_pNero.lock()->ChangeNeroDirection(Nero::Dir_Front);
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
		{
			m_pFSM->ChangeState(NeroFSM::IDLE);
			return S_OK;
		}
		else
		{
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
			return S_OK;
		}
	}
	return S_OK;
}



RunLoop::RunLoop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

RunLoop::~RunLoop()
{
}

RunLoop* RunLoop::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new RunLoop(_pFSM, _nIndex, _pNero);
}

HRESULT RunLoop::StateEnter()
{
	NeroState::StateEnter();
	UINT CurAnimationIndex = m_pNero.lock()->Get_CurAnimationIndex();

	if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
	{
		switch (CurAnimationIndex)
		{
		case Nero::ANI_RUNSTART_FROM_COMBOA1:
			m_pNero.lock()->ChangeAnimation("RunLoop_From_ComboA1", true, Nero::ANI_RUNLOOP_FROM_COMBOA1);
			break;
		case Nero::ANI_RUNSTART_FROM_COMBOA2:
			m_pNero.lock()->ChangeAnimation("RunLoop_From_ComboA1", true, Nero::ANI_RUNLOOP_FROM_COMBOA1);
			break;
		default:
			m_pNero.lock()->ChangeAnimation("RunLoop", true, Nero::ANI_RUNLOOP);
			break;
		}
	}
	else
		m_pNero.lock()->ChangeAnimation("Cbs_RunLoop", true, Nero::ANI_CBS_RUNLOOP);

	

	return S_OK;
}

HRESULT RunLoop::StateExit()
{
	NeroState::StateExit();
	//m_pNero.lock()->SetRotationAngle(0.f);
	m_fRequireTimeForDash = 0.f;
	return S_OK;
}

HRESULT RunLoop::StateUpdate(const float _fDeltaTime)
{
	//NeroState::StateUpdate(_fDeltaTime);
	if (Input::GetKey(DIK_LSHIFT))
		m_pNero.lock()->Locking();
	else
		m_pNero.lock()->SetOffLockOnMonster();

	m_pNero.lock()->SetAngleFromCamera();
	m_pNero.lock()->IncreaseDistance(MaxDistance,_fDeltaTime);
	if (Input::GetKey(DIK_W) || Input::GetKey(DIK_S) || Input::GetKey(DIK_A) || Input::GetKey(DIK_D))
	{
		//키입력이 특정 시간이 넘었다
		//-> 그러면 대쉬 루프로 변환하고
		//칼들고 달리고있었으면 칼 집어넣고
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
		{
			KeyInput_Run(NeroFSM::RUNLOOP);
			m_fRequireTimeForDash += _fDeltaTime;
			if (m_fRequireTimeForDash >= 1.2f
				&& Nero::ANI_RUNLOOP == m_pNero.lock()->Get_CurAnimationIndex())
			{
				m_pFSM->ChangeState(NeroFSM::DASHLOOP);
				return S_OK;
			}
		}
		else
			KeyInput_Cbs_Run();
	}
	else
	{
		//그냥 달리는거면 이거고
		m_pFSM->ChangeState(NeroFSM::RUNSTOP);
		return S_OK;
		//칼들고 달리는거면 멈추면서 칼만 넣는거

	}
	return S_OK;
}

RunStartFront::RunStartFront(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

RunStartFront::~RunStartFront()
{
}

RunStartFront* RunStartFront::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new RunStartFront(_pFSM, _nIndex, _pNero);
}

HRESULT RunStartFront::StateEnter()
{
	NeroState::StateEnter();
	//앞으로 달리는 애니메이션으로 세팅
	UINT CurAnimationIndex = m_pNero.lock()->Get_CurAnimationIndex();
	UINT PreAnimationIndex = m_pNero.lock()->Get_PreAnimationIndex();
	m_pNero.lock()->Reset_RotationAngle();
	m_pNero.lock()->Reset_RootRotation();
	m_pNero.lock()->SetAngleFromCamera();

	m_pNero.lock()->ChangeNeroDirection(Nero::Dir_Front);
	if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
	{
		switch (CurAnimationIndex)
		{
		case Nero::ANI_IDLE_NORMAL:
			m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_IDLE_BATTLE:
			m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_IDLE_FROM_COMBOA1:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA1", false, Nero::ANI_RUNSTART_FROM_COMBOA1);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_IDLE_FROM_COMBOA1_LOOP:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA1", false, Nero::ANI_RUNSTART_FROM_COMBOA1);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_IDLE_FROM_COMBOA1_END:
			m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_IDLE_FROM_COMBOA2:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA2", false, Nero::ANI_RUNSTART_FROM_COMBOA2);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_IDLE_FROM_COMBOA2_LOOP:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA2", false, Nero::ANI_RUNSTART_FROM_COMBOA2);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_IDLE_FROM_COMBOA2_END:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA2", false, Nero::ANI_RUNSTART_FROM_COMBOA2);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_SHUFFLE:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA2", false, Nero::ANI_RUNSTART_FROM_COMBOA2);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		case Nero::ANI_RUNSTOP:
			m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_OVERTURE_IDLE:
			m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_OVERTURE_SHOOT:
			m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_OVERTURE_SHOOT_UP:
			m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_OVERTURE_SHOOT_DOWN:
			m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_WIRE_SNATCH_PULL:
			m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_DASHSTOP:
			m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_JUMP_LANDING:
			m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		case Nero::ANI_JUMP_FRONT_LANDING:
			m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			break;
		default:
			m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA1", false, Nero::ANI_RUNSTART_FROM_COMBOA1);
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
			break;
		}
	}
	else
	{
		//단테 달리기로 바꾸기
		NeroState::SetCbsIdle();
		m_pNero.lock()->ChangeAnimation("Cbs_RunStartFront", false, Nero::ANI_CBS_RUNSTART0);
	}
	

	return S_OK;
}

HRESULT RunStartFront::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT RunStartFront::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	// 시작하는 애니메니메이션이 끝났으면 Loop로 변환
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();
	m_pNero.lock()->SetAngleFromCamera();
	if (Input::GetKey(DIK_W))
	{
		if (0.2f >= fCurAnimationTime)
			m_pNero.lock()->IncreaseDistance(MaxDistance, _fDeltaTime);
		if (m_pNero.lock()->IsAnimationEnd())
		{
			m_pFSM->ChangeState(NeroFSM::RUNLOOP);
			return S_OK;
		}
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Run();
		else
			NeroState::KeyInput_Cbs_Run();
	}
	else
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
		{
			m_pFSM->ChangeState(NeroFSM::IDLE);
			return S_OK;
		}
		else
		{
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
			return S_OK;
		}
	}

	return S_OK;
}

RunStop::RunStop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

RunStop::~RunStop()
{
}

RunStop* RunStop::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new RunStop(_pFSM, _nIndex, _pNero);
}

HRESULT RunStop::StateEnter()
{
	NeroState::StateEnter();
	//상태에 따라서
	UINT CurWeaponIndex = m_pNero.lock()->Get_CurWeaponIndex();
	UINT CurAnimationIndex = m_pNero.lock()->Get_CurAnimationIndex();

	if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
	{
		if (Nero::ANI_RUNLOOP_FROM_COMBOA1 == CurAnimationIndex)
			m_pNero.lock()->ChangeAnimation("Idle_From_ComboA1_End", false, Nero::ANI_IDLE_FROM_COMBOA1_END);
		else if (Nero::ANI_RUNSTART_FROM_COMBOA1 == CurAnimationIndex)
		{
			m_pNero.lock()->ChangeAnimation("Idle_From_ComboA1_End", false, Nero::ANI_IDLE_FROM_COMBOA1_END);
		}
		else if (Nero::ANI_RUNSTART_FROM_COMBOA2 == CurAnimationIndex)
		{
			m_pNero.lock()->ChangeAnimation("Idle_From_ComboA1_End", false, Nero::ANI_IDLE_FROM_COMBOA1_END);
		}
		else
			m_pNero.lock()->ChangeAnimation("RunStop", false, Nero::ANI_RUNSTOP);
	}
	else
		m_pNero.lock()->ChangeAnimation("Cbs_RunStop", false, Nero::ANI_CBS_RUNSTOP);
	

	return S_OK;
}

HRESULT RunStop::StateExit()
{
	NeroState::StateExit();
	m_pNero.lock()->ChangeNeroDirection(Nero::Dir_Front);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
	return S_OK;
}

HRESULT RunStop::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	UINT CurAnimationIndex = m_pNero.lock()->Get_CurAnimationIndex();
	m_pNero.lock()->DecreaseDistance(OGDistance, _fDeltaTime);
	if (0.5f <= fCurrAnimationTime && Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
	{
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
	}
	if (0.57f <= fCurrAnimationTime)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Idle(NeroFSM::RUNSTOP);
		else
		{
			NeroState::KeyInput_Cbs_Idle();
		}
	}
	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
		{
			m_pFSM->ChangeState(NeroFSM::IDLE);
			return S_OK;
		}
		else
		{
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
			return S_OK;
		}
	}

	return S_OK;
}

RunTurn::RunTurn(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

RunTurn::~RunTurn()
{
}

RunTurn* RunTurn::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new RunTurn(_pFSM, _nIndex, _pNero);
}

HRESULT RunTurn::StateEnter()
{
	return S_OK;
}

HRESULT RunTurn::StateExit()
{
	return S_OK;
}

HRESULT RunTurn::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	return S_OK;
}

RunTurn_L::RunTurn_L(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

RunTurn_L::~RunTurn_L()
{
}

RunTurn_L* RunTurn_L::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new RunTurn_L(_pFSM, _nIndex, _pNero);
}

HRESULT RunTurn_L::StateEnter()
{
	return S_OK;
}

HRESULT RunTurn_L::StateExit()
{
	return S_OK;
}

HRESULT RunTurn_L::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	return S_OK;
}
#pragma endregion

#pragma region DASH // 대쉬



DashLoop::DashLoop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

DashLoop::~DashLoop()
{
}

DashLoop* DashLoop::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new DashLoop(_pFSM, _nIndex, _pNero);
}

HRESULT DashLoop::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("DashLoop", true, Nero::ANI_DASHLOOP);
	m_pNero.lock()->ChangeNeroDirection(Nero::Dir_Front);
	m_pNero.lock()->ChangeNeroDirection(Nero::Dir_Front);
	m_fGradient = 0.f;
	return S_OK;
}

HRESULT DashLoop::StateExit()
{
	return S_OK;
}

HRESULT DashLoop::StateUpdate(const float _fDeltaTime)
{
	//NeroState::StateUpdate(_fDeltaTime);
	UINT Ex_Gauge = m_pNero.lock()->Get_ExGaugeCount();
	m_pNero.lock()->IncreaseDistance(MaxDistance, _fDeltaTime);
	if (Input::GetKey(DIK_LSHIFT))
	{
		//락온
		if (Input::GetKey(DIK_Q))
		{
			m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
			m_pFSM->ChangeState(NeroFSM::WIRE_SNATCH_PULL);
			return S_OK;
		}
		else if (Input::GetKey(DIK_W))
		{
			if (Input::GetKey(DIK_S) && Input::GetMouse(DIM_L))
			{
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
				if (Ex_Gauge > 0)
					m_pFSM->ChangeState(NeroFSM::SKILL_SHUFFLE_EX);
				else
					m_pFSM->ChangeState(NeroFSM::SKILL_SHUFFLE);
				return S_OK;
			}
			else if (Input::GetMouse(DIM_L))
			{
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
				if (Ex_Gauge > 0)
					m_pFSM->ChangeState(NeroFSM::SKILL_STREAK_EX3);
				else
					m_pFSM->ChangeState(NeroFSM::SKILL_STREAK);
				return S_OK;
			}
			else if (Input::GetMouse(DIM_R))
			{

			}
			else if (Input::GetMouse(DIM_M))
			{
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
				m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT_UP);
				return S_OK;
			}
		}
		else if (Input::GetKey(DIK_S))
		{
			if (Input::GetMouse(DIM_L))
			{
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
				m_pFSM->ChangeState(NeroFSM::SKILL_FLOAT_GROUND);
				return S_OK;
			}
			else if (Input::GetMouse(DIM_R))
			{
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
				m_pFSM->ChangeState(NeroFSM::SKILL_HR_EX_START);
				return S_OK;
			}
			else if (Input::GetMouse(DIM_M))
			{
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
				m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT_DOWN);
				return S_OK;
			}
			else if (Input::GetKey(DIK_SPACE))
			{
				m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
				m_pNero.lock()->Set_JumpDir(Nero::Back);
				m_pFSM->ChangeState(NeroFSM::JUMP_START);

				return S_OK;
			}
		}
	}

	if (Input::GetMouse(DIM_L))
	{
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
		m_pFSM->ChangeState(NeroFSM::ATT1_DASH);
		return S_OK;
	}
	else if (Input::GetMouse(DIM_M))
	{
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
		m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT);
		return S_OK;
	}
	else if (Input::GetKeyDown(DIK_SPACE))
	{
		//점프
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
		m_pNero.lock()->Set_JumpDir(Nero::Front);
		m_pFSM->ChangeState(NeroFSM::JUMP_START);
		return S_OK;
	}
	else if (Input::GetKeyDown(DIK_LCONTROL))
	{
	}

	else if (Input::GetKey(DIK_Q))
	{
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
		m_pFSM->ChangeState(NeroFSM::BUSTER_START);
		return S_OK;
	}

	else if (Input::GetKey(DIK_F))
	{
		if (m_pNero.lock()->Get_IsMajinMode())
			return S_OK;
		//변신게이지 있는지 체크
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
		m_pFSM->ChangeState(NeroFSM::TO_MAJIN);
		return S_OK;
	}

	else if (Input::GetKey(DIK_W))
	{
		if (Input::GetKey(DIK_A))
		{
			//왼쪽으로 45도 회전 // 점차 누적시켜서 45도 완성
			m_fGradient = FMath::Lerp(m_fGradient, -60.f * m_pNero.lock()->GetDashLoopDir(), _fDeltaTime * 7.0);
			m_pNero.lock()->SetAngleFromCamera(m_fGradient);
		}
		else if (Input::GetKey(DIK_D))
		{
			//오른쪽으로 45도 회전
			m_fGradient = FMath::Lerp(m_fGradient, 60.f * m_pNero.lock()->GetDashLoopDir(), _fDeltaTime * 7.0);
			m_pNero.lock()->SetAngleFromCamera(m_fGradient);
		}
		else if (Input::GetKey(DIK_S))
		{
			//DashTurn으로 변환
			m_pFSM->ChangeState(NeroFSM::DASHTURN);
			m_pNero.lock()->SetAngleFromCamera();
			return S_OK;
		}
		else
		{
			m_fGradient = FMath::Lerp(m_fGradient, 0.f, _fDeltaTime * 7.0);
			m_pNero.lock()->SetAngleFromCamera(m_fGradient);
		}
	}
	else if (Input::GetKey(DIK_S))
	{
		if (Input::GetKey(DIK_A))
		{
			//왼쪽으로 45도 회전 // 점차 누적시켜서 45도 완성
			m_fGradient = FMath::Lerp(m_fGradient, -60.f * m_pNero.lock()->GetDashLoopDir(), _fDeltaTime * 7.0);
			m_pNero.lock()->SetAngleFromCamera(m_fGradient);
		}
		else if (Input::GetKey(DIK_D))
		{
			//오른쪽으로 45도 회전
			m_fGradient = FMath::Lerp(m_fGradient, 60.f * m_pNero.lock()->GetDashLoopDir(), _fDeltaTime * 7.f);
			m_pNero.lock()->SetAngleFromCamera(m_fGradient);
		}
		else if (Input::GetKey(DIK_W))
		{
			//DashTurn으로 변환
			m_pFSM->ChangeState(NeroFSM::DASHTURN);
			m_pNero.lock()->SetAngleFromCamera();
			return S_OK;
		}
		else
		{
			m_fGradient = FMath::Lerp(m_fGradient, 0.f, _fDeltaTime * 7.f);
			m_pNero.lock()->SetAngleFromCamera(m_fGradient);
		}
	}
	else if (Input::GetKey(DIK_A))
	{
		if (Input::GetKey(DIK_D))
		{
			m_pFSM->ChangeState(NeroFSM::DASHTURN);
			m_pNero.lock()->SetAngleFromCamera();
			return S_OK;
		}
		else
		{
			m_fGradient = FMath::Lerp(m_fGradient, -60.f * m_pNero.lock()->GetDashLoopDir(), _fDeltaTime * 7.0);
			m_pNero.lock()->SetAngleFromCamera(m_fGradient);
		}

	}
	else if (Input::GetKey(DIK_D))
	{
		if (Input::GetKey(DIK_A))
		{
			m_pFSM->ChangeState(NeroFSM::DASHTURN);
			m_pNero.lock()->SetAngleFromCamera();
			return S_OK;
		}
		else
		{
			m_fGradient = FMath::Lerp(m_fGradient, 60.f * m_pNero.lock()->GetDashLoopDir(), _fDeltaTime * 7.0);
			m_pNero.lock()->SetAngleFromCamera(m_fGradient);
		}
	}
	else
	{
		m_pFSM->ChangeState(NeroFSM::DASHSTOP);
		m_pNero.lock()->SetAngleFromCamera();
		return S_OK;
	}

	if (Input::GetKey(DIK_LSHIFT))
		m_pNero.lock()->Locking();
	else
		m_pNero.lock()->SetOffLockOnMonster();
	return S_OK;
}

DashStop::DashStop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

DashStop::~DashStop()
{
}

DashStop* DashStop::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new DashStop(_pFSM, _nIndex, _pNero);
}

HRESULT DashStop::StateEnter()
{
	NeroState::StateEnter();

	m_pNero.lock()->ChangeAnimation("DashStop", false, Nero::ANI_DASHSTOP);
	return S_OK;
}

HRESULT DashStop::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT DashStop::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	m_pNero.lock()->DecreaseDistance(OGDistance, _fDeltaTime);

	if (0.58f <= fCurAnimationTime)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Idle(NeroFSM::DASHSTOP);
		else
			NeroState::KeyInput_Cbs_Idle();
	}

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}
	return S_OK;
}

DashTurn::DashTurn(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

DashTurn::~DashTurn()
{
}

DashTurn* DashTurn::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new DashTurn(_pFSM, _nIndex, _pNero);
}

HRESULT DashTurn::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("DashTurn", false, Nero::ANI_DASHTURN);
	m_bRotationEnable = true;
	return S_OK;
}

HRESULT DashTurn::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT DashTurn::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	if (m_pNero.lock()->IsAnimationEnd())
	{
		//m_pNero.lock()->SetRotationAngle(180.f);
		if (Input::GetKey(DIK_W) || Input::GetKey(DIK_S) || Input::GetKey(DIK_A) || Input::GetKey(DIK_D))
		{
			m_pFSM->ChangeState(NeroFSM::DASHLOOP);
			m_pNero.lock()->SetDashLoopDir();
			return S_OK;
		}
		else
		{
			m_pFSM->ChangeState(NeroFSM::IDLE);
			return S_OK;
		}
	}
	m_pNero.lock()->SetAngleFromCamera();
	return S_OK;
}

#pragma endregion

#pragma region WALK // 걷기 




WalkLoop::WalkLoop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

WalkLoop::~WalkLoop()
{
}

WalkLoop* WalkLoop::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new WalkLoop(_pFSM, _nIndex, _pNero);
}

HRESULT WalkLoop::StateEnter()
{
	return S_OK;
}

HRESULT WalkLoop::StateExit()
{
	return S_OK;
}

HRESULT WalkLoop::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	return S_OK;
}

WalkStart::WalkStart(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

WalkStart::~WalkStart()
{
}

WalkStart* WalkStart::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new WalkStart(_pFSM, _nIndex, _pNero);
}

HRESULT WalkStart::StateEnter()
{
	return S_OK;
}

HRESULT WalkStart::StateExit()
{
	return S_OK;
}

HRESULT WalkStart::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	return S_OK;
}


WalkStop::WalkStop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

WalkStop::~WalkStop()
{
}

WalkStop* WalkStop::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new WalkStop(_pFSM, _nIndex, _pNero);
}

HRESULT WalkStop::StateEnter()
{
	return S_OK;
}

HRESULT WalkStop::StateExit()
{
	return S_OK;
}

HRESULT WalkStop::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	return S_OK;
}

#pragma endregion

#pragma region EVADE // 회피 



Evade_L::Evade_L(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Evade_L::~Evade_L()
{
}

Evade_L* Evade_L::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Evade_L(_pFSM, _nIndex, _pNero);
}

HRESULT Evade_L::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Evade_L", false, Nero::ANI_EVADE_L);

	return S_OK;
}

HRESULT Evade_L::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Evade_L::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	UINT CurWeaponIndex = m_pNero.lock()->Get_CurWeaponIndex();

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == CurWeaponIndex)
		{
			m_pFSM->ChangeState(NeroFSM::IDLE);
			return S_OK;
		}
		else
		{
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
			return S_OK;
		}
	}


	return S_OK;
}

Evade_R::Evade_R(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Evade_R::~Evade_R()
{
}

Evade_R* Evade_R::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Evade_R(_pFSM, _nIndex, _pNero);
}

HRESULT Evade_R::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Evade_R", false, Nero::ANI_EVADE_R);
	return S_OK;
}

HRESULT Evade_R::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Evade_R::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	UINT CurWeaponIndex = m_pNero.lock()->Get_CurWeaponIndex();

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == CurWeaponIndex)
		{
			m_pFSM->ChangeState(NeroFSM::IDLE);
			return S_OK;
		}
		else
		{
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
			return S_OK;
		}
	}

	return S_OK;
}


#pragma endregion


#pragma region HIT // 맞음




HitFront::HitFront(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

HitFront::~HitFront()
{
}

HitFront* HitFront::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new HitFront(_pFSM, _nIndex, _pNero);
}

HRESULT HitFront::StateEnter()
{
	NeroState::StateEnter();

	m_pNero.lock()->ChangeAnimation("HitFront", false, Nero::ANI_HITFRONT);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
	return S_OK;
}

HRESULT HitFront::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT HitFront::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}
	return S_OK;
}

HitFront_Big::HitFront_Big(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

HitFront_Big::~HitFront_Big()
{
}

HitFront_Big* HitFront_Big::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new HitFront_Big(_pFSM, _nIndex, _pNero);
}

HRESULT HitFront_Big::StateEnter()
{
	return S_OK;
}

HRESULT HitFront_Big::StateExit()
{
	return S_OK;
}

HRESULT HitFront_Big::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	return S_OK;
}

Hit_Air::Hit_Air(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Hit_Air::~Hit_Air()
{
}


Hit_Air* Hit_Air::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Hit_Air(_pFSM, _nIndex, _pNero);
}

HRESULT Hit_Air::StateEnter()
{
	return S_OK;
}

HRESULT Hit_Air::StateExit()
{
	return S_OK;
}

HRESULT Hit_Air::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	return S_OK;
}

Wind_Pressure_Small::Wind_Pressure_Small(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Wind_Pressure_Small::~Wind_Pressure_Small()
{
}

Wind_Pressure_Small* Wind_Pressure_Small::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Wind_Pressure_Small(_pFSM, _nIndex, _pNero);
}

HRESULT Wind_Pressure_Small::StateEnter()
{
	return S_OK;
}

HRESULT Wind_Pressure_Small::StateExit()
{
	return S_OK;
}

HRESULT Wind_Pressure_Small::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	return S_OK;
}

Wind_Pressure_Big::Wind_Pressure_Big(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Wind_Pressure_Big::~Wind_Pressure_Big()
{
}

Wind_Pressure_Big* Wind_Pressure_Big::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Wind_Pressure_Big(_pFSM, _nIndex, _pNero);
}

HRESULT Wind_Pressure_Big::StateEnter()
{
	return S_OK;
}

HRESULT Wind_Pressure_Big::StateExit()
{
	return S_OK;
}

HRESULT Wind_Pressure_Big::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	return S_OK;
}

Wind_Pressure_Small_End::Wind_Pressure_Small_End(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Wind_Pressure_Small_End::~Wind_Pressure_Small_End()
{
}

Wind_Pressure_Small_End* Wind_Pressure_Small_End::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Wind_Pressure_Small_End(_pFSM, _nIndex, _pNero);
}

HRESULT Wind_Pressure_Small_End::StateEnter()
{
	return S_OK;
}

HRESULT Wind_Pressure_Small_End::StateExit()
{
	return S_OK;
}

HRESULT Wind_Pressure_Small_End::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	return S_OK;
}

Wind_Pressure_Big_End::Wind_Pressure_Big_End(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Wind_Pressure_Big_End::~Wind_Pressure_Big_End()
{
}

Wind_Pressure_Big_End* Wind_Pressure_Big_End::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Wind_Pressure_Big_End(_pFSM, _nIndex, _pNero);
}

HRESULT Wind_Pressure_Big_End::StateEnter()
{
	return S_OK;
}

HRESULT Wind_Pressure_Big_End::StateExit()
{
	return S_OK;
}

HRESULT Wind_Pressure_Big_End::StateUpdate(const float _fDeltaTime)
{
	return S_OK;
}

KnockBack_Front::KnockBack_Front(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

KnockBack_Front::~KnockBack_Front()
{
}

KnockBack_Front* KnockBack_Front::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new KnockBack_Front(_pFSM, _nIndex, _pNero);
}

HRESULT KnockBack_Front::StateEnter()
{
	return S_OK;
}

HRESULT KnockBack_Front::StateExit()
{
	return S_OK;
}

HRESULT KnockBack_Front::StateUpdate(const float _fDeltaTime)
{
	return S_OK;
}

KnockBack_Get_Up::KnockBack_Get_Up(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

KnockBack_Get_Up::~KnockBack_Get_Up()
{
}

KnockBack_Get_Up* KnockBack_Get_Up::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new KnockBack_Get_Up(_pFSM, _nIndex, _pNero);
}

HRESULT KnockBack_Get_Up::StateEnter()
{
	return S_OK;
}

HRESULT KnockBack_Get_Up::StateExit()
{
	return S_OK;
}

HRESULT KnockBack_Get_Up::StateUpdate(const float _fDeltaTime)
{
	return S_OK;
}

#pragma endregion

#pragma region DIE // 사망




Die::Die(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Die::~Die()
{
}

Die* Die::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Die(_pFSM, _nIndex, _pNero);
}

HRESULT Die::StateEnter()
{
	return S_OK;
}

HRESULT Die::StateExit()
{
	return S_OK;
}

HRESULT Die::StateUpdate(const float _fDeltaTime)
{
	return S_OK;
}

Resurrection::Resurrection(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Resurrection::~Resurrection()
{
}

Resurrection* Resurrection::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Resurrection(_pFSM, _nIndex, _pNero);
}

HRESULT Resurrection::StateEnter()
{
	return S_OK;
}

HRESULT Resurrection::StateExit()
{
	return S_OK;
}

HRESULT Resurrection::StateUpdate(const float _fDeltaTime)
{
	return S_OK;
}

Resurrection_GetUp::Resurrection_GetUp(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Resurrection_GetUp::~Resurrection_GetUp()
{
}

Resurrection_GetUp* Resurrection_GetUp::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Resurrection_GetUp(_pFSM, _nIndex, _pNero);
}

HRESULT Resurrection_GetUp::StateEnter()
{
	return S_OK;
}

HRESULT Resurrection_GetUp::StateExit()
{
	return S_OK;
}

HRESULT Resurrection_GetUp::StateUpdate(const float _fDeltaTime)
{
	return S_OK;
}

#pragma endregion

#pragma region STUN // 스턴




StunStart::StunStart(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

StunStart::~StunStart()
{
}

StunStart* StunStart::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new StunStart(_pFSM, _nIndex, _pNero);
}

HRESULT StunStart::StateEnter()
{
	NeroState::StateEnter();

	m_pNero.lock()->ChangeAnimation("Stun_Start", false, Nero::ANI_STUN_START);

	if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
	else
		NeroState::SetCbsIdle();

	return S_OK;
}

HRESULT StunStart::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT StunStart::StateUpdate(const float _fDeltaTime)
{
	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::STUN_LOOP);
		return S_OK;
	}
	return S_OK;
}

StunLoop::StunLoop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

StunLoop::~StunLoop()
{
}

StunLoop* StunLoop::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new StunLoop(_pFSM, _nIndex, _pNero);
}

HRESULT StunLoop::StateEnter()
{
	NeroState::StateEnter();

	m_pNero.lock()->ChangeAnimation("Stun_Loop", false, Nero::ANI_STUN_LOOP);

	//m_fStunTime = 4.f;
	return S_OK;
}

HRESULT StunLoop::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT StunLoop::StateUpdate(const float _fDeltaTime)
{
	//m_fStunTime -= _fDeltaTime;

	//if (Input::GetMouseDown(DIM_L))
	//	m_fStunTime -= 0.1f;

	//if (m_fStunTime <= 0.f)
	if(m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::STUN_END);
		return S_OK;
	}
	return S_OK;
}

StunEnd::StunEnd(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

StunEnd::~StunEnd()
{
}

StunEnd* StunEnd::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new StunEnd(_pFSM, _nIndex, _pNero);
}

HRESULT StunEnd::StateEnter()
{
	NeroState::StateEnter();

	m_pNero.lock()->ChangeAnimation("Stun_End", false, Nero::ANI_STUN_END);

	return S_OK;
}

HRESULT StunEnd::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT StunEnd::StateUpdate(const float _fDeltaTime)
{

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);

		return S_OK;
	}
	return S_OK;
}

EarthQuakeStart::EarthQuakeStart(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

EarthQuakeStart::~EarthQuakeStart()
{
}

EarthQuakeStart* EarthQuakeStart::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new EarthQuakeStart(_pFSM, _nIndex, _pNero);
}

HRESULT EarthQuakeStart::StateEnter()
{
	return S_OK;
}

HRESULT EarthQuakeStart::StateExit()
{
	return S_OK;
}

HRESULT EarthQuakeStart::StateUpdate(const float _fDeltaTime)
{
	return S_OK;
}

EarthQuakeLoop::EarthQuakeLoop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

EarthQuakeLoop::~EarthQuakeLoop()
{
}

EarthQuakeLoop* EarthQuakeLoop::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new EarthQuakeLoop(_pFSM, _nIndex, _pNero);
}

HRESULT EarthQuakeLoop::StateEnter()
{
	return S_OK;
}

HRESULT EarthQuakeLoop::StateExit()
{
	return S_OK;
}

HRESULT EarthQuakeLoop::StateUpdate(const float _fDeltaTime)
{
	return S_OK;
}

EarthQuakeEnd::EarthQuakeEnd(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

EarthQuakeEnd::~EarthQuakeEnd()
{
}

EarthQuakeEnd* EarthQuakeEnd::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new EarthQuakeEnd(_pFSM, _nIndex, _pNero);
}

HRESULT EarthQuakeEnd::StateEnter()
{
	return S_OK;
}

HRESULT EarthQuakeEnd::StateExit()
{
	return S_OK;
}

HRESULT EarthQuakeEnd::StateUpdate(const float _fDeltaTime)
{
	return S_OK;
}

#pragma endregion

#pragma region WIRE // 와이어 




Wire_Pull::Wire_Pull(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Wire_Pull::~Wire_Pull()
{
}

Wire_Pull* Wire_Pull::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Wire_Pull(_pFSM, _nIndex, _pNero);
}

HRESULT Wire_Pull::StateEnter()
{
	NeroState::StateEnter();

	//몬스터 위치에 따라서 분기
	m_pNero.lock()->Locking();
	Nero::NeroDirection eDir = m_pNero.lock()->RotateToTargetMonster();
	switch (eDir)
	{
	case Nero::Dir_Front:
		m_pNero.lock()->ChangeAnimation("Wire_Snatch_Pull", false, Nero::ANI_WIRE_SNATCH_PULL);
		break;
	case Nero::Dir_Up:
		m_pNero.lock()->ChangeAnimation("Wire_Snatch_Pull_U", false, Nero::ANI_WIRE_SNATCH_PULL);
		break;
	case Nero::Dir_Down:
		m_pNero.lock()->ChangeAnimation("Wire_Snatch_Pull_D", false, Nero::ANI_WIRE_SNATCH_PULL);
		break;
	case Nero::Dir_End:
		m_pNero.lock()->ChangeAnimation("Wire_Snatch_Pull", false, Nero::ANI_WIRE_SNATCH_PULL);
		break;
	default:
		m_pNero.lock()->ChangeAnimation("Wire_Snatch_Pull", false, Nero::ANI_WIRE_SNATCH_PULL);
		break;
	}
	m_pNero.lock()->Set_Weapon_Coll(Nero::NeroCom_WireArm, true);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WireArm,"Wire_Arm_Start31", false);
	if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
		NeroState::SetCbsIdle();
	return S_OK;
}

HRESULT Wire_Pull::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Wire_Pull::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	//Down 0.35에 키입력

	//Up 0.33에 키입력

	if (0.38f <= fCurAnimationTime)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Idle(NeroFSM::WIRE_SNATCH_PULL);
		else
			NeroState::KeyInput_Cbs_Idle();
	}

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}

	return S_OK;
}

Wire_Fly_Start::Wire_Fly_Start(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Wire_Fly_Start::~Wire_Fly_Start()
{
}

Wire_Fly_Start* Wire_Fly_Start::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Wire_Fly_Start(_pFSM, _nIndex, _pNero);
}

HRESULT Wire_Fly_Start::StateEnter()
{
	NeroState::StateEnter();

	m_pNero.lock()->ChangeAnimation("Wire_HellHound_F", false, Nero::ANI_WIRE_HELLHOUND_F);
	NeroState::ActiveGravity(false);
	m_pNero.lock()->SetFly(true);
	return S_OK;
}

HRESULT Wire_Fly_Start::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Wire_Fly_Start::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);

	m_pNero.lock()->WireFly();

	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::WIRE_HELLHOUND_LOOP);
		return S_OK;
	}
	return S_OK;
}

Wire_Fly_Loop::Wire_Fly_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Wire_Fly_Loop::~Wire_Fly_Loop()
{
}

Wire_Fly_Loop* Wire_Fly_Loop::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Wire_Fly_Loop(_pFSM, _nIndex, _pNero);
}

HRESULT Wire_Fly_Loop::StateEnter()
{
	NeroState::StateEnter();

	m_pNero.lock()->ChangeAnimation("Wire_HellHound_Loop", true, Nero::ANI_WIRE_HELLHOUND_LOOP);

	return S_OK;
}

HRESULT Wire_Fly_Loop::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Wire_Fly_Loop::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	m_pNero.lock()->WireFly();
	return S_OK;
}

Wire_Fly_End::Wire_Fly_End(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Wire_Fly_End::~Wire_Fly_End()
{
}

Wire_Fly_End* Wire_Fly_End::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Wire_Fly_End(_pFSM, _nIndex, _pNero);
}

HRESULT Wire_Fly_End::StateEnter()
{
	NeroState::StateEnter();

	m_pNero.lock()->ChangeAnimation("Wire_HellHound_End", false, Nero::ANI_WIRE_HELLHOUND_END);
	m_pNero.lock()->SetActive_NeroComponent(Nero::NeroCom_WireArm, false);
	m_pNero.lock()->SetAddForce({ 0.f,40.f,0.f });
	return S_OK;
}

HRESULT Wire_Fly_End::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Wire_Fly_End::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);

	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.5f <= fCurAnimationTime)
	{
		NeroState::ActiveGravity(true);
	}

	//특정 시간 지나면 중력 ON; 키입력 ON
	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
		return S_OK;
	}
	return S_OK;
}

Wire_Pull_Air::Wire_Pull_Air(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Wire_Pull_Air::~Wire_Pull_Air()
{
}

Wire_Pull_Air* Wire_Pull_Air::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Wire_Pull_Air(_pFSM, _nIndex, _pNero);
}

HRESULT Wire_Pull_Air::StateEnter()
{
	NeroState::StateEnter();

	//상태에 따라서 분기
	m_pNero.lock()->Locking();
	Nero::NeroDirection eDir = m_pNero.lock()->RotateToTargetMonster();
	switch (eDir)
	{
	case Nero::Dir_Front:
		m_pNero.lock()->ChangeAnimation("Wire_Snatch_Pull_Air", false, Nero::ANI_WIRE_SNATCH_PULL_AIR);
		break;
	case Nero::Dir_Up:
		m_pNero.lock()->ChangeAnimation("Wire_Snatch_Pull_Air_U", false, Nero::ANI_WIRE_SNATCH_PULL_AIR_U);
		break;
	case Nero::Dir_Down:
		m_pNero.lock()->ChangeAnimation("Wire_Snatch_Pull_Air_D", false, Nero::ANI_WIRE_SNATCH_PULL_AIR_D);
		break;
	case Nero::Dir_End:
		m_pNero.lock()->ChangeAnimation("Wire_Snatch_Pull_Air", false, Nero::ANI_WIRE_SNATCH_PULL_AIR);
		break;
	default:
		break;
	}

	//m_pNero.lock()->ChangeAnimation("Wire_Snatch_Pull_Air", false, Nero::ANI_WIRE_SNATCH_PULL_AIR);

	//NeroState::ResetAnimation(0.96f, Nero::ANI_WIRE_SNATCH_PULL_AIR);

	m_pNero.lock()->Set_Weapon_Coll(Nero::NeroCom_WireArm, true);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WireArm,"Wire_Arm_Start31", false);
	if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
		NeroState::SetCbsIdle();
	NeroState::ActiveGravity(false);
	return S_OK;
}

HRESULT Wire_Pull_Air::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Wire_Pull_Air::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();


	if (0.53 <= fCurAnimationTime)
	{
		NeroState::ActiveGravity(true);
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Jump();
		else
			NeroState::KeyInput_Cbs_Jump();
	}

	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
		return S_OK;
	}

	NeroState::IsGround();
	return S_OK;
}

Wire_Pull_Air_Up::Wire_Pull_Air_Up(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Wire_Pull_Air_Up::~Wire_Pull_Air_Up()
{
}

Wire_Pull_Air_Up* Wire_Pull_Air_Up::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Wire_Pull_Air_Up(_pFSM, _nIndex, _pNero);
}

HRESULT Wire_Pull_Air_Up::StateEnter()
{
	return S_OK;
}

HRESULT Wire_Pull_Air_Up::StateExit()
{
	return S_OK;
}

HRESULT Wire_Pull_Air_Up::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	return S_OK;
}

Wire_Pull_Air_Down::Wire_Pull_Air_Down(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Wire_Pull_Air_Down::~Wire_Pull_Air_Down()
{
}

Wire_Pull_Air_Down* Wire_Pull_Air_Down::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Wire_Pull_Air_Down(_pFSM, _nIndex, _pNero);
}

HRESULT Wire_Pull_Air_Down::StateEnter()
{
	return S_OK;
}

HRESULT Wire_Pull_Air_Down::StateExit()
{
	return S_OK;
}

HRESULT Wire_Pull_Air_Down::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	return S_OK;
}


#pragma endregion

#pragma region ATT //검 기본 공격



BT_Att1::BT_Att1(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

BT_Att1::~BT_Att1()
{
}

BT_Att1* BT_Att1::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new BT_Att1(_pFSM, _nIndex, _pNero);
}

HRESULT BT_Att1::StateEnter()
{
	m_iNeroCurWeaponIndex = Nero::NeroCom_RedQueen;
	NeroState::StateEnter();
	//달리고있었으면 대쉬 ComboA로 가야됨
	m_pNero.lock()->ChangeAnimation("ComboA1", false,Nero::ANI_COMBOA1);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_Front);
	
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"ComboA1", false);
	}
	m_pNero.lock()->CheckAutoRotate();

	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT BT_Att1::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT BT_Att1::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	//현재 애니메이션 프레임 위치
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.21f <= fCurrAnimationTime)
	{
		ActiveColl_RedQueen(false);
		NeroState::ActiveTrail(false);
	}

	else if (0.16f <= fCurrAnimationTime)
		ActiveColl_RedQueen(true);


	if (0.26f <= fCurrAnimationTime && fCurrAnimationTime <= 0.38f)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			KeyInput_Idle(NeroFSM::ATT2);
		else
			KeyInput_Cbs_Idle();
	}
	if (0.37f <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::IDLE_START);
		return S_OK;
	}
	return S_OK;
}

BT_Att2::BT_Att2(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

BT_Att2::~BT_Att2()
{
}

BT_Att2* BT_Att2::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new BT_Att2(_pFSM, _nIndex, _pNero);
}

HRESULT BT_Att2::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("ComboA2", false,Nero::ANI_COMBOA2);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen,ATTACKTYPE::Attack_R);
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WingArm_Right,"ComboA2", false);
	}
	m_pNero.lock()->CheckAutoRotate();
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT BT_Att2::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT BT_Att2::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.135f <= fCurrAnimationTime)
	{
		NeroState::ActiveTrail(false);
		m_pNero.lock()->StopEffect(Eff_Trail);
		ActiveColl_RedQueen(false);
	}

	else if (0.1f <= fCurrAnimationTime)
		ActiveColl_RedQueen(true);


	if (0.26f <= fCurrAnimationTime && fCurrAnimationTime <= 0.38f)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			KeyInput_Idle(NeroFSM::ATT3);
		else
			KeyInput_Cbs_Idle();
	}
	if (0.56f <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::IDLE_START);
		return S_OK;
	}

	return S_OK;
}

BT_Att3::BT_Att3(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

BT_Att3::~BT_Att3()
{
}

BT_Att3* BT_Att3::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new BT_Att3(_pFSM, _nIndex, _pNero);
}

HRESULT BT_Att3::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("ComboA3", false, Nero::ANI_COMBOA3);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen,ATTACKTYPE::Attack_L);
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"ComboA3", false);
	}
	m_pNero.lock()->CheckAutoRotate();
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT BT_Att3::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT BT_Att3::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();


	if (0.23f <= fCurrAnimationTime)
	{
		NeroState::ActiveTrail(false);
		ActiveColl_RedQueen(false);
		m_pNero.lock()->StopEffect(Eff_Trail);
	}

	else if (0.16f <= fCurrAnimationTime)
		ActiveColl_RedQueen(true);

	if (0.68f <= fCurrAnimationTime)
	{
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
	}

	if (0.26f <= fCurrAnimationTime && fCurrAnimationTime <= 0.5f)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			KeyInput_Idle(NeroFSM::ATT4);
		else
			KeyInput_Cbs_Idle();
	}

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}
	return S_OK;
}

BT_Att4::BT_Att4(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

BT_Att4::~BT_Att4()
{
}

BT_Att4* BT_Att4::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new BT_Att4(_pFSM, _nIndex, _pNero);
}

HRESULT BT_Att4::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("ComboA4", false, Nero::ANI_COMBOA4);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen,ATTACKTYPE::Attack_KnocBack);
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"ComboA4", false);
	}
	m_pNero.lock()->CheckAutoRotate();
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT BT_Att4::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT BT_Att4::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.23f <= fCurrAnimationTime)
	{
		NeroState::ActiveTrail(false);
		ActiveColl_RedQueen(false);
		m_pNero.lock()->StopEffect(Eff_Trail);
	}

	else if (0.16f <= fCurrAnimationTime)
		ActiveColl_RedQueen(true);

		
	if (0.44f <= fCurrAnimationTime)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Idle(NeroFSM::IDLE);
		else
			NeroState::KeyInput_Cbs_Idle();
	}

	if (0.8f <= fCurrAnimationTime)
	{
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
	}
	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}

	return S_OK;
}

BT_Att_ComboB::BT_Att_ComboB(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

BT_Att_ComboB::~BT_Att_ComboB()
{
}

BT_Att_ComboB* BT_Att_ComboB::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new BT_Att_ComboB(_pFSM, _nIndex, _pNero);
}

HRESULT BT_Att_ComboB::StateEnter()
{
	return S_OK;
}

HRESULT BT_Att_ComboB::StateExit()
{
	return S_OK;
}

HRESULT BT_Att_ComboB::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	return S_OK;
}

BT_Att_ComboC_R_to_L::BT_Att_ComboC_R_to_L(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

BT_Att_ComboC_R_to_L::~BT_Att_ComboC_R_to_L()
{
}

BT_Att_ComboC_R_to_L* BT_Att_ComboC_R_to_L::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new BT_Att_ComboC_R_to_L(_pFSM, _nIndex, _pNero);
}

HRESULT BT_Att_ComboC_R_to_L::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("ComboC_R", false, Nero::ANI_COMBOC_R);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_Front);
	m_pNero.lock()->CheckAutoRotate();
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT BT_Att_ComboC_R_to_L::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT BT_Att_ComboC_R_to_L::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.09f <= fCurrAnimationTime)
	{
		ActiveColl_RedQueen(false);
		NeroState::ActiveTrail(false);
	}
	else if (0.02f <= fCurrAnimationTime)
		ActiveColl_RedQueen(true);

	if (0.22f <= fCurrAnimationTime && fCurrAnimationTime <= 0.35f)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Idle(NeroFSM::ATT_COMBO_C4);
		else
			NeroState::KeyInput_Cbs_Idle();
	}
	else if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}

	return S_OK;
}

BT_Att_ComboC_L_to_R::BT_Att_ComboC_L_to_R(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

BT_Att_ComboC_L_to_R::~BT_Att_ComboC_L_to_R()
{
}

BT_Att_ComboC_L_to_R* BT_Att_ComboC_L_to_R::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new BT_Att_ComboC_L_to_R(_pFSM, _nIndex, _pNero);
}

HRESULT BT_Att_ComboC_L_to_R::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("ComboC_L", false, Nero::ANI_COMBOC_L);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_Front);
	m_pNero.lock()->CheckAutoRotate();
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT BT_Att_ComboC_L_to_R::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT BT_Att_ComboC_L_to_R::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.1f <= fCurrAnimationTime)
	{
		ActiveColl_RedQueen(false);
		NeroState::ActiveTrail(false);
	}
	else if (0.034f <= fCurrAnimationTime)
		ActiveColl_RedQueen(true);

	if (0.22f <= fCurrAnimationTime && fCurrAnimationTime <= 0.35f)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Idle(NeroFSM::ATT_COMBO_C3);
		else
			NeroState::KeyInput_Cbs_Idle();
	}
	else if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}

	return S_OK;
}

BT_Att_ComboC_1::BT_Att_ComboC_1(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

BT_Att_ComboC_1::~BT_Att_ComboC_1()
{
}

BT_Att_ComboC_1* BT_Att_ComboC_1::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new BT_Att_ComboC_1(_pFSM, _nIndex, _pNero);
}

HRESULT BT_Att_ComboC_1::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("ComboC1", false, Nero::ANI_COMBOC1);

	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"ComboC2", false);
	}
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_Front);
	m_pNero.lock()->CheckAutoRotate();
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT BT_Att_ComboC_1::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT BT_Att_ComboC_1::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.22f <= fCurrAnimationTime)
	{
		ActiveColl_RedQueen(false);
		NeroState::ActiveTrail(false);
	}
	else if (0.18f <= fCurrAnimationTime)
		ActiveColl_RedQueen(true);


	if (0.4f <= fCurrAnimationTime && fCurrAnimationTime <= 0.5f)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Idle(NeroFSM::ATT_COMBO_C2);
		else
			NeroState::KeyInput_Cbs_Idle();
	}
	else if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}

	return S_OK;
}

BT_Att_ComboC_2::BT_Att_ComboC_2(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

BT_Att_ComboC_2::~BT_Att_ComboC_2()
{
}

BT_Att_ComboC_2* BT_Att_ComboC_2::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new BT_Att_ComboC_2(_pFSM, _nIndex, _pNero);
}

HRESULT BT_Att_ComboC_2::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("ComboC2", false, Nero::ANI_COMBOC2);
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"ComboC2", false);
	}
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_Front);
	m_pNero.lock()->CheckAutoRotate();
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT BT_Att_ComboC_2::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT BT_Att_ComboC_2::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.1f <= fCurrAnimationTime)
	{
		ActiveColl_RedQueen(false);
		NeroState::ActiveTrail(false);
	}
	else if (0.048f <= fCurrAnimationTime)
		ActiveColl_RedQueen(true);


	if (0.22f <= fCurrAnimationTime && fCurrAnimationTime <= 0.35f)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Idle(NeroFSM::ATT_COMBO_C_L);
		else
			NeroState::KeyInput_Cbs_Idle();
	}
	else if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}
	return S_OK;
}

BT_Att_ComboC_3::BT_Att_ComboC_3(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

BT_Att_ComboC_3::~BT_Att_ComboC_3()
{
}

BT_Att_ComboC_3* BT_Att_ComboC_3::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new BT_Att_ComboC_3(_pFSM, _nIndex, _pNero);
}

HRESULT BT_Att_ComboC_3::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("ComboC3", false, Nero::ANI_COMBOC3);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_Front);

	if (m_pNero.lock()->Get_IsMajinMode())
	{

		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WingArm_Right,"ComboC3", false);
	}
	m_pNero.lock()->CheckAutoRotate();
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT BT_Att_ComboC_3::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT BT_Att_ComboC_3::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.1f <= fCurrAnimationTime)
	{
		ActiveColl_RedQueen(false);
		NeroState::ActiveTrail(false);
	}
	else if (0.048f <= fCurrAnimationTime)
		ActiveColl_RedQueen(true);

	if (0.22f <= fCurrAnimationTime && fCurrAnimationTime <= 0.35f)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Idle(NeroFSM::ATT_COMBO_C_R);
		else
			NeroState::KeyInput_Cbs_Idle();
	}
	else if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}
	return S_OK;
}

BT_Att_ComboC_4::BT_Att_ComboC_4(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

BT_Att_ComboC_4::~BT_Att_ComboC_4()
{
}

BT_Att_ComboC_4* BT_Att_ComboC_4::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new BT_Att_ComboC_4(_pFSM, _nIndex, _pNero);
}

HRESULT BT_Att_ComboC_4::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("ComboC4", false, Nero::ANI_COMBOC4);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_Front);
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"ComboC2", false);
	}
	m_pNero.lock()->CheckAutoRotate();
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT BT_Att_ComboC_4::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT BT_Att_ComboC_4::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();


	if (0.35f <= fCurrAnimationTime)
	{
		ActiveColl_RedQueen(false);
		NeroState::ActiveTrail(false);
	}
	else if (0.3f <= fCurrAnimationTime)
	{
		m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_KnocBack);
		ActiveColl_RedQueen(true);	
		if (m_pNero.lock()->Get_IsMajinMode())
			m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WingArm_Right,"ComboC4", false);
	}
	else if (0.135f <= fCurrAnimationTime)
		ActiveColl_RedQueen(false);
	else if (0.1f <= fCurrAnimationTime)
		ActiveColl_RedQueen(true);


	if (0.65f <= fCurrAnimationTime)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Idle(NeroFSM::IDLE);
		else
			NeroState::KeyInput_Cbs_Idle();
	}

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}
	return S_OK;
}

BT_Att_ComboD_1::BT_Att_ComboD_1(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

BT_Att_ComboD_1::~BT_Att_ComboD_1()
{
}

BT_Att_ComboD_1* BT_Att_ComboD_1::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new BT_Att_ComboD_1(_pFSM, _nIndex, _pNero);
}

HRESULT BT_Att_ComboD_1::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("ComboD1", false, Nero::ANI_COMBOD1);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_Front);
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"ComboD1", false);
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WingArm_Right,"ComboD1", false);
	}
	m_pNero.lock()->CheckAutoRotate();
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT BT_Att_ComboD_1::StateExit()
{
	NeroState::StateExit();
	NeroState::ActiveTrail(false);
	return S_OK;
}

HRESULT BT_Att_ComboD_1::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.1f <= fCurrAnimationTime)
	{
		ActiveColl_RedQueen(false);
	}
	else if (0.07f <= fCurrAnimationTime)
		ActiveColl_RedQueen(true);
	if(0.25f <= fCurrAnimationTime)
		NeroState::ActiveTrail(false);

	if (0.15f <= fCurrAnimationTime && fCurrAnimationTime <= 0.3f)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Idle(NeroFSM::ATT_COMBO_D2);
		else
			NeroState::KeyInput_Cbs_Idle();
	}
	else if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}

	return S_OK;
}

BT_Att_ComboD_2::BT_Att_ComboD_2(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

BT_Att_ComboD_2::~BT_Att_ComboD_2()
{
}

BT_Att_ComboD_2* BT_Att_ComboD_2::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new BT_Att_ComboD_2(_pFSM, _nIndex, _pNero);
}

HRESULT BT_Att_ComboD_2::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("ComboD2", false, Nero::ANI_COMBOD2);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_Front);
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WingArm_Right,"ComboD2", false);
	}
	m_pNero.lock()->CheckAutoRotate();
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT BT_Att_ComboD_2::StateExit()
{
	NeroState::StateExit();
	NeroState::ActiveTrail(false);
	return S_OK;
}

HRESULT BT_Att_ComboD_2::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.1f <= fCurrAnimationTime)
	{
		ActiveColl_RedQueen(false);
	}
	else if (0.06f <= fCurrAnimationTime)
		ActiveColl_RedQueen(true);

	if (0.25f <= fCurrAnimationTime)
		NeroState::ActiveTrail(false);

	if (0.14f <= fCurrAnimationTime && fCurrAnimationTime <= 0.22f)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Idle(NeroFSM::ATT_COMBO_D3);
		else
			NeroState::KeyInput_Cbs_Idle();
	}

	else if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}

	return S_OK;
}

BT_Att_ComboD_3::BT_Att_ComboD_3(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

BT_Att_ComboD_3::~BT_Att_ComboD_3()
{
}

BT_Att_ComboD_3* BT_Att_ComboD_3::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new BT_Att_ComboD_3(_pFSM, _nIndex, _pNero);
}

HRESULT BT_Att_ComboD_3::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("ComboD3", false, Nero::ANI_COMBOD3);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_Front);
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WingArm_Right,"ComboD3", false);
	}
	m_pNero.lock()->CheckAutoRotate();
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT BT_Att_ComboD_3::StateExit()
{
	NeroState::StateExit();
	NeroState::ActiveTrail(false);
	return S_OK;
}

HRESULT BT_Att_ComboD_3::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.1f <= fCurrAnimationTime)
	{
		ActiveColl_RedQueen(false);
	}
	else if (0.07f <= fCurrAnimationTime)
		ActiveColl_RedQueen(true);
	if(0.22f <= fCurrAnimationTime)
		NeroState::ActiveTrail(false);

	if (0.19f <= fCurrAnimationTime && fCurrAnimationTime <= 0.27f)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Idle(NeroFSM::ATT_COMBO_D4);
		else
			NeroState::KeyInput_Cbs_Idle();
	}
	
	else if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}

	return S_OK;
}

BT_Att_ComboD_4::BT_Att_ComboD_4(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

BT_Att_ComboD_4::~BT_Att_ComboD_4()
{
}

BT_Att_ComboD_4* BT_Att_ComboD_4::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new BT_Att_ComboD_4(_pFSM, _nIndex, _pNero);
}

HRESULT BT_Att_ComboD_4::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("ComboD4", false, Nero::ANI_COMBOD4);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_KnocBack);
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WingArm_Right,"ComboD4", false);
	}
	m_pNero.lock()->CheckAutoRotate();
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT BT_Att_ComboD_4::StateExit()
{
	NeroState::StateExit();
	NeroState::ActiveTrail(false);
	return S_OK;
}

HRESULT BT_Att_ComboD_4::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.1f <= fCurrAnimationTime)
	{
		ActiveColl_RedQueen(false);
	}
	else if (0.055f <= fCurrAnimationTime)
		ActiveColl_RedQueen(true);
	if(0.2f <= fCurrAnimationTime)
		NeroState::ActiveTrail(false);

	if (0.3f <= fCurrAnimationTime)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Idle(NeroFSM::IDLE);
		else
			NeroState::KeyInput_Cbs_Idle();
	}	
	else if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}
	return S_OK;
}

BT_Air_Att1::BT_Air_Att1(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

BT_Air_Att1::~BT_Air_Att1()
{
}

BT_Air_Att1* BT_Air_Att1::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new BT_Air_Att1(_pFSM, _nIndex, _pNero);
}

HRESULT BT_Air_Att1::StateEnter()
{
	m_iNeroCurWeaponIndex = Nero::NeroCom_RedQueen;
	NeroState::StateEnter();
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	UINT  CurAnimationIndex = m_pNero.lock()->Get_CurAnimationIndex();
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_R);
	//m_pNero.lock()->Set_JumpDir(Nero::Basic);
	m_pNero.lock()->ChangeAnimation("ComboA_Air1", false, Nero::ANI_COMBOA_AIR1);
	NeroState::ActiveGravity(false);

	if (m_pNero.lock()->IsAnimationEnd() && Nero::ANI_COMBOA_AIR1 == CurAnimationIndex)
	{
		m_pNero.lock()->Set_PlayingTime(0.f);
	}


	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WingArm_Right,"ComboD1", false);
	}
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT BT_Air_Att1::StateExit()
{
	NeroState::StateExit();
	NeroState::ActiveGravity(true);
	return S_OK;
}

HRESULT BT_Air_Att1::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	UINT  CurAnimationIndex = m_pNero.lock()->Get_CurAnimationIndex();

	if (0.32f <= fCurrAnimationTime)
	{
		ActiveColl_RedQueen(false);
		NeroState::ActiveTrail(false);
	}
	else if (0.24f <= fCurrAnimationTime)
		ActiveColl_RedQueen(true);

	if(0.45 <= fCurrAnimationTime)
		NeroState::ActiveGravity(true);



	if(0.8f <= fCurrAnimationTime)
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);

	if (0.36f <= fCurrAnimationTime && fCurrAnimationTime <= 0.5f)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Jump(NeroFSM::AIR_COMBOA1);
		else
			NeroState::KeyInput_Cbs_Jump();
	}
	else if (0.6f <= fCurrAnimationTime && fCurrAnimationTime <= 0.9f)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Jump(NeroFSM::AIR_COMBOA3);
		else
			NeroState::KeyInput_Cbs_Jump();
		return S_OK;
	}

	else if (m_pNero.lock()->IsAnimationEnd()/* && Nero::ANI_COMBOA_AIR1 != CurAnimationIndex */)
	{
		//애니메이션을 멈추든지..
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
		return S_OK;
	}
	NeroState::IsGround();
	return S_OK;
}

BT_Air_Att2::BT_Air_Att2(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

BT_Air_Att2::~BT_Air_Att2()
{
}

BT_Air_Att2* BT_Air_Att2::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new BT_Air_Att2(_pFSM, _nIndex, _pNero);
}

HRESULT BT_Air_Att2::StateEnter()
{
	NeroState::StateEnter();

	m_pNero.lock()->ChangeAnimation("ComboA_Air2", false, Nero::ANI_COMBOA_AIR2);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_L);
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"ComboA3", false);
	}
	m_bActiveColl_RedQueen = false;
	NeroState::ActiveGravity(false);
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT BT_Air_Att2::StateExit()
{
	NeroState::StateExit();
	NeroState::ActiveGravity(true);
	ActiveColl_RedQueen(false);
	return S_OK;
}

HRESULT BT_Air_Att2::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.3f <= fCurrAnimationTime)
	{
		ActiveColl_RedQueen(false);
		NeroState::ActiveTrail(false);
	}
	else if (0.18f <= fCurrAnimationTime)
		ActiveColl_RedQueen(true);

	if(0.45f <= fCurrAnimationTime)
		NeroState::ActiveGravity(true);


	if (0.85f <= fCurrAnimationTime)
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);


	if (0.32f <= fCurrAnimationTime && fCurrAnimationTime <= 0.42f)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Jump(NeroFSM::AIR_COMBOA2);
		else
			NeroState::KeyInput_Cbs_Jump();
		return S_OK;
	}
	else if (0.47f <= fCurrAnimationTime && fCurrAnimationTime <= 0.62f)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Jump(NeroFSM::AIR_COMBOB);
		else
			NeroState::KeyInput_Cbs_Jump();
		return S_OK;
	}

	else if (0.7f <= fCurrAnimationTime && fCurrAnimationTime <= 0.9f)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Jump(NeroFSM::AIR_COMBOA3);
		else
			NeroState::KeyInput_Cbs_Jump();
		return S_OK;
	}


	else if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
		return S_OK;
	}
	NeroState::IsGround();
	return S_OK;
}

BT_Air_Att3::BT_Air_Att3(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

BT_Air_Att3::~BT_Air_Att3()
{
}

BT_Air_Att3* BT_Air_Att3::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new BT_Air_Att3(_pFSM, _nIndex, _pNero);
}

HRESULT BT_Air_Att3::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_Down);
	m_pNero.lock()->ChangeAnimation("ComboA_Air3", false, Nero::ANI_COMBOA_AIR3);

	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"ComboC2", false);
	}
	NeroState::ActiveGravity(false);
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT BT_Air_Att3::StateExit()
{
	NeroState::StateExit();
	NeroState::ActiveGravity(true);
	return S_OK;
}

HRESULT BT_Air_Att3::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.3f <= fCurAnimationTime)
	{
		ActiveColl_RedQueen(false);
		NeroState::ActiveTrail(false);
	}
	else if (0.19f <= fCurAnimationTime)
		ActiveColl_RedQueen(true);

	if(0.56 <= fCurAnimationTime)
		NeroState::ActiveGravity(true);


	if (0.78f <= fCurAnimationTime)
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);

	if (0.57f <= fCurAnimationTime && fCurAnimationTime <= 0.9f)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Jump(NeroFSM::AIR_COMBOA3);
		else
			NeroState::KeyInput_Cbs_Jump();
	}

	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
		return S_OK;
	}

	NeroState::IsGround();
	return S_OK;
}

BT_Air_ComboB::BT_Air_ComboB(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

BT_Air_ComboB::~BT_Air_ComboB()
{
}

BT_Air_ComboB* BT_Air_ComboB::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new BT_Air_ComboB(_pFSM, _nIndex, _pNero);
}

HRESULT BT_Air_ComboB::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_Air_Start);
	m_pNero.lock()->ChangeAnimation("ComboB_Air", false, Nero::ANI_COMBOB_AIR);

	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WingArm_Right,"ComboD1", false);
	}
	NeroState::ActiveGravity(false);
	m_pNero.lock()->SetAddForce({ 0.f,20.f,0.f });
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT BT_Air_ComboB::StateExit()
{
	NeroState::StateExit();
	NeroState::ActiveGravity(true);
	return S_OK;
}

HRESULT BT_Air_ComboB::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (0.47f <= fCurAnimationTime)
	{
		ActiveColl_RedQueen(false);
		NeroState::ActiveTrail(false);
	}
	else if (0.29f <= fCurAnimationTime)
	{
		m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_Air);
		ActiveColl_RedQueen(true);
	}
	else if (0.21f <= fCurAnimationTime)
		ActiveColl_RedQueen(false);
	else if (0.13f <= fCurAnimationTime)
		ActiveColl_RedQueen(true);

	if(0.45 <= fCurAnimationTime)
		NeroState::ActiveGravity(true);


	if (0.1f <= fCurAnimationTime && 0.2f <= fCurAnimationTime && m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"ComboD1", false);
	}

	if (0.82f <= fCurAnimationTime)
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);

	if (0.58f <= fCurAnimationTime && fCurAnimationTime <= 0.9f)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Jump(NeroFSM::AIR_COMBOA3);
		else
			NeroState::KeyInput_Cbs_Jump();
	}

	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
		return S_OK;
	}

	NeroState::IsGround();
	return S_OK;
}

BT_Parrying::BT_Parrying(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

BT_Parrying::~BT_Parrying()
{
}

BT_Parrying* BT_Parrying::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new BT_Parrying(_pFSM, _nIndex, _pNero);
}

HRESULT BT_Parrying::StateEnter()
{
	return S_OK;
}

HRESULT BT_Parrying::StateExit()
{
	return S_OK;
}

HRESULT BT_Parrying::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	return S_OK;
}

#pragma endregion


#pragma region SKILL

Skill_Split::Skill_Split(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Skill_Split::~Skill_Split()
{
}

Skill_Split* Skill_Split::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Skill_Split(_pFSM, _nIndex, _pNero);
}

HRESULT Skill_Split::StateEnter()
{
	m_iNeroCurWeaponIndex = Nero::NeroCom_RedQueen;
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Split_Start", false, Nero::ANI_SPLIT_START);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_Split);
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"Split_Start", false);
	}
	//m_pNero.lock()->SetAngleFromCamera();
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
	NeroState::ActiveGravity(false);
	return S_OK;
}

HRESULT Skill_Split::StateExit()
{
	m_pNero.lock()->Set_Weapon_Coll(Nero::NeroCom_RedQueen, true);

	m_pNero.lock()->SetAddForce({ 0.f,-250.f,0.f });
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT Skill_Split::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::SKILL_SPLIT_LOOP);
		return S_OK;
	}

	return S_OK;
}

Skill_Split_Loop::Skill_Split_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
	,m_fLoopTime(0.f)
{
}

Skill_Split_Loop::~Skill_Split_Loop()
{
}

Skill_Split_Loop* Skill_Split_Loop::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Skill_Split_Loop(_pFSM, _nIndex, _pNero);
}

HRESULT Skill_Split_Loop::StateEnter()
{
	m_pNero.lock()->StopAnimation();

	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"Split_Loop", true);
	}
	return S_OK;
}

HRESULT Skill_Split_Loop::StateExit()
{
	NeroState::StateExit();
	m_pNero.lock()->ContinueAnimiation();
	return S_OK;
}

HRESULT Skill_Split_Loop::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	//땅에 닿았을때
	if (m_pNero.lock()->CheckIsGround())
	{
		m_pFSM->ChangeState(NeroFSM::SKILL_SPLIT_END);
		return S_OK;
	}

	//테스트

	if (Input::GetKey(DIK_M))
	{
		m_pFSM->ChangeState(NeroFSM::SKILL_SPLIT_END);
		return S_OK;
	}

	return S_OK;
}

Skill_Split_Landing::Skill_Split_Landing(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Skill_Split_Landing::~Skill_Split_Landing()
{
}

Skill_Split_Landing* Skill_Split_Landing::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Skill_Split_Landing(_pFSM, _nIndex, _pNero);
}

HRESULT Skill_Split_Landing::StateEnter()
{
	m_bActiveColl_RedQueen = true;
	m_pNero.lock()->ChangeAnimation("Split_Landing", false, Nero::ANI_SPLIT_LANDING);

	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"Split_End", false);
	}
	m_pNero.lock()->SetLinearVelocity();
	NeroState::ActiveGravity(true);
	return S_OK;
}

HRESULT Skill_Split_Landing::StateExit()
{
	NeroState::StateEnter();
	return S_OK;
}

HRESULT Skill_Split_Landing::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.2f <= fCurAnimationTime)
	{
		ActiveColl_RedQueen(false);
		NeroState::ActiveTrail(false);
	}

	if (0.37f <= fCurAnimationTime)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Idle(NeroFSM::SKILL_SPLIT_END);
		else
			NeroState::KeyInput_Cbs_Idle();
	}

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}
	return S_OK;
}

Skill_Float_Ground::Skill_Float_Ground(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Skill_Float_Ground::~Skill_Float_Ground()
{
}

Skill_Float_Ground* Skill_Float_Ground::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Skill_Float_Ground(_pFSM, _nIndex, _pNero);
}

HRESULT Skill_Float_Ground::StateEnter()
{
	m_iNeroCurWeaponIndex = Nero::NeroCom_RedQueen;
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Hr_Ground", false, Nero::ANI_HR_GROUND);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_Air_Start);
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"Hr_Ground", false);
	}
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT Skill_Float_Ground::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Skill_Float_Ground::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.17f <= fCurrAnimationTime)
	{
		ActiveColl_RedQueen(false);
		NeroState::ActiveTrail(false);
	}
	else if (0.11f <= fCurrAnimationTime)
		ActiveColl_RedQueen(true);


	if (0.26f <= fCurrAnimationTime)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Idle(NeroFSM::SKILL_FLOAT_GROUND);
		else
			NeroState::KeyInput_Cbs_Idle();
	}

	if(m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}
	return S_OK;
}

Skill_Float_Ground_Ex3::Skill_Float_Ground_Ex3(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Skill_Float_Ground_Ex3::~Skill_Float_Ground_Ex3()
{
}

Skill_Float_Ground_Ex3* Skill_Float_Ground_Ex3::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Skill_Float_Ground_Ex3(_pFSM, _nIndex, _pNero);
}

HRESULT Skill_Float_Ground_Ex3::StateEnter()
{
	m_iNeroCurWeaponIndex = Nero::NeroCom_RedQueen;
	NeroState::StateEnter();
	m_bActiveColl_RedQueen = false;
	ActiveColl_RedQueen(true);
	m_bPlayOnce = true;
	m_pNero.lock()->ChangeAnimation("Hr_Ex_3", false, Nero::ANI_HR_EX_3);
	m_pNero.lock()->PlayEffect(Eff_FireCircle, Vector3{ 0.f,-0.45f,1.1f }, 0, 5, 0.5f, { 0.0033f,0.0033f,0.0033f });
	return S_OK;
}

HRESULT Skill_Float_Ground_Ex3::StateExit()
{
	return S_OK;
}

HRESULT Skill_Float_Ground_Ex3::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.6f <= fCurrAnimationTime)
	{
		ActiveColl_RedQueen(false);
	}
	else if (0.34f <= fCurrAnimationTime)
		ActiveColl_RedQueen(true);
	else if (0.2f <= fCurrAnimationTime)
		ActiveColl_RedQueen(false);

	if (0.45f <= fCurrAnimationTime && m_bPlayOnce)
	{
		m_bPlayOnce = false;
		m_pNero.lock()->PlayEffect(Eff_FireCircle, Vector3{ 0.f,0.3f,1.45f }, 0, 3, 0.5f, { 0.0033f,0.0033f,0.0033f });
	}

	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::SKILL_FLOAT_GROUND_FINISH);
		return S_OK;
	}

	return S_OK;
}

Skill_Float_Ground_Finish::Skill_Float_Ground_Finish(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Skill_Float_Ground_Finish::~Skill_Float_Ground_Finish()
{
}

Skill_Float_Ground_Finish* Skill_Float_Ground_Finish::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Skill_Float_Ground_Finish(_pFSM, _nIndex, _pNero);
}

HRESULT Skill_Float_Ground_Finish::StateEnter()
{
	NeroState::StateEnter();
	m_bActiveColl_RedQueen = false;
	ActiveColl_RedQueen(true);

	m_pNero.lock()->ChangeAnimation("Hr_Ex_Finish", false, Nero::ANI_HR_EX_FINISH);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_Air_Start);

	m_pNero.lock()->PlayEffect(Eff_FireCircle, Vector3{ 0.f,-0.45f,1.7f }, 0, 1, 0.5f, { 0.004f,0.004f,0.004f });
	return S_OK;
}

HRESULT Skill_Float_Ground_Finish::StateExit()
{
	return S_OK;
}

HRESULT Skill_Float_Ground_Finish::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.07f <= fCurrAnimationTime)
	{
		ActiveColl_RedQueen(false);
	}

	if (0.2f <= fCurrAnimationTime)
	{
		NeroState::ActiveTrail(false);
	}

	if (0.58f <= fCurrAnimationTime)
	{
		NeroState::KeyInput_Idle();
	}

	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::IDLE);
		return S_OK;
	}

	return S_OK;
}


Skill_Shuffle::Skill_Shuffle(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Skill_Shuffle::~Skill_Shuffle()
{
}

Skill_Shuffle* Skill_Shuffle::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Skill_Shuffle(_pFSM, _nIndex, _pNero);
}

HRESULT Skill_Shuffle::StateEnter()
{
	m_iNeroCurWeaponIndex = Nero::NeroCom_RedQueen;
	NeroState::StateEnter();
	//EX게이지에 따라서 분기
	//게이지없을때

	
	m_pNero.lock()->ChangeAnimation("Shuffle", false, Nero::ANI_SHUFFLE);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_Homerun);
	//게이지 있을때
	//m_pNero.lock()->ChangeAnimation("Shuffle_Ex", false, Nero::ANI_SHUFFLE_EX);

	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WingArm_Right,"Shuffle", false);
	}
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT Skill_Shuffle::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Skill_Shuffle::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.28f <= fCurrAnimationTime)
	{
		ActiveColl_RedQueen(false);
	}
	else if (0.23f <= fCurrAnimationTime)
		ActiveColl_RedQueen(true);
	if(0.43f <= fCurrAnimationTime)
		NeroState::ActiveTrail(false);

	if (0.55f <= fCurrAnimationTime)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Idle(NeroFSM::SKILL_SHUFFLE);
		else
			NeroState::KeyInput_Cbs_Idle();
	}

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}
	return S_OK;
}


Skill_Streak::Skill_Streak(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Skill_Streak::~Skill_Streak()
{
}

Skill_Streak* Skill_Streak::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Skill_Streak(_pFSM, _nIndex, _pNero);
}

HRESULT Skill_Streak::StateEnter()
{
	m_iNeroCurWeaponIndex = Nero::NeroCom_RedQueen;
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Streak_Start", false, Nero::ANI_STREAK_START);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_KnocBack);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"Streak_Start", false);

		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WingArm_Right,"Streak_Start", false);
	}
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT Skill_Streak::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Skill_Streak::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	m_pNero.lock()->NeroMoveLerf(Nero::Dir_Front, 0.007f, 0.045f);

	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::SKILL_STREAK_LOOP);
		return S_OK;
	}
	return S_OK;
}

Skill_Streak_Ex3::Skill_Streak_Ex3(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Skill_Streak_Ex3::~Skill_Streak_Ex3()
{
}

Skill_Streak_Ex3* Skill_Streak_Ex3::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Skill_Streak_Ex3(_pFSM, _nIndex, _pNero);
}

HRESULT Skill_Streak_Ex3::StateEnter()
{
	m_iNeroCurWeaponIndex = Nero::NeroCom_RedQueen;
	NeroState::StateEnter();

	m_pNero.lock()->ChangeAnimation("Streak_Ex_Start", false, Nero::ANI_STREAK_EX_START);
	

	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"Streak_Start", false);

		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WingArm_Right,"Streak_Start", false);
	}
	m_bActiveColl_RedQueen = false;
	ActiveColl_RedQueen(true);
	NeroState::ActiveTrail(true);
	m_pNero.lock()->Use_ExGauge(1);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
	return S_OK;
}

HRESULT Skill_Streak_Ex3::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Skill_Streak_Ex3::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::SKILL_STREAK_EX3_RUSH);
		return S_OK;
	}
	m_pNero.lock()->SetPosFireCircle();
	m_pNero.lock()->NeroMoveLerf(Nero::Dir_Front, 0.007f, 0.045f);
	return S_OK;
}

Skill_Streak_Loop::Skill_Streak_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero), m_fLoopTime(0.f)
{

}

Skill_Streak_Loop::~Skill_Streak_Loop()
{
}

Skill_Streak_Loop* Skill_Streak_Loop::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Skill_Streak_Loop(_pFSM, _nIndex, _pNero);
}

HRESULT Skill_Streak_Loop::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Streak_Loop", true, Nero::ANI_STREAK_LOOP);
	m_fLoopTime = 0.2f;

	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"Streak_Loop", true);

		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WingArm_Right,"Streak_Loop", true);
	}
	return S_OK;
}

HRESULT Skill_Streak_Loop::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Skill_Streak_Loop::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	m_fLoopTime -= _fDeltaTime;
	m_pNero.lock()->NeroMoveLerf(Nero::Dir_Front, 0.007f, 0.045f);
	if (m_fLoopTime < 0.f)
	{
		m_pFSM->ChangeState(NeroFSM::SKILL_STREAK_END);
		return S_OK;
	}
	return S_OK;
}

Skill_Streak_End::Skill_Streak_End(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Skill_Streak_End::~Skill_Streak_End()
{
}

Skill_Streak_End* Skill_Streak_End::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Skill_Streak_End(_pFSM, _nIndex, _pNero);
}

HRESULT Skill_Streak_End::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Streak_End", false, Nero::ANI_STREAK_END);

	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"Streak_End", false);

		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WingArm_Right,"Streak_End", false);
	}
	m_bActiveColl_RedQueen = false;
	ActiveColl_RedQueen(true);
	m_pNero.lock()->Reset_LerfAmount();

	return S_OK;
}

HRESULT Skill_Streak_End::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Skill_Streak_End::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.1f <= fCurrAnimationTime)
	{
		ActiveColl_RedQueen(false);
	}
	if(0.2f <= fCurrAnimationTime)
		NeroState::ActiveTrail(false);
	if (0.5f <= fCurrAnimationTime)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Idle(NeroFSM::SKILL_STREAK_END);
		else
			NeroState::KeyInput_Cbs_Idle();
	}
	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}
	return S_OK;
}

Skill_Streak_Ex3_Rush::Skill_Streak_Ex3_Rush(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Skill_Streak_Ex3_Rush::~Skill_Streak_Ex3_Rush()
{
}

Skill_Streak_Ex3_Rush* Skill_Streak_Ex3_Rush::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Skill_Streak_Ex3_Rush(_pFSM, _nIndex, _pNero);
}

HRESULT Skill_Streak_Ex3_Rush::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Streak_Ex_Rush", false, Nero::ANI_STREAK_EX_RUSH);

	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"Streak_Loop", true);

		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WingArm_Right,"Streak_Loop", true);
	}
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT Skill_Streak_Ex3_Rush::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Skill_Streak_Ex3_Rush::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.65f <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::SKILL_STREAK_EX3_ROLL_LOOP);
		return S_OK;
	}
	m_pNero.lock()->NeroMoveLerf(Nero::Dir_Front, 0.007f, 0.045f);
	m_pNero.lock()->SetPosFireCircle();
	return S_OK;
}

Skill_Streak_Ex3_Roll_Loop::Skill_Streak_Ex3_Roll_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Skill_Streak_Ex3_Roll_Loop::~Skill_Streak_Ex3_Roll_Loop()
{
}

Skill_Streak_Ex3_Roll_Loop* Skill_Streak_Ex3_Roll_Loop::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Skill_Streak_Ex3_Roll_Loop(_pFSM, _nIndex, _pNero);
}

HRESULT Skill_Streak_Ex3_Roll_Loop::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Streak_Ex_Roll_Start", true, Nero::ANI_STREAK_END);
	m_pNero.lock()->Reset_LerfAmount();
	m_bPlayOnce = true;
	m_pNero.lock()->PlayEffect(Eff_FireCircle, { 0.f,0.f,0.3f }, 0, 5, 0.8f, {0.0035f,0.0035f,0.0035f});
	return S_OK;
}

HRESULT Skill_Streak_Ex3_Roll_Loop::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Skill_Streak_Ex3_Roll_Loop::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fAccTime = m_pNero.lock()->Get_PlayingAccTime();

	if (1.f <= fAccTime && m_bPlayOnce)
	{
		m_pNero.lock()->PlayEffect(Eff_FireCircle, { 0.f, 0.f,0.25f }, 0, 3,0.8f, { 0.0035f,0.0035f,0.0035f });
		m_bPlayOnce = false;
	}

	if (1.96f <= fAccTime)
	{
		m_pFSM->ChangeState(NeroFSM::SKILL_STREAK_EX3_ROLL_END);
		return S_OK;
	}
	m_pNero.lock()->SetPosFireCircle();
	m_pNero.lock()->NeroMoveLerf(Nero::Dir_Front, 0.002f, 0.022f);
	return S_OK;
}

Skill_Streak_Ex3_Roll_End::Skill_Streak_Ex3_Roll_End(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Skill_Streak_Ex3_Roll_End::~Skill_Streak_Ex3_Roll_End()
{
}

Skill_Streak_Ex3_Roll_End* Skill_Streak_Ex3_Roll_End::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Skill_Streak_Ex3_Roll_End(_pFSM, _nIndex, _pNero);
}

HRESULT Skill_Streak_Ex3_Roll_End::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Streak_Ex_Roll_End", false, Nero::ANI_STREAK_END);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_KnocBack);
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"Streak_End", false);

		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WingArm_Right,"Streak_End", false);
	}
	m_bActiveColl_RedQueen = true;
	m_pNero.lock()->Reset_LerfAmount();
	m_pNero.lock()->PlayEffect(Eff_FireCircle, { 0.f,0.f,0.f }, 0, 1);
	return S_OK;
}

HRESULT Skill_Streak_Ex3_Roll_End::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Skill_Streak_Ex3_Roll_End::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.1f <= fCurrAnimationTime)
	{
		ActiveColl_RedQueen(false);
	}
	if(0.4f <= fCurrAnimationTime)
		NeroState::ActiveTrail(false);
	if (0.55f <= fCurrAnimationTime)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Idle(NeroFSM::SKILL_STREAK_EX3_ROLL_END);
		else
			NeroState::KeyInput_Cbs_Idle();
	}
	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}
	m_pNero.lock()->SetPosFireCircle();
	return S_OK;
}

#pragma endregion

#pragma region Overture // 오버추어



Overture_Shoot::Overture_Shoot(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Overture_Shoot::~Overture_Shoot()
{
}

Overture_Shoot* Overture_Shoot::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Overture_Shoot(_pFSM, _nIndex, _pNero);
}

HRESULT Overture_Shoot::StateEnter()
{
	NeroState::StateEnter();

	m_pNero.lock()->ChangeAnimation("Overture_Shoot", false, Nero::ANI_OVERTURE_SHOOT);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Overture,"Shoot_Front", false);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_Overture, ATTACKTYPE::Attack_KnocBack);
	m_pNero.lock()->Set_Weapon_Coll(Nero::NeroCom_Overture, true);
	m_pNero.lock()->CreateOvertureEff();
	return S_OK;
}

HRESULT Overture_Shoot::StateExit()
{
	NeroState::StateExit();
	m_pNero.lock()->Set_Weapon_Coll(Nero::NeroCom_Overture, false);
	return S_OK;
}

HRESULT Overture_Shoot::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if(0.2f <= fCurrAnimationTime)
		m_pNero.lock()->Set_Weapon_Coll(Nero::NeroCom_Overture, false);

	if (0.6f <= fCurrAnimationTime)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Idle(NeroFSM::OVERTURE_SHOOT);
		else
			NeroState::KeyInput_Cbs_Idle();
	}

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}

	return S_OK;
}

Overture_Shoot_Up::Overture_Shoot_Up(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Overture_Shoot_Up::~Overture_Shoot_Up()
{
}

Overture_Shoot_Up* Overture_Shoot_Up::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Overture_Shoot_Up(_pFSM, _nIndex, _pNero);
}

HRESULT Overture_Shoot_Up::StateEnter()
{
	NeroState::StateEnter();

	m_pNero.lock()->ChangeAnimation("Overture_Shoot_Up", false, Nero::ANI_OVERTURE_SHOOT_UP);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Overture,"Shoot_Front", false);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_Overture, ATTACKTYPE::Attack_Air_Start);
	m_pNero.lock()->Set_Weapon_Coll(Nero::NeroCom_Overture, true);
	m_pNero.lock()->CreateOvertureEff(Nero::EffDir_Up);
	return S_OK;
}

HRESULT Overture_Shoot_Up::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Overture_Shoot_Up::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.2f <= fCurrAnimationTime)
		m_pNero.lock()->Set_Weapon_Coll(Nero::NeroCom_Overture, false);

	if (0.53f <= fCurrAnimationTime)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Idle(NeroFSM::OVERTURE_SHOOT_UP);
		else
			NeroState::KeyInput_Cbs_Idle();
	}

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}

	return S_OK;
}

Overture_Shoot_Down::Overture_Shoot_Down(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Overture_Shoot_Down::~Overture_Shoot_Down()
{
}

Overture_Shoot_Down* Overture_Shoot_Down::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Overture_Shoot_Down(_pFSM, _nIndex, _pNero);
}

HRESULT Overture_Shoot_Down::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Overture_Shoot_Down", false, Nero::ANI_OVERTURE_SHOOT_DOWN);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Overture,"Shoot_Front", false);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_Overture, ATTACKTYPE::Attack_Down);
	m_pNero.lock()->Set_Weapon_Coll(Nero::NeroCom_Overture, true);
	m_pNero.lock()->CreateOvertureEff(Nero::EffDir_Down);
	return S_OK;
}

HRESULT Overture_Shoot_Down::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Overture_Shoot_Down::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.2f <= fCurrAnimationTime)
		m_pNero.lock()->Set_Weapon_Coll(Nero::NeroCom_Overture, false);

	if (0.5f <= fCurrAnimationTime)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Idle(NeroFSM::OVERTURE_SHOOT_DOWN);
		else
			NeroState::KeyInput_Cbs_Idle();
	}

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}

	return S_OK;
}

Overture_Shoot_Air::Overture_Shoot_Air(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Overture_Shoot_Air::~Overture_Shoot_Air()
{
}

Overture_Shoot_Air* Overture_Shoot_Air::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Overture_Shoot_Air(_pFSM, _nIndex, _pNero);
}

HRESULT Overture_Shoot_Air::StateEnter()
{
	NeroState::StateEnter();

	m_pNero.lock()->ChangeAnimation("Overture_Shoot_Air", false, Nero::ANI_OVERTURE_SHOOT_AIR);
	NeroState::ResetAnimation(0.96f, Nero::ANI_OVERTURE_SHOOT_AIR);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Overture,"Shoot_Front", false);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_Overture, ATTACKTYPE::Attack_Air);
	m_pNero.lock()->Set_Weapon_Coll(Nero::NeroCom_Overture, true);
	m_pNero.lock()->CreateOvertureEff(Nero::EffDir_Front);

	NeroState::ActiveGravity(false);
	return S_OK;
}

HRESULT Overture_Shoot_Air::StateExit()
{
	NeroState::StateExit();
	NeroState::ActiveGravity(true);
	return S_OK;
}

HRESULT Overture_Shoot_Air::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.2f <= fCurAnimationTime)
		m_pNero.lock()->Set_Weapon_Coll(Nero::NeroCom_Overture, false);

	if (0.37 <= fCurAnimationTime)
		NeroState::ActiveGravity(true);

	if (0.65f <= fCurAnimationTime && fCurAnimationTime <= 0.9f)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Jump(NeroFSM::OVERTURE_SHOOT_AIR);
		else
			NeroState::KeyInput_Cbs_Jump();
	}
	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
		return S_OK;
	}

	NeroState::IsGround();
	return S_OK;
}

Overture_Shoot_Air_Up::Overture_Shoot_Air_Up(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Overture_Shoot_Air_Up::~Overture_Shoot_Air_Up()
{
}

Overture_Shoot_Air_Up* Overture_Shoot_Air_Up::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Overture_Shoot_Air_Up(_pFSM, _nIndex, _pNero);
}

HRESULT Overture_Shoot_Air_Up::StateEnter()
{
	NeroState::StateEnter();

	m_pNero.lock()->ChangeAnimation("Overture_Shoot_Air_Up", false, Nero::ANI_OVERTURE_SHOOT_AIR_UP);
	NeroState::ResetAnimation(0.96f, Nero::ANI_OVERTURE_SHOOT_AIR_UP);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Overture,"Shoot_Front", false);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_Overture, ATTACKTYPE::Attack_Air_Start);
	m_pNero.lock()->Set_Weapon_Coll(Nero::NeroCom_Overture, true);
	m_pNero.lock()->CreateOvertureEff(Nero::EffDir_Up);

	NeroState::ActiveGravity(false);
	return S_OK;
}

HRESULT Overture_Shoot_Air_Up::StateExit()
{
	NeroState::StateExit();
	NeroState::ActiveGravity(true);
	return S_OK;
}

HRESULT Overture_Shoot_Air_Up::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.2f <= fCurAnimationTime)
		m_pNero.lock()->Set_Weapon_Coll(Nero::NeroCom_Overture, false);

	if (0.37 <= fCurAnimationTime)
		NeroState::ActiveGravity(true);

	if (0.65f <= fCurAnimationTime && fCurAnimationTime <= 0.9f)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Jump(NeroFSM::OVERTURE_SHOOT_AIR_UP);
		else
			NeroState::KeyInput_Cbs_Jump();
	}
	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
		return S_OK;
	}

	NeroState::IsGround();
	return S_OK;
}

Overture_Shoot_Air_Down::Overture_Shoot_Air_Down(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Overture_Shoot_Air_Down::~Overture_Shoot_Air_Down()
{
}

Overture_Shoot_Air_Down* Overture_Shoot_Air_Down::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Overture_Shoot_Air_Down(_pFSM, _nIndex, _pNero);
}

HRESULT Overture_Shoot_Air_Down::StateEnter()
{
	NeroState::StateEnter();

	m_pNero.lock()->ChangeAnimation("Overture_Shoot_Air_Down", false, Nero::ANI_OVERTURE_SHOOT_AIR_DOWN);
	NeroState::ResetAnimation(0.96f, Nero::ANI_OVERTURE_SHOOT_AIR_DOWN);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Overture,"Shoot_Front", false);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_Overture, ATTACKTYPE::Attack_Down);
	m_pNero.lock()->Set_Weapon_Coll(Nero::NeroCom_Overture, true);
	m_pNero.lock()->CreateOvertureEff(Nero::EffDir_Down);

	NeroState::ActiveGravity(false);
	return S_OK;
}

HRESULT Overture_Shoot_Air_Down::StateExit()
{
	NeroState::StateExit();
	NeroState::ActiveGravity(true);
	return S_OK;
}

HRESULT Overture_Shoot_Air_Down::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.2f <= fCurAnimationTime)
		m_pNero.lock()->Set_Weapon_Coll(Nero::NeroCom_Overture, false);

	if (0.37 <= fCurAnimationTime)
		NeroState::ActiveGravity(true);

	if (0.65f <= fCurAnimationTime && fCurAnimationTime <= 0.9f)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Jump(NeroFSM::OVERTURE_SHOOT_AIR_DOWN);
		else
			NeroState::KeyInput_Cbs_Jump();
	}
	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
		return S_OK;
	}

	NeroState::IsGround();
	return S_OK;
}

#pragma endregion

#pragma region Cbs // 삼절곤



Cbs_Idle::Cbs_Idle(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Cbs_Idle::~Cbs_Idle()
{
}

Cbs_Idle* Cbs_Idle::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Cbs_Idle(_pFSM, _nIndex, _pNero);
}

HRESULT Cbs_Idle::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Cbs_Idle", true, Nero::ANI_CBS_IDLE);
	m_pNero.lock()->SetCbsIdle();
	NeroState::ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Short);
	return S_OK;
}

HRESULT Cbs_Idle::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Cbs_Idle::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	NeroState::KeyInput_Cbs_Idle();

	if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}
	return S_OK;
}

Cbs_ComboA1::Cbs_ComboA1(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Cbs_ComboA1::~Cbs_ComboA1()
{
}

Cbs_ComboA1* Cbs_ComboA1::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Cbs_ComboA1(_pFSM, _nIndex, _pNero);
}

HRESULT Cbs_ComboA1::StateEnter()
{
	m_iNeroCurWeaponIndex = Nero::NeroCom_Cbs_Short;
	NeroState::StateEnter();
	m_pNero.lock()->ChangeWeapon(Nero::NeroCom_Cbs_Short);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_Cbs_Short, Nero::WS_Battle);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Short, "Cbs_ComboA1", false);
	m_pNero.lock()->ChangeAnimation("Cbs_ComboA1", false, Nero::ANI_CBS_COMBOA1);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_Cbs_Short, ATTACKTYPE::Attack_Front);
	ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Short);
	m_pNero.lock()->CheckAutoRotate();

	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeNewSword(WingSword_Ar1, false);
	}
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT Cbs_ComboA1::StateExit()
{
	NeroState::StateExit();
	NeroState::ActiveTrail(false);
	return S_OK;
}

HRESULT Cbs_ComboA1::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	UINT PreAnimationIndex = m_pNero.lock()->Get_PreAnimationIndex();
	if(0.27f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Short);
	else if (0.22f <= fCurrAnimationTime)
		ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Short);
	else if (0.12f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Short);
		

	if (0.27f <= fCurrAnimationTime && fCurrAnimationTime <= 0.37f)
	{
		if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Cbs_Idle(NeroFSM::CBS_COMBOA2);
		else
			NeroState::KeyInput_Idle();
	}

	if (0.6f <= fCurrAnimationTime)
	{
		NeroState::ActiveTrail(false);
	}


	if (Nero::ANI_CBS_COMBOA5 != PreAnimationIndex && 0.84f <= fCurrAnimationTime)
		NeroState::SetCbsIdle();

	else if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::IDLE);
		return S_OK;
	}
	return S_OK;
}

Cbs_ComboA2::Cbs_ComboA2(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Cbs_ComboA2::~Cbs_ComboA2()
{
}

Cbs_ComboA2* Cbs_ComboA2::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Cbs_ComboA2(_pFSM, _nIndex, _pNero);
}

HRESULT Cbs_ComboA2::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Cbs_ComboA2", false, Nero::ANI_CBS_COMBOA2);
	m_pNero.lock()->ChangeWeapon(Nero::NeroCom_Cbs_Short);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_Cbs_Short, Nero::WS_Battle);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Short, "Cbs_ComboA2", false);
	ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Short);
	m_pNero.lock()->CheckAutoRotate();
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeNewSword(WingSword_Ar2, false);
	}
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT Cbs_ComboA2::StateExit()
{
	NeroState::StateExit();
	NeroState::ActiveTrail(false);
	return S_OK;
}

HRESULT Cbs_ComboA2::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.28f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Short);
	else if (0.2f <= fCurrAnimationTime)
		ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Short);
	else if (0.13f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Short);

	if (0.6f <= fCurrAnimationTime)
	{
		NeroState::ActiveTrail(false);
	}


	if (0.28f <= fCurrAnimationTime && fCurrAnimationTime <= 0.38f)
	{
		if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Cbs_Idle(NeroFSM::CBS_COMBOA3);
		else
			NeroState::KeyInput_Idle();
	}

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::IDLE);
		return S_OK;
	}

	if (0.77f <= fCurrAnimationTime)
		NeroState::SetCbsIdle();

	return S_OK;
}

Cbs_ComboA3::Cbs_ComboA3(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Cbs_ComboA3::~Cbs_ComboA3()
{
}

Cbs_ComboA3* Cbs_ComboA3::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Cbs_ComboA3(_pFSM, _nIndex, _pNero);
}

HRESULT Cbs_ComboA3::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Cbs_ComboA3", false, Nero::ANI_CBS_COMBOA3);
	m_pNero.lock()->ChangeWeapon(Nero::NeroCom_Cbs_Short);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_Cbs_Short, 2);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Short, "Cbs_ComboA3", false);
	
	ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Short);
	m_pNero.lock()->CheckAutoRotate();

	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeNewSword(WingSword_Ar3, false);
	}
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT Cbs_ComboA3::StateExit()
{
	NeroState::StateExit();
	NeroState::ActiveTrail(false);
	return S_OK;
}

HRESULT Cbs_ComboA3::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (0.3f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Short);
	else if (0.2f <= fCurrAnimationTime)
		ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Short);
	else if (0.14f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Short);

	if (0.3f <= fCurrAnimationTime && fCurrAnimationTime <= 0.4f)
	{
		if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Cbs_Idle(NeroFSM::CBS_COMBOA4);
		else
			NeroState::KeyInput_Idle();
	}
	if (0.6f <= fCurrAnimationTime)
	{
		NeroState::ActiveTrail(false);
	}

	
	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::IDLE);
		return S_OK;
	}

	if (0.81f <= fCurrAnimationTime)
		NeroState::SetCbsIdle();
	else if(0.37 <= fCurrAnimationTime)
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_Cbs_Short, 1);
	else
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_Cbs_Short, 2);

	return S_OK;
}

Cbs_ComboA4::Cbs_ComboA4(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Cbs_ComboA4::~Cbs_ComboA4()
{
}

Cbs_ComboA4* Cbs_ComboA4::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Cbs_ComboA4(_pFSM, _nIndex, _pNero);
}

HRESULT Cbs_ComboA4::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Cbs_ComboA4", false, Nero::ANI_CBS_COMBOA4);
	m_pNero.lock()->ChangeWeapon(Nero::NeroCom_Cbs_Short);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_Cbs_Short, Nero::WS_Battle);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Short, "Cbs_ComboA4", false);

	ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Short);
	m_pNero.lock()->CheckAutoRotate();

	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeNewSword(WingSword_Ar4, false);
	}
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT Cbs_ComboA4::StateExit()
{
	NeroState::StateExit();
	NeroState::ActiveTrail(false);
	return S_OK;
}

HRESULT Cbs_ComboA4::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (0.28f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Short);
	else if (0.17f <= fCurrAnimationTime)
		ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Short);
	else if (0.1f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Short);

	if (0.28f <= fCurrAnimationTime && fCurrAnimationTime <= 0.38f)
	{
		if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Cbs_Idle(NeroFSM::CBS_COMBOA5);
		else
			NeroState::KeyInput_Idle();
	}

	if (0.6f <= fCurrAnimationTime)
	{
		NeroState::ActiveTrail(false);
	}


	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::IDLE);
		return S_OK;
	}

	if (0.81f <= fCurrAnimationTime)
		NeroState::SetCbsIdle();
	else if (0.14 <= fCurrAnimationTime && fCurrAnimationTime <= 0.31f)
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_Cbs_Short, 2);
	else
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_Cbs_Short, 1);

	return S_OK;
}

Cbs_ComboA5::Cbs_ComboA5(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Cbs_ComboA5::~Cbs_ComboA5()
{
}

Cbs_ComboA5* Cbs_ComboA5::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Cbs_ComboA5(_pFSM, _nIndex, _pNero);
}

HRESULT Cbs_ComboA5::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Cbs_ComboA5", false, Nero::ANI_CBS_COMBOA5);
	m_pNero.lock()->ChangeWeapon(Nero::NeroCom_Cbs_Short);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_Cbs_Short, 1);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Short, "Cbs_ComboA5", false);
	
	ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Short);
	m_pNero.lock()->CheckAutoRotate();
	m_bPlayOnce = true;
	NeroState::ActiveTrail(true);
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeNewSword(WingSword_HT, false);
	}
	return S_OK;
}

HRESULT Cbs_ComboA5::StateExit()
{
	NeroState::StateExit();
	NeroState::ActiveTrail(false);
	return S_OK;
}

HRESULT Cbs_ComboA5::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.6f <= fCurrAnimationTime)
	{
		NeroState::ActiveTrail(false);
	}

	if (0.33f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Short);
	else if (0.26f <= fCurrAnimationTime)
	{
		m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_Cbs_Short, ATTACKTYPE::Attack_KnocBack);
		ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Short);
	}
	else if (0.14f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Short);

	if (0.8f <= fCurrAnimationTime)
		NeroState::SetCbsIdle();
	else if(0.35f <= fCurrAnimationTime && fCurrAnimationTime <=  0.4f)
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_Cbs_Short, 2);
	else
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_Cbs_Short, 1);




	if (0.83f <= fCurrAnimationTime)
	{
		if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Cbs_Idle();
		else
			NeroState::KeyInput_Idle();
	}
	else if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::IDLE);
		return S_OK;
	}

	return S_OK;
}

#pragma endregion


#pragma region Cbs_Skill // 삼절곤 스킬



Cbs_SKill_Crystal::Cbs_SKill_Crystal(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Cbs_SKill_Crystal::~Cbs_SKill_Crystal()
{
}

Cbs_SKill_Crystal* Cbs_SKill_Crystal::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Cbs_SKill_Crystal(_pFSM, _nIndex, _pNero);
}

HRESULT Cbs_SKill_Crystal::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Cbs_SKill_Crystal", false, Nero::ANI_CBS_SKILL_CRYSTAL);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_Cbs_Short, Nero::WS_Battle);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Short, "Cbs_Crystal_Start", false);

	return S_OK;
}

HRESULT Cbs_SKill_Crystal::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Cbs_SKill_Crystal::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::IDLE);
		return S_OK;
	}
	if (0.88f <= fCurrAnimationTime)
		NeroState::SetCbsIdle();
	return S_OK;
}

Cbs_SKill_IceAge_Start::Cbs_SKill_IceAge_Start(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Cbs_SKill_IceAge_Start::~Cbs_SKill_IceAge_Start()
{
}

Cbs_SKill_IceAge_Start* Cbs_SKill_IceAge_Start::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Cbs_SKill_IceAge_Start(_pFSM, _nIndex, _pNero);
}

HRESULT Cbs_SKill_IceAge_Start::StateEnter()
{
	m_iNeroCurWeaponIndex = Nero::NeroCom_Cbs_Short;
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Cbs_SKill_IceAge_Start", false, Nero::ANI_CBS_SKILL_ICEAGE_START);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_Cbs_Short, Nero::WS_Battle);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Short, "Cbs_IceAge_Loop", true);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_Cbs_Short, ATTACKTYPE::Attack_Front);
	NeroState::ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Short);
	m_pNero.lock()->PlayEffect(GAMEOBJECTTAG::Eff_IceAge);
	m_pNero.lock()->ChangeWeaponCollSize(0.3f);
	return S_OK;
}

HRESULT Cbs_SKill_IceAge_Start::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Cbs_SKill_IceAge_Start::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::CBS_ICEAGE_LOOP);
		return S_OK;
	}

	return S_OK;
}

Cbs_SKill_IceAge_Loop::Cbs_SKill_IceAge_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
	, m_fLoopTime(0.f)
{
}

Cbs_SKill_IceAge_Loop::~Cbs_SKill_IceAge_Loop()
{
}

Cbs_SKill_IceAge_Loop* Cbs_SKill_IceAge_Loop::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Cbs_SKill_IceAge_Loop(_pFSM, _nIndex, _pNero);
}

HRESULT Cbs_SKill_IceAge_Loop::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Cbs_SKill_IceAge_Loop", true, Nero::ANI_CBS_SKILL_ICEAGE_LOOP);
	//m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_Cbs_Short, Nero::WS_Battle);
	//m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Short, "Cbs_IceAge_Loop", true);

	m_fLoopTime = 1.5f;
	NeroState::ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Short);
	return S_OK;
}

HRESULT Cbs_SKill_IceAge_Loop::StateExit()
{
	NeroState::StateExit();
	m_fLoopTime = 0.f;
	NeroState::ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Short);
	return S_OK;
}

HRESULT Cbs_SKill_IceAge_Loop::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingAccTime();
	
	m_fLoopTime -= _fDeltaTime;
	if (0.8f <= fCurrAnimationTime)
		NeroState::ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Short);
	else if (0.7f <= fCurrAnimationTime)
		NeroState::ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Short);
	else if (0.6f <= fCurrAnimationTime)
		NeroState::ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Short);
	else if (0.5f <= fCurrAnimationTime)
		NeroState::ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Short);
	else if (0.4f <= fCurrAnimationTime)
		NeroState::ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Short);
	else if (0.3f <= fCurrAnimationTime)
		NeroState::ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Short);
	else if (0.2f <= fCurrAnimationTime)
		NeroState::ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Short);
	else if (0.1f <= fCurrAnimationTime)
		NeroState::ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Short);
	

	if (m_fLoopTime < 0.f)
	{
		m_pFSM->ChangeState(NeroFSM::CBS_ICEAGE_END);
		return S_OK;
	}

	return S_OK;
}

Cbs_SKill_IceAge_End::Cbs_SKill_IceAge_End(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Cbs_SKill_IceAge_End::~Cbs_SKill_IceAge_End()
{
}

Cbs_SKill_IceAge_End* Cbs_SKill_IceAge_End::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Cbs_SKill_IceAge_End(_pFSM, _nIndex, _pNero);
}

HRESULT Cbs_SKill_IceAge_End::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Cbs_SKill_IceAge_End", false, Nero::ANI_CBS_SKILL_ICEAGE_END);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Short, "Cbs_IceAge_End", false);
	NeroState::ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Short);
	return S_OK;
}

HRESULT Cbs_SKill_IceAge_End::StateExit()
{
	NeroState::StateExit();
	m_pNero.lock()->ChangeWeaponCollSize();
	return S_OK;
}

HRESULT Cbs_SKill_IceAge_End::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::IDLE);
		return S_OK;
	}

	if (0.51f <= fCurrAnimationTime)
		NeroState::SetCbsIdle();

	return S_OK;
}

Cbs_SKill_Revolver_Start::Cbs_SKill_Revolver_Start(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Cbs_SKill_Revolver_Start::~Cbs_SKill_Revolver_Start()
{
}

Cbs_SKill_Revolver_Start* Cbs_SKill_Revolver_Start::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Cbs_SKill_Revolver_Start(_pFSM, _nIndex, _pNero);
}

HRESULT Cbs_SKill_Revolver_Start::StateEnter()
{
	m_iNeroCurWeaponIndex = Nero::NeroCom_Cbs_Short;
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Cbs_SKill_Revolver_Start", false, Nero::ANI_CBS_SKILL_REVOLVER_START);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_Cbs_Short, Nero::WS_Battle);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_Cbs_Short, ATTACKTYPE::Attack_Front);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Short, "Cbs_Revolver_End", false);
	m_pNero.lock()->SetAddForce({ 0.f,90.f,0.f });

	
	m_pNero.lock()->SetFly(true);
	return S_OK;
}

HRESULT Cbs_SKill_Revolver_Start::StateExit()
{
	NeroState::StateExit();
	NeroState::ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Short);
	return S_OK;
}

HRESULT Cbs_SKill_Revolver_Start::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::CBS_REVOLVER_LOOP);
		return S_OK;
	}

	return S_OK;
}

Cbs_SKill_Revolver_Loop::Cbs_SKill_Revolver_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero), m_iLoopCount(0)
{
}

Cbs_SKill_Revolver_Loop::~Cbs_SKill_Revolver_Loop()
{
}

Cbs_SKill_Revolver_Loop* Cbs_SKill_Revolver_Loop::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Cbs_SKill_Revolver_Loop(_pFSM, _nIndex, _pNero);
}

HRESULT Cbs_SKill_Revolver_Loop::StateEnter()
{
	m_iNeroCurWeaponIndex = Nero::NeroCom_Cbs_Short;
	NeroState::StateEnter();
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_Cbs_Short, Nero::WS_Battle);
	m_pNero.lock()->ChangeAnimation("Cbs_SKill_Revolver_Loop", true, Nero::ANI_CBS_SKILL_REVOLVER_LOOP);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Short, "Cbs_Revolver_Loop", true);
	m_pNero.lock()->Set_Weapon_Coll(Nero::NeroCom_Cbs_Short, true);
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeNewSword(WingSword_Stinger_Loop, true);
	}
	return S_OK;
}

HRESULT Cbs_SKill_Revolver_Loop::StateExit()
{
	NeroState::StateExit();
	NeroState::ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Short);
	return S_OK;
}

HRESULT Cbs_SKill_Revolver_Loop::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	//얘는 그냥 땅에 닿으면 멈추는걸로 바꿔야됨
	m_pNero.lock()->NeroMove(Nero::Dir_Front, 0.01f);
	m_pNero.lock()->SetAddForce({ 0.f,2.5f,0.f });
	if (m_pNero.lock()->CheckIsGround())
	{
		m_pFSM->ChangeState(NeroFSM::CBS_REVOLVER_END);
		return S_OK;
	}
	NeroState::ActiveTrail(true);
	return S_OK;
}

Cbs_SKill_Revolver_End::Cbs_SKill_Revolver_End(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Cbs_SKill_Revolver_End::~Cbs_SKill_Revolver_End()
{
}

Cbs_SKill_Revolver_End* Cbs_SKill_Revolver_End::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Cbs_SKill_Revolver_End(_pFSM, _nIndex, _pNero);
}

HRESULT Cbs_SKill_Revolver_End::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Cbs_SKill_Revolver_End", false, Nero::ANI_CBS_SKILL_REVOLVER_END);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Short, "Cbs_Revolver_End", false);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_Cbs_Short, ATTACKTYPE::Attack_KnocBack);
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeNewSword(WingSword_Stinger_End, false);
	}
	m_pNero.lock()->ChangeNeroDirection(Nero::Dir_Front);
	return S_OK;
}

HRESULT Cbs_SKill_Revolver_End::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Cbs_SKill_Revolver_End::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.19f <= fCurrAnimationTime)
	{
		NeroState::ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Short);
		NeroState::ActiveTrail(false);
	}
	else if(0.1 <= fCurrAnimationTime)
		NeroState::ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Short);

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::IDLE);
		return S_OK;
	}

	if (0.78f <= fCurrAnimationTime)
		NeroState::SetCbsIdle();

	return S_OK;
}

Cbs_SKill_Swing::Cbs_SKill_Swing(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Cbs_SKill_Swing::~Cbs_SKill_Swing()
{
}

Cbs_SKill_Swing* Cbs_SKill_Swing::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Cbs_SKill_Swing(_pFSM, _nIndex, _pNero);
}

HRESULT Cbs_SKill_Swing::StateEnter()
{
	m_iNeroCurWeaponIndex = Nero::NeroCom_Cbs_Short;
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Cbs_SKill_Swing", false, Nero::ANI_CBS_SKILL_SWING);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_Cbs_Short, Nero::WS_Battle);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Short, "Cbs_Swing_Fly", false);

	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_Cbs_Short, ATTACKTYPE::Attack_Front);
	NeroState::ActiveGravity(false);
	NeroState::ActiveTrail(true);

	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeNewSword(WingSword_Helm, false);
	}
	return S_OK;
}

HRESULT Cbs_SKill_Swing::StateExit()
{
	NeroState::StateExit();
	NeroState::SetCbsIdle();
	return S_OK;
}

HRESULT Cbs_SKill_Swing::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);

	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.38f <= fCurrAnimationTime)
		NeroState::ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Short);
	else if(0.12f <= fCurrAnimationTime)
		NeroState::ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Short);

	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
		return S_OK;
	}

	if (0.48f <= fCurrAnimationTime)
	{
		NeroState::ActiveTrail(false);
		NeroState::ActiveGravity(true);
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Jump();
		else
			NeroState::KeyInput_Cbs_Jump();
	}

	if (0.8f <= fCurrAnimationTime)
		NeroState::SetCbsIdle();

	NeroState::IsGround();
	return S_OK;
}

Middle_Cbs_BiAttack::Middle_Cbs_BiAttack(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)

{
}

Middle_Cbs_BiAttack::~Middle_Cbs_BiAttack()
{
}

Middle_Cbs_BiAttack* Middle_Cbs_BiAttack::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Middle_Cbs_BiAttack(_pFSM, _nIndex, _pNero);
}

HRESULT Middle_Cbs_BiAttack::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Middle_Cbs_BiAttack", false, Nero::ANI_MIDDLE_CBS_BIATTACK);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Middle, "Middle_Cbs_BiAttack", false);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_Cbs_Middle, 0);
	NeroState::ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Middle);

	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeNewSword(WingSword_Stinger_Loop, true);
		m_bPlayOnce = true;
	}
	return S_OK;
}

HRESULT Middle_Cbs_BiAttack::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Middle_Cbs_BiAttack::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.65f <= fCurrAnimationTime)
	{
		NeroState::ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Middle);

	}
	else if (0.2f <= fCurrAnimationTime)
	{
		NeroState::ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Middle);

	}
	else if (0.15f <= fCurrAnimationTime)
		NeroState::ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Middle);

	if (0.45f <= fCurrAnimationTime && m_bPlayOnce)
	{
		m_bPlayOnce = false;
		m_pNero.lock()->ChangeNewSword(WingSword_Stinger_End, false);
	}

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::IDLE);
		return S_OK;
	}

	if (0.97f <= fCurrAnimationTime)
		NeroState::SetCbsIdle();

	if (0.76f <= fCurrAnimationTime)
	{
		NeroState::KeyInput_Cbs_Idle();
	}

	return S_OK;
}

Middle_Cbs_BlitzAttack::Middle_Cbs_BlitzAttack(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Middle_Cbs_BlitzAttack::~Middle_Cbs_BlitzAttack()
{
}

Middle_Cbs_BlitzAttack* Middle_Cbs_BlitzAttack::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Middle_Cbs_BlitzAttack(_pFSM, _nIndex, _pNero);
}

HRESULT Middle_Cbs_BlitzAttack::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Middle_Cbs_BlitzAttack", false, Nero::ANI_MIDDLE_CBS_BLITZATTACK);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Middle,"Middle_Cbs_BlitzAttack", false);

	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeNewSword(WingSword_Ar3, false);
	}
	return S_OK;
}

HRESULT Middle_Cbs_BlitzAttack::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Middle_Cbs_BlitzAttack::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::IDLE);
		return S_OK;
	}

	if (0.9f <= fCurrAnimationTime)
		NeroState::SetCbsIdle();

	if (0.73f <= fCurrAnimationTime)
	{
		NeroState::KeyInput_Cbs_Idle();
	}

	return S_OK;
}

Middle_Cbs_Satellite::Middle_Cbs_Satellite(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Middle_Cbs_Satellite::~Middle_Cbs_Satellite()
{
}

Middle_Cbs_Satellite* Middle_Cbs_Satellite::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Middle_Cbs_Satellite(_pFSM, _nIndex, _pNero);
}

HRESULT Middle_Cbs_Satellite::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Middle_Cbs_Satellite", false, Nero::ANI_MIDDLE_CBS_SATELLITE);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Middle, "Middle_Cbs_Satellite", false);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_Cbs_Middle, 0);
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeNewSword(WingSword_ComboB1, false);
	}
	return S_OK;
}

HRESULT Middle_Cbs_Satellite::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Middle_Cbs_Satellite::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.65f <= fCurrAnimationTime)
		NeroState::ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Middle);
	else if(0.19f <=fCurrAnimationTime)
		NeroState::ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Middle);

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::IDLE);
		return S_OK;
	}

	if (0.82f <= fCurrAnimationTime)
		NeroState::SetCbsIdle();
	if (0.72f <= fCurrAnimationTime)
	{
		NeroState::KeyInput_Cbs_Idle();
	}
	return S_OK;
}

Middle_Cbs_Satellite_Air::Middle_Cbs_Satellite_Air(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Middle_Cbs_Satellite_Air::~Middle_Cbs_Satellite_Air()
{
}

Middle_Cbs_Satellite_Air* Middle_Cbs_Satellite_Air::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Middle_Cbs_Satellite_Air(_pFSM, _nIndex, _pNero);
}

HRESULT Middle_Cbs_Satellite_Air::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Middle_Cbs_Satellite_Air", false, Nero::ANI_MIDDLE_CBS_SATELLITE_AIR);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Middle, "Middle_Cbs_Satellite_Air", false);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_Cbs_Middle, 0);
	NeroState::ActiveGravity(false);
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeNewSword(WingSword_ComboB1, false);
	}
	return S_OK;
}

HRESULT Middle_Cbs_Satellite_Air::StateExit()
{
	NeroState::StateExit();
	NeroState::SetCbsIdle();
	return S_OK;
}

HRESULT Middle_Cbs_Satellite_Air::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);

	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.59f <= fCurrAnimationTime)
		NeroState::ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Middle);
	else if (0.18f <= fCurrAnimationTime)
		NeroState::ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Middle);

	if (0.77f <= fCurrAnimationTime)
	{
		NeroState::ActiveGravity(true);
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Jump();
		else
			NeroState::KeyInput_Cbs_Jump();
	}

	if (0.83f <= fCurrAnimationTime)
		NeroState::SetCbsIdle();

	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
		return S_OK;
	}

	NeroState::IsGround();
	return S_OK;
}

Middle_Cbs_Strike::Middle_Cbs_Strike(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Middle_Cbs_Strike::~Middle_Cbs_Strike()
{
}

Middle_Cbs_Strike* Middle_Cbs_Strike::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Middle_Cbs_Strike(_pFSM, _nIndex, _pNero);
}

HRESULT Middle_Cbs_Strike::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Middle_Cbs_Strike", false, Nero::ANI_MIDDLE_CBS_STRIKE);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Middle, "Middle_Cbs_Strike", false);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_Cbs_Middle, ATTACKTYPE::Attack_KnocBack);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_Cbs_Middle, 1);
	return S_OK;
}

HRESULT Middle_Cbs_Strike::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Middle_Cbs_Strike::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (0.28f <= fCurrAnimationTime)
		NeroState::ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Middle);
	else if (0.1f <= fCurrAnimationTime)
		NeroState::ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Middle);

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::IDLE);
		return S_OK;
	}

	if (0.71f <= fCurrAnimationTime)
		NeroState::SetCbsIdle();

	if (0.59f <= fCurrAnimationTime)
	{
		NeroState::KeyInput_Cbs_Idle();
	}
	return S_OK;
}

Middle_Cbs_Strike_Air::Middle_Cbs_Strike_Air(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Middle_Cbs_Strike_Air::~Middle_Cbs_Strike_Air()
{
}

Middle_Cbs_Strike_Air* Middle_Cbs_Strike_Air::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Middle_Cbs_Strike_Air(_pFSM, _nIndex, _pNero);
}

HRESULT Middle_Cbs_Strike_Air::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Middle_Cbs_Strike_Air", false, Nero::ANI_MIDDLE_CBS_STRIKE_AIR);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Middle, "Middle_Cbs_Strike_Air", false);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_Cbs_Middle, ATTACKTYPE::Attack_KnocBack);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_Cbs_Middle, 1);
	NeroState::ActiveGravity(false);
	return S_OK;
}

HRESULT Middle_Cbs_Strike_Air::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Middle_Cbs_Strike_Air::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.28f <= fCurrAnimationTime)
		NeroState::ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Middle);
	else if (0.1f <= fCurrAnimationTime)
		NeroState::ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Middle);

	if (m_pNero.lock()->IsAnimationEnd())
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);

	if (0.65f <= fCurrAnimationTime)
	{
		NeroState::ActiveGravity(true);
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Jump();
		else
			NeroState::KeyInput_Cbs_Jump();
	}

	if (0.71f <= fCurrAnimationTime)
		NeroState::SetCbsIdle();

	NeroState::IsGround();
	return S_OK;
}

Middle_Cbs_Strike_Air_Down::Middle_Cbs_Strike_Air_Down(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Middle_Cbs_Strike_Air_Down::~Middle_Cbs_Strike_Air_Down()
{
}

Middle_Cbs_Strike_Air_Down* Middle_Cbs_Strike_Air_Down::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Middle_Cbs_Strike_Air_Down(_pFSM, _nIndex, _pNero);
}

HRESULT Middle_Cbs_Strike_Air_Down::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Middle_Cbs_Strike_Air_Down", false, Nero::ANI_MIDDLE_CBS_STRIKE_AIR_DOWN);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Middle, "Middle_Cbs_Strike_Air", false);

	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_Cbs_Middle, ATTACKTYPE::Attack_KnocBack);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_Cbs_Middle, 1);
	NeroState::ActiveGravity(false);
	return S_OK;
}

HRESULT Middle_Cbs_Strike_Air_Down::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Middle_Cbs_Strike_Air_Down::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.28f <= fCurrAnimationTime)
		NeroState::ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Middle);
	else if (0.1f <= fCurrAnimationTime)
		NeroState::ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Middle);

	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
		return S_OK;
	}

	if (0.65f <= fCurrAnimationTime)
	{
		NeroState::ActiveGravity(true);
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Jump();
		else
			NeroState::KeyInput_Cbs_Jump();
	}

	if (0.79f <= fCurrAnimationTime)
		NeroState::SetCbsIdle();

	NeroState::IsGround();
	return S_OK;
}

Middle_Cbs_Strike_Air_Up::Middle_Cbs_Strike_Air_Up(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Middle_Cbs_Strike_Air_Up::~Middle_Cbs_Strike_Air_Up()
{
}

Middle_Cbs_Strike_Air_Up* Middle_Cbs_Strike_Air_Up::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Middle_Cbs_Strike_Air_Up(_pFSM, _nIndex, _pNero);
}

HRESULT Middle_Cbs_Strike_Air_Up::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Middle_Cbs_Strike_Air_Up", false, Nero::ANI_MIDDLE_CBS_STRIKE_AIR_UP);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Middle, "Middle_Cbs_Strike_Air", false);

	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_Cbs_Middle, ATTACKTYPE::Attack_KnocBack);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_Cbs_Middle, 1);
	NeroState::ActiveGravity(false);
	return S_OK;
}

HRESULT Middle_Cbs_Strike_Air_Up::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Middle_Cbs_Strike_Air_Up::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.28f <= fCurrAnimationTime)
		NeroState::ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Middle);
	else if (0.1f <= fCurrAnimationTime)
		NeroState::ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Middle);

	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
		return S_OK;
	}

	if (0.65f <= fCurrAnimationTime)
	{
		NeroState::ActiveGravity(true);
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Jump();
		else
			NeroState::KeyInput_Cbs_Jump();
	}

	if (0.81f <= fCurrAnimationTime)
		NeroState::SetCbsIdle();

	NeroState::IsGround();
	return S_OK;
}

Middle_Cbs_Strike_Down::Middle_Cbs_Strike_Down(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Middle_Cbs_Strike_Down::~Middle_Cbs_Strike_Down()
{
}

Middle_Cbs_Strike_Down* Middle_Cbs_Strike_Down::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Middle_Cbs_Strike_Down(_pFSM, _nIndex, _pNero);
}

HRESULT Middle_Cbs_Strike_Down::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Middle_Cbs_Strike_Down", false, Nero::ANI_MIDDLE_CBS_STRIKE_DOWN);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Middle, "Middle_Cbs_Strike", false);

	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_Cbs_Middle, ATTACKTYPE::Attack_KnocBack);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_Cbs_Middle, 1);
	return S_OK;
}

HRESULT Middle_Cbs_Strike_Down::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Middle_Cbs_Strike_Down::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.28f <= fCurrAnimationTime)
		NeroState::ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Middle);
	else if (0.1f <= fCurrAnimationTime)
		NeroState::ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Middle);

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::IDLE);
		return S_OK;
	}

	if (0.71f <= fCurrAnimationTime)
		NeroState::SetCbsIdle();

	return S_OK;
}

Middle_Cbs_Strike_Up::Middle_Cbs_Strike_Up(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Middle_Cbs_Strike_Up::~Middle_Cbs_Strike_Up()
{
}

Middle_Cbs_Strike_Up* Middle_Cbs_Strike_Up::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Middle_Cbs_Strike_Up(_pFSM, _nIndex, _pNero);
}

HRESULT Middle_Cbs_Strike_Up::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Middle_Cbs_Strike_Up", false, Nero::ANI_MIDDLE_CBS_STRIKE_UP);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Middle, "Middle_Cbs_Strike", false);

	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_Cbs_Middle, ATTACKTYPE::Attack_KnocBack);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_Cbs_Middle, 1);

	return S_OK;
}

HRESULT Middle_Cbs_Strike_Up::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Middle_Cbs_Strike_Up::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.28f <= fCurrAnimationTime)
		NeroState::ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Middle);
	else if (0.1f <= fCurrAnimationTime)
		NeroState::ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Middle);

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::IDLE);
		return S_OK;
	}
	if (0.71f <= fCurrAnimationTime)
		NeroState::SetCbsIdle();

	return S_OK;
}
//썬더불렛
Middle_Cbs_ThunderBullet::Middle_Cbs_ThunderBullet(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Middle_Cbs_ThunderBullet::~Middle_Cbs_ThunderBullet()
{
}

Middle_Cbs_ThunderBullet* Middle_Cbs_ThunderBullet::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Middle_Cbs_ThunderBullet(_pFSM, _nIndex, _pNero);
}

HRESULT Middle_Cbs_ThunderBullet::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Middle_Cbs_ThunderBullet", false, Nero::ANI_MIDDLE_CBS_THUNDERBULLET);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Middle, "Middle_Cbs_ThunderBullet", false);
	return S_OK;
}

HRESULT Middle_Cbs_ThunderBullet::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Middle_Cbs_ThunderBullet::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::IDLE);
		return S_OK;
	}

	if (0.77f <= fCurrAnimationTime)
		NeroState::SetCbsIdle();

	return S_OK;
}

#pragma endregion

#pragma region Pole // 삼절곤 봉 버전



Pole_ComboA1::Pole_ComboA1(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Pole_ComboA1::~Pole_ComboA1()
{
}

Pole_ComboA1* Pole_ComboA1::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Pole_ComboA1(_pFSM, _nIndex, _pNero);
}

HRESULT Pole_ComboA1::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Pole_ComboA1", false, Nero::ANI_POLE_COMBOA1);
	m_pNero.lock()->ChangeWeapon(Nero::NeroCom_Cbs_Long);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_Cbs_Long, ATTACKTYPE::Attack_R);
	m_pNero.lock()->CheckAutoRotate();

	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeNewSword(WingSword_ComboA1, false);
	}
	return S_OK;
}

HRESULT Pole_ComboA1::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Pole_ComboA1::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.22f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Long);
	else if (0.15f <= fCurrAnimationTime)
		ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Long);


	if (0.2f <= fCurrAnimationTime && fCurrAnimationTime <= 0.29f)
	{
		if (Input::GetMouse(DIM_M))
		{
			ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Long);
			m_pFSM->ChangeState(NeroFSM::POLE_COMBOA2);
			return S_OK;
		}
	}

	else if (0.31f <= fCurrAnimationTime && fCurrAnimationTime <= 0.42f)
	{
		if (Input::GetMouse(DIM_M))
		{
			// 레드퀸 콤보 땅바닥 찍는거 시작 
			m_pFSM->ChangeState(NeroFSM::POLE_COMBOB1);
			return S_OK;
		}
	}


	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::IDLE);
		return S_OK;
	}
	if (0.71 <= fCurrAnimationTime)
		NeroState::SetCbsIdle();
	else if (0.45 <= fCurrAnimationTime)
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Middle, "Middle_Cbs_PoleComboA1_End", false, {}, true);

	if (0.59f <= fCurrAnimationTime)
	{
		NeroState::KeyInput_Cbs_Idle();
	}
	return S_OK;
}

Pole_ComboA2::Pole_ComboA2(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Pole_ComboA2::~Pole_ComboA2()
{
}

Pole_ComboA2* Pole_ComboA2::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Pole_ComboA2(_pFSM, _nIndex, _pNero);
}

HRESULT Pole_ComboA2::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Pole_ComboA2", false, Nero::ANI_POLE_COMBOA2);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_Cbs_Long, ATTACKTYPE::Attack_L);
	m_pNero.lock()->CheckAutoRotate();
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeNewSword(WingSword_ComboA2, false);
	}
	return S_OK;
}

HRESULT Pole_ComboA2::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Pole_ComboA2::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.15f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Long);
	else if (0.1f <= fCurrAnimationTime)
		ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Long);


	if (0.2f <= fCurrAnimationTime && fCurrAnimationTime <= 0.29f)
	{
		if (Input::GetMouse(DIM_M))
		{
			ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Long);
			m_pFSM->ChangeState(NeroFSM::POLE_COMBOA3);
			return S_OK;
		}
	}
	else if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::IDLE);
		return S_OK;
	}

	if (0.72f <= fCurrAnimationTime)
		NeroState::SetCbsIdle();
	else if (0.36f <= fCurrAnimationTime)
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Middle, "Middle_Cbs_PoleComboA2_End", false, {}, true);

	if (0.46f <= fCurrAnimationTime)
	{
		NeroState::KeyInput_Cbs_Idle();
	}
	return S_OK;
}

Pole_ComboA3::Pole_ComboA3(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Pole_ComboA3::~Pole_ComboA3()
{
}

Pole_ComboA3* Pole_ComboA3::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Pole_ComboA3(_pFSM, _nIndex, _pNero);
}

HRESULT Pole_ComboA3::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Pole_ComboA3", false, Nero::ANI_POLE_COMBOA3);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_Cbs_Long, ATTACKTYPE::Attack_KnocBack);
	m_pNero.lock()->CheckAutoRotate();
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeNewSword(WingSword_HT, false);
	}
	return S_OK;
}

HRESULT Pole_ComboA3::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Pole_ComboA3::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.27f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Long);
	else if (0.21f <= fCurrAnimationTime)
		ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Long);

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::IDLE);
		return S_OK;
	}

	if (0.72f <= fCurrAnimationTime)
		NeroState::SetCbsIdle();
	else if (0.53f <= fCurrAnimationTime)
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Middle, "Middle_Cbs_PoleComboA3_End", false, {}, true);

	if (0.53f <= fCurrAnimationTime)
	{
		NeroState::KeyInput_Cbs_Idle();
	}
	return S_OK;
}

Pole_ComboB1::Pole_ComboB1(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Pole_ComboB1::~Pole_ComboB1()
{
}

Pole_ComboB1* Pole_ComboB1::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Pole_ComboB1(_pFSM, _nIndex, _pNero);
}

HRESULT Pole_ComboB1::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Pole_ComboB1", false, Nero::ANI_POLE_COMBOB1);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_Cbs_Long, ATTACKTYPE::Attack_Front);
	m_pNero.lock()->CheckAutoRotate();

	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeNewSword(WingSword_ComboB2_Loop, true);
	}
	return S_OK;
}

HRESULT Pole_ComboB1::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Pole_ComboB1::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (0.34f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Long);
	else if (0.32f <= fCurrAnimationTime)
		ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Long);
	else if (0.27f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Long);
	else if (0.23f <= fCurrAnimationTime)
		ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Long);
	else if (0.21f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Long);
	else if (0.18f <= fCurrAnimationTime)
		ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Long);
	else if (0.15f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Long);
	else if (0.11f <= fCurrAnimationTime)
		ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Long);

	if (0.46f <= fCurrAnimationTime)
		m_pNero.lock()->SetActive_NeroComponent(Nero::NeroCom_NewWingSword, false);


	if (0.36f <= fCurrAnimationTime && fCurrAnimationTime <= 0.46f)
	{
		if (Input::GetMouse(DIM_M))
		{
			m_pFSM->ChangeState(NeroFSM::POLE_COMBOB2);
			return S_OK;
		}
	}

	else if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::IDLE);
		return S_OK;
	}

	if (0.83f <= fCurrAnimationTime)
		NeroState::SetCbsIdle();
	else if (0.53f <= fCurrAnimationTime)
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Middle, "Middle_Cbs_PoleComboB1_End", false, {}, true);

	if (0.53f <= fCurrAnimationTime)
	{
		NeroState::KeyInput_Cbs_Idle();
	}
	return S_OK;
}

Pole_ComboB2::Pole_ComboB2(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)

{
}

Pole_ComboB2::~Pole_ComboB2()
{
}

Pole_ComboB2* Pole_ComboB2::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Pole_ComboB2(_pFSM, _nIndex, _pNero);
}

HRESULT Pole_ComboB2::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Pole_ComboB2", false, Nero::ANI_POLE_COMBOB2);
	m_pNero.lock()->CheckAutoRotate();
	return S_OK;
}

HRESULT Pole_ComboB2::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Pole_ComboB2::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.39f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Long);
	else if (0.35f <= fCurrAnimationTime)
		ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Long);
	else if (0.32f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Long);
	else if (0.28f <= fCurrAnimationTime)
		ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Long);
	else if (0.24f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Long);
	else if (0.21f <= fCurrAnimationTime)
		ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Long);
	else if (0.18f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Long);
	else if (0.14f <= fCurrAnimationTime)
		ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Long);

	if (0.48f <= fCurrAnimationTime)
		m_pNero.lock()->SetActive_NeroComponent(Nero::NeroCom_NewWingSword, false);


	if (0.39f <= fCurrAnimationTime && fCurrAnimationTime <= 0.48f)
	{
		if (Input::GetMouse(DIM_M))
		{
			m_pFSM->ChangeState(NeroFSM::POLE_COMBOB3);
			return S_OK;
		}
	}
	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::IDLE);
		return S_OK;
	}

	if (0.76f <= fCurrAnimationTime)
		NeroState::SetCbsIdle();
	else if (0.59f <= fCurrAnimationTime)
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Middle, "Middle_Cbs_PoleComboB2_End", false, {}, true);

	if (0.62f <= fCurrAnimationTime)
	{
		NeroState::KeyInput_Cbs_Idle();
	}

	return S_OK;
}

Pole_ComboB3::Pole_ComboB3(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Pole_ComboB3::~Pole_ComboB3()
{
}

Pole_ComboB3* Pole_ComboB3::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Pole_ComboB3(_pFSM, _nIndex, _pNero);
}

HRESULT Pole_ComboB3::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Pole_ComboB3", false, Nero::ANI_POLE_COMBOB3);
	m_pNero.lock()->CheckAutoRotate();
	return S_OK;
}

HRESULT Pole_ComboB3::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Pole_ComboB3::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.42f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Long);
	else if (0.38f <= fCurrAnimationTime)
		ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Long);
	else if (0.34f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Long);
	else if (0.31f <= fCurrAnimationTime)
		ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Long);
	else if (0.26f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Long);
	else if (0.23f <= fCurrAnimationTime)
		ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Long);
	else if (0.2f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Long);
	else if (0.16f <= fCurrAnimationTime)
		ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Long);

	if (0.53f <= fCurrAnimationTime)
		m_pNero.lock()->SetActive_NeroComponent(Nero::NeroCom_NewWingSword, false);


	if (0.44f <= fCurrAnimationTime && fCurrAnimationTime <= 0.53f)
	{
		if (Input::GetMouse(DIM_M))
		{
			m_pFSM->ChangeState(NeroFSM::POLE_COMBOB4);
			return S_OK;
		}
	}

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::IDLE);
		return S_OK;
	}

	if (0.83 <= fCurrAnimationTime)
		NeroState::SetCbsIdle();
	else if (0.53 <= fCurrAnimationTime)
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Middle, "Middle_Cbs_PoleComboB3_End", false, {}, true);

	if (0.53f <= fCurrAnimationTime)
	{
		NeroState::KeyInput_Cbs_Idle();
	}

	return S_OK;
}

Pole_ComboB4::Pole_ComboB4(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Pole_ComboB4::~Pole_ComboB4()
{
}

Pole_ComboB4* Pole_ComboB4::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Pole_ComboB4(_pFSM, _nIndex, _pNero);
}

HRESULT Pole_ComboB4::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Pole_ComboB4", false, Nero::ANI_POLE_COMBOB4);
	m_pNero.lock()->CheckAutoRotate();
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeNewSword(WingSword_ComboB2_End, false);
	}
	return S_OK;
}

HRESULT Pole_ComboB4::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Pole_ComboB4::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.31f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Long);
	else if (0.24f <= fCurrAnimationTime)
	{
		m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_Cbs_Long, ATTACKTYPE::Attack_KnocBack);
		ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Long);
	}
	else if (0.17f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Long);
	else if (0.15f <= fCurrAnimationTime)
		ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Long);
	else if (0.12f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Long);
	else if (0.08f <= fCurrAnimationTime)
		ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Long);

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::IDLE);
		return S_OK;
	}

	if (0.81f <= fCurrAnimationTime)
		NeroState::SetCbsIdle();
	else if (0.55f <= fCurrAnimationTime)
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Middle, "Middle_Cbs_PoleComboB4_End", false, {}, true);

	if (0.48f <= fCurrAnimationTime)
	{
		NeroState::KeyInput_Cbs_Idle();
	}
	return S_OK;
}

Pole_WhirlWind_Start::Pole_WhirlWind_Start(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Pole_WhirlWind_Start::~Pole_WhirlWind_Start()
{
}

Pole_WhirlWind_Start* Pole_WhirlWind_Start::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Pole_WhirlWind_Start(_pFSM, _nIndex, _pNero);
}

HRESULT Pole_WhirlWind_Start::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Pole_WhirlWind_Start", false, Nero::ANI_POLE_WHIRLWIND_START);
	m_pNero.lock()->ChangeWeapon(Nero::NeroCom_Cbs_Long);

	ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Long);
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeNewSword(WingSword_Ar3, false);
		for (int i = 0; i < 3; ++i)
		{
			m_pPlayOnce[i] = true;
		}
	}
	return S_OK;
}

HRESULT Pole_WhirlWind_Start::StateExit()
{
	NeroState::StateExit();
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeNewSword(WingSword_Ar3, false);
	}
	return S_OK;
}

HRESULT Pole_WhirlWind_Start::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.74f <= fCurrAnimationTime)
	{
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Long);
		if (m_pPlayOnce[2])
		{
			m_pPlayOnce[2] = false;
			m_pNero.lock()->ChangeNewSword(WingSword_Ar4, false);
		}
	}
	else if (0.56f <= fCurrAnimationTime)
	{
		ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Long);
		if (m_pPlayOnce[1])
		{
			m_pPlayOnce[1] = false;
			m_pNero.lock()->ChangeNewSword(WingSword_Ar3, false);
		}
	}
	else if (0.41f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Long);
	else if (0.32f <= fCurrAnimationTime)
	{
		ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Long);
		if (m_pPlayOnce[0])
		{
			m_pPlayOnce[0] = false;
			m_pNero.lock()->ChangeNewSword(WingSword_Ar4, false);
		}
	}

	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::POLE_WHIRLWIND_LOOP);
		return S_OK;
	}

	return S_OK;
}

Pole_WhirlWind_Loop::Pole_WhirlWind_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
	,m_iLoopCount(0)
	, m_fLoopTime(0.f)
{
}

Pole_WhirlWind_Loop::~Pole_WhirlWind_Loop()
{
}

Pole_WhirlWind_Loop* Pole_WhirlWind_Loop::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Pole_WhirlWind_Loop(_pFSM, _nIndex, _pNero);
}

HRESULT Pole_WhirlWind_Loop::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Pole_WhirlWind_Loop", true, Nero::ANI_POLE_WHIRLWIND_LOOP);
	m_fLoopTime = 1.f;
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeNewSword(WingSword_Ar4, false, false);
	}
	return S_OK;
}

HRESULT Pole_WhirlWind_Loop::StateExit()
{
	NeroState::StateExit();
	ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Long);
	return S_OK;
}

HRESULT Pole_WhirlWind_Loop::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	m_fLoopTime -= _fDeltaTime;

	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.67f <= fCurrAnimationTime)
	{
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Long);
		if (m_pNero.lock()->Get_IsMajinMode())
		{
			m_pNero.lock()->ChangeNewSword(WingSword_Ar4, false, true);
		}
	}
	else if (0.26f <= fCurrAnimationTime)
	{
		ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Long);
		if (m_pNero.lock()->Get_IsMajinMode())
		{
			m_pNero.lock()->ChangeNewSword(WingSword_Ar3, false, true);
		}
	}

	if (m_fLoopTime < 0.f)
	{
		m_pFSM->ChangeState(NeroFSM::POLE_WHIRLWIND_END);
		return S_OK;
	}
	return S_OK;
}

Pole_WhirlWind_End::Pole_WhirlWind_End(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Pole_WhirlWind_End::~Pole_WhirlWind_End()
{
}

Pole_WhirlWind_End* Pole_WhirlWind_End::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Pole_WhirlWind_End(_pFSM, _nIndex, _pNero);
}

HRESULT Pole_WhirlWind_End::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Pole_WhirlWind_End", false, Nero::ANI_POLE_WHIRLWIND_END);
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeNewSword(WingSword_HT, false);
	}
	return S_OK;
}

HRESULT Pole_WhirlWind_End::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Pole_WhirlWind_End::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.25f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Long);
	else if (0.21f <= fCurrAnimationTime)
	{
		m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_Cbs_Long, ATTACKTYPE::Attack_KnocBack);
		ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Long);
	}
	else if (0.12f <= fCurrAnimationTime)
		ActiveColl_Cbs(false, Nero::NeroCom_Cbs_Long);
	else if (0.08f <= fCurrAnimationTime)
		ActiveColl_Cbs(true, Nero::NeroCom_Cbs_Long);

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::IDLE);
		return S_OK;
	}

	if (0.8f <= fCurrAnimationTime)
		NeroState::SetCbsIdle();
	else if (0.6f <= fCurrAnimationTime)
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_Cbs_Middle, "Middle_Cbs_Flame_WhirlWInd_Club_Finish", false, {}, true);

	return S_OK;
}


#pragma endregion

ComboA_Dash::ComboA_Dash(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{

}

ComboA_Dash::~ComboA_Dash()
{

}

ComboA_Dash* ComboA_Dash::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new ComboA_Dash(_pFSM, _nIndex, _pNero);
}

HRESULT ComboA_Dash::StateEnter()
{
	m_iNeroCurWeaponIndex = Nero::NeroCom_RedQueen;
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("ComboA1_Dash", false, Nero::ANI_COMBOA1_DASH);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_Front);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"ComboA1", false);
	}
	m_bActiveColl_RedQueen = false;
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT ComboA_Dash::StateExit()
{
	NeroState::StateExit();
	ActiveColl_RedQueen(false);
	return S_OK;
}

HRESULT ComboA_Dash::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.2f <= fCurrAnimationTime)
	{
		ActiveColl_RedQueen(false);
		NeroState::ActiveTrail(false);
	}
	else if (0.13f <= fCurrAnimationTime)
		ActiveColl_RedQueen(true);


	if (0.18f <= fCurrAnimationTime && fCurrAnimationTime <= 0.35f)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			KeyInput_Idle(NeroFSM::ATT2);
		else
			KeyInput_Cbs_Idle();
	}
	else if (0.37f <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::IDLE_START);
		return S_OK;
	}

	return S_OK;
}

Skill_Caliber::Skill_Caliber(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{

}

Skill_Caliber::~Skill_Caliber()
{

}

Skill_Caliber* Skill_Caliber::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Skill_Caliber(_pFSM, _nIndex, _pNero);
}

HRESULT Skill_Caliber::StateEnter()
{
	m_iNeroCurWeaponIndex = Nero::NeroCom_RedQueen;
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Caliber_Start", false, Nero::ANI_CALIBER_START);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_KnocBack);
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"Streak_Start", false);
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WingArm_Right,"Streak_Start", false);
	}
	NeroState::ActiveGravity(false);
	//m_pNero.lock()->SetAddForce_Dir(Nero::Dir_Front, 3.f);
	return S_OK;
}

HRESULT Skill_Caliber::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Skill_Caliber::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	m_pNero.lock()->NeroMove(Nero::Dir_Front, 0.01f);

	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::SKILL_CALIBER_END);
		return S_OK;
	}
	return S_OK;
}

Skill_Caliber_End::Skill_Caliber_End(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{

}

Skill_Caliber_End::~Skill_Caliber_End()
{

}

Skill_Caliber_End* Skill_Caliber_End::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{

	return new Skill_Caliber_End(_pFSM, _nIndex, _pNero);
}

HRESULT Skill_Caliber_End::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Caliber_End", false, Nero::ANI_CALIBER_END);
	m_bActiveColl_RedQueen = false;
	NeroState::ActiveColl_RedQueen(true);
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"Streak_End", false);
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WingArm_Right,"Streak_End", false);
	}
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT Skill_Caliber_End::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Skill_Caliber_End::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.19f <= fCurAnimationTime)
	{
		NeroState::ActiveColl_RedQueen(false);
	}

	if (0.33 <= fCurAnimationTime)
	{
		NeroState::ActiveGravity(true);
		NeroState::ActiveTrail(false);
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Jump();
		else
			NeroState::KeyInput_Cbs_Jump();
	}

	if (0.8f <= fCurAnimationTime)
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen,Nero::WS_Idle);

	if (m_pNero.lock()->IsAnimationEnd() || 0.9 <= fCurAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
		return S_OK;
	}
	NeroState::IsGround();
	return S_OK;
}

Hr_Air::Hr_Air(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{

}

Hr_Air::~Hr_Air()
{

}

Hr_Air* Hr_Air::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Hr_Air(_pFSM, _nIndex, _pNero);
}

HRESULT Hr_Air::StateEnter()
{
	m_iNeroCurWeaponIndex = Nero::NeroCom_RedQueen;
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Hr_Air", false, Nero::ANI_HR_AIR);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_Air_Start);
	m_bActiveColl_RedQueen = false;
	ActiveColl_RedQueen(true);
	m_pNero.lock()->SetAddForce({ 0.f,120.f,0.f });
	m_pNero.lock()->SetFly(true);
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT Hr_Air::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Hr_Air::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.5f <= fCurAnimationTime)
	{
		ActiveColl_RedQueen(false);
		NeroState::ActiveTrail(false);
	}

	if (0.83f <= fCurAnimationTime)
	{
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Jump(NeroFSM::SKILL_HR_AIR);
		else
			NeroState::KeyInput_Cbs_Jump();
	}

	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
		return S_OK;
	}
	return S_OK;
}

Hr_Ex_Start::Hr_Ex_Start(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{

}

Hr_Ex_Start::~Hr_Ex_Start()
{

}

Hr_Ex_Start* Hr_Ex_Start::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Hr_Ex_Start(_pFSM, _nIndex, _pNero);
}

HRESULT Hr_Ex_Start::StateEnter()
{
	m_iNeroCurWeaponIndex = Nero::NeroCom_RedQueen;
	NeroState::StateEnter();

	m_pNero.lock()->ChangeAnimation("Hr_Ex_Start", false, Nero::ANI_HR_EX_START);

	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"Hr_Ground", false);
	}
	NeroState::ActiveTrail(true);
	return S_OK;
}

HRESULT Hr_Ex_Start::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Hr_Ex_Start::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();
	UINT Ex_Gauge = m_pNero.lock()->Get_ExGaugeCount();

	if (0 < Ex_Gauge)
	{
		//게이지 있을때	
		if (m_pNero.lock()->IsAnimationEnd())
		{
			m_pFSM->ChangeState(NeroFSM::SKILL_HR_EX_AIR_ROLL_START);
			return S_OK;
		}
	}
	else
	{
		//게이지 없을때
		if (m_pNero.lock()->IsAnimationEnd())
		{
			m_pFSM->ChangeState(NeroFSM::SKILL_HR_AIR);
			return S_OK;
		}
	}

	return S_OK;
}

Hr_Ex_Finish::Hr_Ex_Finish(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{

}

Hr_Ex_Finish::~Hr_Ex_Finish()
{

}

Hr_Ex_Finish* Hr_Ex_Finish::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Hr_Ex_Finish(_pFSM, _nIndex, _pNero);
}

HRESULT Hr_Ex_Finish::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Hr_Ex_Finish", false, Nero::ANI_HR_EX_FINISH);

	return S_OK;
}

HRESULT Hr_Ex_Finish::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Hr_Ex_Finish::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}
	return S_OK;
}

Hr_Ex_Air_Roll_Start::Hr_Ex_Air_Roll_Start(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{

}

Hr_Ex_Air_Roll_Start::~Hr_Ex_Air_Roll_Start()
{

}

Hr_Ex_Air_Roll_Start* Hr_Ex_Air_Roll_Start::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Hr_Ex_Air_Roll_Start(_pFSM, _nIndex, _pNero);
}

HRESULT Hr_Ex_Air_Roll_Start::StateEnter()
{
	m_iNeroCurWeaponIndex = Nero::NeroCom_RedQueen;
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Hr_Ex_Air_Roll_Start", false, Nero::ANI_HR_EX_AIR_ROLL_START);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_Air_Start);
	
	m_bActiveColl_RedQueen = false;
	m_pNero.lock()->SetFly(true);
	ActiveColl_RedQueen(true);
	NeroState::ActiveTrail(true);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
	m_pNero.lock()->Use_ExGauge(1);
	//m_pNero.lock()->PlayEffect(Eff_FireCircle, { -0.1f,0.f,0.f }, 0, 5, 0.7f);
	//나중에 생길 불꽃이펙트 여기서
	return S_OK;
}

HRESULT Hr_Ex_Air_Roll_Start::StateExit()
{
	NeroState::StateExit();

	//ActiveColl_RedQueen(false);
	m_pNero.lock()->SetAddForce({ 0.f,190.f,0.f });
	return S_OK;
}

HRESULT Hr_Ex_Air_Roll_Start::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::SKILL_HR_EX_AIR_ROLL_LOOP);
		return S_OK;
	}
	return S_OK;
}

Hr_Ex_Air_Roll_Loop::Hr_Ex_Air_Roll_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{

}

Hr_Ex_Air_Roll_Loop::~Hr_Ex_Air_Roll_Loop()
{

}

Hr_Ex_Air_Roll_Loop* Hr_Ex_Air_Roll_Loop::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Hr_Ex_Air_Roll_Loop(_pFSM, _nIndex, _pNero);
}

HRESULT Hr_Ex_Air_Roll_Loop::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Hr_Ex_Air_Roll_Loop", true, Nero::ANI_HR_EX_AIR_ROLL_LOOP);
	
	m_bPlayOnce = true;
	m_bPlayOnce2 = true;
	return S_OK;
}

HRESULT Hr_Ex_Air_Roll_Loop::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Hr_Ex_Air_Roll_Loop::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAccTime = m_pNero.lock()->Get_PlayingAccTime();

	if (0.6f <= fCurrAccTime && m_bPlayOnce)
	{
		m_bPlayOnce = false;
		m_pNero.lock()->PlayEffect(Eff_FireCircle, { 0.f,-0.4f,-0.4f }, 0, 5, 1.3f, { 0.003f,0.003f,0.003f });
	}
	if (2.f <= fCurrAccTime && m_bPlayOnce2)
	{
		m_bPlayOnce2 = false;
		m_pNero.lock()->PlayEffect(Eff_FireCircle, { 0.f,-0.4f,-0.55f }, 0, 3, 1.1f, { 0.003f,0.003f,0.003f });
	}

	if (2.96f <= fCurrAccTime)
	{
		m_pFSM->ChangeState(NeroFSM::SKILL_HR_EX_AIR_ROLL_END);
		return S_OK;
	}


	return S_OK;
}

Hr_Ex_Air_Roll_End::Hr_Ex_Air_Roll_End(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{

}

Hr_Ex_Air_Roll_End::~Hr_Ex_Air_Roll_End()
{

}

Hr_Ex_Air_Roll_End* Hr_Ex_Air_Roll_End::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Hr_Ex_Air_Roll_End(_pFSM, _nIndex, _pNero);
}

HRESULT Hr_Ex_Air_Roll_End::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Hr_Ex_Air_Roll_End", false, Nero::ANI_HR_EX_AIR_ROLL_END);
	m_bActiveColl_RedQueen = false;
	ActiveColl_RedQueen(true);
	m_pNero.lock()->PlayEffect(Eff_FireCircle, {0.f ,-0.4f,-0.75f }, 0, 1, 0.7f);

	return S_OK;
}

HRESULT Hr_Ex_Air_Roll_End::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Hr_Ex_Air_Roll_End::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.5f <= fCurAnimationTime)
	{
		NeroState::ActiveTrail(false);
		ActiveColl_RedQueen(false);
	}

	if (0.83f <= fCurAnimationTime)
	{
		m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			KeyInput_Jump(NeroFSM::SKILL_HR_EX_AIR_ROLL_END);
		else
			KeyInput_Cbs_Jump();
	}


	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
		return S_OK;
	}
	return S_OK;
}

Skill_Split_Ex::Skill_Split_Ex(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{

}

Skill_Split_Ex::~Skill_Split_Ex()
{

}

Skill_Split_Ex* Skill_Split_Ex::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Skill_Split_Ex(_pFSM, _nIndex, _pNero);
}

HRESULT Skill_Split_Ex::StateEnter()
{
	m_iNeroCurWeaponIndex = Nero::NeroCom_RedQueen;
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("SplitEx_Start", false, Nero::ANI_SPLITEX_START);
	
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_Split);
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"Split_Start", false);
	}
	m_bActiveColl_RedQueen = false;

	NeroState::ActiveGravity(false);
	NeroState::ActiveTrail(true);
	m_pNero.lock()->Use_ExGauge(1);
	return S_OK;
}

HRESULT Skill_Split_Ex::StateExit()
{
	NeroState::StateExit();
	ActiveColl_RedQueen(true);
	m_pNero.lock()->SetAddForce({ 0.f,-250.f,0.f });

	return S_OK;
}

HRESULT Skill_Split_Ex::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::SKILL_SPLIT_EX_LOOP);
		return S_OK;
	}
	return S_OK;
}

Skill_Split_Ex_Loop::Skill_Split_Ex_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
	,m_fLoopTime(0.f)
{

}

Skill_Split_Ex_Loop::~Skill_Split_Ex_Loop()
{

}

Skill_Split_Ex_Loop* Skill_Split_Ex_Loop::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Skill_Split_Ex_Loop(_pFSM, _nIndex, _pNero);
}

HRESULT Skill_Split_Ex_Loop::StateEnter()
{
	m_pNero.lock()->ChangeAnimation("SplitEx_Loop", true, Nero::ANI_SPLITEX_LOOP);

	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"Split_Loop", true);
	}

	return S_OK;
}

HRESULT Skill_Split_Ex_Loop::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Skill_Split_Ex_Loop::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	//땅에 닿았을때
	if (m_pNero.lock()->CheckIsGround())
	{
		m_pFSM->ChangeState(NeroFSM::SKILL_SPLIT_EX_END);
		return S_OK;
	}

	//테스트
	if (Input::GetKey(DIK_M))
	{
		m_pFSM->ChangeState(NeroFSM::SKILL_SPLIT_EX_END);
		return S_OK;
	}
	return S_OK;
}

Skill_Split_Ex_Landing::Skill_Split_Ex_Landing(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{

}

Skill_Split_Ex_Landing::~Skill_Split_Ex_Landing()
{

}

Skill_Split_Ex_Landing* Skill_Split_Ex_Landing::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Skill_Split_Ex_Landing(_pFSM, _nIndex, _pNero);
}

HRESULT Skill_Split_Ex_Landing::StateEnter()
{
	m_pNero.lock()->ChangeAnimation("SplitEx_Landing", false, Nero::ANI_SPLITEX_LANDING);

	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"Split_End", false);
	}
	m_bActiveColl_RedQueen = true;

	NeroState::ActiveGravity(true);
	return S_OK;
}

HRESULT Skill_Split_Ex_Landing::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Skill_Split_Ex_Landing::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.16f <= fCurAnimationTime)
	{
		ActiveColl_RedQueen(false);
		NeroState::ActiveTrail(false);
	}

	if (0.37f <= fCurAnimationTime)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Idle(NeroFSM::SKILL_SPLIT_EX_END);
		else
			NeroState::KeyInput_Cbs_Idle();
	}

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}
	return S_OK;
}

Air_Dive_Slash_Start::Air_Dive_Slash_Start(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{

}

Air_Dive_Slash_Start::~Air_Dive_Slash_Start()
{

}

Air_Dive_Slash_Start* Air_Dive_Slash_Start::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Air_Dive_Slash_Start(_pFSM, _nIndex, _pNero);
}

HRESULT Air_Dive_Slash_Start::StateEnter()
{
	m_iNeroCurWeaponIndex = Nero::NeroCom_RedQueen;
	NeroState::StateEnter();
	m_bActiveColl_RedQueen = false;
	m_pNero.lock()->ChangeAnimation("Air_Dive_Slash_Start", false, Nero::ANI_AIR_DIVE_SLASH_START);
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_KnocBack);
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"Streak_Start", false);
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WingArm_Right,"Streak_Start", false);
	}
	NeroState::ActiveTrail(true);
	NeroState::ActiveGravity(false);
	return S_OK;
}

HRESULT Air_Dive_Slash_Start::StateExit()
{
	NeroState::StateExit();

	ActiveColl_RedQueen(true);
	m_pNero.lock()->SetAddForce_Dir(Nero::Dir_Front_Down, 500.f);

	return S_OK;
}

HRESULT Air_Dive_Slash_Start::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::SKILL_AIR_DIVE_SLASH_LOOP);
		return S_OK;
	}
	return S_OK;
}

Air_Dive_Slash_Loop::Air_Dive_Slash_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{

}

Air_Dive_Slash_Loop::~Air_Dive_Slash_Loop()
{

}

Air_Dive_Slash_Loop* Air_Dive_Slash_Loop::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Air_Dive_Slash_Loop(_pFSM, _nIndex, _pNero);
}

HRESULT Air_Dive_Slash_Loop::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->StopAnimation();

	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"Streak_Loop", true);
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WingArm_Right,"Streak_Loop", true);
	}

	return S_OK;
}

HRESULT Air_Dive_Slash_Loop::StateExit()
{
	NeroState::StateExit();
	m_pNero.lock()->ContinueAnimiation();

	return S_OK;
}

HRESULT Air_Dive_Slash_Loop::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	// 땅에 닿았을때
	if (m_pNero.lock()->CheckIsGround())
	{
		m_pFSM->ChangeState(NeroFSM::SKILL_AIR_DIVE_SLASH_END);
		return S_OK;
	}

	//테스트용

	if (Input::GetKey(DIK_M))
	{
		m_pFSM->ChangeState(NeroFSM::SKILL_AIR_DIVE_SLASH_END);
		return S_OK;
	}

	return S_OK;
}

Air_Dive_Slash_End::Air_Dive_Slash_End(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{

}

Air_Dive_Slash_End::~Air_Dive_Slash_End()
{

}

Air_Dive_Slash_End* Air_Dive_Slash_End::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Air_Dive_Slash_End(_pFSM, _nIndex, _pNero);
}

HRESULT Air_Dive_Slash_End::StateEnter()
{
	NeroState::StateEnter();
	m_bActiveColl_RedQueen = true;
	m_pNero.lock()->ChangeAnimation("Air_Dive_Slash_Finish", false, Nero::ANI_AIR_DIVE_SLASH_FINISH);
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"Streak_End", false);
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WingArm_Right,"Streak_End", false);
	}
	m_pNero.lock()->SetLinearVelocity();
	NeroState::ActiveGravity(true);
	return S_OK;
}

HRESULT Air_Dive_Slash_End::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Air_Dive_Slash_End::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.16f <= fCurAnimationTime)
	{
		ActiveColl_RedQueen(false);
		NeroState::ActiveTrail(false);
	}

	if (0.53f <= fCurAnimationTime)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Idle(NeroFSM::SKILL_AIR_DIVE_SLASH_END);
		else
			NeroState::KeyInput_Cbs_Idle();
	}


	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}
	return S_OK;
}

Skill_Shuffle_Ex::Skill_Shuffle_Ex(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM,_nIndex,_pNero)
{
}

Skill_Shuffle_Ex::~Skill_Shuffle_Ex()
{
}

Skill_Shuffle_Ex* Skill_Shuffle_Ex::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Skill_Shuffle_Ex(_pFSM,_nIndex,_pNero);
}

HRESULT Skill_Shuffle_Ex::StateEnter()
{
	m_iNeroCurWeaponIndex = Nero::NeroCom_RedQueen;
	NeroState::StateEnter();

	m_pNero.lock()->ChangeAnimation("Shuffle_Ex", false, Nero::ANI_SHUFFLE_EX);
	
	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_KnocBack);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Battle);
	if (m_pNero.lock()->Get_IsMajinMode())
	{
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_WIngArm_Left,"Shuffle_Ex", false);
	}
	NeroState::ActiveTrail(true);
	m_pNero.lock()->Use_ExGauge(1);
	m_bPlayOnce = true;
	m_bPlayOnce2 = true;
	return S_OK;
}

HRESULT Skill_Shuffle_Ex::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Skill_Shuffle_Ex::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.44f <= fCurrAnimationTime)
	{
		ActiveColl_RedQueen(false);
	}
	else if (0.4f <= fCurrAnimationTime)
	{
		ActiveColl_RedQueen(true);
		if (m_bPlayOnce2)
		{
			m_bPlayOnce2 = false;
			m_pNero.lock()->PlayEffect(Eff_FireCircle, Vector3{ 0.f,0.3f,0.65f }, 0, 1, 0.7f, {0.004f,0.004f,0.004f});
		}
	}
	else if (0.28f <= fCurrAnimationTime)
		ActiveColl_RedQueen(false);
	else if (0.2f <= fCurrAnimationTime)
	{
		ActiveColl_RedQueen(true);
		if (m_bPlayOnce)
		{
			m_bPlayOnce = false;
			m_pNero.lock()->PlayEffect(Eff_FireCircle, Vector3{ 0.f,0.3f,0.45f }, 0, 3, 0.4f, { 0.0033f,0.0033f,0.0033f });
		}
	}
	m_pNero.lock()->SetPosFireCircle();
	if(0.53 <= fCurrAnimationTime)
		NeroState::ActiveTrail(false);


	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}
	return S_OK;
}

Skill_Float_Ground_Ex3_Start::Skill_Float_Ground_Ex3_Start(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM,_nIndex,_pNero)
{
}

Skill_Float_Ground_Ex3_Start::~Skill_Float_Ground_Ex3_Start()
{
}

Skill_Float_Ground_Ex3_Start* Skill_Float_Ground_Ex3_Start::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Skill_Float_Ground_Ex3_Start(_pFSM,_nIndex,_pNero);
}

HRESULT Skill_Float_Ground_Ex3_Start::StateEnter()
{
	m_iNeroCurWeaponIndex = Nero::NeroCom_RedQueen;
	NeroState::StateEnter();


	m_pNero.lock()->ChangeAnimation("Hr_Ex_Start", false, Nero::ANI_HR_EX_2);

	

	m_pNero.lock()->Set_Weapon_AttType(Nero::NeroCom_RedQueen, ATTACKTYPE::Attack_Front);
	NeroState::ActiveTrail(true);
	m_pNero.lock()->Use_ExGauge(1);
	return S_OK;
}

HRESULT Skill_Float_Ground_Ex3_Start::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Skill_Float_Ground_Ex3_Start::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::SKILL_FLOAT_GROUND_EX3);
		return S_OK;
	}
	return S_OK;
}

Buster_Start::Buster_Start(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM,_nIndex,_pNero)
{
}

Buster_Start::~Buster_Start()
{
}

Buster_Start* Buster_Start::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Buster_Start(_pFSM, _nIndex, _pNero);
}

HRESULT Buster_Start::StateEnter()
{
	NeroState::StateEnter();

	m_pNero.lock()->ChangeAnimation("Buster_Start", false, Nero::ANI_BUSTER_START);
	m_pNero.lock()->Set_Weapon_Coll(Nero::NeroCom_BusterArm, true);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_BusterArm,"Buster_Catch", false);
	if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
		NeroState::SetCbsIdle();
	return S_OK;
}

HRESULT Buster_Start::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Buster_Start::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}

	return S_OK;
}

To_Majin::To_Majin(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM,_nIndex,_pNero)
{
}

To_Majin::~To_Majin()
{
}

To_Majin* To_Majin::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new To_Majin(_pFSM,_nIndex,_pNero);
}

HRESULT To_Majin::StateEnter()
{
	NeroState::StateEnter();

	m_pNero.lock()->ChangeAnimation("To_Majin", false, Nero::ANI_BUSTER_START);
	m_pNero.lock()->SetActive_NeroComponent(Nero::NeroCom_Wings, true);
	if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
		NeroState::SetCbsIdle();
	m_pNero.lock()->Change_To_MajinMode();

	return S_OK;
}

HRESULT To_Majin::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT To_Majin::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}

	return S_OK;
}

Buster_Air_Catch::Buster_Air_Catch(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM,_nIndex,_pNero)
{
}

Buster_Air_Catch::~Buster_Air_Catch()
{
}

Buster_Air_Catch* Buster_Air_Catch::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Buster_Air_Catch(_pFSM,_nIndex,_pNero);
}

HRESULT Buster_Air_Catch::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Buster_Air_Catch", false, Nero::ANI_BUSTER_AIR_CATCH);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
	m_pNero.lock()->Set_Weapon_Coll(Nero::NeroCom_BusterArm, true);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_BusterArm, "Buster_Catch", false);
	if (Nero::NeroCom_RedQueen != m_iNeroCurWeaponIndex)
		NeroState::SetCbsIdle();
	NeroState::ActiveGravity(false);
	return S_OK;
}

HRESULT Buster_Air_Catch::StateExit()
{
	NeroState::StateExit();
	NeroState::ActiveGravity(true);
	return S_OK;
}

HRESULT Buster_Air_Catch::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.6f <= fCurAnimationTime)
		NeroState::ActiveGravity(true);

	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
		return S_OK;
	}
	return S_OK;
}

Buster_Strike_Common::Buster_Strike_Common(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM,_nIndex,_pNero)
{
}

Buster_Strike_Common::~Buster_Strike_Common()
{
}

Buster_Strike_Common* Buster_Strike_Common::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Buster_Strike_Common(_pFSM,_nIndex,_pNero);
}

HRESULT Buster_Strike_Common::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Buster_Strike_Common", false, Nero::ANI_BUSTER_STRIKE_COMMON);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_BusterArm, "Buster_Common", false);
	return S_OK;
}

HRESULT Buster_Strike_Common::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Buster_Strike_Common::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}
	return S_OK;
}

Buster_Strike_Common_Air::Buster_Strike_Common_Air(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM,_nIndex,_pNero)
{
}

Buster_Strike_Common_Air::~Buster_Strike_Common_Air()
{
}

Buster_Strike_Common_Air* Buster_Strike_Common_Air::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Buster_Strike_Common_Air(_pFSM, _nIndex, _pNero);
}

HRESULT Buster_Strike_Common_Air::StateEnter()
{
	NeroState::StateEnter();

	m_pNero.lock()->ChangeAnimation("Buster_Strike_Common_Air", false, Nero::ANI_BUSTER_STRIKE_COMMON_AIR);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_BusterArm, "Buster_Common", false);
	NeroState::ActiveGravity(false);
	return S_OK;
}

HRESULT Buster_Strike_Common_Air::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Buster_Strike_Common_Air::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);

	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.85f <= fCurAnimationTime)
	{
		NeroState::ActiveGravity(true);
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Jump();
		else
			NeroState::KeyInput_Cbs_Jump();
	}

	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
		return S_OK;
	}
	NeroState::IsGround();
	return S_OK;
}

M_Buster_Strike_Common_Start::M_Buster_Strike_Common_Start(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

M_Buster_Strike_Common_Start::~M_Buster_Strike_Common_Start()
{
}

M_Buster_Strike_Common_Start* M_Buster_Strike_Common_Start::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new M_Buster_Strike_Common_Start(_pFSM, _nIndex, _pNero);
}

HRESULT M_Buster_Strike_Common_Start::StateEnter()
{
	NeroState::StateEnter();
	return S_OK;
}

HRESULT M_Buster_Strike_Common_Start::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT M_Buster_Strike_Common_Start::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	return S_OK;
}

M_Buster_Strike_Common_Loop::M_Buster_Strike_Common_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

M_Buster_Strike_Common_Loop::~M_Buster_Strike_Common_Loop()
{
}

M_Buster_Strike_Common_Loop* M_Buster_Strike_Common_Loop::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new M_Buster_Strike_Common_Loop(_pFSM, _nIndex, _pNero);
}

HRESULT M_Buster_Strike_Common_Loop::StateEnter()
{
	NeroState::StateEnter();
	return S_OK;
}

HRESULT M_Buster_Strike_Common_Loop::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT M_Buster_Strike_Common_Loop::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	return S_OK;
}

em0000_Buster_Start::em0000_Buster_Start(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

em0000_Buster_Start::~em0000_Buster_Start()
{
}

em0000_Buster_Start* em0000_Buster_Start::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new em0000_Buster_Start(_pFSM, _nIndex, _pNero);
}

HRESULT em0000_Buster_Start::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("em0000_Buster_Start", false,Nero::ANI_EM0000_BUSTER_START);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_BusterArm, "em0000_Buster_Start", false);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_BusterArm_Left, "em0000_Buster_Start", false);
	return S_OK;
}

HRESULT em0000_Buster_Start::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT em0000_Buster_Start::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::EM0000_BUSTER_FINISH);
		return S_OK;
	}
	return S_OK;
}

em0000_Buster_Finish::em0000_Buster_Finish(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

em0000_Buster_Finish::~em0000_Buster_Finish()
{
}

em0000_Buster_Finish* em0000_Buster_Finish::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new em0000_Buster_Finish(_pFSM, _nIndex, _pNero);
}

HRESULT em0000_Buster_Finish::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("em0000_Buster_End", false, Nero::ANI_EM0000_BUSTER_FINISH);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_BusterArm, "em0000_Buster_End", false);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_BusterArm_Left, "em0000_Buster_End", false);
	return S_OK;
}

HRESULT em0000_Buster_Finish::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT em0000_Buster_Finish::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}
	return S_OK;
}

em0000_M_Buster::em0000_M_Buster(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

em0000_M_Buster::~em0000_M_Buster()
{
}

em0000_M_Buster* em0000_M_Buster::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new em0000_M_Buster(_pFSM, _nIndex, _pNero);
}

HRESULT em0000_M_Buster::StateEnter()
{
	NeroState::StateEnter();
	return S_OK;
}

HRESULT em0000_M_Buster::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT em0000_M_Buster::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	return S_OK;
}

em0000_Buster_Air::em0000_Buster_Air(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

em0000_Buster_Air::~em0000_Buster_Air()
{
}

em0000_Buster_Air* em0000_Buster_Air::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new em0000_Buster_Air(_pFSM, _nIndex, _pNero);
}

HRESULT em0000_Buster_Air::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("em0000_Buster_Air", false, Nero::ANI_EM0000_BUSTER_AIR);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_BusterArm, "em0000_Buster_Air", false);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_BusterArm_Left, "em0000_Buster_Air", false);
	NeroState::ActiveGravity(false);
	return S_OK;
}

HRESULT em0000_Buster_Air::StateExit()
{
	NeroState::StateExit();
	NeroState::ActiveGravity(true);
	return S_OK;
}

HRESULT em0000_Buster_Air::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);

	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.85f <= fCurAnimationTime)
	{
		NeroState::ActiveGravity(true);
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			NeroState::KeyInput_Jump();
		else
			NeroState::KeyInput_Cbs_Jump();
	}

	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
		return S_OK;
	}
	NeroState::IsGround();

	return S_OK;
}

em5000_Buster_Start::em5000_Buster_Start(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

em5000_Buster_Start::~em5000_Buster_Start()
{
}

em5000_Buster_Start* em5000_Buster_Start::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new em5000_Buster_Start(_pFSM, _nIndex, _pNero);
}

HRESULT em5000_Buster_Start::StateEnter()
{

	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("em5000_Buster_Start", false, Nero::ANI_EM5000_BUSTER_START);
	m_pNero.lock()->Set_PlayingTime(0.25f);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_BusterArm, "em5000_Buster_Start", false);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_BusterArm_Left, "em5000_Buster_Start", false);
	return S_OK;
}

HRESULT em5000_Buster_Start::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT em5000_Buster_Start::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::EM5000_BUSTER_SWING);
		return S_OK;
	}
	return S_OK;
}

em5000_Buster_Swing::em5000_Buster_Swing(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

em5000_Buster_Swing::~em5000_Buster_Swing()
{
}

em5000_Buster_Swing* em5000_Buster_Swing::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new em5000_Buster_Swing(_pFSM, _nIndex, _pNero);
}

HRESULT em5000_Buster_Swing::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("em5000_Buster_Swing", false, Nero::ANI_EM5000_BUSTER_SWING);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_BusterArm, "em5000_Buster_Swing", false);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_BusterArm_Left, "em5000_Buster_Swing", false);
	return S_OK;
}

HRESULT em5000_Buster_Swing::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT em5000_Buster_Swing::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);

	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::EM5000_BUSTER_SWING_LOOP);
		return S_OK;
	}
	return S_OK;
}

em5000_Buster_Swing_Loop::em5000_Buster_Swing_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

em5000_Buster_Swing_Loop::~em5000_Buster_Swing_Loop()
{
}

em5000_Buster_Swing_Loop* em5000_Buster_Swing_Loop::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new em5000_Buster_Swing_Loop(_pFSM, _nIndex, _pNero);
}

HRESULT em5000_Buster_Swing_Loop::StateEnter()
{
	NeroState::StateEnter();
	//m_pNero.lock()->ChangeAnimation("em5000_Buster_Swing_Loop", true, Nero::ANI_EM5000_BUSTER_SWING_LOOP);
	m_pNero.lock()->ChangeAnimation("em5000_Buster_Swing_Loop", false, Nero::ANI_EM5000_BUSTER_SWING_LOOP);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_BusterArm, "em5000_Buster_Swing_Loop", true);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_BusterArm_Left, "em5000_Buster_Swing_Loop", true);
	return S_OK;
}

HRESULT em5000_Buster_Swing_Loop::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT em5000_Buster_Swing_Loop::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);

	float fAccAnimationTime = m_pNero.lock()->Get_PlayingAccTime();

	//if (2.97 <= fAccAnimationTime)
	//{
	//	m_pFSM->ChangeState(NeroFSM::EM5000_BUSTER_FINISH);
	//	return S_OK;
	//}
	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::EM5000_BUSTER_FINISH);
		return S_OK;
	}
	return S_OK;
}

em5000_Buster_Finish::em5000_Buster_Finish(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

em5000_Buster_Finish::~em5000_Buster_Finish()
{
}

em5000_Buster_Finish* em5000_Buster_Finish::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new em5000_Buster_Finish(_pFSM, _nIndex, _pNero);
}

HRESULT em5000_Buster_Finish::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("em5000_Buster_Finish", false, Nero::ANI_EM5000_BUSTER_FINISH);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_BusterArm, "em5000_Buster_End", false);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_BusterArm_Left, "em5000_Buster_End", false);
	return S_OK;
}

HRESULT em5000_Buster_Finish::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT em5000_Buster_Finish::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);

	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.15f <= fCurAnimationTime)
	{
		//요기가 날리는 타이밍임 
	}

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}
	return S_OK;
}

Jog_Loop::Jog_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM,_nIndex,_pNero)
{
}

Jog_Loop::~Jog_Loop()
{
}

Jog_Loop* Jog_Loop::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Jog_Loop(_pFSM,_nIndex,_pNero);
}

HRESULT Jog_Loop::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Jog_Loop", true, Nero::ANI_JOG_LOOP);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_BusterArm, Nero::WS_Idle);
	return S_OK;
}

HRESULT Jog_Loop::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Jog_Loop::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	//m_pNero.lock()->SetAngleFromCamera();
	//m_pNero.lock()->IncreaseDistance(MaxDistance, _fDeltaTime);
	//if (Input::GetKey(DIK_W) || Input::GetKey(DIK_S) || Input::GetKey(DIK_A) || Input::GetKey(DIK_D))
	//{
	//	//키입력이 특정 시간이 넘었다
	//	//-> 그러면 대쉬 루프로 변환하고
	//	//칼들고 달리고있었으면 칼 집어넣고
	//	KeyInput_Run(NeroFSM::RUNLOOP);
	//}
	//else
	//{
	//	//그냥 달리는거면 이거고
	//	m_pFSM->ChangeState(NeroFSM::RUNSTOP);
	//	//칼들고 달리는거면 멈추면서 칼만 넣는거

	//}
	//return S_OK;
	return S_OK;
}

Jog_Stop::Jog_Stop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM,_nIndex,_pNero)
{
}

Jog_Stop::~Jog_Stop()
{
}

Jog_Stop* Jog_Stop::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Jog_Stop(_pFSM,_nIndex,_pNero);
}

HRESULT Jog_Stop::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Jog_Stop", false, Nero::ANI_JOG_STOP);
	return S_OK;
}

HRESULT Jog_Stop::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Jog_Stop::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	return S_OK;
}

Jog_Turn_180::Jog_Turn_180(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM,_nIndex,_pNero)
{
}

Jog_Turn_180::~Jog_Turn_180()
{
}

Jog_Turn_180* Jog_Turn_180::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Jog_Turn_180(_pFSM,_nIndex,_pNero);
}

HRESULT Jog_Turn_180::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Jog_Turn_180", false, Nero::ANI_JOG_TURN_180);
	return S_OK;
}

HRESULT Jog_Turn_180::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Jog_Turn_180::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	return S_OK;
}

Jog_Turn_180_L::Jog_Turn_180_L(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM,_nIndex,_pNero)
{
}

Jog_Turn_180_L::~Jog_Turn_180_L()
{
}

Jog_Turn_180_L* Jog_Turn_180_L::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Jog_Turn_180_L(_pFSM,_nIndex,_pNero);
}

HRESULT Jog_Turn_180_L::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Jog_Turn_180_L", false, Nero::ANI_JOG_TURN_180_L);
	return S_OK;
}

HRESULT Jog_Turn_180_L::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Jog_Turn_180_L::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	return S_OK;
}

em200_Buster_Start::em200_Buster_Start(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM,_nIndex,_pNero)
{
}

em200_Buster_Start::~em200_Buster_Start()
{
}

em200_Buster_Start* em200_Buster_Start::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new em200_Buster_Start(_pFSM,_nIndex,_pNero);
}

HRESULT em200_Buster_Start::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("em200_Buster_Start", false, Nero::ANI_EM200_BUSTER_START);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_BusterArm, "em200_Buster_Start", false);
	return S_OK;
}

HRESULT em200_Buster_Start::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT em200_Buster_Start::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);

	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::EM200_BUSTER_LOOP);
		return S_OK;
	}
	return S_OK;
}

em200_Buster_Loop::em200_Buster_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM,_nIndex,_pNero)
{
}

em200_Buster_Loop::~em200_Buster_Loop()
{
}

em200_Buster_Loop* em200_Buster_Loop::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new em200_Buster_Loop(_pFSM,_nIndex,_pNero);
}

HRESULT em200_Buster_Loop::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("em200_Buster_Loop", true, Nero::ANI_EM200_BUSTER_LOOP);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_BusterArm, "em200_Buster_Loop", true);
	return S_OK;
}

HRESULT em200_Buster_Loop::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT em200_Buster_Loop::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);

	float fAccAnimationTime = m_pNero.lock()->Get_PlayingAccTime();

	if (1.98f <= fAccAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::EM200_BUSTER_FINSH);
		return S_OK;
	}

	return S_OK;
}

em200_BusterFinish::em200_BusterFinish(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM,_nIndex,_pNero)
{
}

em200_BusterFinish::~em200_BusterFinish()
{
}

em200_BusterFinish* em200_BusterFinish::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new em200_BusterFinish(_pFSM,_nIndex,_pNero);
}

HRESULT em200_BusterFinish::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("em200_Buster_Finish", false, Nero::ANI_EM200_BUSTER_FINISH);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_BusterArm, "em200_Buster_End", false);
	return S_OK;
}

HRESULT em200_BusterFinish::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT em200_BusterFinish::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		return S_OK;
	}
	return S_OK;
}

em200_Buster_Air_Start::em200_Buster_Air_Start(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM,_nIndex,_pNero)
{
}

em200_Buster_Air_Start::~em200_Buster_Air_Start()
{
}

em200_Buster_Air_Start* em200_Buster_Air_Start::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new em200_Buster_Air_Start(_pFSM,_nIndex,_pNero);
}

HRESULT em200_Buster_Air_Start::StateEnter()
{
	NeroState::StateEnter();

	m_pNero.lock()->ChangeAnimation("em200_Buster_Air_Start", false, Nero::ANI_EM200_BUSTER_AIR_START);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_BusterArm, "em200_Buster_Air_Start", false);
	NeroState::ActiveGravity(false);
	return S_OK;
}

HRESULT em200_Buster_Air_Start::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT em200_Buster_Air_Start::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);

	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::EM200_BUSTER_AIR_LOOP);
		return S_OK;
	}

	return S_OK;
}

em200_Buster_Air_Loop::em200_Buster_Air_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM,_nIndex,_pNero)
{
}

em200_Buster_Air_Loop::~em200_Buster_Air_Loop()
{
}

em200_Buster_Air_Loop* em200_Buster_Air_Loop::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new em200_Buster_Air_Loop(_pFSM,_nIndex,_pNero);
}

HRESULT em200_Buster_Air_Loop::StateEnter()
{
	NeroState::StateEnter();

	m_pNero.lock()->ChangeAnimation("em200_Buster_Air_Loop", true, Nero::ANI_EM200_BUSTER_AIR_LOOP);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_BusterArm, "em200_Buster_Air_Loop", true);

	return S_OK;
}

HRESULT em200_Buster_Air_Loop::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT em200_Buster_Air_Loop::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);

	float fAccAnimationTime = m_pNero.lock()->Get_PlayingAccTime();

	if (1.97f <= fAccAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::EM200_BUSTER_AIR_FINSH);
		return S_OK;
	}
	return S_OK;
}

em200_Buster_Air_Finish::em200_Buster_Air_Finish(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM,_nIndex,_pNero)
{
}

em200_Buster_Air_Finish::~em200_Buster_Air_Finish()
{
}

em200_Buster_Air_Finish* em200_Buster_Air_Finish::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new em200_Buster_Air_Finish(_pFSM,_nIndex,_pNero);
}

HRESULT em200_Buster_Air_Finish::StateEnter()
{
	NeroState::StateEnter();

	m_pNero.lock()->ChangeAnimation("em200_Buster_Air_Finish", false, Nero::ANI_EM200_BUSTER_AIR_FINISH);
	m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_BusterArm, "em200_Buster_Air_End", false);
	return S_OK;
}

HRESULT em200_Buster_Air_Finish::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT em200_Buster_Air_Finish::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);

	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.8f <= fCurAnimationTime)
	{
		NeroState::ActiveGravity(true);
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
		{
			NeroState::KeyInput_Jump();
		}
		else
		{
			NeroState::KeyInput_Cbs_Jump();
		}
	}

	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
		return S_OK;
	}



	return S_OK;
}


Cbs_Dash::Cbs_Dash(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Cbs_Dash::~Cbs_Dash()
{
}

Cbs_Dash* Cbs_Dash::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Cbs_Dash(_pFSM, _nIndex, _pNero);
}

HRESULT Cbs_Dash::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Cbs_Dash", false, Nero::ANI_CBS_DASH);

	NeroState::SetCbsIdle();
	return S_OK;
}

HRESULT Cbs_Dash::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Cbs_Dash::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	m_pNero.lock()->SetAngleFromCamera();

	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.6f <= fCurAnimationTime)
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
		{
			NeroState::KeyInput_Run();
		}
		else
		{
			NeroState::KeyInput_Cbs_Idle();
		}
	}

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
		{
			m_pFSM->ChangeState(NeroFSM::IDLE);
		}
		else
		{
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		}
		return S_OK;
	}
	return S_OK;
}


StateReset::StateReset(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

StateReset::~StateReset()
{
}

StateReset* StateReset::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new StateReset(_pFSM, _nIndex, _pNero);
}

HRESULT StateReset::StateEnter()
{
	return S_OK;
}

HRESULT StateReset::StateExit()
{
	return S_OK;
}

HRESULT StateReset::StateUpdate(const float _fDeltaTime)
{
	return S_OK;
}

Hit_Air_Away::Hit_Air_Away(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Hit_Air_Away::~Hit_Air_Away()
{
}

Hit_Air_Away* Hit_Air_Away::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Hit_Air_Away(_pFSM, _nIndex, _pNero);
}

HRESULT Hit_Air_Away::StateEnter()
{
	NeroState::StateEnter();

	m_pNero.lock()->ChangeAnimation("Hit_Air_Away", false, Nero::ANI_HIT_AIR_AWAY);
	m_pNero.lock()->SetAddForce({ 0.f,30.f,0.f });
	return S_OK;
}

HRESULT Hit_Air_Away::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Hit_Air_Away::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);

	if (m_pNero.lock()->CheckIsGround())
	{
		m_pFSM->ChangeState(NeroFSM::HIT_GROUND_AWAY);
		return S_OK;
	}
	return S_OK;
}

//

Hit_Ground_Away::Hit_Ground_Away(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Hit_Ground_Away::~Hit_Ground_Away()
{
}

Hit_Ground_Away* Hit_Ground_Away::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Hit_Ground_Away(_pFSM, _nIndex, _pNero);
}

HRESULT Hit_Ground_Away::StateEnter()
{
	NeroState::StateEnter();

	m_pNero.lock()->ChangeAnimation("Hit_Ground_Away", false, Nero::ANI_HIT_GROUND_AWAY);

	return S_OK;
}

HRESULT Hit_Ground_Away::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Hit_Ground_Away::StateUpdate(const float _fDeltaTime)
{
	NeroState::StateUpdate(_fDeltaTime);
	
	//키입력

	if (m_pNero.lock()->IsAnimationEnd())
	{
		if (Nero::NeroCom_RedQueen == m_iNeroCurWeaponIndex)
		{
			m_pFSM->ChangeState(NeroFSM::IDLE);
		}
		else
		{
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
		}
		return S_OK;
	}
	return S_OK;
}

TransformToShinMajin::TransformToShinMajin(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

TransformToShinMajin::~TransformToShinMajin()
{
}

TransformToShinMajin* TransformToShinMajin::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new TransformToShinMajin(_pFSM, _nIndex, _pNero);
}

HRESULT TransformToShinMajin::StateEnter()
{
	m_pNero.lock()->ChangeAnimation("TransformToMajin", false, Nero::ANI_TRANSFORM_TO_SHINMAJIN);
	m_pNero.lock()->ChangeWeapon(Nero::NeroCom_RedQueen);
	m_pNero.lock()->Set_Weapon_State(Nero::NeroCom_RedQueen, Nero::WS_Idle);
	return S_OK;
}

HRESULT TransformToShinMajin::StateExit()
{
	m_pNero.lock()->SetActive_NeroComponent(Nero::NeroCom_All_Weapon, false);
	return S_OK;
}

HRESULT TransformToShinMajin::StateUpdate(const float _fDeltaTime)
{
	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::SHINMAJIN_ENTER);
	}
	return S_OK;
}





TransformToOrigin::TransformToOrigin(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

TransformToOrigin::~TransformToOrigin()
{
}

TransformToOrigin* TransformToOrigin::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new TransformToOrigin(_pFSM, _nIndex, _pNero);
}

HRESULT TransformToOrigin::StateEnter()
{
	m_pNero.lock()->ChangeAnimation("TransformToOrigin", false, Nero::ANI_TRANSFORM_TO_ORIGIN_DANTE);

	return S_OK;
}

HRESULT TransformToOrigin::StateExit()
{
	return S_OK;
}

HRESULT TransformToOrigin::StateUpdate(const float _fDeltaTime)
{
	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::IDLE);
	}
	return S_OK;
}

ShinMajinEnter::ShinMajinEnter(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

ShinMajinEnter::~ShinMajinEnter()
{
}

ShinMajinEnter* ShinMajinEnter::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new ShinMajinEnter(_pFSM, _nIndex, _pNero);
}

HRESULT ShinMajinEnter::StateEnter()
{
	m_pNero.lock()->ChangeMeshIndex(Nero::SHINMAJIN_DANTE);
	m_pNero.lock()->ChangeAnimation("ShinMajinEnter", false, Nero::ANI_SHINMAJIN_ENTER);

	return S_OK;
}

HRESULT ShinMajinEnter::StateExit()
{
	return S_OK;
}

HRESULT ShinMajinEnter::StateUpdate(const float _fDeltaTime)
{
	if (m_pNero.lock()->IsAnimationEnd())
	{
		m_pFSM->ChangeState(NeroFSM::SHINMAJIN_JUDGEMNET);
	}
	return S_OK;
}

ShinMajinJudgement::ShinMajinJudgement(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

ShinMajinJudgement::~ShinMajinJudgement()
{
}

ShinMajinJudgement* ShinMajinJudgement::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new ShinMajinJudgement(_pFSM, _nIndex, _pNero);
}

HRESULT ShinMajinJudgement::StateEnter()
{
	m_pNero.lock()->ChangeAnimation("Judgement", false, Nero::ANI_SHINMAJIN_JUDGEMENT);
	
	for (int i = 0; i < 4; ++i)
	{
		m_bPlayOnce[i] = true;
	}
	return S_OK;
}

HRESULT ShinMajinJudgement::StateExit()
{
	return S_OK;
}

HRESULT ShinMajinJudgement::StateUpdate(const float _fDeltaTime)
{
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.307f <= fCurAnimationTime && m_bPlayOnce[0])
	{
		m_bPlayOnce[0] = false;
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_JudgementSword, "CINEMA_4D___", false);	}

	if (0.317f <= fCurAnimationTime && m_bPlayOnce[1])
	{
		m_bPlayOnce[1] = false;
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_JudgementShadow1, "CINEMA_4D___", false);
	}

	if (0.327f <= fCurAnimationTime && m_bPlayOnce[2])
	{
		m_bPlayOnce[2] = false;
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_JudgementShadow2, "CINEMA_4D___", false);
	}

	if (0.337f <= fCurAnimationTime && m_bPlayOnce[3])
	{
		m_bPlayOnce[3] = false;
		m_pNero.lock()->ChangeAnimation_Weapon(Nero::NeroCom_JudgementShadow3, "CINEMA_4D___", false);
	}

	if (m_pNero.lock()->IsAnimationEnd())
	{

	}
	return S_OK;
}