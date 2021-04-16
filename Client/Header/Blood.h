#ifndef __BLOOD_H__
#define __BLOOD_H__
#include "GameObject.h"
#include "RenderInterface.h"

class Blood : public ENGINE::GameObject,
	public ENGINE::RenderInterface
{
private:
	std::shared_ptr<ENGINE::StaticMesh> _StaticMesh{};
private:
	explicit Blood() = default;
	virtual ~Blood() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;

public:
	static Blood* Create();
public:
	 void    RenderForwardAlphaBlendImplementation(const DrawInfo& _ImplInfo);
	void    RenderDebugImplementation(const DrawInfo& _ImplInfo);
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


private:

	float   m_fTextTime = 0.f;
	int		m_iIndex = 0;
	bool	m_bTest = false;
};
#endif //