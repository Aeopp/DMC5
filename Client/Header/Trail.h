#ifndef __TRAIL_H_
#define __TRAIL_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include <optional>

class Trail :	public ENGINE::GameObject,
				public ENGINE::RenderInterface
{
private:
	std::shared_ptr<ENGINE::Texture> _WaveMask{};
	std::shared_ptr<ENGINE::StaticMesh> _WaveCircle{};

	float WaveIntencity = 0.0f;
	float WaveScale = 0.00299f;
	float WaveSpeed = 0.9f;

	float MinAlpha = 0.5f;
	float EndT = 1.f;
	float T = 0.0f;
	float UV_VOffset = 0.512480f;
	float Sin{};

	Vector4 _Color = { 1.f,1.f,1.f,1.f};

private:
	explicit Trail() = default;
	virtual ~Trail() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static Trail* Create();
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
	void PlayStart(const std::optional<Vector3> & Location = std::nullopt);
private:
	void PlayEnd();
	bool bWaveDistortion{ false };
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
};
#endif //