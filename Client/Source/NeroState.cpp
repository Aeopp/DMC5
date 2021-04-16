#include "stdafx.h"
#include "NeroState.h"
#include "Nero.h"
#include "NeroFSM.h"


#pragma region PARENT // 부모


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
	UINT RQ_Gage = m_pNero.lock()->Get_RQ_Gage();
	if (Input::GetKey(DIK_LSHIFT))
	{
		//락온
		if (Input::GetKey(DIK_Q))
		{
			m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
			m_pFSM->ChangeState(NeroFSM::WIRE_SNATCH_PULL);
		}
		else if (Input::GetKey(DIK_W))
		{
			if (Input::GetKey(DIK_S) && Input::GetMouse(DIM_L))
			{
				m_pNero.lock()->Set_RQ_State(Nero::WS_Battle);
				if(RQ_Gage > 0)
					m_pFSM->ChangeState(NeroFSM::SKILL_SHUFFLE_EX);
				else
					m_pFSM->ChangeState(NeroFSM::SKILL_SHUFFLE);
			}
			else if (Input::GetMouse(DIM_L))
			{
				m_pNero.lock()->Set_RQ_State(Nero::WS_Battle);
				if (RQ_Gage > 0)
					m_pFSM->ChangeState(NeroFSM::SKILL_STREAK_EX3);
				else
					m_pFSM->ChangeState(NeroFSM::SKILL_STREAK);
			}
			else if (Input::GetMouse(DIM_R))
			{
				
			}
			else if (Input::GetMouse(DIM_M))
			{
				m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
				m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT_UP);
			}
		}
		else if (Input::GetKey(DIK_S))
		{
			if (Input::GetMouse(DIM_L))
			{
				m_pNero.lock()->Set_RQ_State(Nero::WS_Battle);
				m_pFSM->ChangeState(NeroFSM::SKILL_FLOAT_GROUND);
			}
			else if (Input::GetMouse(DIM_R))
			{
				m_pNero.lock()->Set_RQ_State(Nero::WS_Battle);
				m_pFSM->ChangeState(NeroFSM::SKILL_HR_EX_START);
			}
			else if (Input::GetMouse(DIM_M))
			{
				m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
				m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT_DOWN);
			}
			else if (Input::GetKey(DIK_SPACE))
			{
				m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
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
				m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
				m_pFSM->ChangeState(NeroFSM::EVADE_L);
			}
		}
		else if (Input::GetKey(DIK_D))
		{
			if (Input::GetKey(DIK_SPACE))
			{
				// 오른쪽 구르기
				m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
				m_pFSM->ChangeState(NeroFSM::EVADE_R);
			}
		}
	}

	else if (Input::GetMouseDown(DIM_L))
	{
		m_pNero.lock()->Set_RQ_State(Nero::WS_Battle);
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
	}
	else if (Input::GetMouseDown(DIM_M))
	{
		m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
		m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT);
	}
	else if (Input::GetKeyDown(DIK_SPACE))
	{
		//점프
		m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
		m_pNero.lock()->Set_JumpDir(Nero::Basic);
		m_pFSM->ChangeState(NeroFSM::JUMP_START);
	}
	else if (Input::GetKeyDown(DIK_LCONTROL))
	{
		m_pNero.lock()->ChangeWeapon(Nero::Cbs);
		m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
	}

	else if (Input::GetKey(DIK_Q))
	{
		m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
		m_pFSM->ChangeState(NeroFSM::BUSTER_START);
	}

	else if (Input::GetKey(DIK_F))
	{
		//변신게이지 있는지 체크
		m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
		m_pFSM->ChangeState(NeroFSM::TO_MAJIN);
	}

	else if (Input::GetKey(DIK_W))
	{
		if ((NeroFSM::ATT1 <= _nIndex && _nIndex <= NeroFSM::ATT4))
		{

		}
		else if (Input::GetKey(DIK_SPACE))
		{
			m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
			m_pNero.lock()->Set_JumpDir(Nero::Front);
			m_pFSM->ChangeState(NeroFSM::JUMP_START);

			return S_OK;
		}
		else
		{
			m_pFSM->ChangeState(NeroFSM::RUNSTART);
		}
	}
	else if (Input::GetKey(DIK_S))
	{
		//180도 회전
		m_pFSM->ChangeState(NeroFSM::RUNSTART_180);
	}
	else if (Input::GetKey(DIK_A))
	{
		//왼쪽으로 90도 회전
		m_pFSM->ChangeState(NeroFSM::RUNSTART_L);
	}
	else if (Input::GetKey(DIK_D))
	{
		//오른쪽으로 90도 회전
		m_pFSM->ChangeState(NeroFSM::RUNSTART_R);
	}
	//테스트
	else if (Input::GetKeyDown(DIK_5))
	{
		m_pNero.lock()->IncreaseRQ_Gage();
	}
	return S_OK;
}

HRESULT NeroState::KeyInput_Run(const int _nIndex)
{
	UINT RQ_Gage = m_pNero.lock()->Get_RQ_Gage();
	if (Input::GetKey(DIK_LSHIFT))
	{
		//락온
		if (Input::GetKey(DIK_Q))
		{
			m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
			m_pFSM->ChangeState(NeroFSM::WIRE_SNATCH_PULL);
		}
		else if(Input::GetKey(DIK_W))
		{
			if (Input::GetKey(DIK_S) && Input::GetMouse(DIM_L))
			{
				m_pNero.lock()->Set_RQ_State(Nero::WS_Battle);
				if (RQ_Gage > 0)
					m_pFSM->ChangeState(NeroFSM::SKILL_SHUFFLE_EX);
				else
					m_pFSM->ChangeState(NeroFSM::SKILL_SHUFFLE);
			}
			else if (Input::GetMouse(DIM_L))
			{
				m_pNero.lock()->Set_RQ_State(Nero::WS_Battle);
				if (RQ_Gage > 0)
					m_pFSM->ChangeState(NeroFSM::SKILL_STREAK_EX3);
				else
					m_pFSM->ChangeState(NeroFSM::SKILL_STREAK);
			}
			else if (Input::GetMouse(DIM_R))
			{

			}
			else if (Input::GetMouse(DIM_M))
			{
				m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
				m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT_UP);
			}
		}
		else if (Input::GetKey(DIK_S))
		{
			if (Input::GetMouse(DIM_L))
			{
				m_pNero.lock()->Set_RQ_State(Nero::WS_Battle);
				m_pFSM->ChangeState(NeroFSM::SKILL_FLOAT_GROUND);
			}
			else if (Input::GetMouse(DIM_R))
			{
				m_pNero.lock()->Set_RQ_State(Nero::WS_Battle);
				m_pFSM->ChangeState(NeroFSM::SKILL_HR_EX_START);
			}
			else if (Input::GetMouse(DIM_M))
			{
				m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
				m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT_DOWN);
			}
			else if (Input::GetKey(DIK_SPACE))
			{
				m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
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
				m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
				m_pFSM->ChangeState(NeroFSM::EVADE_L);
			}
		}
		else if (Input::GetKey(DIK_D))
		{
			if (Input::GetKey(DIK_SPACE))
			{
				// 오른쪽 구르기
				m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
				m_pFSM->ChangeState(NeroFSM::EVADE_R);
			}
		}
	}

	else if (Input::GetMouseDown(DIM_L))
	{
		m_pNero.lock()->Set_RQ_State(Nero::WS_Battle);
		m_pFSM->ChangeState(NeroFSM::ATT1_DASH);
	}
	else if (Input::GetMouseDown(DIM_M))
	{
		m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
		m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT);
	}
	else if (Input::GetKeyDown(DIK_SPACE))
	{
		//점프
		m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
		m_pNero.lock()->Set_JumpDir(Nero::Front);
		m_pFSM->ChangeState(NeroFSM::JUMP_START);
	}
	else if (Input::GetKeyDown(DIK_LCONTROL))
	{
		//m_pNero.lock()->ChangeWeapon(Nero::Cbs);
		//m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
	}

	else if (Input::GetKey(DIK_Q))
	{
		m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
		m_pFSM->ChangeState(NeroFSM::BUSTER_START);
	}

	else if (Input::GetKey(DIK_F))
	{
		//변신게이지 있는지 체크
		m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
		m_pFSM->ChangeState(NeroFSM::TO_MAJIN);
	}

	else if (Input::GetKey(DIK_W))
	{

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

	return S_OK;
}

