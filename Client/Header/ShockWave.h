#ifndef __SHOCKWAVE_H_
#define __SHOCKWAVE_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include "Vertexs.h"
#include <optional>

class ShockWave : public ENGINE::GameObject,
	              public ENGINE::RenderInterface
{
public:
	enum  Option :int32
	{
		Weak=0,
		Middle,
		Strong,
		GoliathJumpAttack,
		GoliathPunch,
		Change,
		Hit,
		SnatchRush,
		None,
	};
private:
	std::shared_ptr<ENGINE::StaticMesh> Mesh{};
	std::shared_ptr<ENGINE::StaticMesh> _PlaneMesh{};
private:
	explicit ShockWave();
	virtual ~ShockWave() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static ShockWave* Create();
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
	void PlayStart(const Vector3& PlayLocation,
				   const int32& _Option ,
					const bool bBlurMsk=false);
	void PlayEnd();
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
private:
	bool bDirectionReverse = false;
	bool bBlurMsk = false;
	float T = 0.0f;
	float EndT = 1.f;
	float PlayTime = 1.f;
	std::shared_ptr<ENGINE::Texture> BlurMskMap{};
	std::pair<float, float> ScaleLerp{0.001f,0.01f};
	std::pair<float, float> BlurAlpha{0.f,0.5f};
	std::pair<float, float> BlurIntencity{0.f,2.f};

	float BlurMaxLength = 20.f;
	float DistortionAlpha=1.f;
	float DistortionIntencity = 0.f;
	Vector4 Color{ 0.f,0.f,0.f,0.f };

	int32 EditOption = Option::None;
};
#endif //

