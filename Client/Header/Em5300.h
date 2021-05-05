#ifndef Em5300_h__
#define Em5300_h__

#include "Monster.h"


class Car;
class Nero;
class Em5300Rush;
class Em5300Missile;
class Em5300Rain;
class Em5300Homing;
class Em5300 final : public Monster
{
public:
	enum Em5300_State
	{
		Attack_Bug,
		Attack_Homing_End,
		Attack_Homing_Loop,
		Attack_Homing_Start,
		Attack_Laser_End,
		Attack_Laser_Loop,
		Attack_Laser_Start,
		Attack_Missile2_Start,
		Attack_Missile2_Attack,
		Attack_Missile2_Loop,
		Attack_Missile2_End,
		New_Missile_End,
		New_Missile_Loop,
		New_Missile_Start,
		New_Missile_Attack,
		Attack_Rain_End,
		Attack_Rain_Loop,
		Attack_Rain_Start,
		Attack_Rush_Loop,
		Attack_Rush_Start,
		Hit_Buster_Start,
		Hit_Buster_Stab,
		Hit_Buster_Stab_Loop,
		Hit_Buster_Stab_Throw,
		Hit_Buster_Stab_End,
		Dead,
		Down_Dead,
		Down_Loop,
		Hit_Falling_End,
		Hit_Falling_Loop,
		Hit_Falling_Start,
		Idle,
		Move_Back_End,
		Move_Back_Loop,
		Move_Back_Start,
		Move_Front_End,
		Move_Front_Loop,
		Move_Front_Start,
		State_END		
	};
private:
	explicit Em5300() = default;
	virtual ~Em5300() = default;

	virtual void Free() override;
	virtual std::string GetName() override;

public:
	static Em5300* Create();
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

	Em5300_State Get_State() { return m_eState; }
private:
	//몬스터 상태
	Em5300_State	m_eState =State_END;		
	//TestPlayer 받아옴.
	std::weak_ptr<ENGINE::Transform> m_pPlayerTrans;
	std::weak_ptr<Nero>				 m_pPlayer;

	std::weak_ptr<Car>				 m_pCar;
	std::weak_ptr<ENGINE::Transform> m_pCarTrans;
	//공격 및 이동 관련
	bool		m_bMove = false;
	float		m_fMoveTime = 0.f;

	bool		m_bRushAttack = false;	
	float		m_fRushAttackTime = 0.f;
	bool		m_bRushDir = false;
	Vector3		m_vRushDir;
	float		m_fRushTime = 0.f;

	bool		m_bMissile = false;
	float		m_fMissileTime = 0.f;

	bool		m_bRain = false;
	float		m_fRainTime = 0.f;

	bool		m_bMissile2 = false;
	float		m_fMissile2Time = 0.f;

	bool		m_bHoming = false;
	float		m_fHomingTime = 0.f;
	weak_ptr<Em5300Missile>	m_pBullet[32];
	weak_ptr<Em5300Rain>	m_pRain[12];
	weak_ptr<Em5300Homing>	m_pHoming[8];

	//전투 시작 테스트 용
	std::weak_ptr<Em5300Rush>	m_pRush;
	std::weak_ptr<ENGINE::CapsuleCollider> m_pCollider;
	weak_ptr<class Liquid> m_pBlood;

	float	m_fTest = 0.f;
	float	m_fPower = 0.f;
	Vector3	m_vPower;
	bool	m_bFight = false;


};

#endif // Em5300_h__