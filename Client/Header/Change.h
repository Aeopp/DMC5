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
		const Vector3& Location,
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

	float ColorIntencity{ 1.f };

	float Radian = 0.0f;
	float RadianSpeed = 1.f;
	float VelocityBlurAlpha = 1.f;

	Vector4 _Color{ 1.f,0.f,0.f,100.f/255.f };

	float T{ 0.0f };
	float PlayTime =0.4f;
	float EditPlayTime = 0.4f;

	float EndScale = 0.4f;
};
#endif //

