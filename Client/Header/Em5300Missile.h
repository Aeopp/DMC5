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
	// GameObject��(��) ���� ��ӵ�
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
	void Missile2();
	void Set_Missile(const bool _bMissile) { m_bReadyMissile = _bMissile; }
	void Set_MissilePos(const int _iPos) { m_iMissilePos = _iPos; }
	void Set_StartMissile(const bool _bStartMissile) { m_bStartMissile = _bStartMissile; }

	void Set_Missile2(const bool _bMissile) { m_bReadyMissile2 = _bMissile; }
	void Set_StartMissile2(const bool _bStartMissile) { m_bStartMissile2 = _bStartMissile; }

	Vector3 Get_Test() { return m_vTest; }

	bool Get_StartMissile() { return m_bStartMissile; }
	bool Get_StartMissile2() { return m_bStartMissile2; }

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

	
	weak_ptr<class ArtemisMissile>		  m_pMissile;
private:
	//������ �Һ��� ��Ƽ
	bool								  m_bReadyMissile = false;	//�ҷ� �غ�
	bool								  m_bStartMissile = false;	//�ҷ� ���
	bool								  m_bMissileDir = false;	//���� �ѹ��� ����
	bool								  m_bRotMissile = false;	//�ѹ��� �Ѿ� ȸ��
	int									  m_iMissilePos = 0;
	Vector3								  m_vMissileDir;

	bool								  m_bReadyMissile2 = false;
	bool								  m_bStartMissile2 = false;
	bool							      m_bMissileDir2 = false;
	bool								  m_bRotMissile2 = false;


	Vector3								m_vTest;

	weak_ptr<class Reverberation>		m_pRever;

	bool								m_bJustOne = false;
	int									m_iCount = 0;
	
};
#endif //