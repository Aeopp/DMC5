#ifndef __ENERGISM_H_
#define __ENERGISM_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include <optional>
#include "DynamicLight.h"
#include "ShockWave.h"
#include "Reverberation.h"

class Energism : public ENGINE::GameObject,
				 public ENGINE::RenderInterface
{
private:
	std::shared_ptr<ENGINE::StaticMesh> _Mesh{};
	std::shared_ptr<ENGINE::Texture> _AlbmMap{};
	std::shared_ptr<ENGINE::Texture> _NoiseMap{};

	float T = 0.0f;
private:
	explicit Energism() = default;
	virtual ~Energism() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static Energism* Create();
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
	// 호출해주세요 . 
	void PlayStart(const Vector3& Location, const float Yaw);
	// 업데이트 시마다 호출해주세요  . 
	void UpdateYaw(const float Yaw);
	void PlayEnd();
	void UpdateReverberation(const float DeltaTime);
	void PlayReverberation();
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
private:
	static constexpr uint32 ReverberationCount= 16u;
	std::array<std::weak_ptr< class Reverberation>, ReverberationCount> ReverberationArr{};
	uint32 CurReverberationIdx{ 0u };

	std::pair<float, float>  ReverationStartRange{0.001f,0.002f};
	std::pair<float, float>  ReverationEndRange{ 0.002f,0.003f};

	float ReverberationOffsetScale = 66.890f;
	float ReverberationDelta = 0.1f;
	float CurReverberationDelta = 0.0f;
	float PlayTime = 6.f;
	float TimeCorr = 0.117f;
	float ColorIntencity = 0.201f;
	float CurveScale = 10.f;
	float NoiseFactor = 1.338f;
};
#endif //