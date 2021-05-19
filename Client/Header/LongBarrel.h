#ifndef __LONGBARREL_H_
#define __LONGBARREL_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include <optional>

class LongBarrel : public ENGINE::GameObject
		
{
private:
	struct EffectDesc
	{
		std::weak_ptr<class ThunderBolt> _Effect{};
	};
	std::array<EffectDesc, 10u> _PlayEffectDescs{};
private:
	explicit LongBarrel() = default;
	virtual ~LongBarrel() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static LongBarrel* Create();
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
	void PlayStart( const Vector3& Location ,
					const Vector3& PlayDirection);
	void PlayStart(const uint32 BoneIdx /*0,1*/);
private:
	// pole03 = Idx 2
	float AnimationTiming= 0.295090f;
	// pole01 = Idx 0 
	// float AnimationTiming = 0.268436f;
	float LightFlux = 0.1f;
	std::pair<float, float> PlayTimeRange = {0.5f,1.f};
	float ScaleOffset = 0.004f;
	std::pair<float, float> VelocityOffset{ 0.05f,0.25f};
	float PlayLocationOffset = 0.0000f;
	Vector3 PlayDirection{ 0.f,0.f,0.f };
	float DirYawOffset = 0.025f;
	float DirPitchOffset = 0.025f;
	float T = 0.0f;
};
#endif //