#ifndef __ENERGISMREADY_H_
#define __ENERGISMREADY_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include <optional>
#include "DynamicLight.h"
#include "ShockWave.h"

class EnergismReady : public ENGINE::GameObject,
				      public ENGINE::RenderInterface
{
private:
	std::shared_ptr<ENGINE::StaticMesh> _Mesh{};
	std::shared_ptr<ENGINE::Texture> _LightMsk{};
	std::shared_ptr<ENGINE::Texture> _AlbmMap{};
	std::shared_ptr<ENGINE::Texture> _Noise{};
	std::weak_ptr<ShockWave> _ShockWave{};
	float T = 0.0f;
private:
	explicit EnergismReady() = default;
	virtual ~EnergismReady() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static EnergismReady* Create();
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
	void PlayStart(const Vector3& Location , const float GroundY, const bool bEditPlay=false);
	void PlayEnd();
private:
	void ParticleUpdate(const float DeltaTime);
	void PlayParticle();
	void UpdatePosition(const Vector3& Location);
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
private:
	std::weak_ptr<class NuclearLensFlare> _NuclearLensFlare{};
	DynamicLight _DynamicLight{};
	float CurParticleDelta = 0.0f;

	Vector4 ColorLow 
	{ 108.f / 255.f, 82.f / 255.f, 158.f / 255.f ,1.f };
	Vector4 ColorHigh
	{ 194.f / 255.f, 181.f / 255.f, 247.f / 255.f ,1.f };

	// 에디터 변수
	D3DXCOLOR LaunchColor{ 
		194.f / 255.f, 181.f / 255.f, 247.f / 255.f ,1.f };
	float LaunchRadius{5.f};
	float LaunchFlux{2.2f};

	float GrowTime = 3.f;
	float PlayTime = 10.f;
	float NoiseTimeCorr = 1.f;

	float FluxLow = 0.f;
	float FluxHigh = 1.f;
	float RadiusLow = 0.f;
	float RadiusHigh = 1.f;

	float ParticleWorldScale = 0.000135f;
	float ParticleDelta = 0.016f;

	float StartColorIntencity = 0.020f;
	float EndColorIntencity = 0.020f;
	float GrowEndScale = 0.0005f;
};
#endif //