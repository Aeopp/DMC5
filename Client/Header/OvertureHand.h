#ifndef __EFF_OVERTUREHAND__
#define __EFF_OVERTUREHAND__
#include "Effect.h"

class OvertureHand final : public Effect
{
private:
	std::shared_ptr<ENGINE::StaticMesh> _HandMesh{};
	std::shared_ptr<ENGINE::StaticMesh> _SparkBranchMesh{};
	//std::shared_ptr<ENGINE::StaticMesh> _SparkPartsMesh{};

	std::shared_ptr<ENGINE::Texture> _LightningTex{};
	std::shared_ptr<ENGINE::Texture> _GlowTex{};
	std::shared_ptr<ENGINE::Texture> _LightningColorTex{};
	std::shared_ptr<ENGINE::Texture> _NoiseTex{};

	float _SliceAmount = 0.f;
	float _RandTexV0 = 0.8f;
	float _RandTexV1 = 0.6f;
	float _SparkBranchSubsetIdx = 0.f;
	Matrix _SparkBranchWorldMatrix = Matrix();

private:
	explicit OvertureHand() = default;
	virtual ~OvertureHand() = default;
	// Effect을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
	virtual void RenderReady() override;
	virtual void Imgui_Modify() override;
private:
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
public:
	static OvertureHand* Create();
public:
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT Update(const float _fDeltaTime) override;
	virtual UINT LateUpdate(const float _fDeltaTime) override;
	virtual void Editor() override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
public:
	virtual void Reset() override;
};
#endif // !__EFF_OVERTUREHAND__