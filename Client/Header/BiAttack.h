#ifndef __BIATTACK_H_
#define __BIATTACK_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include "ElectricOccur.h"
#include <optional>

class BiAttack : public ENGINE::GameObject
		
{
private:
	struct EffectDesc
	{
		std::weak_ptr<class ThunderBolt> _Effect{};
	};
	std::array<BiAttack::EffectDesc, 10u> _PlayEffectDescs{};
	std::weak_ptr<ElectricOccur> _ElectricOccur{};
private:
	explicit BiAttack() = default;
	virtual ~BiAttack() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static BiAttack* Create();
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
private:
	float ElectricForwardOffset = 0.05f;
	float LightRadius = 0.5f;
	float OccurScale = 0.0005f;
	float LightFlux = 0.1f;
	std::pair<float, float> PlayTimeRange = {0.395f,0.525f};
	float ScaleOffset = 0.004f;
	std::pair<float, float> VelocityOffset{ 0.1f,0.72f};
	float PlayLocationOffset = 0.0000f;
	Vector3 PlayDirection{ 0.f,0.f,0.f };
	float DirYawOffset = 0.25f;
	float DirPitchOffset = 0.118f;
	float T = 0.0f;
};
#endif //