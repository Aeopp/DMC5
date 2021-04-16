#ifndef __DashEffect_h__
#define __DashEffect_h__
#include "Effect.h"
class DashImpact final : public Effect
{
private:
	explicit DashImpact() = default;
	virtual ~DashImpact() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static DashImpact* Create();
public:
	 void    RenderAlphaBlendEffectImplementation(const DrawInfo&_ImplInfo) ;
public:
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT    Update(const float _fDeltaTime) override;
	virtual UINT    LateUpdate(const float _fDeltaTime) override;
	virtual void    Editor() override;
	virtual void	OnEnable() override;
	virtual void    OnDisable() override;


private:
	std::shared_ptr<ENGINE::StaticMesh> m_ImpactMesh = nullptr;
	std::shared_ptr<ENGINE::Texture>	m_ImpactTexture = nullptr;

	float _SliceAmount = 0.f;
	float _RandTexV0 = 0.8f;
	float _RandTexV1 = 0.6f;
	float _AccumulateTime = 0.f;
	float _PlayingSpeed = 1.f;


	float   m_fTextTime = 0.f;
	int		m_iIndex = 0;
	bool	m_bTest = false;
	float	m_fAlpha = -0.2f;

	Vector3 m_vTest = {0.001f, 0.001f, 0.001f};

};
#endif // !__DashEffect_h__