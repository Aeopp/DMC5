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
	// Ʈ���� ���� . 
	enum class Option : uint8
	{
		None,
		MonsterWave,
	};

	// �Ϲ����� ��Ȳ���� �� �Լ� . 
	void EventRegist(
		/* ȣ�� �ϰ��� �ϴ� �Լ�*/
		const std::function<void()>& CallBack,
		/*Ʈ���� ��ġ */
		const Vector3& Location,
		/*Ʈ���� �ڽ� ������ */
		const Vector3& ColliderBoxSize,
		/* �Լ��� ȣ�� ���ڸ��� Ʈ���Ÿ� Ȱ��ȭ ���� ���� ���� (false �� �ϰ� Ʈ���� ���� ���Ŀ� �Ѵ� �͵� ����)*/
		const bool ImmediatelyEnable,
		/* Ʈ���ſ� �浹 �˻縦 �ϴ� ������Ʈ�� �±� . */
		const GAMEOBJECTTAG& TargetTag = GAMEOBJECTTAG::Player);

	// ���� ���̺긦 ���� �ϰ��� �ϴ� Ʈ���Ŵ� �ش� �����ε��� �ش� �Լ��� ȣ�� . 
	void EventRegist(
					/* �̸� ���ҽ� �ε��� ���� ���� �迭*/
					const std::vector<std::weak_ptr<class Monster>>& WaveMonster,
					/* Ʈ���� ��ġ */
					const Vector3& Location,
					/* Ʈ���� �ڽ� ������ */
					const Vector3& ColliderBoxSize,
					/* �Լ��� ȣ�� ���ڸ��� Ʈ���Ÿ� Ȱ��ȭ ���� ���� ���� (false �� �ϰ� Ʈ���� ���� ���Ŀ� �Ѵ� �͵� ����)*/
					const bool ImmediatelyEnable,
					/* Ʈ���ſ� �浹 �˻縦 �ϴ� ������Ʈ�� �±� . */
					const GAMEOBJECTTAG& TargetTag,
					/* ���͸� ���� �ϰ� �ٷ� ���Ŀ� ȣ�� �ϰ��� �ϴ� �Լ�*/
					const std::function<void()>& SpawnAfterEvent ,
					/* ���� ���̺갡 ������ �� (���͸� ���� ���� �� ����) �� ȣ�� �ϰ��� �ϴ� �Լ�*/
					const std::function<void()>& WaveEndEvent);

	// Ʈ���Ű� �ߵ��� �����̶�� true
	bool IsAfterEvent();
	// Ʈ���� Ȱ��ȭ ���� 
	bool IsEnable();
	// Ʈ���� ���� �ѱ� 
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

	virtual void	OnTriggerEnter(std::weak_ptr<GameObject> _pOther)override;
	virtual void	OnTriggerExit(std::weak_ptr<GameObject> _pOther)override;
private:
	bool bAfterEvent = false;
	bool bEnable = false;
	Vector3 TriggerLocation{ 0.f,0.f,0.f };
	Option _Option = Option::None;
	GAMEOBJECTTAG _TargetTag = GAMEOBJECTTAG::Player;
	// ���Ͱ� �� ���� ������ ȣ�� �Լ�
	std::function<void()> _WaveEndEvent;
	// Ʈ���� �ߵ� �ݹ� �Լ� 
	std::function<void()> _Event{};
	// ���Ͱ� �� ���� ������ üũ�ϱ� ���� �迭 . 
	std::optional<std::vector<std::weak_ptr<class Monster>> > CheckIfTheDead{};
	std::weak_ptr<class BoxCollider> _Collider{};
};
#endif //