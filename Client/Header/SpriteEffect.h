#ifndef __SPRITEEFFECT_H_
#define __SPRITEEFFECT_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include <optional>
#include "FLight.h"
#include "DynamicLight.h"

class SpriteEffect : public ENGINE::GameObject,
					 public ENGINE::RenderInterface
{
public:
private:
	std::shared_ptr<ENGINE::StaticMesh> _StaticMesh{};
	std::shared_ptr<ENGINE::Texture>    _SpriteTex{};
	std::shared_ptr<ENGINE::Texture>    _DistortionTex{};

	int32 SpriteCol;
	int32 SpriteRow;
	float SpriteColIdx;
	float SpriteRowIdx;
	float CurSpriteUpdateTime = 0.0f;

	float PlayTime = 0.15f;
	float SpriteUpdateTime = 0.0f;
	float EditPlayTime = 0.0f;
	float T = 0.0f;

	std::optional<DynamicLight> _DynamicLight{std::nullopt};
private:
	explicit SpriteEffect() = default;
	virtual ~SpriteEffect() = default;

	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static SpriteEffect* Create();
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
	void InitializeFromOption(const uint32 Option);
	void RegistInfo(
		const float DistortionIntencity = 1.f,
		const float   ColorIntencity = 1.f,
		const Vector4 _Color = Vector4{ 1.f,1.f,1.f,1.f });
	void RegistMesh(const std::string& MeshPath);
	void RegistSpriteInfo(const uint32 Col,const uint32 Row);
	void RegistAlbedoTex(const std::string& TexPath);
	void RegistDistortionTex(const std::string& TexPath);
	void RegistDynamicLight(const DynamicLight& _DynamicLight);
public:
	void PlayStart(
		const float PlayTime,const std::optional<Vector3>& Location = std::nullopt);
private:
	void PlayEnd();
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
public:
	float DistortionIntencity = 1.f;
	float ColorIntencity = 0.01f;
	Vector4 _Color{ 1.f,1.f,1.f,1.f };
private:
	std::optional<float > ConstantPlayTime{ std::nullopt };
};
#endif //