HRESULT NeroState::KeyInput_Cbs_Idle(const int _nIndex)
{
	//삼절곤 키입력
	if (Input::GetKey(DIK_LSHIFT))
	{
		//락온
		if (Input::GetKey(DIK_W))
		{
			if (Input::GetKey(DIK_S) && Input::GetMouse(DIM_L))
				m_pFSM->ChangeState(NeroFSM::CBS_ICEAGE_START);
			else if (Input::GetMouse(DIM_L))
			{
				//앞으로 돌아버리는거
				m_pFSM->ChangeState(NeroFSM::CBS_REVOLVER_START);
			}
			else if (Input::GetMouse(DIM_R))
			{
				//땅 꽝 찍는거
				m_pFSM->ChangeState(NeroFSM::MIDDLE_CBS_BLITZATTACK);
			}
			else if (Input::GetKey(DIK_S) && Input::GetMouse(DIM_M))
			{
				//앞으로 나가면서 미친연속공격
				m_pFSM->ChangeState(NeroFSM::MIDDLE_CBS_BI_ATTACK);
			}
			else if (Input::GetMouse(DIM_M))
			{
				m_pFSM->ChangeState(NeroFSM::MIDDLE_CBS_SATELLITE);
			}
		}
		else if (Input::GetKey(DIK_S))
		{
			//락온한 상태로 뒤로
			if (Input::GetMouse(DIM_L))
				m_pFSM->ChangeState(NeroFSM::CBS_CRYSTAL);
			else if (Input::GetMouse(DIM_R))
			{
				//총알 발사
				m_pFSM->ChangeState(NeroFSM::MIDDLE_CBS_THUNDERBULLET);
			}
			else if (Input::GetMouse(DIM_M))
			{
				m_pFSM->ChangeState(NeroFSM::POLE_WHIRLWIND_START);
			}
		}
		else if (Input::GetKey(DIK_A))
		{
			if (Input::GetKey(DIK_SPACE))
			{
				//왼쪽 구르기
				m_pFSM->ChangeState(NeroFSM::EVADE_L);
			}
		}
		else if (Input::GetKey(DIK_D))
		{
			if (Input::GetKey(DIK_SPACE))
			{
				// 오른쪽 구르기
				m_pFSM->ChangeState(NeroFSM::EVADE_R);
			}
		}
	}

	else if (Input::GetKey(DIK_W))
	{
		m_pFSM->ChangeState(NeroFSM::RUNSTART);
	}
	else if (Input::GetKey(DIK_S))
	{
		//180도 회전
		m_pFSM->ChangeState(NeroFSM::RUNSTART_180);
	}
	else if (Input::GetKey(DIK_A))
	{
		//왼쪽으로 90도 회전
		m_pFSM->ChangeState(NeroFSM::RUNSTART_L);
	}
	else if (Input::GetKey(DIK_D))
	{
		//오른쪽으로 90도 회전
		m_pFSM->ChangeState(NeroFSM::RUNSTART_R);
	}
	else if (Input::GetMouseDown(DIM_L))
	{
		m_pFSM->ChangeState(NeroFSM::CBS_COMBOA1);
	}
	else if (Input::GetMouseDown(DIM_M))
	{
		m_pFSM->ChangeState(NeroFSM::POLE_COMBOA1);
	}
	else if (Input::GetMouseDown(DIM_R))
	{
		//몬스터 위치에 따라서 업다운 프론트 결정
		m_pFSM->ChangeState(NeroFSM::MIDDLE_CBS_STRIKE);
	}
	else if (Input::GetKeyDown(DIK_SPACE))
	{
		//점프
	}
	else if (Input::GetKeyDown(DIK_LCONTROL))
	{
		//삼절곤에서 검으로 변환
		m_pNero.lock()->ChangeWeapon(Nero::RQ);
		m_pFSM->ChangeState(NeroFSM::IDLE);
	}
	return S_OK;
}

HRESULT NeroState::KeyInput_Jump(const int _nIndex)
{
	UINT JumpCount = m_pNero.lock()->Get_JumpCount();
	UINT RQ_Gage = m_pNero.lock()->Get_RQ_Gage();

	if (Input::GetKey(DIK_LSHIFT))
	{
		//락온
		if (Input::GetKey(DIK_Q))
		{
			m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
			m_pFSM->ChangeState(NeroFSM::WIRE_SNATCH_PULL_AIR);
		}
		else if (Input::GetKey(DIK_W))
		{
			if (Input::GetKey(DIK_S) && Input::GetMouse(DIM_L))
			{
				m_pNero.lock()->Set_RQ_State(Nero::WS_Battle);
				m_pFSM->ChangeState(NeroFSM::SKILL_CALIBER_START);
			}
			else if (Input::GetMouse(DIM_L))
			{
				m_pNero.lock()->Set_RQ_State(Nero::WS_Battle);
				m_pFSM->ChangeState(NeroFSM::SKILL_AIR_DIVE_SLASH_START);
			}
			else if (Input::GetMouse(DIM_R))
			{
				
			}
			else if (Input::GetMouse(DIM_M))
			{
				//오버츄어 공중에서 위로
				m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
				m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT_AIR_UP);
			}
		}
		else if (Input::GetKey(DIK_S))
		{
			if (Input::GetMouse(DIM_L))
			{
				//게이지 따라서 분기
				m_pNero.lock()->Set_RQ_State(Nero::WS_Battle);
				if(RQ_Gage > 0)
					m_pFSM->ChangeState(NeroFSM::SKILL_SPLIT_EX_START);
				else
					m_pFSM->ChangeState(NeroFSM::SKILL_SPLIT_START);


			}
			else if (Input::GetMouse(DIM_R))
			{
				//총알 발사
			}
			else if (Input::GetMouse(DIM_M))
			{
				//오버추어 공중에서 아래로
				m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
				m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT_AIR_DOWN);
			}
			else if (0 < JumpCount && Input::GetKey(DIK_SPACE))
			{
				//뒤로 2단 점프
				m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
				m_pNero.lock()->Set_JumpDir(Nero::Back);
				m_pFSM->ChangeState(NeroFSM::JUMP_TWICE);
			}
		}
	}

	else if (Input::GetMouseDown(DIM_L))
	{
		m_pNero.lock()->Set_RQ_State(Nero::WS_Battle);
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
		
	}
	else if (Input::GetMouseDown(DIM_M))
	{
		//오버추어 앞으로
		m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
		m_pFSM->ChangeState(NeroFSM::OVERTURE_SHOOT_AIR);
	}
	else if (0 < JumpCount && Input::GetKeyDown(DIK_SPACE))
	{
		//제자리 2단 점프
		m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
		m_pNero.lock()->Set_JumpDir(Nero::Basic);
		m_pFSM->ChangeState(NeroFSM::JUMP_TWICE);
	}
	else if (Input::GetKeyDown(DIK_LCONTROL))
	{
		//무기 교체를 할지는 생각해봐야됨

		//m_pNero.lock()->ChangeWeapon(Nero::Cbs);
		//m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
	}
	else if (Input::GetKey(DIK_W))
	{

	}
	else if (Input::GetKey(DIK_S))
	{
		m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
		if (0 < JumpCount && Input::GetKey(DIK_SPACE))
		{
			//앞으로 2단 점프
			m_pNero.lock()->Set_JumpDir(Nero::Front);
			m_pFSM->ChangeState(NeroFSM::JUMP_TWICE);
		}
	}
	else if (Input::GetKey(DIK_A))
	{

	}
	else if (Input::GetKey(DIK_D))
	{

	}
	return S_OK;
}

HRESULT NeroState::KeyInput_Cbs_Jump(const int _nIndex)
{
	return S_OK;
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
		m_pNero.lock()->ChangeAnimation("Idle_From_ComboA1_Loop", true, Nero::ANI_IDLE_FROM_COMBOA1_LOOP);
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
		break;
	case Nero::ANI_JUMP_LANDING:
		m_pNero.lock()->ChangeAnimation("Idle_Battle", true, Nero::ANI_IDLE_BATTLE);
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
		//m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
		m_pNero.lock()->ChangeAnimation("Idle_Normal", true,Nero::ANI_IDLE_NORMAL);
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
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	UINT CurAnimationIndex = m_pNero.lock()->Get_CurAnimationIndex();
	UINT PreAnimationIndex = m_pNero.lock()->Get_PreAnimationIndex();

	switch (CurAnimationIndex)
	{
	case Nero::ANI_IDLE_FROM_COMBOA1:
		if (fCurrAnimationTime <= 0.5f)
		{
			// 레드퀸 콤보 땅바닥 찍는거 시작 
			KeyInput_Idle(NeroFSM::ATT_COMBO_C1);
		}
		else
			KeyInput_Idle(NeroFSM::IDLE);
		break;
	case Nero::ANI_IDLE_FROM_COMBOA2:
		if (fCurrAnimationTime <= 0.5f)
		{
			KeyInput_Idle(NeroFSM::ATT_COMBO_D1);
		}
		else
			KeyInput_Idle(NeroFSM::IDLE);

		break;
	default:
		break;
	}

	if (0.95f <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::IDLE);
	}
	//NeroState::KeyInput_Idle();
	return S_OK;
}

Idle_Battle::Idle_Battle(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Idle_Battle::~Idle_Battle()
{
}

Idle_Battle* Idle_Battle::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Idle_Battle(_pFSM, _nIndex, _pNero);
}

HRESULT Idle_Battle::StateEnter()
{
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Idle_Battle", true, Nero::ANI_IDLE_BATTLE);
	return S_OK;
}

HRESULT Idle_Battle::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Idle_Battle::StateUpdate(const float _fDeltaTime)
{
	//런스타트 배틀
	NeroState::StateUpdate(_fDeltaTime);
	NeroState::KeyInput_Idle();
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

	switch (iJumpDir)
	{
	case Nero::Basic:
		m_pNero.lock()->ChangeAnimation("Jump", false, Nero::ANI_JUMP);
		break;
	case Nero::Front:
		m_pNero.lock()->ChangeAnimation("Jump_Front", false, Nero::ANI_JUMP_FRONT);
		break;
	case Nero::Back:
		m_pNero.lock()->ChangeAnimation("Jump_Back", false, Nero::ANI_JUMP_BACK);
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
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96 <= fCurAnimationTime /* || 땅에 닿았다*/)
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LANDING);
	}
	KeyInput_Jump();
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
	//m_pNero.lock()->ChangeAnimationIndex(Nero::ANI_JUMP_LOOP);
	return S_OK;
}

