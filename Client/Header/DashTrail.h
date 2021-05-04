#ifndef __DASHTRAIL_H_
#define __DASHTRAIL_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include <optional>

class DashTrail  :   public ENGINE::GameObject,
	                 public ENGINE::RenderInterface
{
private:
	std::map<int32, std::shared_ptr<StaticMesh>> _Meshs{};
	float TrailEndTime = 0.0f;
	float CurTrailTime = 0.0f;

	float ColorIntencity = 1.f;
	float DistortionIntencity = 1.f;
	Vector4 _Color{1.f,1.f,1.f,1.f};
	float EditTrailEndTime = 1.f;
private:
	explicit DashTrail() = default;
	virtual ~DashTrail() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static DashTrail* Create();
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
	void PlayStart(
		const float TrailEndTime = 0.0f
		, const std::optional<Vector3>& Location = std::nullopt);
private:
	void PlayEnd();
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
};
#endif //