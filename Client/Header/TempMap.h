#ifndef __TEMPMAP__H_
#define __TEMPMAP__H_
#include "GameObject.h"
#include "RenderInterface.h"

class TempMap : public ENGINE::GameObject ,
				   public ENGINE::RenderInterface				
{
private:
	std::shared_ptr<ENGINE::StaticMesh> _StaticMesh{};

	//2021.04.17 권현재 Collider 테스트
	std::weak_ptr<CapsuleCollider> m_pCollider;
private:
	explicit TempMap() = default;
	virtual ~TempMap() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static TempMap* Create();
public:
	virtual void    RenderReady()                          override;
public:
	void RenderInit();
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT    Update(const float _fDeltaTime) override;
	virtual UINT    LateUpdate(const float _fDeltaTime) override;
	virtual void    Editor()override;
	virtual void	OnEnable() override;
	virtual void    OnDisable() override;
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderGBuffer(const DrawInfo& _Info);
	void RenderShadow(const DrawInfo& _Info);
};
#endif //__TEMPMAP__H_