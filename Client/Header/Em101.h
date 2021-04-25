#ifndef Em101_h__
#define Em101_h__

#include "Monster.h"

class TestObject;
class Em101 final : public Monster
{
private:
	enum Em101_State
	{
		Attack_A,
		Attack_B,
		Attack_Front,
		Dead,
		Down_Back_Loop,
		Down_Back_Start,
		Down_Front_End,
		Down_Front_Loop,
		Down_Front_Start,
		Down_Stand_Up,
		Hit_Air,
		Hit_Back,
		Hit_End,
		Hit_Front,
		Hit_L,
		Hit_R,
		Move_End,
		Move_Loop,
		Move_Start,
		State_END
	};



private:
	explicit Em101() = default;
	virtual ~Em101() = default;

	virtual void Free() override;
	virtual std::string GetName() override;

public:
	static Em101* Create();
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

	// ������ �Լ�....
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);
	void RenderInit();
public:
	virtual void Rotate(const float _fDeltaTime) override;
	virtual void Update_Angle() override;

private:
	//���� ����
	Em101_State	m_eState =State_END;		
	//TestPlayer �޾ƿ�.
	std::weak_ptr<ENGINE::Transform> m_pPlayerTrans;
	std::weak_ptr<TestObject>		 m_pPlayer;


	//���� �� �̵� ����
	bool		m_bMove = false;
	float		m_fMoveTime = 0.f;

	bool		m_bAttack = false;	
	float		m_fAttackTime = 0.f;


	//���� ���� �׽�Ʈ ��
	bool		m_bTest = false;
};

#endif // Em101_h__