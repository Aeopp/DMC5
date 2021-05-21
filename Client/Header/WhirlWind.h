#ifndef __WHIRLWIND_H_
#define __WHIRLWIND_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include "Vertexs.h"
#include <optional>
#include "FLight.h"

class WhirlWind :  public ENGINE::GameObject,
			       public ENGINE::RenderInterface
{
private:
	std::shared_ptr<ENGINE::StaticMesh> _Mesh{};
	std::shared_ptr<ENGINE::Texture> _BlurMap{};
private:
	explicit WhirlWind()  ;
	virtual ~WhirlWind() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static WhirlWind* Create();
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
		           // vMyPos
				   const Vector3& Location,
				   // Look 
				   const Vector3& Direction,
				   const bool bEditPlay=false);
	void PlayEnd();
public:
	void ParticleUpdate(const float DeltaTime);
	void RenderDebug(const DrawInfo& _Info);
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
private:
	float PlayTime = 0.2f;
	float T = 0.0f;
	float ForwardOffset    = 0.30f;
	float ParticleDelta    = 0.0016f;
	float CurParticleDelta = 0.0f;
	float Velocity = 0.f;
	float BlurIntencity = 1.f;
	Vector3 EditPlayDirection{ 1.f,0.f,0.f };
	Vector3 CurrentDirection{ 1.f,0.f,0.f };

	bool bEditPlay = false;

	D3DXCOLOR PtLightColor{ 214.f/255.f , 124.f/255.f,  68.f/255.f ,1.f};
	std::pair<float, float> LightFluxLerp{16.360f,8.180f};
	float PointLightRadius = 14.315f;
	std::weak_ptr<class FLight> PtLight{};
};
#endif //

