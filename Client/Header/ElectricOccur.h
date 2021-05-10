#ifndef __ELECTRICOCCUR_H_
#define __ELECTRICOCCUR_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include "Vertexs.h"
#include <optional>

class ElectricOccur : public ENGINE::GameObject,
			       public ENGINE::RenderInterface
{
private:
	std::shared_ptr<ENGINE::StaticMesh> Mesh{};
private:
	explicit ElectricOccur()  ;
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
public:
	void PlayParticle();
	void RenderDebug(const DrawInfo& _Info);
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
private:
	float ParticleTime = 0.1f;
	float CurParticleTime = 0.0f;
	float T = 0.0f;
	float PlayTime = 3.f;
	float ColorIntencity = 0.1f;
};
#endif //

