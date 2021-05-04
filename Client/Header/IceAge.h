#ifndef __ICEAGE__H_
#define __ICEAGE__H_
#include "GameObject.h"
#include "RenderInterface.h"
#include <optional>
#include "Vertexs.h"
#include <vector>
#include "ShapeParticle.h"


class IceAge :     public ENGINE::GameObject,
			       public ENGINE::RenderInterface
{
public:
	//struct Particle
	//{
	//	static inline float  ColorIntencity = 1.f;
	//	static inline float  EmissiveIntencity = 1.f;
	//	static inline float  AlphaFactor = 1.f;
	//	static inline float  SpecularPow = 20.f;

	//	float  ColorIntencity = 1.f;
	//	float  EmissiveIntencity = 1.f;
	//	float  AlphaFactor = 1.f;
	//	float  SpecularPow = 20.f;

	//	Vector3 Scale{1.f,1.f,1.f};
	//	Vector3 Rotation{0.f,0.f,0.f};
	//	Vector3 Location{0.f,0.f,0.f};
	//};
private:
	std::shared_ptr<ENGINE::StaticMesh> Inner{};
	std::shared_ptr<ENGINE::StaticMesh> IceParticle{};
	std::shared_ptr<ENGINE::Texture>    Albedo{};
	std::shared_ptr<ENGINE::Texture>    TrailMap{};
	std::shared_ptr<ENGINE::Texture>    EmssiveMskMap{};
	std::shared_ptr<ENGINE::Texture>    NoiseMap{};
private:
	explicit IceAge()  ;
	virtual ~IceAge() = default;
	// GameObject��(��) ���� ��ӵ�
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static IceAge* Create();
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
		const float YawRotationSpeed = FMath::PI,
		const float PlayTime = 5.f);
	void PlayEnd();
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
	void RenderEftIceParticle(const DrawInfo& _Info);
private:
	// std::weak_ptr< ShapeParticle> _GeneratorParticle{};
	// std::vector<Particle> _IceParticles{};

	float EmissiveIntencity{ 1.f };
	float ColorIntencity{ 1.f };
	float DistortionIntencity{ 1.f };

	float Radian = 0.0f;
	float RadianSpeed = 1.f;

	Vector3 NoiseScale      { 1.f,2.f,3.f };
	Vector3 NoiseScrollSpeed{ 1.f,2.f,3.f };

	Vector2 NoiseDistortion0{ 0.3f,1.f };
	Vector2 NoiseDistortion1{ 0.5f,0.77f};
	Vector2 NoiseDistortion2{ 0.77f,0.5f};

	float RollRotationSpeed = FMath::PI;

	float T{ 0.0f };
	float PlayTime = 5.f;

	float EditRotationSpeed{ 0.f };
};
#endif //