HRESULT Jump_Fly_Loop::StateExit()
{
	m_pNero.lock()->ContinueAnimiation();
	return S_OK;
}

HRESULT Jump_Fly_Loop::StateUpdate(const float _fDeltaTime)
{
	NeroState::KeyInput_Jump();

	//테스트용

	if (Input::GetKey(DIK_M))
		m_pFSM->ChangeState(NeroFSM::JUMP_LANDING);
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
		m_pNero.lock()->ChangeAnimation("Jump_Twice", false, Nero::ANI_JUMP_TWICE);
		break;
	case Nero::Front:
		m_pNero.lock()->ChangeAnimation("Jump_Front_Twice", false, Nero::ANI_JUMP_FRONT_TWICE);
		break;
	case Nero::Back:
		m_pNero.lock()->ChangeAnimation("Jump_Back_Twice", false, Nero::ANI_JUMP_BACK_TWICE);
		break;
	default:
		break;
	}
	return S_OK;
}

HRESULT Jump_Twice::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Jump_Twice::StateUpdate(const float _fDeltaTime)
{
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96 <= fCurAnimationTime)
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
	NeroState::KeyInput_Jump();
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

	//if (점프가 높이 뛰었다)
	//{
	//	m_pNero.lock()->ChangeAnimation("Jump_Landing_High", false, Nero::ANI_JUMP_LANDING_HIGH);
	//	return S_OK;
	//}
	UINT iJumpDir = m_pNero.lock()->Get_JumpDir();

	switch (iJumpDir)
	{
	case Nero::Basic:
		m_pNero.lock()->ChangeAnimation("Jump_Landing", false, Nero::ANI_JUMP_LANDING);
		break;
	case Nero::Front:
		m_pNero.lock()->ChangeAnimation("Jump_Front_Landing", false, Nero::ANI_JUMP_FRONT_LANDING);
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT Jump_Front_Landing::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Jump_Front_Landing::StateUpdate(const float _fDeltaTime)
{
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96 <= fCurAnimationTime)
	{
		UINT  iCurWeaponIndex = m_pNero.lock()->Get_CurWeaponIndex();
		switch (iCurWeaponIndex)
		{
		case Nero::RQ:
			m_pFSM->ChangeState(NeroFSM::IDLE);
			break;
		case Nero::Cbs:
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
			break;
		default:
			m_pFSM->ChangeState(NeroFSM::IDLE);
			break;
		}
		
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
	m_pNero.lock()->ChangeAnimation("RunStart270", false,Nero::ANI_RUNSTART270);

	return S_OK;
}

HRESULT RunStartLeft::StateExit()
{
	//끝날때 회전
	NeroState::StateExit();
	return S_OK;
}

HRESULT RunStartLeft::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (Input::GetKey(DIK_A))
	{
		if (0.96 <= fCurrAnimationTime)
		{
			m_pFSM->ChangeState(NeroFSM::RUNLOOP);
		}
	}
	else
	{
		m_pFSM->ChangeState(NeroFSM::IDLE);
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
	m_pNero.lock()->ChangeAnimation("RunStart90", false, Nero::ANI_RUNSTART90);


	return S_OK;
}

HRESULT RunStartRight::StateExit()
{
	//끝날때 회전
	NeroState::StateExit();

	return S_OK;
}

HRESULT RunStartRight::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (Input::GetKey(DIK_D))
	{
		if (0.96 <= fCurrAnimationTime)
		{
			m_pFSM->ChangeState(NeroFSM::RUNLOOP);
		}
	}
	else
	{
		m_pFSM->ChangeState(NeroFSM::IDLE);
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
	m_pNero.lock()->ChangeAnimation("RunStart180", false,Nero::ANI_RUNSTART180);

	return S_OK;
}

HRESULT RunStart180::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT RunStart180::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (Input::GetKey(DIK_S))
	{
		if (0.96 <= fCurrAnimationTime)
		{
			m_pFSM->ChangeState(NeroFSM::RUNLOOP);
		}
	}
	else
	{
		m_pFSM->ChangeState(NeroFSM::IDLE);
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
	UINT PreAnimationIndex = m_pNero.lock()->Get_PreAnimationIndex();

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

	

	return S_OK;
}

HRESULT RunLoop::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT RunLoop::StateUpdate(const float _fDeltaTime)
{
	if (Input::GetKey(DIK_W) || Input::GetKey(DIK_S) || Input::GetKey(DIK_A) || Input::GetKey(DIK_D))
	{
		//키입력이 특정 시간이 넘었다
		//-> 그러면 대쉬 루프로 변환하고
		//칼들고 달리고있었으면 칼 집어넣고
		KeyInput_Run(NeroFSM::RUNLOOP);
	}
	else
	{
		//그냥 달리는거면 이거고
		m_pFSM->ChangeState(NeroFSM::RUNSTOP);
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

	switch (CurAnimationIndex)
	{
	case Nero::ANI_IDLE_NORMAL:
		m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
		break;
	case Nero::ANI_IDLE_BATTLE:
		m_pNero.lock()->ChangeAnimation("RunStart0", false,Nero::ANI_RUNSTART0);
		break;
	case Nero::ANI_IDLE_FROM_COMBOA1:
		m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA1", false, Nero::ANI_RUNSTART_FROM_COMBOA1);
		break;
	case Nero::ANI_IDLE_FROM_COMBOA1_LOOP:
		m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA1", false, Nero::ANI_RUNSTART_FROM_COMBOA1);
		break;
	case Nero::ANI_IDLE_FROM_COMBOA1_END:
		m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
		break;
	case Nero::ANI_IDLE_FROM_COMBOA2:
		m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA2", false, Nero::ANI_RUNSTART_FROM_COMBOA2);
		break;
	case Nero::ANI_IDLE_FROM_COMBOA2_LOOP:
		m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA2", false, Nero::ANI_RUNSTART_FROM_COMBOA2);
		break;												 
	case Nero::ANI_IDLE_FROM_COMBOA2_END:					 
		m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA2", false, Nero::ANI_RUNSTART_FROM_COMBOA2);
		break;
	case Nero::ANI_SHUFFLE:
		m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA2", false, Nero::ANI_RUNSTART_FROM_COMBOA2);
		break;
	case Nero::ANI_RUNSTOP:
		m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
		break;
	case Nero::ANI_OVERTURE_IDLE:
		m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
		break;
	case Nero::ANI_OVERTURE_SHOOT:
		m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
		break;
	case Nero::ANI_OVERTURE_SHOOT_UP:
		m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
		break;
	case Nero::ANI_OVERTURE_SHOOT_DOWN:
		m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
		break;
	case Nero::ANI_WIRE_SNATCH_PULL:
		m_pNero.lock()->ChangeAnimation("RunStart0", false, Nero::ANI_RUNSTART0);
		break;
	default:
		m_pNero.lock()->ChangeAnimation("RunStart_From_ComboA1", false, Nero::ANI_RUNSTART_FROM_COMBOA1);
		break;
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
	// 시작하는 애니메니메이션이 끝났으면 Loop로 변환
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (Input::GetKey(DIK_W))
	{
		if (0.98 <= fCurrAnimationTime)
		{
			m_pFSM->ChangeState(NeroFSM::RUNLOOP);
		}
	}
	else
	{
		m_pFSM->ChangeState(NeroFSM::IDLE);
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
	switch (CurWeaponIndex)
	{
	case Nero::RQ:
		if(Nero::ANI_RUNLOOP_FROM_COMBOA1 == CurAnimationIndex)
			m_pNero.lock()->ChangeAnimation("Idle_From_ComboA1_End", false, Nero::ANI_IDLE_FROM_COMBOA1_END);
		else
			m_pNero.lock()->ChangeAnimation("RunStop", false, Nero::ANI_RUNSTOP);
		break;
	case Nero::Cbs:
		m_pNero.lock()->ChangeAnimation("Cbs_RunStop", false, Nero::ANI_CBS_RUNSTOP);
		break;
	}
	

	return S_OK;
}

HRESULT RunStop::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT RunStop::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	UINT CurAnimationIndex = m_pNero.lock()->Get_CurAnimationIndex();
	UINT CurWeaponIndex = m_pNero.lock()->Get_CurWeaponIndex();
	if (0.5 <= fCurrAnimationTime)
	{
		switch (CurWeaponIndex)
		{
		case Nero::RQ:
			m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
			break;
		case Nero::Cbs:
			break;
		}
	}
	if (0.62 <= fCurrAnimationTime)
	{
		NeroState::KeyInput_Idle(NeroFSM::RUNSTOP);
	}
	if (0.96 <= fCurrAnimationTime)
	{
		switch (CurWeaponIndex)
		{
		case Nero::RQ:
			m_pFSM->ChangeState(NeroFSM::IDLE);
			break;
		case Nero::Cbs:
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
			break;
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
	return S_OK;
}

HRESULT DashLoop::StateExit()
{
	return S_OK;
}

HRESULT DashLoop::StateUpdate(const float _fDeltaTime)
{
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
	return S_OK;
}

HRESULT DashStop::StateExit()
{
	return S_OK;
}

HRESULT DashStop::StateUpdate(const float _fDeltaTime)
{
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
	return S_OK;
}

HRESULT DashTurn::StateExit()
{
	return S_OK;
}

HRESULT DashTurn::StateUpdate(const float _fDeltaTime)
{
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
	UINT CurWeaponIndex = m_pNero.lock()->Get_CurWeaponIndex();
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96f <= fCurrAnimationTime)
	{
		if(Nero::RQ == CurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
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
	UINT CurWeaponIndex = m_pNero.lock()->Get_CurWeaponIndex();
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96f <= fCurrAnimationTime)
	{
		if (Nero::RQ == CurWeaponIndex)
			m_pFSM->ChangeState(NeroFSM::IDLE);
		else
			m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
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
	return S_OK;
}

HRESULT HitFront::StateExit()
{
	return S_OK;
}

HRESULT HitFront::StateUpdate(const float _fDeltaTime)
{
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
	return S_OK;
}

HRESULT StunStart::StateExit()
{
	return S_OK;
}

HRESULT StunStart::StateUpdate(const float _fDeltaTime)
{
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
	return S_OK;
}

HRESULT StunLoop::StateExit()
{
	return S_OK;
}

HRESULT StunLoop::StateUpdate(const float _fDeltaTime)
{
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
	return S_OK;
}

HRESULT StunEnd::StateExit()
{
	return S_OK;
}

HRESULT StunEnd::StateUpdate(const float _fDeltaTime)
{
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
	m_pNero.lock()->ChangeAnimation("Wire_Snatch_Pull", false, Nero::ANI_WIRE_SNATCH_PULL);
	m_pNero.lock()->SetActive_Wire_Arm(true);
	m_pNero.lock()->Change_WireArm_Animation("Wire_Arm_Start31", true);
	//NeroState::ResetAnimation()
	return S_OK;
}

HRESULT Wire_Pull::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Wire_Pull::StateUpdate(const float _fDeltaTime)
{
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	//Down 0.35에 키입력

	//Up 0.33에 키입력

	if (0.38 <= fCurAnimationTime)
		NeroState::KeyInput_Idle(NeroFSM::WIRE_SNATCH_PULL);

	if (0.96 <= fCurAnimationTime)
		m_pFSM->ChangeState(NeroFSM::IDLE);

	return S_OK;
}

Wire_Pull_Up::Wire_Pull_Up(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Wire_Pull_Up::~Wire_Pull_Up()
{
}

Wire_Pull_Up* Wire_Pull_Up::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Wire_Pull_Up(_pFSM, _nIndex, _pNero);
}

HRESULT Wire_Pull_Up::StateEnter()
{

	return S_OK;
}

HRESULT Wire_Pull_Up::StateExit()
{
	return S_OK;
}

HRESULT Wire_Pull_Up::StateUpdate(const float _fDeltaTime)
{
	return S_OK;
}

Wire_Pull_Down::Wire_Pull_Down(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

Wire_Pull_Down::~Wire_Pull_Down()
{
}

Wire_Pull_Down* Wire_Pull_Down::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new Wire_Pull_Down(_pFSM, _nIndex, _pNero);
}

HRESULT Wire_Pull_Down::StateEnter()
{
	return S_OK;
}

HRESULT Wire_Pull_Down::StateExit()
{
	return S_OK;
}

HRESULT Wire_Pull_Down::StateUpdate(const float _fDeltaTime)
{
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

	return S_OK;
}

HRESULT Wire_Fly_Start::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Wire_Fly_Start::StateUpdate(const float _fDeltaTime)
{
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

	return S_OK;
}

HRESULT Wire_Fly_End::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Wire_Fly_End::StateUpdate(const float _fDeltaTime)
{
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

	m_pNero.lock()->ChangeAnimation("Wire_Snatch_Pull_Air", false, Nero::ANI_WIRE_SNATCH_PULL_AIR);

	NeroState::ResetAnimation(0.96, Nero::ANI_WIRE_SNATCH_PULL_AIR);

	m_pNero.lock()->SetActive_Wire_Arm(true);
	m_pNero.lock()->Change_WireArm_Animation("Wire_Arm_Start31", true);
	return S_OK;
}

HRESULT Wire_Pull_Air::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Wire_Pull_Air::StateUpdate(const float _fDeltaTime)
{
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96 <= fCurAnimationTime)
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);

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
	NeroState::StateEnter();
	//달리고있었으면 대쉬 ComboA로 가야됨
	m_pNero.lock()->ChangeAnimation("ComboA1", false,Nero::ANI_COMBOA1);
	m_pNero.lock()->Set_RQ_State(Nero::WS_Battle);

	return S_OK;
}

HRESULT BT_Att1::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT BT_Att1::StateUpdate(const float _fDeltaTime)
{
	//현재 애니메이션 프레임 위치
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.18f <= fCurrAnimationTime && fCurrAnimationTime <= 0.35f)
	{
		KeyInput_Idle(NeroFSM::ATT2);
	}
	if (0.37f <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::IDLE_START);
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

	return S_OK;
}

HRESULT BT_Att2::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT BT_Att2::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.18f <= fCurrAnimationTime && fCurrAnimationTime <= 0.35f)
	{
		KeyInput_Idle(NeroFSM::ATT3);
	}
	if (0.56f <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::IDLE_START);
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
	return S_OK;
}

HRESULT BT_Att3::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT BT_Att3::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (0.68 <= fCurrAnimationTime)
	{
		m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
	}

	if (0.18f <= fCurrAnimationTime && fCurrAnimationTime <= 0.5f)
	{
		KeyInput_Idle(NeroFSM::ATT4);
	}

	if (0.96 <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::IDLE);
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

	return S_OK;
}

