#ifndef __ARTEMISMISSILE_H_
#define __ARTEMISMISSILE_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include <optional>
#include "Vertexs.h"
#include <vector>
#include "ShapeParticle.h"

class ArtemisMissile : public ENGINE::GameObject,
					   public ENGINE::RenderInterface
{
private:
	std::shared_ptr<ENGINE::StaticMesh> _Mesh{};
	std::shared_ptr<ENGINE::Texture> GradMap{};
private:
	explicit ArtemisMissile()  ;
	virtual ~ArtemisMissile() = default;
	// GameObject��(��) ���� ��ӵ�
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static ArtemisMissile* Create();
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
	void PlayStart(const bool bUlte = false);
	void PlayEnd();
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
	void ParticleUpdate(const float DeltaTime);
private:
	void RushParticle();
private:
	float ParticleDelta = 0.01f;
	float CurParticleDelta = 0.0f;
	float T = 0.0f;

	Vector4 Color{ 
		241.f / 255.f,  
		218.f / 255.f,  
		237.f / 255.f  ,
		1.f };

	float LuminDecayTime = 2.f;
	float StartColorIntencity = 6.8f;
	float ColorIntencity = 0.1f;
	bool  bUlte = false;
};
#endif //

