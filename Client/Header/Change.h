#ifndef __CHANGE_H_
#define __CHANGE_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include <optional>
#include "Vertexs.h"
#include <vector>
#include "ShapeParticle.h"
#include "ShockWave.h"
#include "FLight.h"

class Change : public ENGINE::GameObject,
			   public ENGINE::RenderInterface
{
private:
	std::shared_ptr<ENGINE::StaticMesh> _Mesh{};
	std::shared_ptr<ENGINE::Texture> _AlbmMap{};
	std::shared_ptr<ENGINE::Texture> _AlpgMap{};
	std::shared_ptr<ENGINE::Texture> _MskMap{};
private:
	explicit Change();
	virtual ~Change() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static Change* Create();
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
	void PlayStart(
		const Vector3& Location);
	void PlayEnd();
	void UpdateParticle(const float DeltaTime);
	void UpdateStoneParticle(const float DeltaTime);

private:
	void PlayParticle();
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
private:
	Vector3 StoneLocation{ 0.f,0.f,0.f };
	float ParticleTime{ 0.016f};
	float CurParticleDelta{ 0.0f };

	float StoneParticleTime{ 0.016f };
	float CurStoneParticleDelta{ 0.0f };
	std::weak_ptr<class FLight> PtLight;
	float PtLightRadius = 1.5f;
	float PtLightFlux   = 1.0f;

	std::weak_ptr<class ShockWave> _ShockWave{};

	float ParticleCycle = 0.01f;
	float CurParticleTime = 0.0f;
	float VelocityY = 0.01f;

	float ColorIntencity{ 0.293103f };

	float Radian = 0.0f;
	float RadianSpeed = 1.f;
	float VelocityBlurAlpha = 1.f;

	Vector4 _Color{ 1.f,1.f,1.f,1.f };

	float T{ 0.0f };
	float PlayTime =1.f;

	float EndScale = 0.0045f;
};
#endif //

