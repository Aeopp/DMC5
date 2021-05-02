#ifndef __Em5300Missile_H__
#define __Em5300Missile_H__
#include "GameObject.h"
#include "Monster.h"
#include "RenderInterface.h"

class Em5300;
class Em5300Missile : public Unit ,
				   public ENGINE::RenderInterface				
{
private:
	explicit Em5300Missile() = default;
	virtual ~Em5300Missile() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static Em5300Missile* Create();
public:
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
public:
	void RenderInit();
	void RenderDebug(const DrawInfo& _Info);
	void RenderGBuffer(const DrawInfo& _Info);
	void RenderShadow(const DrawInfo& _Info);

	void Missile();
	void Set_Missile(const bool _bMissile) { m_bReadyMissile = _bMissile; }
	void Set_MissilePos(const int _iPos) { m_iMissilePos = _iPos; }
	void Set_StartMissile(const bool _bStartMissile) { m_bStartMissile = _bStartMissile; }
public:
	virtual void	Hit(BT_INFO _BattleInfo, void* pArg = nullptr) override;
public:
	virtual void	OnTriggerEnter(std::weak_ptr<GameObject> _pOther) override;
public:
	std::shared_ptr<ENGINE::StaticMesh> m_pMesh;

	weak_ptr<SphereCollider>			  m_pCollider;

	std::weak_ptr<Em5300>				  m_pEm5300;
	std::weak_ptr<ENGINE::SkeletonMesh>   m_pEm5300Mesh;
	std::weak_ptr<ENGINE::Transform>	  m_pEm5300Trasform;

	std::optional<Matrix>				  m_ParentBone;
	Matrix*								  m_pParentBone;
	Matrix								  m_ParentWorld;
	Matrix								  m_Result;

	std::weak_ptr<ENGINE::Transform>	  m_pPlayerTrans;
	std::weak_ptr<Nero>					  m_pPlayer;

private:
	//무지성 불변수 파티
	bool								  m_bReadyMissile = false;	//불렛 준비
	bool								  m_bStartMissile = false;	//불렛 출발
	bool								  m_bMissileDir = false;	//방향 한번만 정함
	bool								  m_bRotMissile = false;	//한번만 총알 회전
	int									  m_iMissilePos = 0;
	Vector3								  m_vMissileDir;


};
#endif //