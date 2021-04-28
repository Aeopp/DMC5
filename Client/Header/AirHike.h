#ifndef __AIRHIKE_H_
#define __AIRHIKE_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include <optional>

class AirHike : public ENGINE::GameObject,
				public ENGINE::RenderInterface
{
private:
	std::shared_ptr<ENGINE::StaticMesh> _StaticMesh{};
	std::shared_ptr<ENGINE::Texture> _GradBlue{};

	// Sin Graph
	Vector4 StartColor{ 179.f / 255.f,179.f / 255.f,255.f / 255.f,1.f };
	float   StartIntencity{ 0.12916f };
	float   StartScale{ 0.005170f };

	Vector4 FinalColor{  179.f/255.f,179.f/255.f,255.f/255.f,1.f};
	float   FinalIntencity{ 0.082540f};
	float   FinalScale{ 0.008031f};
	float Speed = 5.274994f;
	
	float T{};
	float Sin{};
private:
	explicit AirHike() = default;
	virtual ~AirHike() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static AirHike* Create();
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
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
};
#endif //