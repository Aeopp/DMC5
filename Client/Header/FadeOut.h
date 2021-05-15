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
	std::shared_ptr<ENGINE::Texture> AlbmMap{};
	float T = 0.0f;
	float Intencity{ 1.f };
	float Noisewrap{ 8.173007f };
	float Timecorr{ 0.304011f };
	Vector4 DistortionColor{ 246.f / 255.f,10.f / 255.f,10.f / 255.f,1.f };
	float DistortionIntencity{ 1.f };
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
	void PlayStart();
	void PlayEnd();
public:
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
};
#endif //