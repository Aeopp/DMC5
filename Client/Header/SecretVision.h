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
		static const uint32  DefaultLife = 4u;
		static inline float  DisappearEndColorIntencity = 177.f;
		static inline float  DisappearAcc = 0.6f;

		float ColorIntencity = 0.0f;
		float AlphaFactor = 0.0f;
		int32 Life = DefaultLife;
		bool bSurvive = true;
		bool bStartInteraction = false;
		float T = 0.0f;
	};
private:
	std::shared_ptr<ENGINE::StaticMesh> _Mesh{};
	std::array<std::shared_ptr<ENGINE::Texture>, 3u > _TextureArr{};
	std::shared_ptr < ENGINE::Texture> _NoiseMap{};
	std::array<SecretVisionDesc, 3u> _SVDescs{};
private:
	explicit SecretVision();
	virtual ~SecretVision() = default;
	// GameObject��(��) ���� ��ӵ�
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

	virtual void OnTriggerEnter(std::weak_ptr<GameObject> _Target)override;
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
public:
	void   PuzzleStart();
	uint32 GetInteractionIdx()const;
	void   SetInteractionEnable(const bool bInteraction);
	void   StartInteraction(const uint32 Idx);
private:
	void Interaction(const uint32 Idx);
	void Disappear(const uint32 Idx);
	void PuzzleEndParticle();
	void Default();
	void PuzzleEnd();
private:
	// ������Ʈ
	std::weak_ptr<BoxCollider> _Collider{};
	bool bEnable = false;
	//
	bool   bInteraction = false;
	uint32 InteractionIdx = 0u;
	std::optional<float> PuzzleStartT = std::nullopt;

	static const inline float DefaultNoiseWrap = 0.6f;
	static const inline float DefaultDistortionIntencity = 1.f;

	bool bDistortion = true;
	float DistortionIntencity = DefaultDistortionIntencity;
	float VisionBias = 0.0001f;

	float NoiseWrap = DefaultNoiseWrap;
	float TimeCorr = 0.111675f;

	float HitMinusNoiseWrap = DefaultNoiseWrap / static_cast<float>(SecretVisionDesc::DefaultLife);
	float HitMinusDistortionIntencity = -0.2f;

	float HitAddColorIntencity = 0.2f;
	float HitAddAlphaFactor    = 0.2f;

	std::optional<float> NhDoorOpenTime{ std::nullopt };

};
#endif //