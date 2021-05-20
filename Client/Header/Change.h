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
	std::shared_ptr<ENGINE::StaticMesh> Inner{};
	std::shared_ptr<ENGINE::Texture>    TrailMap{};
	std::shared_ptr<ENGINE::Texture>    EmssiveMskMap{};
	std::shared_ptr<ENGINE::Texture>    NoiseMap{};
private:
	explicit Change()  ;
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
		const std::optional<Vector3>& Location = std::nullopt,
		const float PlayTime = 0.4f);
	void PlayEnd();
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
private:
	std::weak_ptr<class FLight> PtLight;
	float PtLightRadius = 100.f;
	float PtLightFlux = 1.f;

	std::weak_ptr<class ShockWave> _ShockWave{};

	float ParticleCycle = 0.01f;
	float CurParticleTime = 0.0f;
	
	float EmissiveIntencity{ 0.0f };
	float ColorIntencity{ 10.f };
	float DistortionIntencity{ 1.f };

	float Radian = 0.0f;
	float RadianSpeed = 1.f;
	float VelocityBlurAlpha = 1.f;

	Vector3 NoiseScrollSpeed{ 0.229008f,0.534351f,0.381679f};
	Vector3 NoiseScale{ 0.458015f,1.374046f,0.381679f };

	Vector2 NoiseDistortion0{ 0.3f,1.f };
	Vector2 NoiseDistortion1{ 0.5f,0.77f};
	Vector2 NoiseDistortion2{ 0.77f,0.5f};

	Vector4 _Color{ 1.f,0.f,0.f,100.f/255.f };

	float VelocityBlurIntencity = 0.012f;
	float RollRotationSpeed = FMath::PI;

	float T{ 0.0f };
	float PlayTime =0.4f;
	float EditPlayTime = 0.4f;

	float StartScale = 0.0f;
	float EndScale =0.009f;

	float EditRotationSpeed{ 0.f };
};
#endif //