HRESULT BT_Att4::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT BT_Att4::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.44 <= fCurrAnimationTime)
		NeroState::KeyInput_Idle(NeroFSM::IDLE);

	if (0.8 <= fCurrAnimationTime)
	{
		m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
	}
	if (0.96 <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::IDLE);
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

	return S_OK;
}

HRESULT BT_Att_ComboC_R_to_L::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT BT_Att_ComboC_R_to_L::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (Input::GetMouseDown(DIM_L))
	{
		if (0.22f <= fCurrAnimationTime && fCurrAnimationTime <= 0.35f)
		{
			m_pFSM->ChangeState(NeroFSM::ATT_COMBO_C4);
		}
	}
	else if (0.96 <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::IDLE);
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

	return S_OK;
}

HRESULT BT_Att_ComboC_L_to_R::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT BT_Att_ComboC_L_to_R::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (Input::GetMouseDown(DIM_L))
	{
		if (0.22f <= fCurrAnimationTime && fCurrAnimationTime <= 0.35f)
		{
			m_pFSM->ChangeState(NeroFSM::ATT_COMBO_C3);
		}
	}
	else if (0.96 <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::IDLE);
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
	return S_OK;
}

HRESULT BT_Att_ComboC_1::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT BT_Att_ComboC_1::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (Input::GetMouseDown(DIM_L))
	{
		if (0.4f <= fCurrAnimationTime && fCurrAnimationTime <= 0.5f)
		{
			m_pFSM->ChangeState(NeroFSM::ATT_COMBO_C2);
		}
	}
	else if (0.96 <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::IDLE);
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

	return S_OK;
}

HRESULT BT_Att_ComboC_2::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT BT_Att_ComboC_2::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (Input::GetMouseDown(DIM_L))
	{
		if (0.22f <= fCurrAnimationTime && fCurrAnimationTime <= 0.35f)
		{
			m_pFSM->ChangeState(NeroFSM::ATT_COMBO_C_L);
		}
	}
	else if (0.96 <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::IDLE);
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

	return S_OK;
}

HRESULT BT_Att_ComboC_3::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT BT_Att_ComboC_3::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (Input::GetMouseDown(DIM_L))
	{
		if (0.22f <= fCurrAnimationTime && fCurrAnimationTime <= 0.35f)
		{
			m_pFSM->ChangeState(NeroFSM::ATT_COMBO_C_R);
		}
	}
	else if (0.96 <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::IDLE);
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
	return S_OK;
}

HRESULT BT_Att_ComboC_4::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT BT_Att_ComboC_4::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.65 <= fCurrAnimationTime)
	{
		NeroState::KeyInput_Idle(NeroFSM::IDLE);
	}

	if (0.96 <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::IDLE);
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
	return S_OK;
}

HRESULT BT_Att_ComboD_1::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT BT_Att_ComboD_1::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (Input::GetMouseDown(DIM_L))
	{
		if (0.15f <= fCurrAnimationTime && fCurrAnimationTime <= 0.3f)
		{
			m_pFSM->ChangeState(NeroFSM::ATT_COMBO_D2);
		}
	}
	else if (0.96 <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::IDLE);
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
	return S_OK;
}

