#ifndef __ELECTRICVORTEX_H_
#define __ELECTRICVORTEX_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include "Vertexs.h"
#include "FLight.h"
#include <optional>

class ElectricVortex  : public ENGINE::GameObject,
	public ENGINE::RenderInterface
{
public:
	enum Mode : uint8
	{
		Loop,
		Rand,
		All,
		Set,
		End
	};
private:
	std::shared_ptr<ENGINE::StaticMesh> Mesh{};
	std::shared_ptr<ENGINE::Texture> GradMap{};
	std::shared_ptr<ENGINE::Texture> DistortionMap;
private:
	explicit ElectricVortex();
	virtual ~ElectricVortex() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static ElectricVortex* Create();
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
private:
	std::bitset<4u> _SubsetSets{};
	uint32 CurSubset = 0u;
	uint32 CurSubsetRand = 0u;

	int32 _Mode = Loop;

	float SubsetDelta = 0.016f;
	float CurSubsetDelta = 0.016f;

	float ParticleTime = 0.03f;
	float CurParticleTime = 0.0f;
	float T = 0.0f;
	float PlayTime = 0.35f;
	float ColorIntencity = 0.7f;
	float DistortionIntencity = 0.25f;

	float ScrollSpeed = 445.f;

	float PtLightRadius = 10.f;
	float PtLightFlux = 10.f;
	std::weak_ptr<FLight> PtLight;

	Vector3 StartScale{ 0.0001f,0.0001f,0.0001f };
	Vector3 FinalSacle{ 0.0001f,0.0001f,0.0001f };
};
#endif //

