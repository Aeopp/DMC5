#pragma once
#ifndef NeroFSM_h__
#define NeroFSM_h__

#include "FSMBase.h"
class Nero;
class NeroFSM :    public FSMBase
{
public:
	enum STATE
	{
		IDLE,
		IDLE_START,
		ATT1,
		ATT2,
		ATT3,
		ATT4,
		RUNSTART,
		RUNSTART_L,
		RUNSTART_R,
		RUNSTART_180,
		RUNLOOP,
		RUNSTOP,
		DASHLOOP,
		DASHSTOP,
		DASHTURN,
		JOGLOOP,
		JOGSTOP,
		JOG_TURN_180,
		JOG_TURN_180L,
		ATT_COMBO_C_R,
		ATT_COMBO_C_L,
		ATT_COMBO_C1,
		ATT_COMBO_C2,
		ATT_COMBO_C3,
		ATT_COMBO_C4,
		ATT_COMBO_D1,
		ATT_COMBO_D2,
		ATT_COMBO_D3,
		ATT_COMBO_D4,
		SKILL_STREAK,
		SKILL_STREAK_LOOP,
		SKILL_STREAK_END,
		SKILL_STREAK_EX3,
		SKILL_STREAK_EX3_RUSH,
		SKILL_STREAK_EX3_ROLL_LOOP,
		SKILL_STREAK_EX3_ROLL_END,
		SKILL_FLOAT_GROUND,
		SKILL_FLOAT_GROUND_EX3,
		SKILL_FLOAT_GROUND_EX3_START,
		SKILL_FLOAT_GROUND_FINISH,
		SKILL_SHUFFLE,
		SKILL_SHUFFLE_EX,
		OVERTURE_SHOOT,
		OVERTURE_SHOOT_DOWN,
		OVERTURE_SHOOT_UP,
		CBS_IDLE,
		CBS_COMBOA1,
		CBS_COMBOA2,
		CBS_COMBOA3,
		CBS_COMBOA4,
		CBS_COMBOA5,
		CBS_ICEAGE_START,
		CBS_ICEAGE_LOOP,
		CBS_ICEAGE_END,
		CBS_REVOLVER_START,
		CBS_REVOLVER_LOOP,
		CBS_REVOLVER_END,
		CBS_SWING,
		CBS_CRYSTAL,
		MIDDLE_CBS_BI_ATTACK,
		MIDDLE_CBS_BLITZATTACK,
		MIDDLE_CBS_SATELLITE,
		MIDDLE_CBS_SATELLITE_AIR,
		MIDDLE_CBS_STRIKE,
		MIDDLE_CBS_STRIKE_DOWN,
		MIDDLE_CBS_STRIKE_UP,
		MIDDLE_CBS_STRIKE_AIR,
		MIDDLE_CBS_STRIKE_AIR_DOWN,
		MIDDLE_CBS_STRIKE_AIR_UP,
		MIDDLE_CBS_THUNDERBULLET,
		POLE_COMBOA1,
		POLE_COMBOA2,
		POLE_COMBOA3,
		POLE_COMBOB1,
		POLE_COMBOB2,
		POLE_COMBOB3,
		POLE_COMBOB4,
		POLE_WHIRLWIND_START,
		POLE_WHIRLWIND_LOOP,
		POLE_WHIRLWIND_END,
		EVADE_L,
		EVADE_R,
		JUMP_START,
		JUMP_LOOP,
		JUMP_LANDING,
		JUMP_TWICE,
		SKILL_CALIBER_START,
		SKILL_CALIBER_END,
		SKILL_AIR_DIVE_SLASH_START,
		SKILL_AIR_DIVE_SLASH_LOOP,
		SKILL_AIR_DIVE_SLASH_END,
		SKILL_HR_EX_AIR_ROLL_START,
		SKILL_HR_EX_AIR_ROLL_LOOP,
		SKILL_HR_EX_AIR_ROLL_END,
		SKILL_HR_AIR,
		SKILL_SPLIT_START,
		SKILL_SPLIT_LOOP,
		SKILL_SPLIT_END,
		SKILL_SPLIT_EX_START,
		SKILL_SPLIT_EX_LOOP,
		SKILL_SPLIT_EX_END,
		AIR_COMBOA1,
		AIR_COMBOA2,
		AIR_COMBOA3,
		AIR_COMBOB,
		ATT1_DASH,
		OVERTURE_SHOOT_AIR,
		OVERTURE_SHOOT_AIR_UP,
		OVERTURE_SHOOT_AIR_DOWN,
		WIRE_SNATCH_PULL,
		WIRE_SNATCH_PULL_AIR, 
		WIRE_HELLHOUND_END,
		WIRE_HELLHOUND_START,
		WIRE_HELLHOUND_LOOP,
		//WIRE_HELLHOUND_AIR_END,
		//WIRE_HELLHOUND_AIR_START,
		//WIRE_HELLHOUND_AIR_LOOP,
		SKILL_HR_EX_START,
		BUSTER_START,
		TO_MAJIN,
		HIT_FRONT,
		BUSTER_AIR_CATCH,
		BUSTER_STRIKE_COMMON,
		BUSTER_STRIKE_COMMON_AIR,
		M_BUSTER_STRIKE_COMMON_START,
		M_BUSTER_STRIKE_COMMON_LOOP,
		EM0000_BUSTER_START,
		EM0000_BUSTER_FINISH,
		EM0000_M_BUSTER,
		EM0000_BUSTER_AIR,
		EM5000_BUSTER_START,
		EM5000_BUSTER_SWING,
		EM5000_BUSTER_SWING_LOOP,
		EM5000_BUSTER_FINISH,
		EM200_BUSTER_START,
		EM200_BUSTER_LOOP,
		EM200_BUSTER_FINSH,
		EM200_BUSTER_AIR_START,
		EM200_BUSTER_AIR_LOOP,
		EM200_BUSTER_AIR_FINSH,
		CBS_DASH,
		STATERESET,
		HIT_AIR_AWAY,
		HIT_GROUND_AWAY,
		STUN_START,
		STUN_LOOP,
		STUN_END,
		TRANSFORM_SHINMAJIN,
		TRANSFORM_ORIGIN_DANTE,
		SHINMAJIN_ENTER,
		SHINMAJIN_JUDGEMNET,
		SHINMAJIN_IDLE,
		WINDPRESSURE,
		WINDPRESSURE_END,
		CUTSCENE_WINDPRESSURE,
		PROVOKE1,
		PROVOKE3,
		PROVOKE9,
		PROVOKE10,
		DIE,
		DIE2,
		DIE2_LOOP,
		DIE_END,
		STATE_END
	};


private:
	weak_ptr<Nero>	m_pNero;

public:
	explicit NeroFSM(weak_ptr<Nero> _pNero);
	virtual ~NeroFSM();
public:
	static NeroFSM* Create(weak_ptr<Nero> _pNero);
public:
	// FSMBase을(를) 통해 상속됨
	virtual HRESULT ReadyFSM()							override;
	virtual HRESULT UpdateFSM(const float _fDeltaTime)	override;

};



#endif // NeroFSM_h__