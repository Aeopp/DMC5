#ifndef __TRIGGER_H_
#define __TRIGGER_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include "BoxCollider.h"
#include <optional>
#include <functional>

class Trigger : public ENGINE::GameObject ,
				public ENGINE::RenderInterface
{
public:
	enum class Option : uint8
	{
		None,
		MonsterWave,
	};

	// 일반적
	void EventRegist(const std::function<void()>& CallBack,
		const Vector3& Location,
		const Vector3& ColliderBoxSize,
		const bool ImmediatelyEnable,/*생성 하자마자 활성화 ??*/
		const GAMEOBJECTTAG& TargetTag = GAMEOBJECTTAG::Player);
	// 몬스터 웨이브
	void EventRegist(const std::vector<std::weak_ptr<class Monster>>& WaveMonster,
					const Vector3& Location,
					const Vector3& ColliderBoxSize,
					const bool ImmediatelyEnable,
					const GAMEOBJECTTAG& TargetTag,
					const std::function<void()>& SpawnAfterEvent );

	bool IsEnable();
	void TriggerEnable();
	void TriggerDisable();
private:
	explicit Trigger() = default;
	virtual ~Trigger() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static Trigger* Create();
public:
	virtual void RenderReady()override;

	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT    Update(const float _fDeltaTime) override;
	virtual UINT    LateUpdate(const float _fDeltaTime) override;
	virtual void    Editor()   override;
	virtual void	OnEnable() override;
	virtual void    OnDisable()override;

	virtual void	OnTriggerEnter(std::weak_ptr<GameObject> _pOther)override;
	virtual void	OnTriggerExit(std::weak_ptr<GameObject> _pOther)override;
private:
	bool bEnable = false;
	Vector3 TriggerLocation{ 0.f,0.f,0.f };
	Option _Option = Option::None;
	GAMEOBJECTTAG _TargetTag = GAMEOBJECTTAG::Player;
	std::function<void()> _Event{};
	std::weak_ptr<class BoxCollider> _Collider{};
};
#endif //