HRESULT BT_Att_ComboD_2::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT BT_Att_ComboD_2::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (Input::GetMouseDown(DIM_L))
	{
		if (0.14f <= fCurrAnimationTime && fCurrAnimationTime <= 0.22f)
		{
			m_pFSM->ChangeState(NeroFSM::ATT_COMBO_D3);
		}
	}
	else if (0.96 <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::IDLE);
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

	return S_OK;
}

HRESULT BT_Att_ComboD_3::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT BT_Att_ComboD_3::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (Input::GetMouseDown(DIM_L))
	{
		if (0.19f <= fCurrAnimationTime && fCurrAnimationTime <= 0.27f)
		{
			m_pFSM->ChangeState(NeroFSM::ATT_COMBO_D4);
		}
	}
	else if (0.96 <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::IDLE);
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

	return S_OK;
}

HRESULT BT_Att_ComboD_4::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT BT_Att_ComboD_4::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (Input::GetMouseDown(DIM_L))
	{

	}
	else if (0.96 <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::IDLE);
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
	NeroState::StateEnter();
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	UINT  CurAnimationIndex = m_pNero.lock()->Get_CurAnimationIndex();

	//m_pNero.lock()->Set_JumpDir(Nero::Basic);
	m_pNero.lock()->ChangeAnimation("ComboA_Air1", false, Nero::ANI_COMBOA_AIR1);

	if (0.96 <= fCurrAnimationTime && Nero::ANI_COMBOA_AIR1 == CurAnimationIndex)
	{
		m_pNero.lock()->Set_PlayingTime(0.f);
	}
	return S_OK;
}

HRESULT BT_Air_Att1::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT BT_Air_Att1::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	UINT  CurAnimationIndex = m_pNero.lock()->Get_CurAnimationIndex();

	if(0.8 <= fCurrAnimationTime)
		m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);

	if (0.3f <= fCurrAnimationTime && fCurrAnimationTime <= 0.5f)
	{
		NeroState::KeyInput_Jump(NeroFSM::AIR_COMBOA1);
	}
	else if (0.6f <= fCurrAnimationTime && fCurrAnimationTime <= 0.93f)
	{
		NeroState::KeyInput_Jump(NeroFSM::AIR_COMBOA3);
		return S_OK;
	}

	else if (0.96 <= fCurrAnimationTime/* && Nero::ANI_COMBOA_AIR1 != CurAnimationIndex */)
	{
		//애니메이션을 멈추든지..
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
	}

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
	return S_OK;
}

HRESULT BT_Air_Att2::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT BT_Air_Att2::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.85 <= fCurrAnimationTime)
		m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);


	if (0.27f <= fCurrAnimationTime && fCurrAnimationTime <= 0.42f)
	{
		NeroState::KeyInput_Jump(NeroFSM::AIR_COMBOA2);
		return S_OK;
	}
	else if (0.47f <= fCurrAnimationTime && fCurrAnimationTime <= 0.62f)
	{
		NeroState::KeyInput_Jump(NeroFSM::AIR_COMBOB);
		return S_OK;
	}

	else if (0.7f <= fCurrAnimationTime && fCurrAnimationTime <= 0.93f)
	{
		NeroState::KeyInput_Jump(NeroFSM::AIR_COMBOA3);
		return S_OK;
	}


	else if (0.96 <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
	}

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

	m_pNero.lock()->ChangeAnimation("ComboA_Air3", false, Nero::ANI_COMBOA_AIR3);

	return S_OK;
}

HRESULT BT_Air_Att3::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT BT_Air_Att3::StateUpdate(const float _fDeltaTime)
{
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.78 <= fCurAnimationTime)
		m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);

	if (0.57 <= fCurAnimationTime && fCurAnimationTime <= 0.92)
		NeroState::KeyInput_Jump(NeroFSM::AIR_COMBOA3);

	if (0.96 <= fCurAnimationTime)
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
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

	m_pNero.lock()->ChangeAnimation("ComboB_Air", false, Nero::ANI_COMBOB_AIR);

	return S_OK;
}

HRESULT BT_Air_ComboB::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT BT_Air_ComboB::StateUpdate(const float _fDeltaTime)
{
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.82 <= fCurAnimationTime)
		m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);

	if (0.58f <= fCurAnimationTime && fCurAnimationTime <= 0.93f)
		NeroState::KeyInput_Jump(NeroFSM::AIR_COMBOA3);

	if (0.96 <= fCurAnimationTime)
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
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
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Split_Start", false, Nero::ANI_SPLIT_START);

	return S_OK;
}

HRESULT Skill_Split::StateExit()
{
	return S_OK;
}

HRESULT Skill_Split::StateUpdate(const float _fDeltaTime)
{
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96 <= fCurAnimationTime)
		m_pFSM->ChangeState(NeroFSM::SKILL_SPLIT_LOOP);

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
	NeroState::StateEnter();
	m_pNero.lock()->StopAnimation();
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
	//테스트

	if (Input::GetKey(DIK_M))
		m_pFSM->ChangeState(NeroFSM::SKILL_SPLIT_END);

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
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Split_Landing", false, Nero::ANI_SPLIT_LANDING);
	return S_OK;
}

HRESULT Skill_Split_Landing::StateExit()
{
	NeroState::StateEnter();
	return S_OK;
}

HRESULT Skill_Split_Landing::StateUpdate(const float _fDeltaTime)
{
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.37 <= fCurAnimationTime)
		NeroState::KeyInput_Idle(NeroFSM::SKILL_SPLIT_END);

	if (0.96 <= fCurAnimationTime)
		m_pFSM->ChangeState(NeroFSM::IDLE);
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
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Hr_Ground", false, Nero::ANI_HR_GROUND);

	return S_OK;
}

HRESULT Skill_Float_Ground::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Skill_Float_Ground::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.45 <= fCurrAnimationTime)
		NeroState::KeyInput_Idle(NeroFSM::SKILL_FLOAT_GROUND);

	if(0.96 <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::IDLE);
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
	return S_OK;
}

HRESULT Skill_Float_Ground_Ex3::StateExit()
{
	return S_OK;
}

HRESULT Skill_Float_Ground_Ex3::StateUpdate(const float _fDeltaTime)
{
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
	return S_OK;
}

HRESULT Skill_Float_Ground_Finish::StateExit()
{
	return S_OK;
}

HRESULT Skill_Float_Ground_Finish::StateUpdate(const float _fDeltaTime)
{
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
	NeroState::StateEnter();
	//EX게이지에 따라서 분기
	//게이지없을때

	
	m_pNero.lock()->ChangeAnimation("Shuffle", false, Nero::ANI_SHUFFLE);
	//게이지 있을때
	//m_pNero.lock()->ChangeAnimation("Shuffle_Ex", false, Nero::ANI_SHUFFLE_EX);
	return S_OK;
}

HRESULT Skill_Shuffle::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Skill_Shuffle::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.55 <= fCurrAnimationTime)
		NeroState::KeyInput_Idle(NeroFSM::SKILL_SHUFFLE);

	if(0.96 <= fCurrAnimationTime)
		m_pFSM->ChangeState(NeroFSM::IDLE);
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
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Streak_Start", false, Nero::ANI_STREAK_START);

	return S_OK;
}

HRESULT Skill_Streak::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Skill_Streak::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96 <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::SKILL_STREAK_LOOP);
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
	NeroState::StateEnter();

	m_pNero.lock()->ChangeAnimation("Streak_Ex_Start", false, Nero::ANI_STREAK_EX_START);
	m_pNero.lock()->DecreaseRQ_Gage();
	return S_OK;
}

HRESULT Skill_Streak_Ex3::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Skill_Streak_Ex3::StateUpdate(const float _fDeltaTime)
{
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.95 <= fCurAnimationTime)
		m_pFSM->ChangeState(NeroFSM::SKILL_STREAK_EX3_RUSH);
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
	m_fLoopTime = 1.f;
	return S_OK;
}

HRESULT Skill_Streak_Loop::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Skill_Streak_Loop::StateUpdate(const float _fDeltaTime)
{
	m_fLoopTime -= _fDeltaTime;
	if (m_fLoopTime < 0.f)
		m_pFSM->ChangeState(NeroFSM::SKILL_STREAK_END);
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
	return S_OK;
}

HRESULT Skill_Streak_End::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Skill_Streak_End::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (0.5 <= fCurrAnimationTime)
		NeroState::KeyInput_Idle(NeroFSM::SKILL_STREAK_END);
	if (0.96 <= fCurrAnimationTime)
		m_pFSM->ChangeState(NeroFSM::IDLE);
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

	return S_OK;
}

HRESULT Skill_Streak_Ex3_Rush::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Skill_Streak_Ex3_Rush::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if(0.65 <= fCurrAnimationTime)
		m_pFSM->ChangeState(NeroFSM::SKILL_STREAK_EX3_ROLL_LOOP);

	return S_OK;
}

Skill_Streak_Ex3_Roll_Loop::Skill_Streak_Ex3_Roll_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero), m_iLoopCount(0)
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
	m_iLoopCount = 2;

	return S_OK;
}

HRESULT Skill_Streak_Ex3_Roll_Loop::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Skill_Streak_Ex3_Roll_Loop::StateUpdate(const float _fDeltaTime)
{
	float fAccTime = m_pNero.lock()->Get_PlayingAccTime();


	if (3.96 <= fAccTime)
		m_pFSM->ChangeState(NeroFSM::SKILL_STREAK_EX3_ROLL_END);

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


	return S_OK;
}

