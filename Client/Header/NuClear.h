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
	// GameObject��(��) ���� ��ӵ�
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
	void PlayStart(const Vector3& Location , const bool bEditPlay=false);
	void PlayEnd();
private:
	void ParticleUpdate(const float DeltaTime);
	void PlayParticle();
	void KaboomParticle();
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
private:
	bool bExplosion = false;
	bool bEditPlay = false;
	std::weak_ptr<class NuclearLensFlare> _NuclearLensFlare{};
	DynamicLight _DynamicLight{};
	float CurParticleDelta = 0.0f;

	Vector4 ColorLow 
	{ 108.f / 255.f, 82.f / 255.f, 158.f / 255.f ,1.f };
	Vector4 ColorHigh
	{ 194.f / 255.f, 181.f / 255.f, 247.f / 255.f ,1.f };

	// ������ ����
	float EditYVelocity = 0.0115f;

	float ExplosionReadyFluxLow = 0.f;
	float ExplosionReadyFluxHigh = 1.f;
	float ExplosionReadyRadiusLow = 0.f;
	float ExplosionReadyRadiusHigh = 1.f;

	float ExplosionFluxLow = 10.f;
	float ExplosionFluxHigh = 1000.f;
	float ExplosionRadiusLow = 2.f;
	float ExplosionRadiusHigh = 10.f;

	float ParticleWorldScale = 0.0015f;
	float ParticleDelta = 0.016f;

	float GrowEndT = 8.f;
	float ExplosionReadyTime = 24.f;
	float FreeFallTime = 2.5f;
	float ExplosionTime = 4.f;

	float ColorIntencity = 0.32f;
	float GrowEndScale = 0.0010f;
};
#endif //