#ifndef __SATELLITE_H_
#define __SATELLITE_H_
#include "GameObject.h"
#include "RenderInterface.h"

#include <optional>

class Satellite : public ENGINE::GameObject
		
{
private:
	struct EffectDesc
	{
		std::weak_ptr<class ThunderBolt> _Effect{};
	};
	std::array<Satellite::EffectDesc, 40u> _PlayEffectDescs{};
	std::weak_ptr<class ElectricBranch > _ElectricBranch{};
private:
	explicit Satellite() = default;
	virtual ~Satellite() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static Satellite* Create();
public:
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT    Update(const float _fDeltaTime) override;
	virtual UINT    LateUpdate(const float _fDeltaTime) override;
	virtual void    Editor()override;
	virtual void	OnEnable() override;
	virtual void    OnDisable() override;
public:
	void PlayStart(const Vector3& Location ,
					const Vector3& PlayDirection);
	void PlayEnd();
private:
	bool bPlay = false;
	float ThunderBoltDelta= 0.001872f;
	float CurThunderBoltDelta = 0.0f;

	float ElectricForwardOffset = 0.024064f;
	float PlayLocationOffset = 0.1f;
	std::pair<float, float> VelocityOffset{ 0.238235f,0.721925f};
	float LightRadius = 0.5f;
	float BranchScale = 0.005f;
	float LightFlux = 0.1f;
	std::pair<float, float> PlayTimeRange = {0.1f,0.2f};
	float ScaleOffset = 0.002674f;

	Vector3 PlayDirection{ 0.f,0.f,0.f };
	float DirYawOffset = 3.142f;
	float DirPitchOffset = 3.142f;
	float T = 0.0f;
	Vector3 CurrentPlayDireciton{ 0.f,0.f,0.f };
};
#endif //