#pragma once
#ifndef NeroState_h__
#define NeroState_h__

#include "FSMState.h"
class Nero;
class RedQueen;
class NeroState :    public FSMState
{
protected:
	weak_ptr<Nero>	m_pNero;
	//weak_ptr<Animator>	m_pAnimator;
public:
	explicit NeroState(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
	virtual ~NeroState();

public:
	// FSMState을(를) 통해 상속됨
	virtual HRESULT StateEnter()							PURE;
	virtual HRESULT StateExit()								PURE;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	PURE;
protected:
	virtual void	ResetAnimation(float fResetTiming,UINT _CheckIndex);
protected:
	virtual HRESULT KeyInput_Idle(const int _nIndex = -1);
	virtual HRESULT KeyInput_Run(const int _nIndex = -1);
	virtual HRESULT KeyInput_Cbs_Idle(const int _nIndex = -1);
	virtual HRESULT KeyInput_Jump(const int _nIndex = -1);
	virtual HRESULT KeyInput_Cbs_Jump(const int _nIndex = -1);
};

class Idle : public NeroState
{
private:
	explicit Idle(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Idle();

public:
	static Idle* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Idle_Start : public NeroState
{
private:
	explicit Idle_Start(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Idle_Start();

public:
	static Idle_Start* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Idle_Battle : public NeroState
{
private:
	explicit Idle_Battle(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Idle_Battle();

public:
	static Idle_Battle* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};




class Jump_Basic : public NeroState
{
private:
	explicit Jump_Basic(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Jump_Basic();

public:
	static Jump_Basic* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Jump_Fly_Loop : public NeroState
{
private:
	explicit Jump_Fly_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Jump_Fly_Loop();

public:
	static Jump_Fly_Loop* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Jump_Twice : public NeroState
{
private:
	explicit Jump_Twice(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Jump_Twice();

public:
	static Jump_Twice* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Jump_Front_Landing : public NeroState
{
private:
	explicit Jump_Front_Landing(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Jump_Front_Landing();

public:
	static Jump_Front_Landing* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class RunLoop : public NeroState
{
private:
	explicit RunLoop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~RunLoop();

public:
	static RunLoop* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class RunStartFront : public NeroState
{
private:
	explicit RunStartFront(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~RunStartFront();

public:
	static RunStartFront* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;

private:
	
};

class RunStartLeft : public NeroState
{
private:
	explicit RunStartLeft(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~RunStartLeft();

public:
	static RunStartLeft* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class RunStartRight : public NeroState
{
private:
	explicit RunStartRight(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~RunStartRight();

public:
	static RunStartRight* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class RunStart180 : public NeroState
{
private:
	explicit RunStart180(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~RunStart180();

public:
	static RunStart180* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class RunStop : public NeroState
{
private:
	explicit RunStop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~RunStop();

public:
	static RunStop* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class RunTurn : public NeroState
{
private:
	explicit RunTurn(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~RunTurn();

public:
	static RunTurn* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class RunTurn_L : public NeroState
{
private:
	explicit RunTurn_L(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~RunTurn_L();

public:
	static RunTurn_L* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class DashLoop : public NeroState
{
private:
	explicit DashLoop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~DashLoop();

public:
	static DashLoop* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class DashStop : public NeroState
{
private:
	explicit DashStop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~DashStop();

public:
	static DashStop* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class DashTurn : public NeroState
{
private:
	explicit DashTurn(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~DashTurn();

public:
	static DashTurn* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class WalkLoop : public NeroState
{
private:
	explicit WalkLoop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~WalkLoop();

public:
	static WalkLoop* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class WalkStart : public NeroState
{
private:
	explicit WalkStart(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~WalkStart();

public:
	static WalkStart* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class WalkStop : public NeroState
{
private:
	explicit WalkStop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~WalkStop();

public:
	static WalkStop* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Evade_L : public NeroState
{
private:
	explicit Evade_L(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Evade_L();

public:
	static Evade_L* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Evade_R : public NeroState
{
private:
	explicit Evade_R(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Evade_R();

public:
	static Evade_R* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class HitFront : public NeroState
{
private:
	explicit HitFront(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~HitFront();

public:
	static HitFront* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class HitFront_Big : public NeroState
{
private:
	explicit HitFront_Big(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~HitFront_Big();

public:
	static HitFront_Big* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Hit_Air : public NeroState
{
private:
	explicit Hit_Air(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Hit_Air();

public:
	static Hit_Air* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Wind_Pressure_Small : public NeroState
{
private:
	explicit Wind_Pressure_Small(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Wind_Pressure_Small();

public:
	static Wind_Pressure_Small* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Wind_Pressure_Big : public NeroState
{
private:
	explicit Wind_Pressure_Big(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Wind_Pressure_Big();

public:
	static Wind_Pressure_Big* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Wind_Pressure_Small_End : public NeroState
{
private:
	explicit Wind_Pressure_Small_End(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Wind_Pressure_Small_End();

public:
	static Wind_Pressure_Small_End* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Wind_Pressure_Big_End : public NeroState
{
private:
	explicit Wind_Pressure_Big_End(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Wind_Pressure_Big_End();

public:
	static Wind_Pressure_Big_End* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class KnockBack_Front : public NeroState
{
private:
	explicit KnockBack_Front(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~KnockBack_Front();

public:
	static KnockBack_Front* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class KnockBack_Get_Up : public NeroState
{
private:
	explicit KnockBack_Get_Up(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~KnockBack_Get_Up();

public:
	static KnockBack_Get_Up* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Die : public NeroState
{
private:
	explicit Die(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Die();

public:
	static Die* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Resurrection : public NeroState
{
private:
	explicit Resurrection(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Resurrection();

public:
	static Resurrection* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Resurrection_GetUp : public NeroState
{
private:
	explicit Resurrection_GetUp(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Resurrection_GetUp();

public:
	static Resurrection_GetUp* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};


class StunStart : public NeroState
{
private:
	explicit StunStart(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~StunStart();

public:
	static StunStart* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class StunLoop : public NeroState
{
private:
	explicit StunLoop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~StunLoop();

public:
	static StunLoop* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class StunEnd : public NeroState
{
private:
	explicit StunEnd(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~StunEnd();

public:
	static StunEnd* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class EarthQuakeStart : public NeroState
{
private:
	explicit EarthQuakeStart(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~EarthQuakeStart();

public:
	static EarthQuakeStart* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class EarthQuakeLoop : public NeroState
{
private:
	explicit EarthQuakeLoop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~EarthQuakeLoop();

public:
	static EarthQuakeLoop* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class EarthQuakeEnd : public NeroState
{
private:
	explicit EarthQuakeEnd(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~EarthQuakeEnd();

public:
	static EarthQuakeEnd* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Wire_Pull : public NeroState
{
private:
	explicit Wire_Pull(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Wire_Pull();

public:
	static Wire_Pull* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Wire_Pull_Up : public NeroState
{
private:
	explicit Wire_Pull_Up(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Wire_Pull_Up();

public:
	static Wire_Pull_Up* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Wire_Pull_Down : public NeroState
{
private:
	explicit Wire_Pull_Down(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Wire_Pull_Down();

public:
	static Wire_Pull_Down* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Wire_Fly_Start : public NeroState
{
private:
	explicit Wire_Fly_Start(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Wire_Fly_Start();

public:
	static Wire_Fly_Start* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Wire_Fly_Loop : public NeroState
{
private:
	explicit Wire_Fly_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Wire_Fly_Loop();

public:
	static Wire_Fly_Loop* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Wire_Fly_End : public NeroState
{
private:
	explicit Wire_Fly_End(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Wire_Fly_End();

public:
	static Wire_Fly_End* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Wire_Pull_Air : public NeroState
{
private:
	explicit Wire_Pull_Air(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Wire_Pull_Air();

public:
	static Wire_Pull_Air* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Wire_Pull_Air_Up : public NeroState
{
private:
	explicit Wire_Pull_Air_Up(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Wire_Pull_Air_Up();

public:
	static Wire_Pull_Air_Up* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Wire_Pull_Air_Down : public NeroState
{
private:
	explicit Wire_Pull_Air_Down(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Wire_Pull_Air_Down();

public:
	static Wire_Pull_Air_Down* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class BT_Att1 : public NeroState
{
private:
	explicit BT_Att1(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~BT_Att1();

public:
	static BT_Att1* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class BT_Att2 : public NeroState
{
private:
	explicit BT_Att2(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~BT_Att2();

public:
	static BT_Att2* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class BT_Att3 : public NeroState
{
private:
	explicit BT_Att3(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~BT_Att3();

public:
	static BT_Att3* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class BT_Att4 : public NeroState
{
private:
	explicit BT_Att4(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~BT_Att4();

public:
	static BT_Att4* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class BT_Att_ComboB : public NeroState
{
private:
	explicit BT_Att_ComboB(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~BT_Att_ComboB();

public:
	static BT_Att_ComboB* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class BT_Att_ComboC_R_to_L : public NeroState
{
private:
	explicit BT_Att_ComboC_R_to_L(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~BT_Att_ComboC_R_to_L();

public:
	static BT_Att_ComboC_R_to_L* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class BT_Att_ComboC_L_to_R : public NeroState
{
private:
	explicit BT_Att_ComboC_L_to_R(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~BT_Att_ComboC_L_to_R();

public:
	static BT_Att_ComboC_L_to_R* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class BT_Att_ComboC_1 : public NeroState
{
private:
	explicit BT_Att_ComboC_1(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~BT_Att_ComboC_1();

public:
	static BT_Att_ComboC_1* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class BT_Att_ComboC_2 : public NeroState
{
private:
	explicit BT_Att_ComboC_2(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~BT_Att_ComboC_2();

public:
	static BT_Att_ComboC_2* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class BT_Att_ComboC_3 : public NeroState
{
private:
	explicit BT_Att_ComboC_3(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~BT_Att_ComboC_3();

public:
	static BT_Att_ComboC_3* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class BT_Att_ComboC_4 : public NeroState
{
private:
	explicit BT_Att_ComboC_4(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~BT_Att_ComboC_4();

public:
	static BT_Att_ComboC_4* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class BT_Att_ComboD_1 : public NeroState
{
private:
	explicit BT_Att_ComboD_1(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~BT_Att_ComboD_1();

public:
	static BT_Att_ComboD_1* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class BT_Att_ComboD_2 : public NeroState
{
private:
	explicit BT_Att_ComboD_2(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~BT_Att_ComboD_2();

public:
	static BT_Att_ComboD_2* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class BT_Att_ComboD_3 : public NeroState
{
private:
	explicit BT_Att_ComboD_3(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~BT_Att_ComboD_3();

public:
	static BT_Att_ComboD_3* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class BT_Att_ComboD_4 : public NeroState
{
private:
	explicit BT_Att_ComboD_4(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~BT_Att_ComboD_4();

public:
	static BT_Att_ComboD_4* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class BT_Air_Att1 : public NeroState
{
private:
	explicit BT_Air_Att1(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~BT_Air_Att1();

public:
	static BT_Air_Att1* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class BT_Air_Att2 : public NeroState
{
private:
	explicit BT_Air_Att2(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~BT_Air_Att2();

public:
	static BT_Air_Att2* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class BT_Air_Att3 : public NeroState
{
private:
	explicit BT_Air_Att3(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~BT_Air_Att3();

public:
	static BT_Air_Att3* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class BT_Air_ComboB : public NeroState
{
private:
	explicit BT_Air_ComboB(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~BT_Air_ComboB();

public:
	static BT_Air_ComboB* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class BT_Parrying : public NeroState
{
private:
	explicit BT_Parrying(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~BT_Parrying();

public:
	static BT_Parrying* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Skill_Split : public NeroState
{
private:
	explicit Skill_Split(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Skill_Split();

public:
	static Skill_Split* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Skill_Split_Loop : public NeroState
{
private:
	explicit Skill_Split_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Skill_Split_Loop();

public:
	static Skill_Split_Loop* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
private:
	float	m_fLoopTime;
};

class Skill_Split_Landing : public NeroState
{
private:
	explicit Skill_Split_Landing(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Skill_Split_Landing();

public:
	static Skill_Split_Landing* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};



class Skill_Float_Ground : public NeroState
{
private:
	explicit Skill_Float_Ground(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Skill_Float_Ground();

public:
	static Skill_Float_Ground* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Skill_Float_Ground_Ex3 : public NeroState
{
private:
	explicit Skill_Float_Ground_Ex3(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Skill_Float_Ground_Ex3();

public:
	static Skill_Float_Ground_Ex3* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Skill_Float_Ground_Finish : public NeroState
{
private:
	explicit Skill_Float_Ground_Finish(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Skill_Float_Ground_Finish();

public:
	static Skill_Float_Ground_Finish* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Skill_Shuffle : public NeroState
{
private:
	explicit Skill_Shuffle(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Skill_Shuffle();

public:
	static Skill_Shuffle* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Skill_Shuffle_Ex : public NeroState
{
private:
	explicit Skill_Shuffle_Ex(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Skill_Shuffle_Ex();

public:
	static Skill_Shuffle_Ex* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};



class Skill_Streak : public NeroState
{
private:
	explicit Skill_Streak(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Skill_Streak();

public:
	static Skill_Streak* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Skill_Streak_Loop : public NeroState
{
private:
	explicit Skill_Streak_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Skill_Streak_Loop();

public:
	static Skill_Streak_Loop* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;

private:
	float	m_fLoopTime;
};

class Skill_Streak_End : public NeroState
{
private:
	explicit Skill_Streak_End(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Skill_Streak_End();

public:
	static Skill_Streak_End* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Skill_Streak_Ex3 : public NeroState
{
private:
	explicit Skill_Streak_Ex3(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Skill_Streak_Ex3();

public:
	static Skill_Streak_Ex3* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Skill_Streak_Ex3_Rush : public NeroState
{
private:
	explicit Skill_Streak_Ex3_Rush(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Skill_Streak_Ex3_Rush();

public:
	static Skill_Streak_Ex3_Rush* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Skill_Streak_Ex3_Roll_Loop : public NeroState
{
private:
	explicit Skill_Streak_Ex3_Roll_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Skill_Streak_Ex3_Roll_Loop();

public:
	static Skill_Streak_Ex3_Roll_Loop* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;

private:
	int		m_iLoopCount = 0;
};

class Skill_Streak_Ex3_Roll_End : public NeroState
{
private:
	explicit Skill_Streak_Ex3_Roll_End(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Skill_Streak_Ex3_Roll_End();

public:
	static Skill_Streak_Ex3_Roll_End* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class GT_Equip : public NeroState
{
private:
	explicit GT_Equip(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~GT_Equip();

public:
	static GT_Equip* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class GT_Crush_Reload : public NeroState
{
private:
	explicit GT_Crush_Reload(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~GT_Crush_Reload();

public:
	static GT_Crush_Reload* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};


class GT_Crush_Just : public NeroState
{
private:
	explicit GT_Crush_Just(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~GT_Crush_Just();

public:
	static GT_Crush_Just* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class GT_Bomb : public NeroState
{
private:
	explicit GT_Bomb(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~GT_Bomb();

public:
	static GT_Bomb* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class GT_PickUp : public NeroState
{
private:
	explicit GT_PickUp(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~GT_PickUp();

public:
	static GT_PickUp* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class GT_Air_Crush_Reload : public NeroState
{
private:
	explicit GT_Air_Crush_Reload(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~GT_Air_Crush_Reload();

public:
	static GT_Air_Crush_Reload* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};


class GT_Air_Crush_Just : public NeroState
{
private:
	explicit GT_Air_Crush_Just(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~GT_Air_Crush_Just();

public:
	static GT_Air_Crush_Just* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};


class GT_Air_Bomb : public NeroState
{
private:
	explicit GT_Air_Bomb(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~GT_Air_Bomb();

public:
	static GT_Air_Bomb* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Overture_Shoot : public NeroState
{
private:
	explicit Overture_Shoot(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Overture_Shoot();

public:
	static Overture_Shoot* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Overture_Shoot_Up : public NeroState
{
private:
	explicit Overture_Shoot_Up(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Overture_Shoot_Up();

public:
	static Overture_Shoot_Up* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Overture_Shoot_Down : public NeroState
{
private:
	explicit Overture_Shoot_Down(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Overture_Shoot_Down();

public:
	static Overture_Shoot_Down* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Overture_Shoot_Air : public NeroState
{
private:
	explicit Overture_Shoot_Air(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Overture_Shoot_Air();

public:
	static Overture_Shoot_Air* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Overture_Shoot_Air_Up : public NeroState
{
private:
	explicit Overture_Shoot_Air_Up(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Overture_Shoot_Air_Up();

public:
	static Overture_Shoot_Air_Up* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Overture_Shoot_Air_Down : public NeroState
{
private:
	explicit Overture_Shoot_Air_Down(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Overture_Shoot_Air_Down();

public:
	static Overture_Shoot_Air_Down* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Cbs_Idle : public NeroState
{
private:
	explicit Cbs_Idle(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Cbs_Idle();

public:
	static Cbs_Idle* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Cbs_ComboA1 : public NeroState
{
private:
	explicit Cbs_ComboA1(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Cbs_ComboA1();

public:
	static Cbs_ComboA1* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Cbs_ComboA2 : public NeroState
{
private:
	explicit Cbs_ComboA2(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Cbs_ComboA2();

public:
	static Cbs_ComboA2* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Cbs_ComboA3 : public NeroState
{
private:
	explicit Cbs_ComboA3(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Cbs_ComboA3();

public:
	static Cbs_ComboA3* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Cbs_ComboA4 : public NeroState
{
private:
	explicit Cbs_ComboA4(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Cbs_ComboA4();

public:
	static Cbs_ComboA4* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Cbs_ComboA5 : public NeroState
{
private:
	explicit Cbs_ComboA5(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Cbs_ComboA5();

public:
	static Cbs_ComboA5* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Cbs_SKill_Crystal : public NeroState
{
private:
	explicit Cbs_SKill_Crystal(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Cbs_SKill_Crystal();

public:
	static Cbs_SKill_Crystal* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Cbs_SKill_IceAge_Start : public NeroState
{
private:
	explicit Cbs_SKill_IceAge_Start(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Cbs_SKill_IceAge_Start();

public:
	static Cbs_SKill_IceAge_Start* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Cbs_SKill_IceAge_Loop : public NeroState
{
private:
	explicit Cbs_SKill_IceAge_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Cbs_SKill_IceAge_Loop();

public:
	static Cbs_SKill_IceAge_Loop* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
private:
	float	m_fLoopTime;
};

class Cbs_SKill_IceAge_End : public NeroState
{
private:
	explicit Cbs_SKill_IceAge_End(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Cbs_SKill_IceAge_End();

public:
	static Cbs_SKill_IceAge_End* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Cbs_SKill_Revolver_Start : public NeroState
{
private:
	explicit Cbs_SKill_Revolver_Start(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Cbs_SKill_Revolver_Start();

public:
	static Cbs_SKill_Revolver_Start* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Cbs_SKill_Revolver_Loop : public NeroState
{
private:
	explicit Cbs_SKill_Revolver_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Cbs_SKill_Revolver_Loop();

public:
	static Cbs_SKill_Revolver_Loop* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;

private:
	UINT	m_iLoopCount = 0;
};

class Cbs_SKill_Revolver_End : public NeroState
{
private:
	explicit Cbs_SKill_Revolver_End(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Cbs_SKill_Revolver_End();

public:
	static Cbs_SKill_Revolver_End* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Cbs_SKill_Swing : public NeroState
{
private:
	explicit Cbs_SKill_Swing(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Cbs_SKill_Swing();

public:
	static Cbs_SKill_Swing* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Middle_Cbs_BiAttack : public NeroState
{
private:
	explicit Middle_Cbs_BiAttack(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Middle_Cbs_BiAttack();

public:
	static Middle_Cbs_BiAttack* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Middle_Cbs_BlitzAttack : public NeroState
{
private:
	explicit Middle_Cbs_BlitzAttack(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Middle_Cbs_BlitzAttack();

public:
	static Middle_Cbs_BlitzAttack* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Middle_Cbs_Satellite : public NeroState
{
private:
	explicit Middle_Cbs_Satellite(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Middle_Cbs_Satellite();

public:
	static Middle_Cbs_Satellite* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Middle_Cbs_Satellite_Air : public NeroState
{
private:
	explicit Middle_Cbs_Satellite_Air(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Middle_Cbs_Satellite_Air();

public:
	static Middle_Cbs_Satellite_Air* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Middle_Cbs_Strike : public NeroState
{
private:
	explicit Middle_Cbs_Strike(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Middle_Cbs_Strike();

public:
	static Middle_Cbs_Strike* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Middle_Cbs_Strike_Air : public NeroState
{
private:
	explicit Middle_Cbs_Strike_Air(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Middle_Cbs_Strike_Air();

public:
	static Middle_Cbs_Strike_Air* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Middle_Cbs_Strike_Air_Down : public NeroState
{
private:
	explicit Middle_Cbs_Strike_Air_Down(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Middle_Cbs_Strike_Air_Down();

public:
	static Middle_Cbs_Strike_Air_Down* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Middle_Cbs_Strike_Air_Up : public NeroState
{
private:
	explicit Middle_Cbs_Strike_Air_Up(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Middle_Cbs_Strike_Air_Up();

public:
	static Middle_Cbs_Strike_Air_Up* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Middle_Cbs_Strike_Down : public NeroState
{
private:
	explicit Middle_Cbs_Strike_Down(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Middle_Cbs_Strike_Down();

public:
	static Middle_Cbs_Strike_Down* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Middle_Cbs_Strike_Up : public NeroState
{
private:
	explicit Middle_Cbs_Strike_Up(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Middle_Cbs_Strike_Up();

public:
	static Middle_Cbs_Strike_Up* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Middle_Cbs_ThunderBullet : public NeroState
{
private:
	explicit Middle_Cbs_ThunderBullet(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Middle_Cbs_ThunderBullet();

public:
	static Middle_Cbs_ThunderBullet* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Pole_ComboA1 : public NeroState
{
private:
	explicit Pole_ComboA1(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Pole_ComboA1();

public:
	static Pole_ComboA1* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Pole_ComboA2 : public NeroState
{
private:
	explicit Pole_ComboA2(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Pole_ComboA2();

public:
	static Pole_ComboA2* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Pole_ComboA3 : public NeroState
{
private:
	explicit Pole_ComboA3(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Pole_ComboA3();

public:
	static Pole_ComboA3* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Pole_ComboB1 : public NeroState
{
private:
	explicit Pole_ComboB1(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Pole_ComboB1();

public:
	static Pole_ComboB1* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Pole_ComboB2 : public NeroState
{
private:
	explicit Pole_ComboB2(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Pole_ComboB2();

public:
	static Pole_ComboB2* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Pole_ComboB3 : public NeroState
{
private:
	explicit Pole_ComboB3(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Pole_ComboB3();

public:
	static Pole_ComboB3* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Pole_ComboB4 : public NeroState
{
private:
	explicit Pole_ComboB4(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Pole_ComboB4();

public:
	static Pole_ComboB4* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Pole_WhirlWind_Start : public NeroState
{
private:
	explicit Pole_WhirlWind_Start(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Pole_WhirlWind_Start();

public:
	static Pole_WhirlWind_Start* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Pole_WhirlWind_Loop : public NeroState
{
private:
	explicit Pole_WhirlWind_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Pole_WhirlWind_Loop();

public:
	static Pole_WhirlWind_Loop* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
private:
	UINT	m_iLoopCount = 0;
	float	m_fLoopTime = 0.f;
};

class Pole_WhirlWind_End : public NeroState
{
private:
	explicit Pole_WhirlWind_End(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Pole_WhirlWind_End();

public:
	static Pole_WhirlWind_End* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class ComboA_Dash : public NeroState
{
private:
	explicit ComboA_Dash(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~ComboA_Dash();

public:
	static ComboA_Dash* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Skill_Caliber : public NeroState
{
private:
	explicit Skill_Caliber(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Skill_Caliber();

public:
	static Skill_Caliber* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Skill_Caliber_End : public NeroState
{
private:
	explicit Skill_Caliber_End(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Skill_Caliber_End();

public:
	static Skill_Caliber_End* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Hr_Air : public NeroState
{
private:
	explicit Hr_Air(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Hr_Air();

public:
	static Hr_Air* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Hr_Ex_Start : public NeroState
{
private:
	explicit Hr_Ex_Start(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Hr_Ex_Start();

public:
	static Hr_Ex_Start* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Hr_Ex_Finish : public NeroState
{
private:
	explicit Hr_Ex_Finish(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Hr_Ex_Finish();

public:
	static Hr_Ex_Finish* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Hr_Ex_Air_Roll_Start : public NeroState
{
private:
	explicit Hr_Ex_Air_Roll_Start(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Hr_Ex_Air_Roll_Start();

public:
	static Hr_Ex_Air_Roll_Start* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Hr_Ex_Air_Roll_Loop : public NeroState
{
private:
	explicit Hr_Ex_Air_Roll_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Hr_Ex_Air_Roll_Loop();

public:
	static Hr_Ex_Air_Roll_Loop* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;

private:
	int	m_iLoopCount = 0;
};

class Hr_Ex_Air_Roll_End : public NeroState
{
private:
	explicit Hr_Ex_Air_Roll_End(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Hr_Ex_Air_Roll_End();

public:
	static Hr_Ex_Air_Roll_End* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Skill_Split_Ex : public NeroState
{
private:
	explicit Skill_Split_Ex(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Skill_Split_Ex();

public:
	static Skill_Split_Ex* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Skill_Split_Ex_Loop : public NeroState
{
private:
	explicit Skill_Split_Ex_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Skill_Split_Ex_Loop();

public:
	static Skill_Split_Ex_Loop* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
private:
	float	m_fLoopTime;
};

class Skill_Split_Ex_Landing : public NeroState
{
private:
	explicit Skill_Split_Ex_Landing(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Skill_Split_Ex_Landing();

public:
	static Skill_Split_Ex_Landing* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Air_Dive_Slash_Start : public NeroState
{
private:
	explicit Air_Dive_Slash_Start(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Air_Dive_Slash_Start();

public:
	static Air_Dive_Slash_Start* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Air_Dive_Slash_Loop : public NeroState
{
private:
	explicit Air_Dive_Slash_Loop(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Air_Dive_Slash_Loop();

public:
	static Air_Dive_Slash_Loop* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;

};

class Air_Dive_Slash_End : public NeroState
{
private:
	explicit Air_Dive_Slash_End(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Air_Dive_Slash_End();

public:
	static Air_Dive_Slash_End* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Skill_Float_Ground_Ex3_Start : public NeroState
{
private:
	explicit Skill_Float_Ground_Ex3_Start(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Skill_Float_Ground_Ex3_Start();

public:
	static Skill_Float_Ground_Ex3_Start* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class Buster_Start : public NeroState
{
private:
	explicit Buster_Start(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~Buster_Start();

public:
	static Buster_Start* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};

class To_Majin : public NeroState
{
private:
	explicit To_Majin(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);
public:
	virtual ~To_Majin();

public:
	static To_Majin* Create(FSMBase* const _pFSM, const UINT _nIndex, weak_ptr<Nero> _pNero);

public:
	virtual HRESULT StateEnter()							override;
	virtual HRESULT StateExit()								override;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	override;
};





#endif // NeroState_h__