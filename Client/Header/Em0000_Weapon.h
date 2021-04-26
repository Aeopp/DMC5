#ifndef __Em0000_Weapon_h__
#define __Em0000_Weapon_h__
#include "GameObject.h"
#include "RenderInterface.h"
#include "Monster.h"


class Em0000;
class Em0000Weapon : public Unit ,
				   public ENGINE::RenderInterface				
{
private:
	explicit Em0000Weapon() = default;
	virtual ~Em0000Weapon() = default;
	// GameObject��(��) ���� ��ӵ�
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static Em0000Weapon* Create();
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

	// ������ �Լ�
	void RenderInit();
	void RenderDebug(const DrawInfo& _Info);
	void RenderGBuffer(const DrawInfo& _Info);
	void RenderShadow(const DrawInfo& _Info);
private:
	std::shared_ptr<ENGINE::StaticMesh> m_pStaticMesh{};
public:
	virtual void	Hit(BT_INFO _BattleInfo, void* pArg = nullptr) override;
public:
	virtual void	OnTriggerEnter(std::weak_ptr<GameObject> _pOther) override;
public:
	weak_ptr<SphereCollider>			  m_pCollider;

	std::weak_ptr<Em0000>				  m_pEm0000;
	std::weak_ptr<ENGINE::SkeletonMesh>   m_pEm0000Mesh;
	std::weak_ptr<ENGINE::Transform>	  m_pEm0000Trasform;

	std::optional<Matrix>				  m_ParentBone;
	Matrix*								  m_pParentBone;
	Matrix								  m_ParentWorld;
	Matrix								  m_Result;
};
#endif //__Em0000_Weapon_h__