#ifndef Wire_Arm_h__
#define Wire_Arm_h__

#include "GameObject.h"
#include "RenderInterface.h"
class Nero;
class Wire_Arm : public GameObject,
	public ENGINE::RenderInterface
{
private:
	explicit Wire_Arm();
	virtual ~Wire_Arm() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
public:
	static Wire_Arm* Create();
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT Update(const float _fDeltaTime) override;
	virtual UINT LateUpdate(const float _fDeltaTime) override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
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

	// 렌더링 함수 ..
	void RenderInit();
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);

private:
	std::shared_ptr<ENGINE::SkeletonMesh> m_pMesh;
	std::weak_ptr<Nero>					  m_pNero;

	bool								m_bIsRender;
};



#endif // Wire_Arm_h__