#ifndef __BLITZATTACK_H_
#define __BLITZATTACK_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include <optional>

class BlitzAttack : public ENGINE::GameObject
		
{
private:
	struct EffectDesc
	{
		float   SpawnTime = 0.0f;
		Vector3 LocationOffset{ 0.0f,0.0f,0.0f };
		bool bPlayed = true;
		GAMEOBJECTTAG _Tag{};
		std::weak_ptr<class GameObject> _Effect{};
	};
	std::array<EffectDesc, 5u> _PlayEffectDescs{};
private:
	explicit BlitzAttack() = default;
	virtual ~BlitzAttack() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static BlitzAttack* Create();
public:
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT    Update(const float _fDeltaTime) override;
	virtual UINT    LateUpdate(const float _fDeltaTime) override;
	virtual void    Editor()override;
	virtual void	OnEnable() override;
	virtual void    OnDisable() override;
public:
	void PlayStart(const Vector3& Location);
private:
	float T = 0.0f;
	void PlayEnd();
};
#endif //