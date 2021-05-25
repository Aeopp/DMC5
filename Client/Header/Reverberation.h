#ifndef __REVERBERATION_H_
#define __REVERBERATION_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include <optional>
#include "Vertexs.h"
#include <vector>
#include "ShapeParticle.h"

class Reverberation : public ENGINE::GameObject,
	public ENGINE::RenderInterface
{
private:
	std::shared_ptr<ENGINE::StaticMesh> _Mesh{};
	std::shared_ptr<ENGINE::Texture> GradMap{};
private:
	explicit Reverberation();
	virtual ~Reverberation() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static Reverberation* Create();
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
	void PlayStart(const Vector3& Location,
		const Vector3& Direction,
		const float StartScale,
		const float EndScale ,
		const bool bEditPlay=false);
	void PlayEnd();
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
	void ParticleUpdate(const float DeltaTime);
	float Velocity = 0.3f;
private:
	float ParticleDelta = 0.01f;
	float CurParticleDelta = 0.0f;
	float T = 0.0f;
	Matrix DirRotMatrix{ FMath::Identity() };
	Vector3 Direction{ 0.f,0.f,1.f};
	bool bEditPlay = false;

	// 에디터 변수 .
	float PlayTime = 1.25f;
	Vector4 Color{
		241.f / 255.f,
		218.f / 255.f,
		237.f / 255.f  ,
		1.f };

	float ColorIntencityHigh = 0.02f;
	float ColorIntencityLow = 0.01;
	float TimeAcc = 1.0f;
	float StartScale = 0.00075f;
	float EndScale = 0.001f;
};
#endif //

