#ifndef __Em0000_Weapon_h__
#define __Em0000_Weapon_h__
#include "GameObject.h"
#include "RenderInterface.h"
#include "Monster.h"


class Em0000;
class Em0000Weapon : public ENGINE::GameObject ,
				   public ENGINE::RenderInterface				
{
private:
	explicit Em0000Weapon() = default;
	virtual ~Em0000Weapon() = default;
	// GameObject을(를) 통해 상속됨
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

	// 렌더링 함수
	void RenderInit();
	void RenderDebug(const DrawInfo& _Info);
	void RenderGBuffer(const DrawInfo& _Info);
	void RenderShadow(const DrawInfo& _Info);
private:
	std::shared_ptr<ENGINE::StaticMesh> m_pStaticMesh{};



	//무기 붙이기용
	std::weak_ptr<Em0000>				  m_pEm0000;
	std::weak_ptr<ENGINE::SkeletonMesh>   m_pEm0000Mesh;
	std::weak_ptr<ENGINE::Transform>	  m_pEm0000Trasform;

	std::optional<Matrix>				  m_ParentBone;
	Matrix*								  m_pParentBone;
	Matrix								  m_ParentWorld;
	Matrix								  m_Result;

public:
	void SetMesh(std::weak_ptr<ENGINE::SkeletonMesh> _pMesh);
	void SetOwner(std::weak_ptr<Em0000> _pOwner);
};
#endif //__Em0000_Weapon_h__