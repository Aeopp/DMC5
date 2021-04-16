#ifndef Em0000_h__
#define Em0000_h__

#include "Monster.h"


class TestObject;
class Em0000 final : public Monster
{
private:
	enum Em0000_State
	{
		Attack_1,
		Attack_2,
		Attack_Hard,
		Buster_End,
		Buster_Start,
		Dead,
		Guard_End,
		Guard_Hit1,
		Guard_Hit2,
		Guard_Loop,
		Guard_Start,
		Hit_Back,
		Hit_Front,
		Hit_L,
		Hit_R,
		Move_Front_End,
		Move_Front_Loop,
		Move_Front_Start,
		Step_Back,
		Stun_End,
		Stun_Start,
		idle,
		Snatch_Start,
		Snatch_End,
		Air_Start,
		Air_Loop,
		Air_End,
		State_END
	};



private:
	explicit Em0000() = default;
	virtual ~Em0000() = default;

	virtual void Free() override;
	virtual std::string GetName() override;

public:
	static Em0000* Create();
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

	// 렌더링 함수....
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);
	void RenderInit();
public:
	virtual void Rotate(const float _fDeltaTime) override;
	virtual void Update_Angle() override;
private:
	//몬스터 상태
	Em0000_State	m_eState =State_END;		
	//TestPlayer 받아옴.
	std::weak_ptr<ENGINE::Transform> m_pPlayerTrans;
	std::weak_ptr<TestObject>		 m_pPlayer;

	//공격 및 이동 관련
	bool		m_bMove = false;
	float		m_fMoveTime = 0.f;

	bool		m_bAttack = false;	
	float		m_fAttackTime = 0.f;

	//잠깐, 막기!
	bool		m_bGuard = false;
	float		m_fGuardTime = 0.f;
	

	//전투 시작 테스트 용
	bool		m_bTest = false;
};

#endif // Em0000_h__