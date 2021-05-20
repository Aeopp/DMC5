#ifndef __CAR_H__
#define __CAR_H__
#include "GameObject.h"
#include "RenderInterface.h"
#include "Monster.h"

class Em5000;
class TestObject;
class Nero;
class Car final :  public :: Unit ,
				   public ENGINE::RenderInterface				
{
private:
	std::shared_ptr<ENGINE::SkeletonMesh> _StaticMesh{};
private:
	explicit Car() = default;
	virtual ~Car() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static Car* Create();
public:
	void RenderInit();
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);

	virtual void    RenderReady()                          override;
public:
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT    Update(const float _fDeltaTime) override;
	virtual UINT    LateUpdate(const float _fDeltaTime) override;
	virtual void    Editor()override;
	virtual void	OnEnable() override;
	virtual void    OnDisable() override;

	virtual void Hit(BT_INFO _BattleInfo, void* pArg = nullptr) override;
	virtual void OnTriggerEnter(std::weak_ptr<GameObject> _pOther) override;



public:
	void	Throw(const float _fDeltaTime);
	void	BangBang(const float _fDeltaTime);

	std::weak_ptr<ENGINE::Transform>	m_pEm5000Trans;
	std::weak_ptr<Em5000>				m_pEm5000;
	std::weak_ptr<ENGINE::SkeletonMesh> m_pEm5000Mesh;

	std::weak_ptr<ENGINE::Transform>	 m_pPlayerTrans;
	std::weak_ptr<Nero>					 m_pPlayer;


	std::optional<Matrix>				  m_ParentBone;
	Matrix*								  m_pParentBone;
	Matrix								  m_ParentWorld;
	Matrix								  m_Result;

	weak_ptr<BoxCollider>		 	      m_pCollider;

	Vector3 m_vDir;
	float	m_fDir = 0.f;
	float	m_fThrowTime = 0.f;
	float	m_fBangPower = 0.f;
	Vector3 m_vBangDir;
	bool	m_bThrow = false;
	bool	m_bBone = false;
	bool	m_bBang = false;
	
	Vector3	m_vPlayerPos;


};
#endif //