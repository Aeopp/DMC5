#ifndef Em5000_h__
#define Em5000_h__

#include "Monster.h"


class TestObject;
class Car;
class Nero;
class Em5000Hand;
class Em5000 final : public Monster
{
public:
	enum Em5000_State
	{
		Attack_Back_L,
		Attack_Back_R,
		Attack_Finish,
		Attack_Grap_Car,
		Attack_Hack,
		Attack_Jump_Attack,
		Attack_Punch_Twice,
		Attack_Rush_Start,
		Attack_Rush_Loop,
		Attack_Rush_End,
		Attack_Side_L,
		Attack_Side_R,
		Attack_Throw_Car,
		Back_Jump,
		Dead,
		Groggy_Dead,
		Groggy_Start,
		Groggy_Loop,
		Groggy_End,
		Hit_Buster_Start,
		Hit_Buster_Swing_Start,
		Hit_Buster_Swing_Loop,
		Hit_Buster_Swing_Throw,
		Hit_Buster_Swing_End,
		Hit_Buster_Standup,
		Howling,
		Idle,
		Move_Loop,
		Move_Start,
		Tornado_End,
		Tornado_Start,
		Move_Turn_L_90,
		Move_Turn_L_180,
		Move_Turn_R_90,
		Move_Turn_R_180,
		Move_Turn_L,
		Move_Turn_R,
		Car_Rush_Start,
		Car_Rush_Loop,
		Car_Turn_L,
		Car_Turn_R,
		State_END
	};
private:
	explicit Em5000() = default;
	virtual ~Em5000() = default;

	virtual void Free() override;
	virtual std::string GetName() override;

public:
	static Em5000* Create();
public:


	virtual void Fight(const float _fDeltaTime)override;
	virtual void State_Change(const float _fDeltaTime)override;
	virtual void Skill_CoolTime(const float _fDeltaTime)override;

public:
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT Update(const float _fDeltaTime) override;
	virtual UINT LateUpdate(const float _fDeltaTime) override;
	virtual void Editor() override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
public:
	virtual void Hit(BT_INFO _BattleInfo, void* pArg = nullptr) override;
	virtual void Buster(BT_INFO _BattleInfo, void* pArg = nullptr) override;
	virtual void Snatch(BT_INFO _BattleInfo, void* pArg = nullptr) override;
public:
	// 렌더링 함수....
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);
	void RenderInit();

public:
	virtual void Rotate(const float _fDeltaTime) override;
	virtual void Update_Angle() override;
	void		 Set_Rotate();

	virtual void	OnCollisionEnter(std::weak_ptr<GameObject> _pOther);

	virtual void	OnTriggerEnter(std::weak_ptr<GameObject> _pOther);
	void		 Turn();
	void		 Turn_To_Car();
	void		 Update_Angle_ToCar();
	Em5000_State Get_State() { return m_eState; }
	
private:
	//몬스터 상태
	Em5000_State	m_eState =State_END;		


	//공격 및 이동 관련
	bool		m_bMove = false;
	float		m_fMoveTime = 0.f;

	bool		m_bAttack = false;	
	float		m_fAttackTime = 0.f;

	bool		m_bSideAttack = false;
	float		m_fSideAttackTime = 0.f;

	bool		m_bRushAttack = false;
	float		m_fRushAttackTime = 0.f;

	bool		m_bJumpAttack = false;
	float		m_fJumpAttackTime = 0.f;

	bool		m_bThrow = false;
	float		m_fThrowTime = 0.f;

	bool		m_bBackJump = false;
	float		m_fBackJumpTime = 0.f;
	//돌리기
	bool		m_bTurn = false;
	float		m_fTurnTime = 0.f;

	bool		m_bBuster = false;

	bool		m_bTest2 = false;


	//전투 시작 테스트 용
	bool		m_bTest = false;
	std::weak_ptr<Em5000Hand>	m_pHand[2];
	std::weak_ptr<ENGINE::BoxCollider> m_pCollider;

	std::weak_ptr<Car>				 m_pCar[2];
	std::weak_ptr<ENGINE::Transform> m_pCarTrans;


	std::weak_ptr<class StoneDebrisMulti> m_pStone[12];
	std::weak_ptr<class ShockWave>		  m_pWave;
	std::weak_ptr<class StoneDebrisMulti> m_pStone2;
	std::weak_ptr<class BtlPanel>		  m_pBtlPanel;
	
	bool								  m_bStone = false;
	bool								  m_bJustOne[2] = { false,false };
	Vector3								  m_vStonePos{};

	bool								  m_bHitBuster = false;

};

#endif // Em5000_h__