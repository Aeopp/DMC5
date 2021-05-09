#ifndef __FIRECIRCLE_H_
#define __FIRECIRCLE_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include "Vertexs.h"
#include <optional>

class FireCircle : public ENGINE::GameObject,
			       public ENGINE::RenderInterface
{
private:
	std::shared_ptr<ENGINE::StaticMesh> Outer{};
	std::shared_ptr<ENGINE::StaticMesh> Inner{};
	std::shared_ptr<ENGINE::Texture>    SpriteMap{};
	std::shared_ptr<ENGINE::Texture>    TrailMap{};
	std::shared_ptr<ENGINE::Texture>    EmssiveMskMap{};
	std::shared_ptr<ENGINE::Texture>    NoiseMap{};
private:
	explicit FireCircle()  ;
	virtual ~FireCircle() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static FireCircle* Create();
public:
	virtual void    RenderReady()                          override;
public:
	void			RenderInit();
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT    Update(const float _fDeltaTime) override;
	virtual UINT    LateUpdate(const float _fDeltaTime) override;
	virtual void    Editor()override;
	virtual void	OnEnable() override;
	virtual void    OnDisable() override;
public:
	void PlayStart(
		const Vector3& Rotation = { 0.f,0.f,0.f },
		// 재생 시킬 위치 
		const Vector3& Location = { 0.f,0.f,0.f },
		// 재생 시킬 회전 
		const float CurRoll = 0.0f,
		// 회전 속도 . 
		const float RollRotateSpeed = FMath::PI * 4.f,
		const int32 StartSpriteRow = 3,
		const float PlayTime = 1.f,
		const int32 StartSpriteCol = 0,
		const Vector3& Scale = { 0.004f, 0.004f,0.004f });
	void PlayEnd();
public:
	void SpriteUpdate(const float DeltaTime);
	void RenderDebug(const DrawInfo& _Info);
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
private:
	float   SpriteProgressTime = 0.0f;

	bool    bOuterRender = false;

	float   EmissiveIntencity{ 1.f };
	float   ColorIntencity{ 1.f };
	float   OuterDistortionIntencity{ 1.f };
	float   DistortionIntencity{ 1.f };

	float   Radian = 0.0f;
	float   RadianSpeed = 1.f;

	Vector3 NoiseScale      { 1.f,2.f,3.f };
	Vector3 NoiseScrollSpeed{ 1.f,2.f,3.f };

	Vector2 NoiseDistortion0{ 0.3f,1.f };
	Vector2 NoiseDistortion1{ 0.5f,0.77f};
	Vector2 NoiseDistortion2{ 0.77f,0.5f};

	float RollRotationSpeed = FMath::PI;

	float SpriteCurUpdateTime{ 0.0f };
	float SpriteUpdateCycle{ SpriteCurUpdateTime };

	float ClipRange = 0.0f /255.f;

	float SpritePrevRowIdx{ 0.0f };
	float SpritePrevColIdx{ 0.0f };

	float SpriteRowIdx{ 0.0f };
	float SpriteColIdx{ 0.0f };
	float T{ 0.0f };

	float SpriteRow{ 8.f };
	float SpriteCol{ 8.f };

	float PlayTime = 1.f;
	float CurRoll{ 0.f };
	Vector3 _Rotation{ 0.f,0.f,0.f };

	float EditPlayStartPlayTime = 1.f;
	float EditStartRoll{ 0.0f };
	Vector3 EditPlayStartScale{ 0.004f,0.004f,0.004f };
	Vector3 EditPlayStartLocation{ 0.f,0.f,0.f };
	Vector3 EditPlayStartRotation {0.f,0.f,0.f};
	float EditPlayRollRotateSpeed{ 400.f };
	int32 EditSpriteCol{ 0 };
	int32 EditSpriteRow{0 };
};
#endif //

