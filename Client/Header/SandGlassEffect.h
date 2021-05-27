#ifndef __SANDGLASSEFFECT_H_
#define __SANDGLASSEFFECT_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include <optional>
//#include "DynamicLight.h"

class SandGlassEffect :  public ENGINE::GameObject,
				         public ENGINE::RenderInterface
{
private:
	std::shared_ptr<ENGINE::StaticMesh> _Mesh{};
	float T = 0.0f;
	float Sin{};
private:
	explicit SandGlassEffect() = default;
	virtual ~SandGlassEffect() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static SandGlassEffect* Create();
public:
	virtual void    RenderReady() override;
public:
	void			RenderInit();
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT    Update(const float _fDeltaTime) override;
	virtual UINT    LateUpdate(const float _fDeltaTime) override;
	virtual void    Editor()override;
	virtual void	OnEnable() override;
	virtual void    OnDisable() override;
public:
	void PlayStart(const Vector3& Location);
private:
	void UpdateParticle(const float DeltaTime);
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
private:
	bool bInnerFrustum = false;
	float CurParticleTime = 0.00f;
	float ParticleTime = 0.016f;
	Vector3 PlayPos = Vector3(0.f, 0.f, 0.f);

	//DynamicLight _DynamicLight{};
	//// Nuclear 값 복붙
	//Vector4 ColorLow
	//{ 108.f / 255.f, 82.f / 255.f, 158.f / 255.f, 1.f };
	//Vector4 ColorHigh
	//{ 194.f / 255.f, 181.f / 255.f, 247.f / 255.f, 1.f };
	//float ExplosionReadyFluxLow = 0.f;
	//float ExplosionReadyFluxHigh = 0.3f;
	//float ExplosionReadyRadiusLow = 0.f;
	//float ExplosionReadyRadiusHigh = 0.3f;
	//float ExplosionFluxLow = 3.333f;
	//float ExplosionFluxHigh = 333.333f;
	//float ExplosionRadiusLow = 0.666f;
	//float ExplosionRadiusHigh = 3.333f;
};
#endif //