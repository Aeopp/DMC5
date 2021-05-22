#ifndef __LENSFLARE_H_
#define __LENSFLARE_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include <optional>

class LensFlare : public ENGINE::GameObject,
				public ENGINE::RenderInterface
{
private:
	std::shared_ptr<ENGINE::StaticMesh> _Mesh{};
	std::shared_ptr<ENGINE::Texture>    _Alpg{};
private:
	explicit LensFlare() = default;
	virtual ~LensFlare() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static LensFlare* Create();
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
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
private:
	float T = 0.0f;
	float PlayTime = 0.4f;
	float ColorIntencity = 0.001f;
	float StartScale = 0.000f;
	float EndScale = 0.006f;
};
#endif //