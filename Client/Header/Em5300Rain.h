#ifndef __Em5300Rain_H__
#define __Em5300Rain_H__
#include "GameObject.h"
#include "Monster.h"
#include "RenderInterface.h"

class Em5300;
class Em5300Rain : public Unit ,
				   public ENGINE::RenderInterface				
{
private:
	explicit Em5300Rain() = default;
	virtual ~Em5300Rain() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static Em5300Rain* Create();
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

	void Rain(const float _fDeltaTime);
	void Set_Rain(const bool _bRain) { m_bReadyRain = _bRain; }
	void Set_RainPos(const int _iPos) { m_iRainPos = _iPos; }
	void Set_StartRain(const bool _bStartRain) { m_bStartRain = _bStartRain; }
public:
	virtual void	Hit(BT_INFO _BattleInfo, void* pArg = nullptr) override;
public:
	virtual void	OnTriggerEnter(std::weak_ptr<GameObject> _pOther) override;
public:
	std::shared_ptr<ENGINE::StaticMesh>   m_pMesh;

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

	weak_ptr<class ArtemisMissile>		  m_pMissile;
private:
	//무지성 불변수 파티
	bool								  m_bReadyRain = false;	//불렛 준비
	bool								  m_bStartRain = false;	//불렛 출발
	bool								  m_bRaindir = false;
	bool								  m_bWater = false;
	
	float								  m_fRainUpTime = 0.f;
	float								  m_fRainDownTime = 0.f;
	int									  m_iRainPos = 0;

	Vector3								  m_vRainDir;
	Vector3								  m_vPlayerPos[4];

	weak_ptr<class Reverberation>		m_pRever;

	bool								m_bJustOne = false;
	int									m_iCount = 3;

	
};
#endif //