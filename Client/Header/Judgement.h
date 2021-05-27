#ifndef __JUDGEMENT_H_
#define __JUDGEMENT_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include <optional>
#include "ShockWave.h"
#include "CircleWave.h"
#include "JudgementCut.h"

class Judgement : public ENGINE::GameObject,
				  public ENGINE::RenderInterface
{
private:
	std::shared_ptr<ENGINE::StaticMesh> _StaticMesh{};
	std::shared_ptr<ENGINE::Texture> _MagicTexture{};
	std::shared_ptr<ENGINE::Texture> _MagicAlb{};
	std::shared_ptr<ENGINE::Texture> _MagicMsk{};
	std::shared_ptr<ENGINE::Texture> _NrmrMap{};
	
private:
	explicit Judgement() = default;
	virtual ~Judgement() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static Judgement* Create();
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
		const Vector3& Location);
	void PlayEnd();
private:
	void PlayCircleGrowEndParticle();
	void PlayCircleGrowParticle();
	bool bJudgementDayParticlePlay = false;
	void PlayJudgementDayParticle();
	void PlayShockWave();

	void UpdateParticle(const float DeltaTime);
	void UpdateShockWave(const float DeltaTime);

	float GrowLerp()const;
	float GrowEndLerp()const;
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
public:
	bool bCircleGrowEndParticle = false;
	float CurCircleGrowParticleTime = 0.0f;
	float CurJudgementDayParticleTime = 0.0f;
	float T = 0.0f;
	float CurShockDelta = 0.0f;
	std::weak_ptr<ShockWave> _ShockWave{};
	std::weak_ptr<CircleWave> _CircleWave{};
	std::weak_ptr<JudgementCut> _JudgementCut{};

	// 에디터 변수 ......
	float CircleGrowParticleTime= 0.0016f;
	float JudgementDayParticleTime = 0.1f;
	float ShockDelta = 1.3f;
	float ShockScaleFactor = 1000.f;
	
	float PlayTime = 10.f;
	float CircleGrowTime = 1.3f;
	float JudgementReadyTime = 2.0f;

	float GrowEndScale = 0.012f;
	float GrowEndIntencity= 0.2f;
	float JudgementReadyIntencity = 0.01f;
	float DecalIntencity = -0.0001f;

	static inline Vector3 GrowStartColor
	{
		10.f / 255.f,10.f / 255.f, 10.f / 255.f
	};

	static inline Vector3 GrowEndColor
	{
		255.f / 255.f,33.f / 255.f, 33.f / 255.f
	};

	static inline Vector3 JudgementReadyColor
	{
		189.f/255.f,68.f/255.f, 68.f /255.f
	};

	Vector3 DecalColor = { 1.f,1.f,1.f };
};
#endif //