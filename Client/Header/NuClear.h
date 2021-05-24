#ifndef __NUCLEAR_H_
#define __NUCLEAR_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include <optional>
#include "DynamicLight.h"
class NuclearLensFlare;

class NuClear : public ENGINE::GameObject,
				public ENGINE::RenderInterface
{
private:
	std::shared_ptr<ENGINE::StaticMesh> _Mesh{};
	std::shared_ptr<ENGINE::Texture> _LightMsk{};
	std::shared_ptr<ENGINE::Texture> _AlbmMap{};
	float T = 0.0f;
private:
	explicit NuClear() = default;
	virtual ~NuClear() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static NuClear* Create();
public:
	virtual void    RenderReady()                          override;
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
	void PlayEnd();
private:
	void ParticleUpdate(const float DeltaTime);
	void PlayParticle();
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
private:
	std::weak_ptr<class NuclearLensFlare> _NuclearLensFlare{};
	DynamicLight _DynamicLight{};
	float CurParticleDelta = 0.0f;

	// 에디터 변수
	float ExplosionReadyFluxLow = 0.f;
	float ExplosionReadyFluxHigh = 1.f;
	float ExplosionReadyRadiusLow = 0.f;
	float ExplosionReadyRadiusHigh = 1.f;

	float ExplosionFluxLow = 0.f;
	float ExplosionFluxHigh = 1000.f;
	float ExplosionRadiusLow = 0.f;
	float ExplosionRadiusHigh = 10.f;

	float ParticleDelta = 0.016f;

	float GrowEndT = 8.f;
	float ExplosionReadyTime = 24.f;
	float ExplosionTime = 4.f;

	float ColorIntencity = 0.32f;
	float GrowEndScale = 0.0015f;
};
#endif //