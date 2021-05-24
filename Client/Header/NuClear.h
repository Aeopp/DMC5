#ifndef __NUCLEAR_H_
#define __NUCLEAR_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include <optional>
#include "DynamicLight.h"

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
	DynamicLight _DynamicLight{};
	float CurParticleDelta = 0.0f;

	// 에디터 변수
	float FluxLow = 0.5f;
	float FluxHigh = 1.f;
	float RadiusLow = 1.f;
	float RadiusHigh = 3.f;
	float ParticleEndT = 3.f;
	float ParticleDelta = 0.1f;
	float PlayTime = 5.f;
	float ColorIntencity = 0.001f;
	float GrowEndT = 1.f;
	float GrowEndScale = 0.01f;
};
#endif //