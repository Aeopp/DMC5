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

	// �Ϲ���
	void EventRegist(const std::function<void()>& CallBack,
		const Vector3& Location,
		const Vector3& ColliderBoxSize,
		const bool ImmediatelyEnable,/*���� ���ڸ��� Ȱ��ȭ ??*/
		const GAMEOBJECTTAG& TargetTag = GAMEOBJECTTAG::Player);
	// ���� ���̺�
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
	// GameObject��(��) ���� ��ӵ�
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

	virtual void	OnCollisionEnter(std::weak_ptr<GameObject> _pOther)override;
	virtual void	OnCollisionExit(std::weak_ptr<GameObject> _pOther)override;
private:
	Vector3 TriggerLocation{ 0.f,0.f,0.f };
	Option _Option = Option::None;
	GAMEOBJECTTAG _TargetTag = GAMEOBJECTTAG::Player;
	std::function<void()> _Event{};
	std::weak_ptr<class BoxCollider> _Collider{};
};
#endif //