HRESULT Skill_Streak_Ex3_Roll_End::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Skill_Streak_Ex3_Roll_End::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if(0.96 <= fCurrAnimationTime)
		m_pFSM->ChangeState(NeroFSM::IDLE);
	return S_OK;
}

#pragma endregion

#pragma region GT


GT_Equip::GT_Equip(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

GT_Equip::~GT_Equip()
{
}

GT_Equip* GT_Equip::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new GT_Equip(_pFSM, _nIndex, _pNero);
}

HRESULT GT_Equip::StateEnter()
{
	return S_OK;
}

HRESULT GT_Equip::StateExit()
{
	return S_OK;
}

HRESULT GT_Equip::StateUpdate(const float _fDeltaTime)
{
	return S_OK;
}

GT_Crush_Reload::GT_Crush_Reload(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

GT_Crush_Reload::~GT_Crush_Reload()
{
}

GT_Crush_Reload* GT_Crush_Reload::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new GT_Crush_Reload(_pFSM, _nIndex, _pNero);
}

HRESULT GT_Crush_Reload::StateEnter()
{
	return S_OK;
}

HRESULT GT_Crush_Reload::StateExit()
{
	return S_OK;
}

HRESULT GT_Crush_Reload::StateUpdate(const float _fDeltaTime)
{
	return S_OK;
}

GT_Crush_Just::GT_Crush_Just(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

GT_Crush_Just::~GT_Crush_Just()
{
}

GT_Crush_Just* GT_Crush_Just::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new GT_Crush_Just(_pFSM, _nIndex, _pNero);
}

HRESULT GT_Crush_Just::StateEnter()
{
	return S_OK;
}

HRESULT GT_Crush_Just::StateExit()
{
	return S_OK;
}

HRESULT GT_Crush_Just::StateUpdate(const float _fDeltaTime)
{
	return S_OK;
}

GT_Bomb::GT_Bomb(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

GT_Bomb::~GT_Bomb()
{
}

GT_Bomb* GT_Bomb::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new GT_Bomb(_pFSM, _nIndex, _pNero);
}

HRESULT GT_Bomb::StateEnter()
{
	return S_OK;
}

HRESULT GT_Bomb::StateExit()
{
	return S_OK;
}

HRESULT GT_Bomb::StateUpdate(const float _fDeltaTime)
{
	return S_OK;
}

GT_PickUp::GT_PickUp(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

GT_PickUp::~GT_PickUp()
{
}

GT_PickUp* GT_PickUp::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new GT_PickUp(_pFSM, _nIndex, _pNero);
}

HRESULT GT_PickUp::StateEnter()
{
	return S_OK;
}

HRESULT GT_PickUp::StateExit()
{
	return S_OK;
}

HRESULT GT_PickUp::StateUpdate(const float _fDeltaTime)
{
	return S_OK;
}

GT_Air_Crush_Reload::GT_Air_Crush_Reload(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

GT_Air_Crush_Reload::~GT_Air_Crush_Reload()
{
}

GT_Air_Crush_Reload* GT_Air_Crush_Reload::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new GT_Air_Crush_Reload(_pFSM, _nIndex, _pNero);
}

HRESULT GT_Air_Crush_Reload::StateEnter()
{
	return S_OK;
}

HRESULT GT_Air_Crush_Reload::StateExit()
{
	return S_OK;
}

HRESULT GT_Air_Crush_Reload::StateUpdate(const float _fDeltaTime)
{
	return S_OK;
}

GT_Air_Crush_Just::GT_Air_Crush_Just(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

GT_Air_Crush_Just::~GT_Air_Crush_Just()
{
}

GT_Air_Crush_Just* GT_Air_Crush_Just::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new GT_Air_Crush_Just(_pFSM, _nIndex, _pNero);
}

HRESULT GT_Air_Crush_Just::StateEnter()
{
	return S_OK;
}

HRESULT GT_Air_Crush_Just::StateExit()
{
	return S_OK;
}

HRESULT GT_Air_Crush_Just::StateUpdate(const float _fDeltaTime)
{
	return S_OK;
}

GT_Air_Bomb::GT_Air_Bomb(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
{
}

GT_Air_Bomb::~GT_Air_Bomb()
{
}

GT_Air_Bomb* GT_Air_Bomb::Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
{
	return new GT_Air_Bomb(_pFSM, _nIndex, _pNero);
}

HRESULT GT_Air_Bomb::StateEnter()
{
	return S_OK;
}

HRESULT GT_Air_Bomb::StateExit()
{
	return S_OK;
}

HRESULT GT_Air_Bomb::StateUpdate(const float _fDeltaTime)
{
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

	return S_OK;
}

HRESULT Overture_Shoot::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Overture_Shoot::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.6 <= fCurrAnimationTime)
		NeroState::KeyInput_Idle(NeroFSM::OVERTURE_SHOOT);

	if (0.96 <= fCurrAnimationTime)
		m_pFSM->ChangeState(NeroFSM::IDLE);

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

	return S_OK;
}

HRESULT Overture_Shoot_Up::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Overture_Shoot_Up::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.53 <= fCurrAnimationTime)
		NeroState::KeyInput_Idle(NeroFSM::OVERTURE_SHOOT_UP);

	if (0.96 <= fCurrAnimationTime)
		m_pFSM->ChangeState(NeroFSM::IDLE);

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


	return S_OK;
}

HRESULT Overture_Shoot_Down::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Overture_Shoot_Down::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.5 <= fCurrAnimationTime)
		NeroState::KeyInput_Idle(NeroFSM::OVERTURE_SHOOT_DOWN);

	if(0.96 <= fCurrAnimationTime)
		m_pFSM->ChangeState(NeroFSM::IDLE);

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
	NeroState::ResetAnimation(0.96, Nero::ANI_OVERTURE_SHOOT_AIR);

	return S_OK;
}

HRESULT Overture_Shoot_Air::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Overture_Shoot_Air::StateUpdate(const float _fDeltaTime)
{
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.65 <= fCurAnimationTime && fCurAnimationTime <= 0.93)
		NeroState::KeyInput_Jump(NeroFSM::OVERTURE_SHOOT_AIR);
	if (0.96 <= fCurAnimationTime)
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
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
	NeroState::ResetAnimation(0.96, Nero::ANI_OVERTURE_SHOOT_AIR_UP);
	
	return S_OK;
}

HRESULT Overture_Shoot_Air_Up::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Overture_Shoot_Air_Up::StateUpdate(const float _fDeltaTime)
{
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.65 <= fCurAnimationTime && fCurAnimationTime <= 0.93)
		NeroState::KeyInput_Jump(NeroFSM::OVERTURE_SHOOT_AIR_UP);
	if (0.96 <= fCurAnimationTime)
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
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
	NeroState::ResetAnimation(0.96, Nero::ANI_OVERTURE_SHOOT_AIR_DOWN);


	return S_OK;
}

HRESULT Overture_Shoot_Air_Down::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Overture_Shoot_Air_Down::StateUpdate(const float _fDeltaTime)
{
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.65 <= fCurAnimationTime && fCurAnimationTime <= 0.93)
		NeroState::KeyInput_Jump(NeroFSM::OVERTURE_SHOOT_AIR_DOWN);
	if (0.96 <= fCurAnimationTime)
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
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
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Cbs_ComboA1", false, Nero::ANI_CBS_COMBOA1);

	return S_OK;
}

HRESULT Cbs_ComboA1::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Cbs_ComboA1::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (Input::GetMouseDown(DIM_L))
	{
		if (0.27 <= fCurrAnimationTime && fCurrAnimationTime <= 0.37)
			m_pFSM->ChangeState(NeroFSM::CBS_COMBOA2);
	}
	else if(0.96 <= fCurrAnimationTime)
		m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
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


	return S_OK;
}

HRESULT Cbs_ComboA2::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Cbs_ComboA2::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (Input::GetMouseDown(DIM_L))
	{
		if (0.28 <= fCurrAnimationTime && fCurrAnimationTime <= 0.38)
			m_pFSM->ChangeState(NeroFSM::CBS_COMBOA3);
	}
	else if (0.96 <= fCurrAnimationTime)
		m_pFSM->ChangeState(NeroFSM::CBS_IDLE);

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


	return S_OK;
}

HRESULT Cbs_ComboA3::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Cbs_ComboA3::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (Input::GetMouseDown(DIM_L))
	{
		if (0.3 <= fCurrAnimationTime && fCurrAnimationTime <= 0.4)
			m_pFSM->ChangeState(NeroFSM::CBS_COMBOA4);
	}
	else if (0.96 <= fCurrAnimationTime)
		m_pFSM->ChangeState(NeroFSM::CBS_IDLE);

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


	return S_OK;
}

HRESULT Cbs_ComboA4::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Cbs_ComboA4::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (Input::GetMouseDown(DIM_L))
	{
		if (0.28 <= fCurrAnimationTime && fCurrAnimationTime <= 0.38)
			m_pFSM->ChangeState(NeroFSM::CBS_COMBOA5);
	}
	else if (0.96 <= fCurrAnimationTime)
		m_pFSM->ChangeState(NeroFSM::CBS_IDLE);

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


	return S_OK;
}

