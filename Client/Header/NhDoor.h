#ifndef __NHDOOR_H_
#define __NHDOOR_H_
#include "GameObject.h"
#include "RenderInterface.h"

class NhDoor :     public ENGINE::GameObject ,
				   public ENGINE::RenderInterface				
{

private:
	std::shared_ptr<ENGINE::StaticMesh> _StaticMesh{};
	std::shared_ptr<ENGINE::Texture> _DissolveMap{};
	std::shared_ptr<ENGINE::Texture> _BurnMap{};
	Vector3 BurnColor{ 14.f/255.f,1.f,0.f};
	float SliceAmount = 0.0f;
	float BurnSize = 0.25f;
	float EmissionAmount = 100.f;
private:
	explicit NhDoor() = default;
	virtual ~NhDoor() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static NhDoor* Create();
public:
	virtual void    RenderReady()                          override;
public:
	void RenderInit();
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT    Update(const float _fDeltaTime) override;
	virtual UINT    LateUpdate(const float _fDeltaTime) override;
	virtual void    Editor()override;
	virtual void	OnEnable() override;
	virtual void    OnDisable() override;
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderGBuffer(const DrawInfo& _Info);
	void RenderShadow(const DrawInfo& _Info);
public:
	void DissolveStart();
	void DissolveEnd();
private:
	bool bDissolve = false;
	static const inline float DissolveParticleDelta = 0.03f;
	float CurDissolveParticleDelta = 0.0f;
	void  DissolveParticle();
};
#endif //