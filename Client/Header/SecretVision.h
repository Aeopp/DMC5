#ifndef __SECRETVISION_H_
#define __SECRETVISION_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include "Vertexs.h"
#include <optional>

class SecretVision : public ENGINE::GameObject,
			         public ENGINE::RenderInterface
{
	struct SecretVisionDesc
	{
		float ColorIntencity = 0.0f;
		float AlphaFactor = 0.0f;
		float DistortionIntencity = 1.f;
		int32 Life = 6u;
		bool bDistortion = false;
	};
private:
	std::shared_ptr<ENGINE::StaticMesh> _Mesh{};
	std::array<std::shared_ptr<ENGINE::Texture>, 3u > _TextureArr{};
	std::shared_ptr < ENGINE::Texture> _NoiseMap{};
	std::array<SecretVisionDesc, 3u> _SVDescs{};
private:
	explicit SecretVision()  ;
	virtual ~SecretVision() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static SecretVision* Create();
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
	void RenderDebug(const DrawInfo& _Info);
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
private:
	void Interaction(const uint32 Idx);
	void Disappear(const uint32 Idx);
private:
	float VisionBias = 0.0001f;

	float NoiseWrap = 0.6f;
	float TimeCorr = 0.111675f;

	float HitAddColorIntencity = 0.1f;
	float HitAddAlphaFactor  = 0.1f;
	float HitMinusDistortionIntencity = 0.1f;
};
#endif //