HRESULT Cbs_ComboA5::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Cbs_ComboA5::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (Input::GetMouseDown(DIM_L))
	{
		if (0.8 <= fCurrAnimationTime && Input::GetMouseDown(DIM_L))
			m_pFSM->ChangeState(NeroFSM::CBS_COMBOA1);
	}
	else if (0.96 <= fCurrAnimationTime)
		m_pFSM->ChangeState(NeroFSM::CBS_IDLE);

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


	return S_OK;
}

HRESULT Cbs_SKill_Crystal::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Cbs_SKill_Crystal::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96 <= fCurrAnimationTime)
		m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
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
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Cbs_SKill_IceAge_Start", false, Nero::ANI_CBS_SKILL_ICEAGE_START);

	return S_OK;
}

HRESULT Cbs_SKill_IceAge_Start::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Cbs_SKill_IceAge_Start::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96 <= fCurrAnimationTime)
		m_pFSM->ChangeState(NeroFSM::CBS_ICEAGE_LOOP);

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
	m_fLoopTime = 1.5f;
	return S_OK;
}

HRESULT Cbs_SKill_IceAge_Loop::StateExit()
{
	NeroState::StateExit();
	m_fLoopTime = 0.f;
	return S_OK;
}

HRESULT Cbs_SKill_IceAge_Loop::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	
	m_fLoopTime -= _fDeltaTime;
	if (m_fLoopTime < 0.f)
		m_pFSM->ChangeState(NeroFSM::CBS_ICEAGE_END);

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
	return S_OK;
}

HRESULT Cbs_SKill_IceAge_End::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Cbs_SKill_IceAge_End::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96 <= fCurrAnimationTime)
		m_pFSM->ChangeState(NeroFSM::CBS_IDLE);

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
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Cbs_SKill_Revolver_Start", false, Nero::ANI_CBS_SKILL_REVOLVER_START);
	return S_OK;
}

HRESULT Cbs_SKill_Revolver_Start::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Cbs_SKill_Revolver_Start::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96 <= fCurrAnimationTime)
		m_pFSM->ChangeState(NeroFSM::CBS_REVOLVER_LOOP);

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
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Cbs_SKill_Revolver_Loop", true, Nero::ANI_CBS_SKILL_REVOLVER_LOOP);
	//얘는 그냥 땅에 닿으면 멈추는걸로 바꿔야됨
	m_iLoopCount = 10;
	return S_OK;
}

HRESULT Cbs_SKill_Revolver_Loop::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Cbs_SKill_Revolver_Loop::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	//얘는 그냥 땅에 닿으면 멈추는걸로 바꿔야됨
	if (m_iLoopCount <= 0 && 0.96 <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::CBS_REVOLVER_END);
	}
	else if (0.98 <= fCurrAnimationTime)
	{
		--m_iLoopCount;
	}
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
	return S_OK;
}

HRESULT Cbs_SKill_Revolver_End::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Cbs_SKill_Revolver_End::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96 <= fCurrAnimationTime)
		m_pFSM->ChangeState(NeroFSM::CBS_IDLE);

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
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Cbs_SKill_Swing", false, Nero::ANI_CBS_SKILL_SWING);

	return S_OK;
}

HRESULT Cbs_SKill_Swing::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Cbs_SKill_Swing::StateUpdate(const float _fDeltaTime)
{
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

	return S_OK;
}

HRESULT Middle_Cbs_BiAttack::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Middle_Cbs_BiAttack::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96 <= fCurrAnimationTime)
		m_pFSM->ChangeState(NeroFSM::CBS_IDLE);

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

	return S_OK;
}

HRESULT Middle_Cbs_BlitzAttack::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Middle_Cbs_BlitzAttack::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96 <= fCurrAnimationTime)
		m_pFSM->ChangeState(NeroFSM::CBS_IDLE);

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

	return S_OK;
}

HRESULT Middle_Cbs_Satellite::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Middle_Cbs_Satellite::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96 <= fCurrAnimationTime)
		m_pFSM->ChangeState(NeroFSM::CBS_IDLE);

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
	return S_OK;
}

HRESULT Middle_Cbs_Satellite_Air::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Middle_Cbs_Satellite_Air::StateUpdate(const float _fDeltaTime)
{
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

	return S_OK;
}

HRESULT Middle_Cbs_Strike::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Middle_Cbs_Strike::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96 <= fCurrAnimationTime)
		m_pFSM->ChangeState(NeroFSM::CBS_IDLE);

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

	return S_OK;
}

HRESULT Middle_Cbs_Strike_Air::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Middle_Cbs_Strike_Air::StateUpdate(const float _fDeltaTime)
{
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

	return S_OK;
}

HRESULT Middle_Cbs_Strike_Air_Down::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Middle_Cbs_Strike_Air_Down::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96 <= fCurrAnimationTime)
		m_pFSM->ChangeState(NeroFSM::CBS_IDLE);

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


	return S_OK;
}

HRESULT Middle_Cbs_Strike_Air_Up::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Middle_Cbs_Strike_Air_Up::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96 <= fCurrAnimationTime)
		m_pFSM->ChangeState(NeroFSM::CBS_IDLE);

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

	return S_OK;
}

HRESULT Middle_Cbs_Strike_Down::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Middle_Cbs_Strike_Down::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96 <= fCurrAnimationTime)
		m_pFSM->ChangeState(NeroFSM::CBS_IDLE);

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

	return S_OK;
}

HRESULT Middle_Cbs_Strike_Up::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Middle_Cbs_Strike_Up::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96 <= fCurrAnimationTime)
		m_pFSM->ChangeState(NeroFSM::CBS_IDLE);

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

	return S_OK;
}

HRESULT Middle_Cbs_ThunderBullet::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Middle_Cbs_ThunderBullet::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96 <= fCurrAnimationTime)
		m_pFSM->ChangeState(NeroFSM::CBS_IDLE);

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

	return S_OK;
}

HRESULT Pole_ComboA1::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Pole_ComboA1::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (Input::GetMouseDown(DIM_M))
	{
		if (0.2f <= fCurrAnimationTime && fCurrAnimationTime <= 0.29f)
		{
			m_pFSM->ChangeState(NeroFSM::POLE_COMBOA2);
		}

		else if (0.31f <= fCurrAnimationTime && fCurrAnimationTime <= 0.42f)
		{
			// 레드퀸 콤보 땅바닥 찍는거 시작 
			m_pFSM->ChangeState(NeroFSM::POLE_COMBOB1);
		}
	}
	else if (0.96f <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
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

	return S_OK;
}

HRESULT Pole_ComboA2::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Pole_ComboA2::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (Input::GetMouseDown(DIM_M))
	{
		if (0.2f <= fCurrAnimationTime && fCurrAnimationTime <= 0.29f)
		{
			m_pFSM->ChangeState(NeroFSM::POLE_COMBOA3);
		}
	}
	else if (0.96f <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
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

	return S_OK;
}

HRESULT Pole_ComboA3::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Pole_ComboA3::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (Input::GetMouseDown(DIM_M))
	{
		//if (0.2f <= fCurrAnimationTime && fCurrAnimationTime <= 0.29f)
		//{
		//	m_pFSM->ChangeState(NeroFSM::POLE_COMBOA3);
		//}
	}
	else if (0.96f <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
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

	return S_OK;
}

HRESULT Pole_ComboB1::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Pole_ComboB1::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (Input::GetMouseDown(DIM_M))
	{
		if (0.36f <= fCurrAnimationTime && fCurrAnimationTime <= 0.46f)
		{
			m_pFSM->ChangeState(NeroFSM::POLE_COMBOB2);
		}
	}
	else if (0.96f <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
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

	return S_OK;
}

HRESULT Pole_ComboB2::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Pole_ComboB2::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (Input::GetMouseDown(DIM_M))
	{
		if (0.39f <= fCurrAnimationTime && fCurrAnimationTime <= 0.48f)
		{
			m_pFSM->ChangeState(NeroFSM::POLE_COMBOB3);
		}
	}
	else if (0.96f <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
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

	return S_OK;
}

HRESULT Pole_ComboB3::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Pole_ComboB3::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (Input::GetMouseDown(DIM_M))
	{
		if (0.44f <= fCurrAnimationTime && fCurrAnimationTime <= 0.53f)
		{
			m_pFSM->ChangeState(NeroFSM::POLE_COMBOB4);
		}
	}
	else if (0.96f <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
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

	return S_OK;
}

HRESULT Pole_ComboB4::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Pole_ComboB4::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();
	if (Input::GetMouseDown(DIM_L))
	{
		if (0.88f <= fCurrAnimationTime)
		{
			m_pFSM->ChangeState(NeroFSM::CBS_COMBOA1);
		}
	}
	else if (0.96f <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::CBS_IDLE);
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

	return S_OK;
}

HRESULT Pole_WhirlWind_Start::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Pole_WhirlWind_Start::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96 <= fCurrAnimationTime)
		m_pFSM->ChangeState(NeroFSM::POLE_WHIRLWIND_LOOP);

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
	
	return S_OK;
}

HRESULT Pole_WhirlWind_Loop::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Pole_WhirlWind_Loop::StateUpdate(const float _fDeltaTime)
{
	m_fLoopTime -= _fDeltaTime;

	if (m_fLoopTime < 0.f)
		m_pFSM->ChangeState(NeroFSM::POLE_WHIRLWIND_END);
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

	return S_OK;
}

