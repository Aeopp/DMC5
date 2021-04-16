#ifndef Buster_Arm_h__
#define Buster_Arm_h__

#include "GameObject.h"
#include "RenderInterface.h"
class Nero;
class Buster_Arm : public GameObject,
	public ENGINE::RenderInterface
{
private:
	explicit Buster_Arm();
	virtual ~Buster_Arm() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
public:
	static Buster_Arm* Create();
public:
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT Update(const float _fDeltaTime) override;
	virtual UINT LateUpdate(const float _fDeltaTime) override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;

	// 렌더링 함수 ... 
	void RenderInit();
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);
public:
	void ChangeAnimation(const std::string& InitAnimName, const bool  bLoop, const AnimNotify& _Notify = {});
public:
	virtual std::string GetName() override;
	float Get_PlayingTime();
	float Get_PlayingAccTime();
public:
	// RenderInterface을(를) 통해 상속됨
	virtual void RenderReady() override;

	virtual void Editor()override;

private:
	std::shared_ptr<ENGINE::SkeletonMesh> m_pMesh;
	std::weak_ptr<Nero>					  m_pNero;

	bool								m_bIsRender;
};



#endif // Buster_Arm_h__