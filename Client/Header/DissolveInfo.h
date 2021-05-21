#ifndef __DISSOLVEINFO_H_
#define __DISSOLVEINFO_H_
#include "GameObject.h"
#include "RenderInterface.h"

struct DissolveInfo 				
{
public:
	static inline const std::string ShaderStaticName = "gbuffer_ds_dissolve";
	static inline const std::string ShaderSkeletonName = "gbuffer_dsSK_dissolve";
public:
	void Initialize(const std::filesystem::path& MeshPath , const Vector3& _Color ,
					const Vector3 BurnColor = { 14.f / 255.f,1.f,0.f },
					const float BurnSize =0.25f);
	void DissolveStart(const bool bReverse=false,
					   const bool bParticle=true,
					   const float DissolveTime=1.f);
	void DissolveEnd();
	void DissolveVariableBind(ID3DXEffect*const Fx);
	bool DissolveUpdate(const float DeltaTime,const Matrix& RenderMatrix);
	void DissolveEditor();

	bool IsDissolve()const;
private:
	void DissolveParticle();
private:
	std::string DissolveParticleName{};
	uint32 DissolveParticleCount = 60u;
	std::shared_ptr<ENGINE::Texture>    _DissolveMap{};
	std::shared_ptr<ENGINE::Texture>    _BurnMap{};
	Matrix  RenderMatrix{ FMath::Identity() };
	float   DissolveParticleDelta = 0.06f;
	float   CurDissolveParticleDelta = 0.0f;
	Vector3 BurnColor{ 14.f / 255.f,1.f,0.f };
	float   SliceAmount = 0.0f;
	bool	bReverse  = false;
	bool	bParticle = true;
	float   BurnSize = 0.25f;
	float   EmissionAmount = 100.f;
	float   DissolveAcc = 1.f;
	bool    bDissolve = false;


	float EditDissolveTime = 1.f;
	bool bEditDissolveReverse = false;
	bool bEditDissolveParticle = true;
};
#endif //