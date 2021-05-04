#ifndef __SPRITEEFFECT_H_
#define __SPRITEEFFECT_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include <optional>

class SpriteEffect : public ENGINE::GameObject,
	                 public ENGINE::RenderInterface
{
public:
	enum class MeshType : int32
	{
		Static,
		Skeleton
	};
private:
	std::shared_ptr<ENGINE::StaticMesh> _StaticMesh{};
	std::shared_ptr<ENGINE::Texture> _SpriteTex{};
	std::shared_ptr<ENGINE::Texture> _DistortionTex{};

	int32 SpriteCol;
	int32 SpriteRow;
	float SpriteColIdx;
	float SpriteRowIdx;
	float CurSpriteUpdateTime = 0.0f;

	float DistortionIntencity = 1.f;
	float ColorIntencity = 0.001f;
	Vector4 _Color{1.f,1.f,1.f,1.f};

	float SpriteUpdateTime = 0.0f;

	float EditSpriteUpdateTime = 0.0f;
	/*int32 EditRow{0};
	int32 EditCol{0};*/
private:
	explicit SpriteEffect() = default;
	virtual ~SpriteEffect() = default;
	// GameObject을(를) 통해 상속됨
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
	void RegistInfo(const float DistortionIntencity = 1.f,
					const float ColorIntencity = 1.f,
					
					const Vector4 _Color = Vector4{ 1.f,1.f,1.f,1.f });

	void RegistMesh(const std::string& MeshPath);
	void RegistSpriteInfo(const uint32 Col, const uint32 Row);
	void RegistAlbedoTex(const std::string& TexPath );
	void RegistDistortionTex(const std::string& TexPath);
public:
	void PlayStart(
				const float SpriteUpdateTime ,const std::optional<Vector3>& Location = std::nullopt);
private:
	void PlayEnd();
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
};
#endif //