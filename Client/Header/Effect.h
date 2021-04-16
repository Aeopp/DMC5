#ifndef __EFFECT__
#define __EFFECT__
#include "GameObject.h"
#include "RenderInterface.h"

class Effect abstract: public ENGINE::GameObject,
					   public ENGINE::RenderInterface				
{
protected:
	float _AccumulateTime = 0.f;
	float _PlayingSpeed = 1.f;

protected:
	explicit Effect() = default;
	virtual ~Effect() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override PURE;
	virtual std::string GetName() override PURE;
public:
	virtual void    RenderReady() override;
public:
	virtual HRESULT Ready() override PURE;
	virtual HRESULT Awake() override PURE;
	virtual HRESULT Start() override PURE;
	virtual UINT    Update(const float _fDeltaTime) override PURE;
	virtual UINT    LateUpdate(const float _fDeltaTime) override PURE;
	virtual void    Editor() override PURE;
	virtual void	OnEnable() override PURE;
	virtual void    OnDisable() override PURE;
};
#endif // !__EFFECT__