#ifndef __FADEOUT_H_
#define __FADEOUT_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include <optional>

class FadeOut : public ENGINE::GameObject,
				public ENGINE::RenderInterface
{
private:
	std::shared_ptr<ENGINE::Texture> NoiseMap{};
	std::array<std::shared_ptr<ENGINE::Texture>,8u>AlbmMap{};
	float T = 0.0f;
	float Intencity{ 1.f };
	float Noisewrap{ 1.f };
	float Timecorr{ 1.f };
	std::array<Vector3, 2u> DistortionColor
	{
		Vector3{ 0.f,0.f,0.f },
		Vector3{ 1.f,1.f,1.f }
	};
	float DistortionIntencity{ 1.f };
	float NoiseIntencity{ 1.f };
	float Acceleration = 0.2f;
	float EndTime = 1.f;
private:
	explicit FadeOut() = default;
	virtual ~FadeOut() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static FadeOut* Create();
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
	void PlayStart(const uint32 AlbIdx);
	void PlayEnd();
public:
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
private:
	int32 AlbIdx = 0u;
};
#endif //