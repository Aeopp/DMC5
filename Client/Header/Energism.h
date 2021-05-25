#ifndef __ENERGISM_H_
#define __ENERGISM_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include <optional>
#include "DynamicLight.h"
#include "ShockWave.h"

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
	// GameObject��(��) ���� ��ӵ�
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
	void PlayStart(const Vector3& Location);
	void PlayEnd();
private:
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
private:
	float PlayTime = 6.f;
	float TimeCorr = 0.117f;
	float ColorIntencity = 0.201f;
	float CurveScale = 5.017f;
	float NoiseFactor = 0.167f;
};
#endif //