HRESULT Pole_WhirlWind_End::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Pole_WhirlWind_End::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96 <= fCurrAnimationTime)
		m_pFSM->ChangeState(NeroFSM::CBS_IDLE);

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
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("ComboA1_Dash", false, Nero::ANI_COMBOA1_DASH);

	return S_OK;
}

HRESULT ComboA_Dash::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT ComboA_Dash::StateUpdate(const float _fDeltaTime)
{
	float fCurrAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.18f <= fCurrAnimationTime && fCurrAnimationTime <= 0.35f)
	{
		KeyInput_Idle(NeroFSM::ATT2);
	}
	else if (0.37f <= fCurrAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::IDLE_START);
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
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Caliber_Start", false, Nero::ANI_CALIBER_START);

	return S_OK;
}

HRESULT Skill_Caliber::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Skill_Caliber::StateUpdate(const float _fDeltaTime)
{
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96 <= fCurAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::SKILL_CALIBER_END);
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

	return S_OK;
}

HRESULT Skill_Caliber_End::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Skill_Caliber_End::StateUpdate(const float _fDeltaTime)
{
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.8 <= fCurAnimationTime)
		m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);

	if (0.96 <= fCurAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
	}

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
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Hr_Air", false, Nero::ANI_HR_AIR);

	return S_OK;
}

HRESULT Hr_Air::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Hr_Air::StateUpdate(const float _fDeltaTime)
{
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.83 <= fCurAnimationTime)
	{
		m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
		NeroState::KeyInput_Jump(NeroFSM::SKILL_HR_AIR);
	}

	if (0.96 <= fCurAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
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
	NeroState::StateEnter();

	m_pNero.lock()->ChangeAnimation("Hr_Ex_Start", false, Nero::ANI_HR_EX_START);

	return S_OK;
}

HRESULT Hr_Ex_Start::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Hr_Ex_Start::StateUpdate(const float _fDeltaTime)
{
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();
	UINT RQ_Gage = m_pNero.lock()->Get_RQ_Gage();

	if (0 < RQ_Gage)
	{
		//게이지 있을때	
		if (0.95 <= fCurAnimationTime)
		{
			m_pFSM->ChangeState(NeroFSM::SKILL_HR_EX_AIR_ROLL_START);
		}
	}
	else
	{
		//게이지 없을때
		if (0.95 <= fCurAnimationTime)
			m_pFSM->ChangeState(NeroFSM::SKILL_HR_AIR);
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
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96 <= fCurAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::IDLE);
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
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Hr_Ex_Air_Roll_Start", false, Nero::ANI_HR_EX_AIR_ROLL_START);
	m_pNero.lock()->DecreaseRQ_Gage();
	return S_OK;
}

HRESULT Hr_Ex_Air_Roll_Start::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Hr_Ex_Air_Roll_Start::StateUpdate(const float _fDeltaTime)
{
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.94 <= fCurAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::SKILL_HR_EX_AIR_ROLL_LOOP);
	}
	return S_OK;
}

Hr_Ex_Air_Roll_Loop::Hr_Ex_Air_Roll_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero)
	:NeroState(_pFSM, _nIndex, _pNero)
	, m_iLoopCount(0)
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
	//m_pNero.lock()->ChangeAnimation("Hr_Ex_Air_Roll_Loop", true, Nero::ANI_HR_EX_AIR_ROLL_LOOP);
	m_pNero.lock()->ChangeAnimation("Hr_Ex_Air_Roll_Loop", true, Nero::ANI_HR_EX_AIR_ROLL_LOOP);
	m_iLoopCount = 2;
	return S_OK;
}

HRESULT Hr_Ex_Air_Roll_Loop::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Hr_Ex_Air_Roll_Loop::StateUpdate(const float _fDeltaTime)
{
	float fCurrAccTime = m_pNero.lock()->Get_PlayingAccTime();

	if (3.94 <= fCurrAccTime)
		m_pFSM->ChangeState(NeroFSM::SKILL_HR_EX_AIR_ROLL_END);


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

	return S_OK;
}

HRESULT Hr_Ex_Air_Roll_End::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Hr_Ex_Air_Roll_End::StateUpdate(const float _fDeltaTime)
{
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.83 <= fCurAnimationTime)
	{
		m_pNero.lock()->Set_RQ_State(Nero::WS_Idle);
		KeyInput_Jump(NeroFSM::SKILL_HR_EX_AIR_ROLL_END);
	}


	if (0.96 <= fCurAnimationTime)
		m_pFSM->ChangeState(NeroFSM::JUMP_LOOP);
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
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("SplitEx_Start", false, Nero::ANI_SPLITEX_START);
	m_pNero.lock()->DecreaseRQ_Gage();

	return S_OK;
}

HRESULT Skill_Split_Ex::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Skill_Split_Ex::StateUpdate(const float _fDeltaTime)
{
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96 <= fCurAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::SKILL_SPLIT_EX_LOOP);
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
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("SplitEx_Loop", true, Nero::ANI_SPLITEX_LOOP);

	return S_OK;
}

HRESULT Skill_Split_Ex_Loop::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Skill_Split_Ex_Loop::StateUpdate(const float _fDeltaTime)
{

	//땅에 닿았을때
	if(Input::GetKey(DIK_M))
		m_pFSM->ChangeState(NeroFSM::SKILL_SPLIT_EX_END);
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
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("SplitEx_Landing", false, Nero::ANI_SPLITEX_LANDING);

	return S_OK;
}

HRESULT Skill_Split_Ex_Landing::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Skill_Split_Ex_Landing::StateUpdate(const float _fDeltaTime)
{
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.37 <= fCurAnimationTime)
		NeroState::KeyInput_Idle(NeroFSM::SKILL_SPLIT_EX_END);

	if (0.96 <= fCurAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::IDLE);
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
	NeroState::StateEnter();
	m_pNero.lock()->ChangeAnimation("Air_Dive_Slash_Start", false, Nero::ANI_AIR_DIVE_SLASH_START);

	return S_OK;
}

HRESULT Air_Dive_Slash_Start::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Air_Dive_Slash_Start::StateUpdate(const float _fDeltaTime)
{
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96 <= fCurAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::SKILL_AIR_DIVE_SLASH_LOOP);
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
	//m_pNero.lock()->ChangeAnimation("Air_Dive_Slash_Loop", true, Nero::ANI_AIR_DIVE_SLASH_LOOP);
	m_pNero.lock()->StopAnimation();
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
	// 땅에 닿았을때

	//테스트용

	if (Input::GetKey(DIK_M))
	{
		m_pFSM->ChangeState(NeroFSM::SKILL_AIR_DIVE_SLASH_END);
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
	m_pNero.lock()->ChangeAnimation("Air_Dive_Slash_Finish", false, Nero::ANI_AIR_DIVE_SLASH_FINISH);

	return S_OK;
}

HRESULT Air_Dive_Slash_End::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Air_Dive_Slash_End::StateUpdate(const float _fDeltaTime)
{
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.53 <= fCurAnimationTime)
	{
		NeroState::KeyInput_Idle(NeroFSM::SKILL_AIR_DIVE_SLASH_END);
	}


	if (0.96 <= fCurAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::IDLE);
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
	NeroState::StateEnter();

	m_pNero.lock()->ChangeAnimation("Shuffle_Ex", false, Nero::ANI_SHUFFLE_EX);
	m_pNero.lock()->DecreaseRQ_Gage();
	return S_OK;
}

HRESULT Skill_Shuffle_Ex::StateExit()
{
	NeroState::StateExit();
	return S_OK;
}

HRESULT Skill_Shuffle_Ex::StateUpdate(const float _fDeltaTime)
{
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.96 <= fCurAnimationTime)
		m_pFSM->ChangeState(NeroFSM::IDLE);
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
	NeroState::StateEnter();


	m_pNero.lock()->ChangeAnimation("Hr_Ex_Start", false, Nero::ANI_HR_EX_2);

	return S_OK;
}

HRESULT Skill_Float_Ground_Ex3_Start::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Skill_Float_Ground_Ex3_Start::StateUpdate(const float _fDeltaTime)
{
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.95 <= fCurAnimationTime)
	{
		m_pFSM->ChangeState(NeroFSM::SKILL_FLOAT_GROUND_EX3);
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
	m_pNero.lock()->SetActive_Buster_Arm(true);
	m_pNero.lock()->Change_BusterArm_Animation("Buster_Catch", false);
	return S_OK;
}

HRESULT Buster_Start::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT Buster_Start::StateUpdate(const float _fDeltaTime)
{
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.95 <= fCurAnimationTime)
		m_pFSM->ChangeState(NeroFSM::IDLE);

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
	m_pNero.lock()->SetActive_Wings(true);

	return S_OK;
}

HRESULT To_Majin::StateExit()
{
	NeroState::StateExit();

	return S_OK;
}

HRESULT To_Majin::StateUpdate(const float _fDeltaTime)
{
	float fCurAnimationTime = m_pNero.lock()->Get_PlayingTime();

	if (0.95 <= fCurAnimationTime)
		m_pFSM->ChangeState(NeroFSM::IDLE);

	return S_OK;
}
