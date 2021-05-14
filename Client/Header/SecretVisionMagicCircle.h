#ifndef __SV_MAGICCIRCLE_H_
#define __SV_MAGICCIRCLE_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include <optional>

class SecretVisionMagicCircle : public ENGINE::GameObject,
								public ENGINE::RenderInterface
{
public:
	enum TexID : uint32
	{
		SECRETVISION_0 = 0u,
		SECRETVISION_1,
		SECRETVISION_2,

		MAX_VARIATION_IDX
	};
	void SetTexID(SecretVisionMagicCircle::TexID Idx);

private:
	SecretVisionMagicCircle::TexID _TexID = SECRETVISION_0;

	std::shared_ptr<ENGINE::StaticMesh> _StaticMesh{};
	std::shared_ptr<ENGINE::Texture> _SVTex0{};
	std::shared_ptr<ENGINE::Texture> _SVTex1{};
	std::shared_ptr<ENGINE::Texture> _SVTex2{};

	// Sin Graph
	Vector4 StartColor{ 1.f ,1.f,1.f,1.f };
	float   StartIntencity{ 0.12916f };
	float   StartScale{ 0.005170f };

	Vector4 FinalColor{ 100.f /255.f,100.f/255.f,255.f/255.f,0.f};
	float   FinalIntencity{ 0.082540f};
	float   FinalScale{ 0.008031f };
	float Speed = 5.5f;

	float T = 0.0f;
	float Sin{};
private:
	explicit SecretVisionMagicCircle() = default;
	virtual ~SecretVisionMagicCircle() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static SecretVisionMagicCircle* Create();
public:
	virtual void    RenderReady() override;
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
	void RenderDebug(const DrawInfo& _Info);
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
		const std::optional<Vector3>& Rotation = std::nullopt,
		const float StartScale = 0.005170f,
		const float FinalScale = 0.008031f,
		const float PlayTime = FMath::PI);
	void PlayEnd();
private:
	void PlayParticle();
public:
	float ParticleTime = 0.001f;
	float CurParticleTime = 0.0f;

	bool bPlayedEndParticle = false;
	float PlayTime = FMath::PI / 2.f;
};
#endif // !__SV_MAGICCIRCLE_H_