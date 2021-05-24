#ifndef __NUCLEARLENSFLARE_H_
#define __NUCLEARLENSFLARE_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include <optional>
#include "DynamicLight.h"

class NuclearLensFlare : public ENGINE::GameObject,
				  public ENGINE::RenderInterface
{
private:
	std::shared_ptr<ENGINE::StaticMesh> _Mesh{};
	std::shared_ptr<ENGINE::Texture>    _Alpg{};
private:
	explicit NuclearLensFlare() = default;
	virtual ~NuclearLensFlare() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static NuclearLensFlare* Create();
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
	void UpdatePlayVariable(const float Scale , const Vector3 Position);
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
private:
	float T = 0.0f;
	float PlayTime = 24.f;
	float ScaleOffset = 0.005f;
	float ColorIntencity = 0.2f;
};
#endif //