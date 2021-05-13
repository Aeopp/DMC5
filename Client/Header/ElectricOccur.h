#ifndef __ELECTRICOCCUR_H_
#define __ELECTRICOCCUR_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include "Vertexs.h"
#include "FLight.h"
#include <optional>

class ElectricOccur : public ENGINE::GameObject,
	public ENGINE::RenderInterface
{
private:
	std::shared_ptr<ENGINE::StaticMesh> Mesh{};
	std::shared_ptr<ENGINE::Texture> GradMap{};
	std::shared_ptr<Texture> DistortionMap{};
private:
	explicit ElectricOccur();
	virtual ~ElectricOccur() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static ElectricOccur* Create();
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

	float PtLightRadius = 10.f;
	float PtLightFlux = 10.f;
private:
	float ParticleTime = 0.06f;
	float CurParticleTime = 0.0f;
	float T = 0.0f;
	float PlayTime = 0.35f;
	float ColorIntencity = 0.7f;

	float ScrollSpeed = 55.f;

	std::weak_ptr<FLight> PtLight;
	
	float DistortionIntencity = 0.25f;

	Vector3 StartScale{ 0.0001f,0.0001f,0.0001f };
	Vector3 FinalSacle{ 0.0001f,0.0001f,0.0001f };
};
#endif //

