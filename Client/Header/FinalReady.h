//#ifndef __FINALREADY_H_
//#define __FINALREADY_H_
//#include "GameObject.h"
//#include "RenderInterface.h"
//#include <optional>
//
//class FinalReady : public ENGINE::GameObject,
//				   public ENGINE::RenderInterface
//{
//private:
//	std::shared_ptr<ENGINE::StaticMesh> _StaticMesh{};
//	std::shared_ptr<ENGINE::Texture> _Albm{};
//private:
//	explicit FinalReady() = default;
//	virtual ~FinalReady() = default;
//	// GameObject을(를) 통해 상속됨
//	virtual void Free() override;
//	virtual std::string GetName() override;
//public:
//	static FinalReady* Create();
//public:
//	virtual void    RenderReady()                          override;
//public:
//	void			RenderInit();
//	virtual HRESULT Ready() override;
//	virtual HRESULT Awake() override;
//	virtual HRESULT Start() override;
//	virtual UINT    Update(const float _fDeltaTime) override;
//	virtual UINT    LateUpdate(const float _fDeltaTime) override;
//	virtual void    Editor()override;
//	virtual void	OnEnable() override;
//	virtual void    OnDisable() override;
//public:
//	void PlayStart(const Vector3& Location);
//	void PlayEnd();
//	void PlayParticle();
//public:
//	float T = 0.0f;
//	float PlayTime = 5.f;
//
//	void RenderDebug(const DrawInfo& _Info);
//	void RenderAlphaBlendEffect(const DrawInfo& _Info);
//};
//#endif //