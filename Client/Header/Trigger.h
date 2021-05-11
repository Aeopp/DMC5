#ifndef __TRIGGER_H_
#define __TRIGGER_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include "BoxCollider.h"
#include <optional>
#include <functional>

class Trigger : public ENGINE::GameObject
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
					 const bool ImmediatelyEnable, /*생성 하자마자 활성화 ??*/
					 const GAMEOBJECTTAG& TargetTag = GAMEOBJECTTAG::Player);
	// 몬스터 웨이브
	void EventRegist(const std::vector<std::weak_ptr<class Monster>>& 
					 WaveMonster,
					 const std::function<void()>& SpawnAfterEvent,
					 const Vector3& Location,
					 const Vector3& ColliderBoxSize,
					 const bool ImmediatelyEnable, /*생성 하자마자 활성화 ??*/
					 const GAMEOBJECTTAG& TargetTag = GAMEOBJECTTAG::Player);

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
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT    Update(const float _fDeltaTime) override;
	virtual UINT    LateUpdate(const float _fDeltaTime) override;
	virtual void    Editor()   override;
	virtual void	OnEnable() override;
	virtual void    OnDisable()override;

	virtual void	OnCollisionEnter(std::weak_ptr<GameObject> _pOther)override;
	virtual void	OnCollisionExit(std::weak_ptr<GameObject> _pOther)override;
private:
	Option _Option = Option::None;
	GAMEOBJECTTAG _TargetTag = GAMEOBJECTTAG::Player;
	std::function<void()> _Event{};
	std::weak_ptr<class BoxCollider> _Collider{};
};
#endif //