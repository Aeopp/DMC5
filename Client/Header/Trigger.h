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
	// 트리거 종류 . 
	enum class Option : uint8
	{
		None,
		MonsterWave,
	};

	// 일반적인 상황에서 쓸 함수 . 
	void EventRegist(
		/* 호출 하고자 하는 함수*/
		const std::function<void()>& CallBack,
		/*트리거 위치 */
		const Vector3& Location,
		/*트리거 박스 사이즈 */
		const Vector3& ColliderBoxSize,
		/* 함수를 호출 하자마자 트리거를 활성화 할지 말지 여부 (false 로 하고 트리거 스폰 이후에 켜는 것도 가능)*/
		const bool ImmediatelyEnable,
		/* 트리거와 충돌 검사를 하는 오브젝트의 태그 . */
		const GAMEOBJECTTAG& TargetTag = GAMEOBJECTTAG::Player);

	// 몬스터 웨이브를 스폰 하고자 하는 트리거는 해당 오버로딩된 해당 함수를 호출 . 
	void EventRegist(
					/* 미리 리소스 로딩이 끝난 몬스터 배열*/
					const std::vector<std::weak_ptr<class Monster>>& WaveMonster,
					/* 트리거 위치 */
					const Vector3& Location,
					/* 트리거 박스 사이즈 */
					const Vector3& ColliderBoxSize,
					/* 함수를 호출 하자마자 트리거를 활성화 할지 말지 여부 (false 로 하고 트리거 스폰 이후에 켜는 것도 가능)*/
					const bool ImmediatelyEnable,
					/* 트리거와 충돌 검사를 하는 오브젝트의 태그 . */
					const GAMEOBJECTTAG& TargetTag,
					/* 몬스터를 스폰 하고 바로 직후에 호출 하고자 하는 함수*/
					const std::function<void()>& SpawnAfterEvent ,
					/* 몬스터 웨이브가 끝났을 때 (몬스터를 전부 죽인 그 시점) 에 호출 하고자 하는 함수*/
					const std::function<void()>& WaveEndEvent);

	// 트리거가 발동된 다음이라면 true
	bool IsAfterEvent();
	// 트리거 활성화 여부 
	bool IsEnable();
	// 트리거 껐다 켜기 
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
	bool bAfterEvent = false;
	bool bEnable = false;
	Vector3 TriggerLocation{ 0.f,0.f,0.f };
	Option _Option = Option::None;
	GAMEOBJECTTAG _TargetTag = GAMEOBJECTTAG::Player;
	// 몬스터가 다 죽은 시점에 호출 함수
	std::function<void()> _WaveEndEvent;
	// 트리거 발동 콜백 함수 
	std::function<void()> _Event{};
	// 몬스터가 다 죽은 시점을 체크하기 위한 배열 . 
	std::optional<std::vector<std::weak_ptr<class Monster>>> CheckIfTheDead{};
	std::weak_ptr<class BoxCollider> _Collider{};
};
#endif //