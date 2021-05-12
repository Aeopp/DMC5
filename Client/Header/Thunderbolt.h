#ifndef __THUNDERBOLT_H_
#define __THUNDERBOLT_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include "Vertexs.h"
#include "FLight.h"
#include <optional>

class ThunderBolt  : public ENGINE::GameObject,
	public ENGINE::RenderInterface
{
private:
	std::shared_ptr<ENGINE::StaticMesh> Mesh{};
	std::shared_ptr<ENGINE::Texture> GradMap{};
	std::shared_ptr<Texture> DistortionMap{};
private:
	explicit ThunderBolt();
	virtual ~ThunderBolt() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static ThunderBolt* Create();
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
	void PlayStart(const Vector3& PlayLocation);
	void PlayEnd();
	float GetPlayTime();
public:
	void PlayParticle();
	void RenderDebug(const DrawInfo& _Info);
	void RenderAlphaBlendEffect(const DrawInfo& _Info);

	float PtLightRadius = 1.f;
	float PtLightFlux = 1.f;
private:
	float ParticleTime = 0.03f;
	float CurParticleTime = 0.0f;
	float T = 0.0f;
	float PlayTime = 0.58f;
	float ColorIntencity = 0.7f;

	float ScrollSpeed = 445.f;
	float DistortionIntencity = 0.1f;


	std::weak_ptr<FLight> PtLight;

	float Range = 0.0f;
	float EndRange = 0.35f;

	Vector3 StartScale{ 0.0001f,0.0001f,0.0001f };
	Vector3 FinalSacle{ 0.0001f,0.0001f,0.0001f };
};
#endif //

