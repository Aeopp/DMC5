#ifndef Em5300_h__
#define Em5300_h__

#include "Monster.h"


class Car;
class Nero;
class Em5300Rush;
class Em5300Missile;
class Em5300Rain;
class Em5300Homing;
class Em5300Ulte;
class SnatchPoint;
class Trigger;
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
		Attack_Ulte_Move,
		Attack_Ulte_Start,
		Attack_Ulte_Loop,
		Attack_Ulte_Loop2,
		Attack_Ulte_End,
		Attack_Ulte_End_Down,
		Dead,
		Down_Dead,
		Down_Loop,
		Down_Standup,
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
	// ������ �Լ�....
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);
	void RenderInit();

public:
	virtual void Rotate(const float _fDeltaTime) override;
	virtual void Update_Angle() override;
	void		 Set_Rotate();
	void		 SetTrigger(weak_ptr<class Trigger> _Trigger);
	void		 Set_Ulte();
	void		 Particle();

	virtual void	OnCollisionEnter(std::weak_ptr<GameObject> _pOther);

	virtual void	OnTriggerEnter(std::weak_ptr<GameObject> _pOther);

	Em5300_State Get_State() { return m_eState; }
private:
	//���� ����
	Em5300_State	m_eState =State_END;		

	//���� �� �̵� ����
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

	bool		m_bLaser = false;
	float		m_fLaserTime = 0.f;
	weak_ptr<Em5300Missile>	m_pBullet[32];
	weak_ptr<Em5300Rain>	m_pRain[12];
	weak_ptr<Em5300Homing>	m_pHoming[8];
	weak_ptr<Em5300Ulte>	m_pUlte;
	weak_ptr<SnatchPoint>   m_pSnatchPoint[4];

	//���� ���� �׽�Ʈ ��
	std::weak_ptr<Em5300Rush>	m_pRush;
	std::weak_ptr<ENGINE::BoxCollider> m_pCollider;
	weak_ptr<class Liquid> m_pBlood;

	float	m_fTest = 0.01f;
	float	m_fHeight = 0.7f;
	float   m_fCenterY = -0.2f;
	float	m_fPower = 0.f;
	Vector3	m_vPower;
	
	
	bool	m_bFight = false;

	weak_ptr<class Trigger> m_pTrigger;
	weak_ptr<class BtlPanel> m_pPanel;
	weak_ptr<class ShockWave> m_pShockWave;
	weak_ptr<class LensFlare> m_pLens;
	weak_ptr<class Reverberation> m_pRever[10];

	bool	m_bUlte = false;
	bool	m_bLens = false;
	bool	m_bRushLens = false;


	//���� ����Ʈ
	float	m_fOuterCricle = 0.f;
	weak_ptr<class Reverberation> m_pRushRever[12];
	bool	m_bRushCricle = false;
	int		m_iRushIndex = 0;

	float m_fStartScale = 0.0055f;
	float m_fStartScaleAdder = 0.f;

	float m_fEndScale = 0.005f;
	float m_fEndScaleAdder = 0.f;

	///////////
	//ȣ�� ����Ʈ
	weak_ptr<class Reverberation> m_pHomingRever[12];

	bool	m_bCollPlayer = false;
};

#endif // Em5300_h__