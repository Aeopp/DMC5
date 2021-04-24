#ifndef Em5000Hand_h__
#define Em5000Hand_h__

#pragma once
#include "Monster.h"

class Em5000;
class Em5000Hand final : public Monster
{
private:
	explicit Em5000Hand() = default;
	virtual ~Em5000Hand() = default;
	// Monster을(를) 통해 상속됨
	virtual void Free() override;
public:
	static Em5000Hand* Create();
public:
	virtual std::string GetName() override;
	virtual void Fight(const float _fDeltaTime) override;
	virtual void State_Change(const float _fDeltaTime) override;
	virtual void Skill_CoolTime(const float _fDeltaTime) override;
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
public:
	void RenderInit();

	virtual void Rotate(const float _fDeltaTime) override;
	virtual void Update_Angle() override;


public:
	weak_ptr<Em5000>					m_pEm5000;
	weak_ptr<ENGINE::SkeletonMesh>	    m_pEm5000Mesh;
	weak_ptr<ENGINE::Transform>			m_pEm5000Trans;

	optional<Matrix>					  m_ParentBone;
	Matrix*								  m_pParentBone;
	Matrix								  m_ParentWorld;
	Matrix								  m_Result;
		
	bool								  m_bLeft;

	weak_ptr<SphereCollider>			m_pCollider;
};

#endif // Em5000Hand_h__
