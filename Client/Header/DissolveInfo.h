#ifndef __DISSOLVEINFO_H_
#define __DISSOLVEINFO_H_
#include "GameObject.h"
#include "RenderInterface.h"

struct DissolveInfo 				
{
public:
public:
	static inline const std::string ShaderStaticName = "gbuffer_ds_dissolve";
	static inline const std::string ShaderSkeletonName = "gbuffer_dsSK_dissolve";
public:
	void Initialize(const std::filesystem::path& MeshPath , const Vector3& _Color);
	void DissolveStart();
	void DissolveEnd();
	void DissolveVariableBind(ID3DXEffect*const Fx);
	void DissolveUpdate(const float DeltaTime,const Matrix& RenderMatrix);
	void DissolveEditor();
private:
	void DissolveParticle();
private:
	std::string DissolveParticleName{};
	uint32 DissolveParticleCount = 100u;
	std::shared_ptr<ENGINE::Texture>    _DissolveMap{};
	std::shared_ptr<ENGINE::Texture>    _BurnMap{};
	Matrix  RenderMatrix{ FMath::Identity() };
	float   DissolveParticleDelta = 0.03f;
	float   CurDissolveParticleDelta = 0.0f;
	Vector3 BurnColor{ 14.f / 255.f,1.f,0.f };
	float   SliceAmount = 0.0f;
	float   BurnSize = 0.25f;
	float   EmissionAmount = 100.f;
	float   DissolveAcc = 1.f;
	bool    bDissolve = false;
};
#endif //