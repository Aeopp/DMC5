#include "stdafx.h"
#include "..\Header\PreLoader.h"
#include "ParticleSystem.h"
#include "ParticleInstanceDesc.hpp"
#include "Subset.h"
#include "FadeOut.h"
#include "Judgement.h"

void PreLoader::PreLoadResources()
{
	JudgementDayParticlePoolLoad();
	JudgementReadyParticlePoolLoad();
	JudgementCircleGrowParticlePoolLoad();
	JudgementCutparticlePoolLoad();

	SandGlassEffectParticlePoolLoad();
	EnergismReadyParticlePoolLoad();
	KaboomParticlePoolLoad();
	NuClearParticlePoolLoad();
	ReverberationParticlePoolLoad();
	ArtemisMissileParticlePoolLoad();
	ArtemisRushParticlePoolLoad();

	IceAgeParticlePoolLoad();
	IceCbsMidParticlePoolLoad();
	FireParticlePoolLoad();
	
	ElectricParticlePoolLoad();

	ElectricEffectParticlePoolLoad(
		"..\\..\\Usable\\Electric\\Texture\\0001_alpg.tga",8u,8u,"ElectricEffectParticle0");
	ElectricEffectParticlePoolLoad(
		"..\\..\\Usable\\Electric\\Texture\\0002_alpg.tga", 4u, 1u, "ElectricEffectParticle1");
	ElectricEffectParticlePoolLoad(
		"..\\..\\Usable\\Electric\\Texture\\0003_alpg.tga", 8u, 4u, "ElectricEffectParticle2");
	ElectricEffectParticlePoolLoad(
		"..\\..\\Usable\\Electric\\Texture\\0009_alpg.tga", 4u, 4u, "ElectricEffectParticle3");
	ElectricEffectParticlePoolLoad(
		"..\\..\\Usable\\Electric\\Texture\\tex_03_electrical_00_0009_alpg.tga", 4u, 4u, "ElectricEffectParticle4");

	ThunderBoltParticlePoolLoad();
	ThunderBoltEndParticlePoolLoad();

	ThunderBoltSecondParticlePoolLoad();
	ThunderBoltSecondEndParticlePoolLoad();

	ElectricVoertexParticleLoad();

	ElectricBranchParticlePoolLoad();
	ElectricBranchEndParticlePoolLoad();

	AirHikeParticlePoolLoad();
	AirHikeParticleEndPoolLoad();

	SecretVisionDisappearParticlePoolLoad();
	DissolveNhDoorParticlePoolLoad();
	ChangeParticlePoolLoad();

	/*--- bLocalVertexLocationsStorage true인 애들 먼저 로드 --- */
	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = true;
	Resources::Load<ENGINE::StaticMesh>(
		L"..\\..\\Resource\\Mesh\\Static\\Primitive\\sphere00.fbx", _Info);
	Resources::Load<ENGINE::StaticMesh>(
		L"..\\..\\Resource\\Mesh\\Static\\Primitive\\pipe00.fbx", _Info);
	Resources::Load<ENGINE::StaticMesh>(
		L"..\\..\\Resource\\Mesh\\Static\\Primitive\\pipe01.fbx", _Info);
	Resources::Load<ENGINE::StaticMesh>(
		L"..\\..\\Resource\\Mesh\\Static\\Effect\\Stone\\mesh_capcom_debris_stone00_small.fbx", _Info);
	/*--------------------------------------------------------- */

	SVMCParticleEndPoolLoad();
	// 리소스 텍스쳐 이펙트 파이어 1 ~ 15;
	// "..\\..\\Resource\\Texture\\Effect\\Fire\\1.tga"

	for (uint32 i = 1; i <= 15; ++i)
	{
		FireSpritePoolLoad(i);
		FireVortexSpritePoolLoad(i);
	}
};

void PreLoader::IceCbsMidParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Usable\\Ice\\mesh_03_debris_ice00_01.fbx", _Info);

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "IceParticle";
	// 공유 정보 바인드 
	_PushParticle.SharedResourceBind = [](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		if (auto Subset = TargetParticle._Mesh->GetSubset(0).lock();
			Subset)
		{
			Subset->BindProperty(TextureType::DIFFUSE, 0, "AlbmMap", Fx);
			Subset->BindProperty(TextureType::NORMALS, 0, "NrmrMap", Fx);
		}
	};

	_PushParticle.InstanceBind = [](const std::any& _InstanceVariable, ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Ice&>(_InstanceVariable);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 300;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated("IceCbsMid",
			std::move(_PushParticle), PoolSize,false);

	for (auto& _ParticleInstance : *ParticlePool)
	{
		Vector2 Range{ -1.f,1.f };

		const Vector3 TargetLocation = Vector3{ 0.f,0.f,0.f };

		static constexpr float StartVelocityScale = 0.075f;
		static constexpr float SecondVelocityScale = 0.115f;
		static constexpr float ThirdVelocityScale = 0.15f;

		ParticleInstance::Ice _IceValue{};
		_IceValue.ColorIntencity = FMath::Random(0.01f, 0.04f);
		const float LifeTime = FMath::Random(0.20f, 1.f);
		const Vector3 ParticleScale =
			FMath::Random(Vector3{ 0.05f ,0.05f ,0.05f }, 
				Vector3{ 0.067,0.067,0.067f }) * GScale;


		const Vector3 Cp0 = TargetLocation + FMath::Random(Vector3{-1.f,-1.f,-1.f} *StartVelocityScale,
														   Vector3{1.f,1.f,1.f}    *StartVelocityScale);

		const Vector3 Cp1 = Cp0 + FMath::Random(Vector3{ -1.f,-1.f,-1.f } *SecondVelocityScale,
												Vector3{ 1.f,1.f,1.f }    *SecondVelocityScale);

		const Vector3 End = Cp1 + FMath::Random(Vector3{ -1.f,-1.f,-1.f } *ThirdVelocityScale,
												Vector3{ 1.f,1.f,1.f }    *ThirdVelocityScale);

		const Vector3 StartRot = FMath::RandomEuler(1.f);
		const Vector3 EulerVelocity = FMath::RandomEuler(StartVelocityScale);

		const Vector3 RotCp0 = StartRot + EulerVelocity;
		const Vector3 RotCp1 = RotCp0 + EulerVelocity * SecondVelocityScale;
		const Vector3 EndRot = RotCp1 + EulerVelocity * ThirdVelocityScale;

		_ParticleInstance.PreSetup({ TargetLocation,Cp0,Cp1,End },
			{ StartRot,RotCp0,RotCp1,EndRot },
			ParticleScale , 
			LifeTime, 0.0f, _IceValue, std::nullopt);
	}
}

void PreLoader::CbsMidParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>("..\\..\\Usable\\Smoke\\11.tga");

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "ElectricParticle";
	// 공유 정보 바인드
	_PushParticle.SharedResourceBind = [_Tex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		Fx->SetTexture("MskMap", _Tex->GetTexture());
	};

	_PushParticle.InstanceBind = [](const std::any& _InstanceVariable, ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Electric&>(_InstanceVariable);
		Fx->SetFloatArray("ElectricColor", _Value.Color, 3u);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 1500ul;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated("CbsMid",
			std::move(_PushParticle), PoolSize, true);

	{
		for (auto& _ParticleInstance : *ParticlePool)
		{
			
			const Vector3 StartLocation = FMath::RandomVector(10.f);

			const Vector3 Dir = FMath::Normalize(StartLocation);
			Vector3 Cp0{};
			Vector3 Cp1{};
			Vector3 End{};

			const uint32 TargetIdx = (FMath::Random(0u, 3u) % 3u);

			if (TargetIdx == 0)
			{
				Cp0 = StartLocation + FMath::RandomVector(FMath::Random(300.f, 500.f));
				Cp1 = Cp0 + FMath::RandomVector(FMath::Random(300.f, 500.f));
				End = Cp1 + FMath::RandomVector(FMath::Random(300.f, 500.f));
			}
			else if (TargetIdx == 1)
			{
				Cp0 = StartLocation + FMath::RandomVector(FMath::Random(11.f, 22.f));
				Cp1 = Cp0 + FMath::RandomVector(FMath::Random(11.f, 22.f));
				End = Cp1 + FMath::RandomVector(FMath::Random(11.f, 22.f));
			}
			else
			{
				Cp0 = StartLocation + Dir * FMath::Random(1333.f, 2333.f);
				Cp1 = Cp0 + Dir * FMath::Random(1333.f, 2333.f);
				End = Cp1 + Dir * FMath::Random(1333.f, 2333.f);
			}

			const Vector3 StartRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp0 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp1 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 EndRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };

			const float RScale = FMath::Random(0.0022f, 0.006f) * GScale;

			ParticleInstance::Electric _ElectricValue{};

			_ElectricValue.ColorIntencity = FMath::Random(0.44f, 1.f);
			_ElectricValue.Color = FMath::Lerp(Vector3{ 120.f / 255.f,50.f / 255.f, 201.f / 255.f }, Vector3{ 1.f,1.f,1.f }, FMath::Random(0.f, 1.f));
			const float LifeTime = FMath::Random(0.1f, 3.f);

			_ParticleInstance.PreSetup({ StartLocation ,Cp0,Cp1,End },
				{ StartRot,RotCp0,RotCp1,EndRot },
				{ RScale,RScale,RScale }, LifeTime, 0.0f, _ElectricValue, std::nullopt);
		}
	}
}

void PreLoader::IceAgeParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Usable\\Ice\\mesh_03_debris_ice00_01.fbx", _Info);

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "IceParticle";
	// 공유 정보 바인드 
	_PushParticle.SharedResourceBind = [](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		if (auto Subset = TargetParticle._Mesh->GetSubset(0).lock();
			Subset)
		{
			Subset->BindProperty(TextureType::DIFFUSE, 0, "AlbmMap", Fx);
			Subset->BindProperty(TextureType::NORMALS, 0, "NrmrMap", Fx);
		}
	};

	_PushParticle.InstanceBind = [](const std::any& _InstanceVariable, ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Ice&>(_InstanceVariable);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 10000u;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated("Ice", std::move(_PushParticle), PoolSize,false);

	Mesh::InitializeInfo _TargetInfo{};
	_TargetInfo.bLocalVertexLocationsStorage = true;
	// 메시
	auto Inner = Resources::Load<StaticMesh>("..\\..\\Resource\\Mesh\\Static\\Primitive\\nsg.fbx", _TargetInfo);

	const uint32 RangeEnd = Inner->m_spVertexLocations->size() - 1u;

	for (auto& _ParticleInstance : *ParticlePool)
	{
		Vector2 Range{ -1.f,1.f };

		const Vector3 TargetLocation = (*Inner->m_spVertexLocations)[FMath::Random(0u, RangeEnd)];

		Vector3 LocalVtxDir = FMath::Normalize(TargetLocation);
		const Vector3 Right = FMath::Normalize(FMath::Cross(Vector3{ 0.f,1.f,0.f }, LocalVtxDir));
		const Vector3 Up = FMath::Normalize(FMath::Cross(Up, LocalVtxDir));

		static constexpr std::pair<float, float> StartScaleRange = { 100.f,200.f };

		static constexpr std::pair<float, float> SecondScaleRange = { StartScaleRange.second * 0.9f * 0.5f,
																   StartScaleRange.second * 0.9f };

		static constexpr std::pair<float, float> ThirdScaleRange = { SecondScaleRange.second * 0.9f * 0.5f,
																   SecondScaleRange.second * 0.9f };

		const std::pair<float, float> UpRotation = (FMath::Random(0, 1) == 0) ?
			std::pair<float, float>{-FMath::HalfPI, 0.f} : std::pair<float, float>{ 0.f, FMath::HalfPI };

		const float Cp0Pitch = FMath::Random(0.f, FMath::HalfPI);
		const float Cp0Yaw = FMath::Random(UpRotation.first, UpRotation.second);
		const float Cp0Scale = FMath::Random(StartScaleRange.first, StartScaleRange.second);

		LocalVtxDir = FMath::RotationVecNormal(LocalVtxDir, Right, Cp0Pitch);
		LocalVtxDir = FMath::RotationVecNormal(LocalVtxDir, Up, Cp0Yaw);
		const Vector3 Cp0 = TargetLocation + (LocalVtxDir * Cp0Scale);

		const float Cp1Pitch = FMath::Random(0.f, FMath::HalfPI);
		const float Cp1Yaw = FMath::Random(UpRotation.first, UpRotation.second);
		const float Cp1Scale = FMath::Random(SecondScaleRange.first, SecondScaleRange.second);

		LocalVtxDir = FMath::RotationVecNormal(LocalVtxDir, Right, Cp1Pitch);
		LocalVtxDir = FMath::RotationVecNormal(LocalVtxDir, Up, Cp1Yaw);
		const Vector3 Cp1 = Cp0 + (LocalVtxDir * Cp1Scale);

		const float Cp2Pitch = FMath::Random(0.f, FMath::HalfPI);
		const float Cp2Yaw = FMath::Random(UpRotation.first, UpRotation.second);
		const float Cp2Scale = FMath::Random(ThirdScaleRange.first, ThirdScaleRange.second);

		LocalVtxDir = FMath::RotationVecNormal(LocalVtxDir, Right, Cp2Pitch);
		LocalVtxDir = FMath::RotationVecNormal(LocalVtxDir, Up, Cp2Yaw);
		const Vector3 End = Cp1 + (LocalVtxDir * Cp2Scale);

		Vector2 ScaleRange{ 0.05f,0.09f };
		const Vector3 PScale = FMath::Random(
			Vector3(0.05f, 0.05f, 0.05f),
			Vector3(0.09f, 0.09f, 0.09f)) * GScale;

		const Vector3 StartRot = {
			FMath::Random(-360.f,360.f),FMath::Random(-360.f,360.f),FMath::Random(-360.f,360.f) };
		const Vector3 RotCp0 = StartRot + Vector3{ Cp0Pitch , Cp0Yaw,Cp0Scale };
		const Vector3 RotCp1 = RotCp0 + Vector3{ Cp1Pitch , Cp1Yaw,Cp1Scale };
		const Vector3 EndRot = RotCp1 + Vector3{ Cp2Pitch,Cp2Yaw,Cp2Scale };

		ParticleInstance::Ice _IceValue{};

		_IceValue.ColorIntencity = FMath::Random(0.02f, 0.08f);

		const float LifeTime = FMath::Random(0.8f, 1.3f);

		_ParticleInstance.PreSetup({ TargetLocation,Cp0,Cp1,End },
			{ StartRot,RotCp0,RotCp1,EndRot },
			PScale, LifeTime, 0.0f, _IceValue, std::nullopt);
	}
}

void PreLoader::ChangeParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Usable\\Ice\\mesh_03_debris_ice00_01.fbx", _Info);

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "ChangeParticle";
	// 공유 정보 바인드 
	_PushParticle.SharedResourceBind = [](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		if (auto Subset = TargetParticle._Mesh->GetSubset(0).lock();
			Subset)
		{
			Subset->BindProperty(TextureType::DIFFUSE, 0, "AlbmMap", Fx);
			Subset->BindProperty(TextureType::NORMALS, 0, "NrmrMap", Fx);
		}
	};

	_PushParticle.InstanceBind = [](const std::any& _InstanceVariable, ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Ice&>(_InstanceVariable);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 10000u;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated("ChangeParticle", std::move(_PushParticle), PoolSize, false);

	Mesh::InitializeInfo _TargetInfo{};
	_TargetInfo.bLocalVertexLocationsStorage = true;
	// 메시
	auto Inner = Resources::Load<StaticMesh>("..\\..\\Resource\\Mesh\\Static\\Primitive\\nsg.fbx", _TargetInfo);

	const uint32 RangeEnd = Inner->m_spVertexLocations->size() - 1u;

	for (auto& _ParticleInstance : *ParticlePool)
	{
		Vector2 Range{ -1.f,1.f };

		const Vector3 TargetLocation = (*Inner->m_spVertexLocations)[FMath::Random(0u, RangeEnd)];

		Vector3 LocalVtxDir = FMath::Normalize(TargetLocation);
		const Vector3 Right = FMath::Normalize(FMath::Cross(Vector3{ 0.f,1.f,0.f }, LocalVtxDir));
		const Vector3 Up = FMath::Normalize(FMath::Cross(Up, LocalVtxDir));

		static constexpr std::pair<float, float> StartScaleRange = { 100.f,200.f };

		static constexpr std::pair<float, float> SecondScaleRange = { StartScaleRange.second * 1.5f * 0.5f,
																      StartScaleRange.second * 1.5f };

		static constexpr std::pair<float, float> ThirdScaleRange = { SecondScaleRange.second * 2.f * 0.5f,
																   SecondScaleRange.second * 2.f };

		const Vector3 Cp0=TargetLocation + FMath::RandomVector(FMath::Random(StartScaleRange));
		const Vector3 Cp1= Cp0+FMath::RandomVector(FMath::Random(SecondScaleRange));
		const Vector3 End = Cp1 + FMath::RandomVector(FMath::Random(ThirdScaleRange));
		
		const Vector3 PScale = FMath::Random(
			Vector3(0.025f, 0.025f, 0.025f),
			Vector3(0.05f, 0.05f, 0.05f)) * GScale;

		const Vector3 StartRot = {
			FMath::Random(-360.f,360.f),FMath::Random(-360.f,360.f),FMath::Random(-360.f,360.f) };
		const Vector3 RotCp0 = StartRot + FMath::RandomEuler(1.f);
		const Vector3 RotCp1 = RotCp0 + FMath::RandomEuler(1.f);
		const Vector3 EndRot = RotCp1 + FMath::RandomEuler(1.f);

		ParticleInstance::Ice _IceValue{};

		_IceValue.ColorIntencity = FMath::Random(1.f, 2.f);

		const float LifeTime = FMath::Random(0.8f, 1.3f);

		_ParticleInstance.PreSetup({ TargetLocation,Cp0,Cp1,End },
			{ StartRot,RotCp0,RotCp1,EndRot },
			PScale, LifeTime, 0.0f, _IceValue, std::nullopt);
	}
}

void PreLoader::ArtemisMissileParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>("..\\..\\Usable\\Smoke\\11.tga");

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "ArtemisMissileParticle";
	// 공유 정보 바인드
	_PushParticle.SharedResourceBind = [_Tex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		Fx->SetTexture("MskMap", _Tex->GetTexture());
	};

	_PushParticle.InstanceBind = [](const std::any& _InstanceVariable,
									ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Artemis&>(_InstanceVariable);
		Fx->SetFloatArray("_Color", _Value.Color, 3u);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 33333u;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated("ArtemisMissileParticle",
			std::move(_PushParticle), PoolSize, true);

	_Info.bLocalVertexLocationsStorage = true;
	auto _TargetMesh = Resources::Load<StaticMesh>(
		"..\\..\\Usable\\LocationMesh\\sphere00.fbx", _Info);

	if (_TargetMesh->m_spVertexLocations)
	{
		for (auto& _ParticleInstance : *ParticlePool)
		{
			const uint32 Idx =
				FMath::Random(0u, uint32(_TargetMesh->m_spVertexLocations->size() - 1));

			Vector3 StartLocation =
				(*_TargetMesh->m_spVertexLocations)[Idx] + FMath::RandomVector(33.f);

			const Vector3 Dir = FMath::Normalize(StartLocation);

			const uint32 IdxRemain    = Idx % 4;

			Vector3 Cp0 = StartLocation;
			Vector3 Cp1 = Cp0;
			Vector3 End = Cp1;

			if (IdxRemain == 0)
			{
				StartLocation +=		FMath::RandomVector(200.f);
				Cp0 = StartLocation + FMath::RandomVector(300.f);
				Cp1 = Cp0 +			 FMath::RandomVector(300.f);
				End = Cp1 +			FMath::RandomVector(300.f);
			}
			else
			{
				// 지나간 자리에 남으며 사라지기 직전부터 운동시작. 
				Cp0 = StartLocation;
				Cp1 = Cp0;
				End = Cp1;
			}

			const Vector3 StartRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp0 = StartRot+Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp1 = RotCp0+Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 EndRot = RotCp1 +Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };

			constexpr float ScaleFactor = 3.3f;
			const float RScale = FMath::Random(
				0.0055f * ScaleFactor,
				0.0075f * ScaleFactor) 
				* GScale;

			ParticleInstance::Artemis _Value{};

			_Value.ColorIntencity = FMath::Random(0.7f, 1.4f);

			_Value.Color = FMath::Lerp(
				Vector3{ 241.f / 255.f,  190.f / 255.f,  190.f / 255.f }, 
				Vector3{ 1.f,1.f,1.f }, 
				FMath::Random(0.f, 1.f));

			const float LifeTime = FMath::Random(2.f, 2.1f);

			_ParticleInstance.PreSetup(
				{ StartLocation ,Cp0,Cp1,End },
				{ StartRot,RotCp0,RotCp1,EndRot },
				{ RScale,RScale,RScale }, 
				LifeTime, 
				0.0f, 
				_Value,
				std::nullopt);
		}
	}
}

void PreLoader::ReverberationParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>("..\\..\\Usable\\Smoke\\11.tga");

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "ArtemisMissileParticle";
	// 공유 정보 바인드
	_PushParticle.SharedResourceBind = [_Tex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		Fx->SetTexture("MskMap", _Tex->GetTexture());
	};

	_PushParticle.InstanceBind = [](const std::any& _InstanceVariable,
		ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Artemis&>(_InstanceVariable);
		Fx->SetFloatArray("_Color", _Value.Color, 3u);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 20000u;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated("ReverberationParticle",
			std::move(_PushParticle), PoolSize, true);

	_Info.bLocalVertexLocationsStorage = true;
	auto _TargetMesh = Resources::Load<StaticMesh>(
		"..\\..\\Usable\\LocationMesh\\pipe01.fbx", _Info);

	if (_TargetMesh->m_spVertexLocations)
	{
		for (auto& _ParticleInstance : *ParticlePool)
		{
			const uint32 MaxIdx = (_TargetMesh->m_spVertexLocations->size() - 1);
			const uint32 Idx =
				FMath::Random(0u, MaxIdx);

			const uint32 PrevIdx = FMath::Clamp(Idx - 1, 0u, MaxIdx);
			const uint32 NextIdx = FMath::Clamp(Idx + 1, 0u, MaxIdx);

			Vector3 StartLocation =
				FMath::BezierCurve((*_TargetMesh->m_spVertexLocations)[PrevIdx],
					(*_TargetMesh->m_spVertexLocations)[Idx],
					(*_TargetMesh->m_spVertexLocations)[NextIdx], FMath::Random(0.f, 1.f));

			const Vector3 Dir = FMath::Normalize(StartLocation);

			const uint32 IdxRemain = Idx % 4;

			Vector3 Cp0 = StartLocation;
			Vector3 Cp1 = Cp0;
			Vector3 End = Cp1;

			if (IdxRemain == 0)
			{
				StartLocation += FMath::RandomVector(5.f);
				Cp0 = StartLocation + FMath::RandomVector(5.f);
				Cp1 = Cp0 + FMath::RandomVector(5.f);
				End = Cp1 + FMath::RandomVector(5.f);
			}
			else
			{
				// 지나간 자리에 남으며 사라지기 직전부터 운동시작. 
				Cp0 = StartLocation;
				Cp1 = Cp0;
				End = Cp1;
			}

			const Vector3 StartRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp0 = StartRot + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp1 = RotCp0 + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 EndRot = RotCp1 + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };

			constexpr float ScaleFactor = 3.3f;
			const float RScale = FMath::Random(
				0.0055f * ScaleFactor,
				0.0075f * ScaleFactor)
				* GScale;

			ParticleInstance::Artemis _Value{};

			_Value.ColorIntencity = FMath::Random(0.7f, 1.4f);

			_Value.Color = FMath::Lerp(
				Vector3{ 241.f / 255.f,  190.f / 255.f,  190.f / 255.f },
				Vector3{ 1.f,1.f,1.f },
				FMath::Random(0.f, 1.f));

			const float LifeTime = FMath::Random(0.4f, 0.6f);

			_ParticleInstance.PreSetup(
				{ StartLocation ,Cp0,Cp1,End },
				{ StartRot,RotCp0,RotCp1,EndRot },
				{ RScale,RScale,RScale },
				LifeTime,
				0.0f,
				_Value,
				std::nullopt);
		}
	}
}

void PreLoader::ArtemisRushParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>("..\\..\\Usable\\Smoke\\11.tga");

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "ArtemisMissileParticle";
	// 공유 정보 바인드
	_PushParticle.SharedResourceBind = [_Tex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		Fx->SetTexture("MskMap", _Tex->GetTexture());
	};

	_PushParticle.InstanceBind = [](const std::any& _InstanceVariable,
		ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Artemis&>(_InstanceVariable);
		Fx->SetFloatArray("_Color", _Value.Color, 3u);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 1000u;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated("ArtemisRushParticle",
			std::move(_PushParticle), PoolSize, true);

	/*_Info.bLocalVertexLocationsStorage = true;
	auto _TargetMesh = Resources::Load<StaticMesh>(
		"..\\..\\Usable\\LocationMesh\\sphere00.fbx", _Info);*/

		// if (_TargetMesh->m_spVertexLocations)
	{
		for (auto& _ParticleInstance : *ParticlePool)
		{

			Vector3 StartLocation = FMath::RandomVector(200.f);
			Vector3 Cp0 = StartLocation + FMath::RandomVector(200.f);
			Vector3 Cp1 = Cp0 + FMath::RandomVector(200.f);
			Vector3 End = Cp1 + FMath::RandomVector(200.f);

			const Vector3 StartRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp0 = StartRot + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp1 = RotCp0 + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 EndRot = RotCp1 + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };

			constexpr float ScaleFactor = 3.3f;
			const float RScale = FMath::Random(
				0.0055f * ScaleFactor,
				0.0075f * ScaleFactor)
				* GScale;

			ParticleInstance::Artemis _Value{};

			_Value.ColorIntencity = FMath::Random(0.7f, 1.4f);

			_Value.Color = FMath::Lerp(
				Vector3{ 241.f / 255.f,  190.f / 255.f,  190.f / 255.f },
				Vector3{ 1.f,1.f,1.f },
				FMath::Random(0.f, 1.f));

			const float LifeTime = FMath::Random(2.f, 2.1f);

			_ParticleInstance.PreSetup(
				{ StartLocation ,Cp0,Cp1,End },
				{ StartRot,RotCp0,RotCp1,EndRot },
				{ RScale,RScale,RScale },
				LifeTime,
				0.0f,
				_Value,
				std::nullopt);
		}
	}
};

void PreLoader::ArtemisCylinderParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>("..\\..\\Usable\\Smoke\\11.tga");

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "ArtemisMissileParticle";
	// 공유 정보 바인드
	_PushParticle.SharedResourceBind = [_Tex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		Fx->SetTexture("MskMap", _Tex->GetTexture());
	};

	_PushParticle.InstanceBind = [](
		const std::any& _InstanceVariable,
		ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Artemis&>(_InstanceVariable);
		Fx->SetFloatArray("_Color", _Value.Color, 3u);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 5555u;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated("ArtemisCylinderParticle",
			std::move(_PushParticle), PoolSize, true);

	_Info.bLocalVertexLocationsStorage = true;
	auto _TargetMesh = Resources::Load<StaticMesh>(
		"..\\..\\Usable\\LocationMesh\\Cylinder.fbx", _Info);

	if (_TargetMesh->m_spVertexLocations)
	{
		for (auto& _ParticleInstance : *ParticlePool)
		{
			const uint32 Idx =
				FMath::Random(0u, uint32(_TargetMesh->m_spVertexLocations->size() - 1));

			Vector3 StartLocation =
				(*_TargetMesh->m_spVertexLocations)[Idx] + FMath::RandomVector(22.f);

			static const Vector3 Up{ 0.f,1.f,0.f };

			Vector3 Dir =
				FMath::RotationVecNormal(FMath::Normalize(StartLocation), Up,
					-FMath::HalfPI);

			Vector3 Cp0 = StartLocation + Dir * 200.f;
			Dir = FMath::RotationVecNormal(FMath::Normalize(Dir), Up, -FMath::HalfPI);
			Vector3 Cp1 = Cp0 + Dir * 200.f;
			// Dir = FMath::RotationVecNormal(FMath::Normalize(Dir), Up, -FMath::HalfPI);
			Vector3 End = { 0.f,0.f,0.f };


			const Vector3 StartRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp0 = StartRot + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp1 = RotCp0 + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 EndRot = RotCp1 + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };

			constexpr float ScaleFactor = 3.3f;
			const float RScale = FMath::Random(
				0.0055f * ScaleFactor,
				0.0075f * ScaleFactor)
				* GScale;

			ParticleInstance::Artemis _Value{};

			_Value.ColorIntencity = FMath::Random(0.7f, 1.4f);

			_Value.Color = FMath::Lerp(
				Vector3{ 241.f / 255.f,  190.f / 255.f,  190.f / 255.f },
				Vector3{ 1.f,1.f,1.f },
				FMath::Random(0.f, 1.f));

			const float LifeTime = FMath::Random(2.f, 2.1f);

			_ParticleInstance.PreSetup(
				{ StartLocation ,Cp0,Cp1,End },
				{ StartRot,RotCp0,RotCp1,EndRot },
				{ RScale,RScale,RScale },
				LifeTime,
				0.0f,
				_Value,
				std::nullopt);
		}
	}
};

void PreLoader::EnergismReadyParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>(
		"..\\..\\Usable\\Smoke\\11.tga");

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "ArtemisMissileParticle";
	// 공유 정보 바인드
	_PushParticle.SharedResourceBind = [_Tex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		Fx->SetTexture("MskMap", _Tex->GetTexture());
	};

	_PushParticle.InstanceBind = [](
		const std::any& _InstanceVariable,
		ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Artemis&>(_InstanceVariable);
		Fx->SetFloatArray("_Color", _Value.Color, 3u);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 1111u;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated("EnergismReadyParticle",
			std::move(_PushParticle), PoolSize, true);

	{
		for (auto& _ParticleInstance : *ParticlePool)
		{
			const Vector3 StartLocation =
				FMath::RandomVector(1677.f);
			const float LerpScale = FMath::Random(0.0f, 1.f);
			const Vector3 End = FMath::Lerp(StartLocation, Vector3{ 0.f,0.f,0.f },
				LerpScale);

			const Vector3 Cp0 = FMath::Lerp(StartLocation, End, 0.33f) + FMath::RandomVector(555.f * LerpScale);
			const Vector3 Cp1 = FMath::Lerp(StartLocation, End, 0.66f) + FMath::RandomVector(555.f * LerpScale);

			const Vector3 StartRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp0 = StartRot + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp1 = RotCp0 + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 EndRot = RotCp1 + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };

			constexpr float ScaleFactor = 17.f;
			const float RScale = FMath::Random(
				0.0055f * ScaleFactor,
				0.0075f * ScaleFactor)
				* GScale;

			ParticleInstance::Artemis _Value{};

			_Value.ColorIntencity = FMath::Random(0.9f, 1.2f);

			_Value.Color = FMath::Lerp(
				Vector3{ 148.f / 255.f,  148.f / 255.f,  231.f / 255.f },
				Vector3{ 1.f,1.f,1.f },
				FMath::Random(0.f, 1.f));

			const float LifeTime = FMath::Random(0.1f, 2.f);

			_ParticleInstance.PreSetup(
				{ StartLocation ,Cp0,Cp1,End },
				{ StartRot,RotCp0,RotCp1,EndRot },
				{ RScale,RScale,RScale },
				LifeTime,
				0.0f,
				_Value,
				std::nullopt);
		}
	}
}
void PreLoader::SandGlassEffectParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>(
		"..\\..\\Usable\\Smoke\\11.tga");

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "SandGlassEffectParticle";
	// 공유 정보 바인드
	_PushParticle.SharedResourceBind = [_Tex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		Fx->SetTexture("MskMap", _Tex->GetTexture());
	};

	_PushParticle.InstanceBind = [](
		const std::any& _InstanceVariable,
		ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Artemis&>(_InstanceVariable);
		Fx->SetFloatArray("_Color", _Value.Color, 3u);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 888u;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated("SandGlassEffectParticle",
			std::move(_PushParticle), PoolSize, true);

	{
		for (auto& _ParticleInstance : *ParticlePool)
		{
			const Vector3 StartLocation = FMath::RandomVector(333.f);
			const Vector3 Cp0 = StartLocation + FMath::RandomVector(333.f);
			const Vector3 Cp1 = StartLocation + FMath::RandomVector(333.f);
			const Vector3 End = StartLocation + FMath::RandomVector(333.f);

			const Vector3 StartRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp0 = StartRot + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp1 = RotCp0 + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 EndRot = RotCp1 + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };

			constexpr float ScaleFactor = 3.f;
			const float RScale = FMath::Random(
				0.0055f * ScaleFactor,
				0.0075f * ScaleFactor)
				* GScale;

			ParticleInstance::Artemis _Value{};

			_Value.ColorIntencity = FMath::Random(0.2f, 0.2f);

			_Value.Color = FMath::Lerp(
				Vector3{ 182.f / 255.f,  206.f / 255.f,  61.f / 255.f },
				Vector3{ 206.f,118.f,74.f },
				FMath::Random(0.f, 1.f));

			const float LifeTime = FMath::Random(0.5f, 1.f);

			_ParticleInstance.PreSetup(
				{ StartLocation ,Cp0,Cp1,End },
				{ StartRot,RotCp0,RotCp1,EndRot },
				{ RScale,RScale,RScale },
				LifeTime,
				0.0f,
				_Value,
				std::nullopt);
		}
	}
};

void PreLoader::JudgementReadyParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>(
		"..\\..\\Usable\\Smoke\\11.tga");

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "JudgementParticle";
	// 공유 정보 바인드
	_PushParticle.SharedResourceBind = [_Tex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		Fx->SetTexture("MskMap", _Tex->GetTexture());
	};

	_PushParticle.InstanceBind = [](
		const std::any& _InstanceVariable,
		ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Judgement&>(_InstanceVariable);
		Fx->SetFloatArray("_Color", _Value.Color, 3u);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 2500ul;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated(
			"JudgementReady",
			std::move(_PushParticle), PoolSize, true);

	_Info.bLocalVertexLocationsStorage = true;
	auto _TargetMesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Effect\\AirHike\\AirHike.fbx", _Info);

	{
		for (auto& _ParticleInstance : *ParticlePool)
		{
			auto SpVtxLocations = _TargetMesh->m_spVertexLocations;
			const uint32 VtxCnt = SpVtxLocations->size();

			Vector3 StartLocation = (*SpVtxLocations)[FMath::Random(0u, VtxCnt - 1u)];
			Vector3 Cp0 = StartLocation + FMath::RandomVector(0.f);
			Cp0.y += FMath::Random(0.f, 50.f);
			Vector3 Cp1 = Cp0 + FMath::RandomVector(0.f);
			Cp1.y += FMath::Random(0.f, 50.f);
			Vector3 End = Cp1 + FMath::RandomVector(0.f);
			End.y += FMath::Random(0.f, 50.f);

			const Vector3 StartRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp0 = StartRot + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp1 = RotCp0 + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 EndRot = RotCp1 + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };

			constexpr float ScaleFactor = 10.f;
			const float RScale = FMath::Random(
				0.0055f * ScaleFactor,
				0.0075f * ScaleFactor)
				* GScale;

			ParticleInstance::Judgement _Value{};

			_Value.ColorIntencity = FMath::Random(0.025f, 0.05f);

			_Value.Color = FMath::Random(Vector3{ 255.0f,35.f,72.f }, Vector3{ 255.f,46.f,28.f });

			const float LifeTime = FMath::Random(0.1f, 2.f);

			_ParticleInstance.PreSetup(
				{ StartLocation ,Cp0,Cp1,End },
				{ StartRot,RotCp0,RotCp1,EndRot },
				{ RScale,RScale,RScale },
				LifeTime,
				0.0f,
				_Value,
				std::nullopt);
		}
	}
};

void PreLoader::JudgementCircleGrowParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>(
		"..\\..\\Usable\\Smoke\\11.tga");

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "JudgementParticle";
	// 공유 정보 바인드
	_PushParticle.SharedResourceBind = [_Tex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		Fx->SetTexture("MskMap", _Tex->GetTexture());
	};

	_PushParticle.InstanceBind = [](
		const std::any& _InstanceVariable,
		ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Judgement&>(_InstanceVariable);
		Fx->SetFloatArray("_Color", _Value.Color, 3u);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 9999ul;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated(
			"CircleGrowParticle",
			std::move(_PushParticle), PoolSize, true);

	_Info.bLocalVertexLocationsStorage = true;
	auto _TargetMesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Effect\\AirHike\\AirHike.fbx", _Info);

	{
		for (auto& _ParticleInstance : *ParticlePool)
		{
			auto SpVtxLocations = _TargetMesh->m_spVertexLocations;
			const uint32 VtxCnt = SpVtxLocations->size();

			Vector3 StartLocation = (*SpVtxLocations)[FMath::Random(0u, VtxCnt - 1u)];
			Vector3 Cp0 = StartLocation + FMath::RandomVector(0.f);
			Vector3 Cp1 = StartLocation + FMath::RandomVector(3.f);
			Vector3 End = StartLocation + FMath::RandomVector(6.f);
			// End.y += FMath::Random(0.f,111.f);

			const Vector3 StartRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp0 = StartRot + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp1 = RotCp0 + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 EndRot = RotCp1 + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };

			constexpr float ScaleFactor = 10.f;
			const float RScale = FMath::Random(
				0.0055f * ScaleFactor,
				0.0075f * ScaleFactor)
				* GScale;

			ParticleInstance::Judgement _Value{};

			_Value.ColorIntencity = FMath::Random(0.025f, 0.05f);

			_Value.Color = FMath::Random(Vector3{ 255.0f,35.f,72.f }, Vector3{ 255.f,46.f,28.f });

			const float LifeTime = FMath::Random(0.1f, 2.f);

			_ParticleInstance.PreSetup(
				{ StartLocation ,Cp0,Cp1,End },
				{ StartRot,RotCp0,RotCp1,EndRot },
				{ RScale,RScale,RScale },
				LifeTime,
				0.0f,
				_Value,
				std::nullopt);
		}
	}
};

void PreLoader::JudgementCutparticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Usable\\Ice\\mesh_03_debris_ice00_01.fbx", _Info);

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "JudgementCutParticle";
	// 공유 정보 바인드 
	_PushParticle.SharedResourceBind = [](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		if (auto Subset = TargetParticle._Mesh->GetSubset(0).lock();
			Subset)
		{
			Subset->BindProperty(TextureType::DIFFUSE, 0, "AlbmMap", Fx);
			Subset->BindProperty(TextureType::NORMALS, 0, "NrmrMap", Fx);
		}
	};

	_PushParticle.InstanceBind = [](const std::any& _InstanceVariable, ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Ice&>(_InstanceVariable);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 15000u;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated("JudgementCut", std::move(_PushParticle), PoolSize, false);

	Mesh::InitializeInfo _TargetInfo{};
	_TargetInfo.bLocalVertexLocationsStorage = true;
	// 메시
	auto Inner = Resources::Load<StaticMesh>("..\\..\\Resource\\Mesh\\Static\\Primitive\\nsg.fbx", _TargetInfo);

	const uint32 RangeEnd = Inner->m_spVertexLocations->size() - 1u;

	for (auto& _ParticleInstance : *ParticlePool)
	{
		Vector2 Range{ -1.f,1.f };

		const Vector3 TargetLocation = (*Inner->m_spVertexLocations)[FMath::Random(0u, RangeEnd)];

		Vector3 LocalVtxDir = FMath::Normalize(TargetLocation);
		const Vector3 Right = FMath::Normalize(FMath::Cross(Vector3{ 0.f,1.f,0.f }, LocalVtxDir));
		const Vector3 Up = FMath::Normalize(FMath::Cross(Up, LocalVtxDir));

		static constexpr std::pair<float, float> StartScaleRange = { 100.f,200.f };

		static constexpr std::pair<float, float> SecondScaleRange = { StartScaleRange.second * 1.5f * 0.5f,
																	  StartScaleRange.second * 1.5f };

		static constexpr std::pair<float, float> ThirdScaleRange = { SecondScaleRange.second * 2.f * 0.5f,
																   SecondScaleRange.second * 2.f };

		const Vector3 Cp0 = TargetLocation + FMath::RandomVector(FMath::Random(StartScaleRange));
		const Vector3 Cp1 = Cp0 + FMath::RandomVector(FMath::Random(SecondScaleRange));
		const Vector3 End = Cp1 + FMath::RandomVector(FMath::Random(ThirdScaleRange));

		const Vector3 PScale = FMath::Random(
			Vector3(0.035f, 0.035f, 0.035f),
			Vector3(0.065f, 0.065f, 0.065f)) * GScale;

		const Vector3 StartRot = {
			FMath::Random(-360.f,360.f),FMath::Random(-360.f,360.f),FMath::Random(-360.f,360.f) };
		const Vector3 RotCp0 = StartRot + FMath::RandomEuler(1.f);
		const Vector3 RotCp1 = RotCp0 + FMath::RandomEuler(1.f);
		const Vector3 EndRot = RotCp1 + FMath::RandomEuler(1.f);

		ParticleInstance::Ice _IceValue{};

		_IceValue.ColorIntencity = FMath::Random(1.5f, 3.f);

		const float LifeTime = FMath::Random(2.f, 4.f);

		_ParticleInstance.PreSetup({ TargetLocation,Cp0,Cp1,End },
			{ StartRot,RotCp0,RotCp1,EndRot },
			PScale, LifeTime, 0.0f, _IceValue, std::nullopt);
	}
};
void PreLoader::JudgementDayParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>(
		"..\\..\\Usable\\Smoke\\11.tga");

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "JudgementParticle";
	// 공유 정보 바인드
	_PushParticle.SharedResourceBind = [_Tex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		Fx->SetTexture("MskMap", _Tex->GetTexture());
	};

	_PushParticle.InstanceBind = [](
		const std::any& _InstanceVariable,
		ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Judgement&>(_InstanceVariable);
		Fx->SetFloatArray("_Color", _Value.Color, 3u);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};
	const uint64 PoolSize = 3333ul;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated(
			"JudgementDay",
			std::move(_PushParticle), PoolSize, true);

	{
		for (auto& _ParticleInstance : *ParticlePool)
		{
			Vector3 Direction = FMath::Normalize( FMath::RandomVector(1.f));
			Vector3 StartLocation = FMath::RandomVector(150.f);

			Vector3 Cp0 = StartLocation + Direction * FMath::Random(0.f,1.f) 
				+  FMath::RandomVector(1.f);

			Vector3 Cp1 = Cp0 + Direction * FMath::Random(0.f, 11.f)
				+ FMath::RandomVector(11.f);

			Vector3 End = Cp1 + Direction * FMath::Random(0.f, 100.f)
				+ FMath::RandomVector(777.f);

			// End.y += FMath::Random(0.f,111.f);

			const Vector3 StartRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp0 = StartRot + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp1 = RotCp0 + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 EndRot = RotCp1 + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };

			constexpr float ScaleFactor = 4.f;
			const float RScale = FMath::Random(
				0.0055f * ScaleFactor,
				0.0075f * ScaleFactor)
				* GScale;

			ParticleInstance::Judgement _Value{};

			_Value.ColorIntencity = FMath::Random(0.007f, 0.014f);

			_Value.Color = FMath::Random(Vector3{ 255.0f,73.f,60.f}, Vector3{ 255.f,60.f ,73.f });

			const float LifeTime = FMath::Random(0.5f, 1.f);

			_ParticleInstance.PreSetup(
				{ StartLocation ,Cp0,Cp1,End },
				{ StartRot,RotCp0,RotCp1,EndRot },
				{ RScale,RScale,RScale },
				LifeTime,
				0.0f,
				_Value,
				std::nullopt);
		}
	}
};

void PreLoader::NuClearParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>(
		"..\\..\\Usable\\Smoke\\11.tga");

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "ArtemisMissileParticle";
	// 공유 정보 바인드
	_PushParticle.SharedResourceBind = [_Tex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		Fx->SetTexture("MskMap", _Tex->GetTexture());
	};

	_PushParticle.InstanceBind = [](
		const std::any& _InstanceVariable,
		ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Artemis&>(_InstanceVariable);
		Fx->SetFloatArray("_Color", _Value.Color, 3u);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 2222u;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated("NuClearParticle",
			std::move(_PushParticle), PoolSize, true);
	
	{
		for (auto& _ParticleInstance : *ParticlePool)
		{
			const Vector3 StartLocation =
					FMath::RandomVector(1677.f);
			const float LerpScale = FMath::Random(0.0f, 1.f); 
			const Vector3 End =  FMath::Lerp(StartLocation , Vector3{ 0.f,0.f,0.f } ,
				LerpScale);

			const Vector3 Cp0 = FMath::Lerp(StartLocation, End,0.33f) + FMath::RandomVector(555.f * LerpScale);
			const Vector3 Cp1 = FMath::Lerp(StartLocation, End, 0.66f) + FMath::RandomVector(555.f * LerpScale);

			const Vector3 StartRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp0 = StartRot + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp1 = RotCp0 + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 EndRot = RotCp1 + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };

			constexpr float ScaleFactor = 17.f;
			const float RScale = FMath::Random(
				0.0055f * ScaleFactor,
				0.0075f * ScaleFactor)
				* GScale;

			ParticleInstance::Artemis _Value{};

			_Value.ColorIntencity = FMath::Random(1.2f, 2.f);
			  
			_Value.Color = FMath::Lerp(
				Vector3{ 148.f / 255.f,  148.f / 255.f,  231.f / 255.f },
				Vector3{ 1.f,1.f,1.f },
				FMath::Random(0.f, 1.f));

			const float LifeTime = FMath::Random(0.1f, 2.f);

			_ParticleInstance.PreSetup(
				{ StartLocation ,Cp0,Cp1,End },
				{ StartRot,RotCp0,RotCp1,EndRot },
				{ RScale,RScale,RScale },
				LifeTime,
				0.0f,
				_Value,
				std::nullopt);
		}
	}
}

void PreLoader::KaboomParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>(
		"..\\..\\Usable\\Smoke\\11.tga");

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "ArtemisMissileParticle";
	// 공유 정보 바인드
	_PushParticle.SharedResourceBind = [_Tex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		Fx->SetTexture("MskMap", _Tex->GetTexture());
	};

	_PushParticle.InstanceBind = [](
		const std::any& _InstanceVariable,
		ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Artemis&>(_InstanceVariable);
		Fx->SetFloatArray("_Color", _Value.Color, 3u);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 3333u;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated("Kaboom",
			std::move(_PushParticle), PoolSize, true);

	{
		for (auto& _ParticleInstance : *ParticlePool)
		{
			const Vector3 StartLocation = {0.f,0.f,0.f};
			const Vector3 Next = FMath::Random(Vector3{ -3000.f , 0.f,-3000.f }, { 3000.f , 2000.f,3000.f });
			const Vector3 Cp0 = StartLocation + Next;
			const Vector3 Dir = FMath::Normalize(Next);
			const Vector3 Cp1 = Cp0 + Dir*(500.f);
			const Vector3 End = Cp1 + Dir*(250.f);

			const Vector3 StartRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp0 = StartRot + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp1 = RotCp0 + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 EndRot = RotCp1 + Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };

			constexpr float ScaleFactor = 17.f;
			const float RScale = FMath::Random(
				0.0055f * ScaleFactor,
				0.0075f * ScaleFactor)
				* GScale;

			ParticleInstance::Artemis _Value{};

			_Value.ColorIntencity = FMath::Random(1.2f, 2.f);

			_Value.Color = FMath::Lerp(
				Vector3{ 148.f / 255.f,  148.f / 255.f,  231.f / 255.f },
				Vector3{ 1.f,1.f,1.f },
				FMath::Random(0.f, 1.f));

			const float LifeTime = FMath::Random(2.f, 4.f);

			_ParticleInstance.PreSetup(
				{ StartLocation ,Cp0,Cp1,End },
				{ StartRot,RotCp0,RotCp1,EndRot },
				{ RScale,RScale,RScale },
				LifeTime,
				0.0f,
				_Value,
				std::nullopt);
		}
	}
}

void PreLoader::FireParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "FireParticle";
	// 공유 정보 바인드 

	auto _Tex = Resources::Load<Texture>("..\\..\\Resource\\Texture\\Effect\\Fire\\11.tga");

	_PushParticle.SharedResourceBind = [_Tex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		Fx->SetTexture("AlbmMap", _Tex->GetTexture());
	};

	_PushParticle.InstanceBind = [](const std::any& _InstanceVariable, ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Fire&>(_InstanceVariable);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 1000;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated("FireParticle", std::move(_PushParticle), PoolSize, true);

	for (auto& _ParticleInstance : *ParticlePool)
	{
		Vector2 Range{ -1.f,1.f };

		const Vector3 TargetLocation = Vector3{ 0.f,0.f,0.f };

		static constexpr float StartVelocityScale = 100.f;
		static constexpr float SecondVelocityScale = 200.f;
		static constexpr float ThirdVelocityScale = 300.f;

		ParticleInstance::Fire _FireValue{};
		_FireValue.ColorIntencity = FMath::Random(0.3f, 0.5f);
		const float LifeTime = FMath::Random(0.20f, 1.f);

		const Vector3 ParticleScale =
			FMath::Random(Vector3{ 0.05f ,0.05f ,0.05f },
				Vector3{ 0.067,0.067,0.067f }) * GScale;

		/*const Vector3 ParticleScale =
			FMath::Random(Vector3{ 1.f ,1.f,1.f },
				Vector3{ 1.f,1.f,1.f }) * GScale;*/

		const Vector3 Cp0 = TargetLocation + FMath::Random(Vector3{ -1.f,-1.f,-1.f } *StartVelocityScale,
			Vector3{ 1.f,1.f,1.f }    *StartVelocityScale);

		const Vector3 Cp1 = Cp0 + FMath::Random(Vector3{ -1.f,-1.f,-1.f } *SecondVelocityScale,
			Vector3{ 1.f,1.f,1.f }    *SecondVelocityScale);

		const Vector3 End = Cp1 + FMath::Random(Vector3{ -1.f,-1.f,-1.f } *ThirdVelocityScale,
			Vector3{ 1.f,1.f,1.f }    *ThirdVelocityScale);

		const Vector3 StartRot = FMath::RandomEuler(1.f);
		const Vector3 EulerVelocity = FMath::RandomEuler(StartVelocityScale);

		const Vector3 RotCp0 = StartRot + EulerVelocity;
		const Vector3 RotCp1 = RotCp0 + EulerVelocity * SecondVelocityScale;
		const Vector3 EndRot = RotCp1 + EulerVelocity * ThirdVelocityScale;

		const auto _SpriteDesc =
			ParticleSystem::ParticleInstance::SpriteDesc::Make(8, 4, 0, FMath::Random(0u, 7u),
				FMath::Random(0.01f, 0.03f),
				true, false);

		_ParticleInstance.PreSetup({ TargetLocation,Cp0,Cp1,End },
			{ StartRot,RotCp0,RotCp1,EndRot },
			ParticleScale,
			LifeTime, 0.0f, _FireValue, _SpriteDesc);
	}
}
void PreLoader::FireVortexSpritePoolLoad(const uint32 SpriteIdx)
{
	ENGINE::ParticleSystem::Particle _PushParticle{};
	Mesh::InitializeInfo _Info{};
	_PushParticle._Mesh = Resources::Load<StaticMesh>("..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	const std::string IdxName = std::to_string(SpriteIdx);
	const std::string Ext = ".tga";
	const std::string CurLoadPath = "..\\..\\Resource\\Texture\\Effect\\Fire\\" + IdxName + Ext;

	auto _AlbTex = Resources::Load<Texture>(CurLoadPath, _Info);
	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "FireSprite";
	// 공유 정보 바인드 

	_PushParticle.SharedResourceBind = [_AlbTex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		Fx->SetTexture("AlbmMap", _AlbTex->GetTexture());
	};

	_PushParticle.InstanceBind = [](const std::any& _InstanceVariable, ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::FireSprite&>(_InstanceVariable);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 4000ul; 

	const std::string ParticleName = "FireVortexSprite" + IdxName;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated(ParticleName,
			std::move(_PushParticle), PoolSize, true);

	Mesh::InitializeInfo _InitInfo{};
	_InitInfo.bLocalVertexLocationsStorage = true;
	auto _Vortex = Resources::Load<StaticMesh>("..\\..\\Resource\\Mesh\\Static\\Effect\\vortex.fbx" , _InitInfo);

	const uint64 _VtxCnt =_Vortex->m_spVertexLocations->size();

	for (auto& _ParticleInstance : *ParticlePool)
	{
		Vector3 TargetLocation = Vector3{ 0.f,0.f,0.f };
		TargetLocation = (*_Vortex->m_spVertexLocations)[FMath::Random<uint32>(0u, _VtxCnt-1)];
		TargetLocation += FMath::RandomVector(2.f);
		const Vector3 Direction = FMath::Normalize(TargetLocation);

		static constexpr float StartVelocityScale = 0.00f;
		static constexpr float SecondVelocityScale = 5.f;
		static constexpr float ThirdVelocityScale = 10.f;

		ParticleInstance::FireSprite _FireValue{};
		_FireValue.ColorIntencity = FMath::Random(0.3f, 0.5f);
		const float LifeTime = FMath::Random(0.1f, 0.5f);

		/*const Vector3 ParticleScale =
			FMath::Random(Vector3{ 0.2f ,0.2f,0.2f },
				Vector3{ 0.4f,0.4f,0.4f}) * GScale;*/

		const Vector3 ParticleScale =
			FMath::Random(Vector3{ 0.8f ,0.8f,0.8f },
				Vector3{ 1.6f,1.6f,1.6f}) * GScale;

		const Vector3 Cp0 = TargetLocation + Direction * (StartVelocityScale)+FMath::RandomVector(StartVelocityScale);


		const Vector3 Cp1 = Cp0 + Direction * (SecondVelocityScale)+FMath::RandomVector(SecondVelocityScale);

		const Vector3 End = Cp1 + Direction * (ThirdVelocityScale)+FMath::RandomVector(ThirdVelocityScale);

		const Vector3 StartRot = Vector3{ 0.f,0.f,0.f };
		const Vector3 RotCp0 = StartRot + Vector3{ 0.f,0.f,0.f };
		const Vector3 RotCp1 = RotCp0 + Vector3{ 0.f,0.f,0.f };
		const Vector3 EndRot = RotCp1 + Vector3{ 0.f,0.f,0.f };

		ParticleSystem::ParticleInstance::SpriteDesc _SpriteDesc{};

		uint32 SpriteColCnt = 8u;
		uint32 SpriteRowCnt = 4u;
		uint32 SpriteCurCol = 0u;
		uint32 SpriteCurRow = FMath::Random(0u, 7u);
		bool bColLoop = true;
		bool bRowLoop = true;

		switch (SpriteIdx)
		{
		case 1:
			SpriteColCnt = 16u;
			SpriteRowCnt = 4u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		case 2:
			SpriteColCnt = 16u;
			SpriteRowCnt = 4u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		case 3:
			SpriteColCnt = 16u;
			SpriteRowCnt = 4u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		case 4:
			SpriteColCnt = 8u;
			SpriteRowCnt = 8u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		case 5:
			SpriteColCnt = 8u;
			SpriteRowCnt = 8u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		case 6:
			SpriteColCnt = 8u;
			SpriteRowCnt = 4u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		case 7:
			SpriteColCnt = 8u;
			SpriteRowCnt = 8u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		case 8:
			SpriteColCnt = 8u;
			SpriteRowCnt = 8u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		case 9:
			SpriteColCnt = 8u;
			SpriteRowCnt = 4u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		case 10:
			SpriteColCnt = 8u;
			SpriteRowCnt = 8u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		case 11:
			SpriteColCnt = 8u;
			SpriteRowCnt = 4u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		case 12:
			SpriteColCnt = 8u;
			SpriteRowCnt = 4u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		case 13:
			SpriteColCnt = 8u;
			SpriteRowCnt = 8u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		case 14:
			SpriteColCnt = 16u;
			SpriteRowCnt = 8u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		case 15:
			SpriteColCnt = 8u;
			SpriteRowCnt = 4u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		default:
			break;
		}

		_SpriteDesc = ParticleSystem::ParticleInstance::SpriteDesc::Make(
			SpriteColCnt, SpriteRowCnt, SpriteCurCol, SpriteCurRow,
			FMath::Random(0.008f, 0.016f),
			true, true);

		_ParticleInstance.PreSetup({ TargetLocation,Cp0,Cp1,End },
			{ StartRot,RotCp0,RotCp1,EndRot },
			ParticleScale,
			LifeTime, 0.0f, _FireValue, _SpriteDesc);
	}

}
;

void PreLoader::FireSpritePoolLoad(const uint32 SpriteIdx)
{
	ENGINE::ParticleSystem::Particle _PushParticle{};
	Mesh::InitializeInfo _Info{};
	_PushParticle._Mesh = Resources::Load<StaticMesh>("..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx",_Info);

	const std::string IdxName = std::to_string(SpriteIdx);
	const std::string Ext = ".tga";
	const std::string CurLoadPath = "..\\..\\Resource\\Texture\\Effect\\Fire\\" + IdxName + Ext;

	auto _AlbTex = Resources::Load<Texture>(CurLoadPath ,_Info);
	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "FireSprite";
	// 공유 정보 바인드 

	_PushParticle.SharedResourceBind = [_AlbTex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		Fx->SetTexture("AlbmMap", _AlbTex->GetTexture());
	};

	_PushParticle.InstanceBind = [](const std::any& _InstanceVariable, ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::FireSprite&>(_InstanceVariable);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 15000ul;

	const std::string ParticleName = "FireSprite" + IdxName;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated(ParticleName, 
			std::move(_PushParticle), PoolSize, true);

	for (auto& _ParticleInstance : *ParticlePool)
	{
		Vector2 Range{ -1.f,1.f };

		const Vector3 TargetLocation = Vector3{ 0.f,0.f,0.f };

		static constexpr float StartVelocityScale = 0.10f;
		static constexpr float SecondVelocityScale = 0.10f;
		static constexpr float ThirdVelocityScale = 0.10f;

		ParticleInstance::FireSprite _FireValue{};
		_FireValue.ColorIntencity = FMath::Random(0.2f, 0.4f);
		const float LifeTime = FMath::Random(0.20f, 0.5f);

		const Vector3 ParticleScale =
			FMath::Random(Vector3{ 0.05f ,0.05f ,0.05f },
				Vector3{ 0.067,0.067,0.067f }) * GScale;

		const Vector3 Cp0 = TargetLocation + FMath::RandomVector(StartVelocityScale);
		
		const Vector3 Cp1 = Cp0 + FMath::RandomVector(SecondVelocityScale);

		const Vector3 End = Cp1 + FMath::RandomVector(ThirdVelocityScale);

		const Vector3 StartRot = Vector3{ 0.f,0.f,0.f };
		const Vector3 RotCp0 = StartRot + Vector3{ 0.f,0.f,0.f };
		const Vector3 RotCp1 = RotCp0 + Vector3{ 0.f,0.f,0.f };
		const Vector3 EndRot = RotCp1 + Vector3{ 0.f,0.f,0.f };
		
		ParticleSystem::ParticleInstance::SpriteDesc _SpriteDesc{};

		uint32 SpriteColCnt = 8u;
		uint32 SpriteRowCnt = 4u;
		uint32 SpriteCurCol = 0u;
		uint32 SpriteCurRow = FMath::Random(0u, 7u);
		bool bColLoop = true;
		bool bRowLoop = true;

		switch (SpriteIdx)
		{
		case 1:
			SpriteColCnt = 16u;
			SpriteRowCnt = 4u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		case 2:
			SpriteColCnt = 16u;
			SpriteRowCnt = 4u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		case 3:
			SpriteColCnt = 16u;
			SpriteRowCnt = 4u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		case 4:
			SpriteColCnt = 8u;
			SpriteRowCnt = 8u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		case 5:
			SpriteColCnt = 8u;
			SpriteRowCnt = 8u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		case 6:
			SpriteColCnt = 8u;
			SpriteRowCnt = 4u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		case 7:
			SpriteColCnt = 8u;
			SpriteRowCnt = 8u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		case 8:
			SpriteColCnt = 8u;
			SpriteRowCnt = 8u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		case 9:
			SpriteColCnt = 8u;
			SpriteRowCnt = 4u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		case 10:
			SpriteColCnt = 8u;
			SpriteRowCnt = 8u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		case 11:
			SpriteColCnt = 8u;
			SpriteRowCnt = 4u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		case 12:
			SpriteColCnt = 8u;
			SpriteRowCnt = 4u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		case 13:
			SpriteColCnt = 8u;
			SpriteRowCnt = 8u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		case 14:
			SpriteColCnt = 16u;
			SpriteRowCnt = 8u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		case 15:
			SpriteColCnt = 8u;
			SpriteRowCnt = 4u;
			SpriteCurCol = 0u;
			SpriteCurRow = FMath::Random(0u, SpriteRowCnt - 1u);
			bColLoop = true;
			bRowLoop = true;
			break;
		default:
			break;
		}

		_SpriteDesc=	ParticleSystem::ParticleInstance::SpriteDesc::Make(
			SpriteColCnt, SpriteRowCnt, SpriteCurCol, SpriteCurRow,
			FMath::Random(0.016f, 0.032f),
			true, true);

		_ParticleInstance.PreSetup({ TargetLocation,Cp0,Cp1,End },
			{ StartRot,RotCp0,RotCp1,EndRot },
			ParticleScale,
			LifeTime, 0.0f, _FireValue, _SpriteDesc);
	}
};

void PreLoader::ElectricEffectParticlePoolLoad(
	const std::filesystem::path& _TexPath,
	const uint32 SpriteColCnt,
	const uint32 SpriteRowCnt,
	const std::string& ParticleName)
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _AlbTex = Resources::Load<Texture>("..\\..\\Resource\\Texture\\Effect\\lightning_alb.png");

	auto _Tex = Resources::Load<Texture>(_TexPath);
	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "ElectricEffectParticle";
	// 공유 정보 바인드 
	_PushParticle.SharedResourceBind = [_Tex, _AlbTex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		static const Vector3 ElectricColor{ 181.f / 255.f, 173.f / 255.f ,206.f / 255.f };
		Fx->SetFloatArray("_Color", ElectricColor, 3u);
		Fx->SetTexture("AlbmMap", _AlbTex->GetTexture());
		Fx->SetTexture("AlpMskMap", _Tex->GetTexture());
	};

	_PushParticle.InstanceBind = [](const std::any& _InstanceVariable, ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Electric&>(_InstanceVariable);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 100;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated(ParticleName,
			std::move(_PushParticle), PoolSize, true);

	for (auto& _ParticleInstance : *ParticlePool)
	{
		const Vector3 StartLocation{ 0.f,0.f,0.f };
		const Vector3 Cp0 = StartLocation + FMath::RandomVector(FMath::Random(1000.f, 1500.f));
		const Vector3 Cp1 = Cp0 + FMath::RandomVector(FMath::Random(1000.f, 1500.f));
		const Vector3 End = Cp1 + FMath::RandomVector(FMath::Random(1000.f, 1500.f));

		const Vector3 StartRot = Vector3{ 0.f,0.f,0.f };
		const Vector3 RotCp0 = Vector3{ 0.f,0.f,0.f };
		const Vector3 RotCp1 = Vector3{ 0.f,0.f,0.f };
		const Vector3 EndRot = Vector3{ 0.f,0.f,0.f };

		const float RScale = FMath::Random(0.1f, 1.f) * GScale;

		ParticleInstance::Electric _ElectricValue{};

		_ElectricValue.ColorIntencity = FMath::Random(0.01f, 0.01f);

		const float LifeTime = FMath::Random(0.8f, 1.3f);

		const auto _SpriteDesc = ParticleSystem::ParticleInstance::SpriteDesc::Make
		(SpriteColCnt, SpriteRowCnt, 0u, FMath::Random(0u, SpriteRowCnt - 1u),
			FMath::Random(0.01f, 0.1f), true, true);

		_ParticleInstance.PreSetup({ StartLocation ,Cp0,Cp1,End },
			{ StartRot,RotCp0,RotCp1,EndRot },
			{ RScale,RScale,RScale }, LifeTime, 0.0f,
			_ElectricValue, _SpriteDesc);
	}
};

void PreLoader::ElectricParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>("..\\..\\Usable\\Smoke\\11.tga");

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "ElectricParticle";
	// 공유 정보 바인드
	_PushParticle.SharedResourceBind = [_Tex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{

		Fx->SetTexture("MskMap",_Tex->GetTexture());
	};

	_PushParticle.InstanceBind = [](const std::any& _InstanceVariable, ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Electric&>(_InstanceVariable);
		Fx->SetFloatArray("ElectricColor", _Value.Color, 3u);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 1000ul;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated("ElectricParticle",
			std::move(_PushParticle), PoolSize, true);

	_Info.bLocalVertexLocationsStorage = true;
	auto _TargetMesh = Resources::Load<StaticMesh>(
		"..\\..\\Usable\\Electric\\magic_eletric00.fbx",_Info);
	if (_TargetMesh->m_spVertexLocations)
	{
		for (auto& _ParticleInstance : *ParticlePool)
		{
			const uint32 Idx = FMath::Random(0u,uint32(_TargetMesh->m_spVertexLocations->size() - 1));

			const Vector3 StartLocation =
				(*_TargetMesh->m_spVertexLocations)[Idx];

			const Vector3 Dir = FMath::Normalize(StartLocation);
			Vector3 Cp0{};
			Vector3 Cp1{};
			Vector3 End{};
			const uint32 TargetIdx = (Idx % 3);
			if (TargetIdx==0)
			{
				Cp0 = StartLocation + FMath::RandomVector(FMath::Random(300.f, 500.f));
				Cp1 = Cp0 + FMath::RandomVector(FMath::Random(300.f, 500.f));
				End = Cp1 + FMath::RandomVector(FMath::Random(300.f, 500.f));
			}
			else if (TargetIdx==1)
			{
				Cp0 = StartLocation + FMath::RandomVector(FMath::Random(11.f, 22.f));
				Cp1 = Cp0 + FMath::RandomVector(FMath::Random(11.f, 22.f));
				End = Cp1 + FMath::RandomVector(FMath::Random(11.f, 22.f));
			}
			else
			{
				Cp0 = StartLocation + Dir * FMath::Random(1333.f, 2333.f);
				Cp1 = Cp0 + Dir * FMath::Random(1333.f, 2333.f);
				End = Cp1 + Dir * FMath::Random(1333.f, 2333.f);
			}

			const Vector3 StartRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp0 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp1 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 EndRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };

			const float RScale = FMath::Random(0.0022f, 0.006f) * GScale;

			ParticleInstance::Electric _ElectricValue{};

			_ElectricValue.ColorIntencity = FMath::Random(0.44f, 1.f);
			_ElectricValue.Color = FMath::Lerp(Vector3{ 120.f / 255.f,50.f / 255.f, 201.f / 255.f }, Vector3{ 1.f,1.f,1.f }, FMath::Random(0.f, 1.f));
			const float LifeTime = FMath::Random(0.1f, 3.f);

			_ParticleInstance.PreSetup({ StartLocation ,Cp0,Cp1,End },
				{ StartRot,RotCp0,RotCp1,EndRot },
				{ RScale,RScale,RScale }, LifeTime, 0.0f, _ElectricValue, std::nullopt);
		}
	}
}

void PreLoader::ElectricVoertexParticleLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>("..\\..\\Usable\\Smoke\\11.tga");

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "ElectricParticle";
	// 공유 정보 바인드
	_PushParticle.SharedResourceBind = [_Tex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{

		Fx->SetTexture("MskMap", _Tex->GetTexture());
	};

	_PushParticle.InstanceBind = [](const std::any& _InstanceVariable, ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Electric&>(_InstanceVariable);
		Fx->SetFloatArray("ElectricColor", _Value.Color, 3u);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 3000ul;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated("ElectricVortexParticle",
			std::move(_PushParticle), PoolSize, true);

	_Info.bLocalVertexLocationsStorage = true;
	auto _TargetMesh = Resources::Load<StaticMesh>(
		"..\\..\\Usable\\Electric\\magic_eletric02.fbx", _Info);

	if (_TargetMesh->m_spVertexLocations)
	{
		for (auto& _ParticleInstance : *ParticlePool)
		{
			const uint32 Idx = FMath::Random(0u, uint32(_TargetMesh->m_spVertexLocations->size() - 1));

			const Vector3 StartLocation =
				(*_TargetMesh->m_spVertexLocations)[Idx];

			const Vector3 Dir = FMath::Normalize(StartLocation);
			Vector3 Cp0{};
			Vector3 Cp1{};
			Vector3 End{};


			
				Cp0 = StartLocation + FMath::RandomVector(FMath::Random(300.f, 500.f));
				Cp1 = Cp0 + FMath::RandomVector(FMath::Random(300.f, 500.f));
				End = Cp1 + FMath::RandomVector(FMath::Random(300.f, 500.f));
			
	

			const Vector3 StartRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp0 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp1 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 EndRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };

			const float RScale = FMath::Random(0.0066f, 0.01f) * GScale;

			ParticleInstance::Electric _ElectricValue{};

			_ElectricValue.ColorIntencity = FMath::Random(0.44f, 1.f);
			_ElectricValue.Color = FMath::Lerp(Vector3{ 120.f / 255.f,50.f / 255.f, 201.f / 255.f }, Vector3{ 1.f,1.f,1.f }, FMath::Random(0.f, 1.f));
			const float LifeTime = FMath::Random(0.1f, 0.6f);

			_ParticleInstance.PreSetup({ StartLocation ,Cp0,Cp1,End },
				{ StartRot,RotCp0,RotCp1,EndRot },
				{ RScale,RScale,RScale }, LifeTime, 0.0f, _ElectricValue, std::nullopt);
		}
	}
}

void PreLoader::ThunderBoltParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>("..\\..\\Usable\\Smoke\\11.tga");

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "ElectricParticle";
	// 공유 정보 바인드
	_PushParticle.SharedResourceBind = [_Tex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		Fx->SetTexture("MskMap", _Tex->GetTexture());
	};

	_PushParticle.InstanceBind = [](const std::any& _InstanceVariable, ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Electric&>(_InstanceVariable);
		Fx->SetFloatArray("ElectricColor", _Value.Color, 3u);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 10000ul;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated("ThunderBoltParticle",
			std::move(_PushParticle), PoolSize, true);

	_Info.bLocalVertexLocationsStorage = true;
	auto _TargetMesh = Resources::Load<StaticMesh>(
		"..\\..\\Usable\\Electric\\magic_eletric01.fbx", _Info);
	if (_TargetMesh->m_spVertexLocations)
	{
		for (auto& _ParticleInstance : *ParticlePool)
		{
			const uint32 Idx = FMath::Random(0u, uint32(_TargetMesh->m_spVertexLocations->size() - 1));

			const Vector3 StartLocation =
				(*_TargetMesh->m_spVertexLocations)[Idx];

			const Vector3 Dir = FMath::Normalize(StartLocation);
			Vector3 Cp0{};
			Vector3 Cp1{};
			Vector3 End{};

			Cp0 = StartLocation + FMath::RandomVector(FMath::Random(66.f, 66.f));
			Cp1 = Cp0 + FMath::RandomVector(FMath::Random(33.f, 66.f));
			End = Cp1 + FMath::RandomVector(FMath::Random(33.f, 66.f));
	

			const Vector3 StartRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp0 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp1 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 EndRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };

			const float RScale = FMath::Random(0.0022f, 0.006f) * GScale;

			ParticleInstance::Electric _ElectricValue{};

			_ElectricValue.ColorIntencity = FMath::Random(0.44f, 1.f);
			_ElectricValue.Color = FMath::Lerp(Vector3{ 120.f / 255.f,50.f / 255.f, 201.f / 255.f }, Vector3{ 1.f,1.f,1.f }, FMath::Random(0.f, 1.f));
			const float LifeTime = FMath::Random(0.1f, 3.f);

			_ParticleInstance.PreSetup({ StartLocation ,Cp0,Cp1,End },
				{ StartRot,RotCp0,RotCp1,EndRot },
				{ RScale,RScale,RScale }, LifeTime, 0.0f, _ElectricValue, std::nullopt);
		}
	}
};

void PreLoader::ThunderBoltEndParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>("..\\..\\Usable\\Smoke\\11.tga");

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "ElectricParticle";
	// 공유 정보 바인드
	_PushParticle.SharedResourceBind = [_Tex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		Fx->SetTexture("MskMap", _Tex->GetTexture());
	};

	_PushParticle.InstanceBind = [](const std::any& _InstanceVariable, ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Electric&>(_InstanceVariable);
		Fx->SetFloatArray("ElectricColor", _Value.Color, 3u);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 10000ul;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated("ThunderBoltEndParticle",
			std::move(_PushParticle), PoolSize, true);

	_Info.bLocalVertexLocationsStorage = true;
	auto _TargetMesh = Resources::Load<StaticMesh>(
		"..\\..\\Usable\\Electric\\magic_eletric01.fbx", _Info);
	if (_TargetMesh->m_spVertexLocations)
	{
		for (auto& _ParticleInstance : *ParticlePool)
		{
			const uint32 Idx = FMath::Random(0u, uint32(_TargetMesh->m_spVertexLocations->size() - 1));

			const Vector3 StartLocation =
				(*_TargetMesh->m_spVertexLocations)[Idx];

			const Vector3 Dir = FMath::Normalize(StartLocation);
			Vector3 Cp0{};
			Vector3 Cp1{};
			Vector3 End{};

			{
				Cp0 = StartLocation + -Dir * FMath::Random(99.f, 180.f);
				Cp1 = Cp0 + -Dir * FMath::Random(99.f, 180.f);
				End = Cp1 + Dir * FMath::Random(99.f, 180.f);
			}

			const Vector3 StartRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp0 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp1 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 EndRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };

			const float RScale = FMath::Random(0.0077f, 0.01f) * GScale;

			ParticleInstance::Electric _ElectricValue{};

			_ElectricValue.ColorIntencity = FMath::Random(0.44f, 1.f);
			_ElectricValue.Color = FMath::Lerp(Vector3{ 120.f / 255.f,50.f / 255.f, 201.f / 255.f }, Vector3{ 1.f,1.f,1.f }, FMath::Random(0.f, 1.f));
			const float LifeTime = FMath::Random(0.1f, 3.f);

			_ParticleInstance.PreSetup({ StartLocation ,Cp0,Cp1,End },
				{ StartRot,RotCp0,RotCp1,EndRot },
				{ RScale,RScale,RScale }, LifeTime, 0.0f, _ElectricValue, std::nullopt);
		}
	}
}

void PreLoader::ThunderBoltSecondParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>("..\\..\\Usable\\Smoke\\11.tga");

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "ElectricParticle";
	// 공유 정보 바인드
	_PushParticle.SharedResourceBind = [_Tex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		Fx->SetTexture("MskMap", _Tex->GetTexture());
	};

	_PushParticle.InstanceBind = [](const std::any& _InstanceVariable, ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Electric&>(_InstanceVariable);
		Fx->SetFloatArray("ElectricColor", _Value.Color, 3u);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 10000ul;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated("ThunderBoltSecondParticle",
			std::move(_PushParticle), PoolSize, true);

	_Info.bLocalVertexLocationsStorage = true;
	auto _TargetMesh = Resources::Load<StaticMesh>(
		"..\\..\\Usable\\Electric\\Spark\\parts.fbx", _Info);

	if (_TargetMesh->m_spVertexLocations)
	{
		for (auto& _ParticleInstance : *ParticlePool)
		{
			const uint32 Idx = FMath::Random(0u, uint32(_TargetMesh->m_spVertexLocations->size() - 1));

			const Vector3 StartLocation =
				(*_TargetMesh->m_spVertexLocations)[Idx];

			const Vector3 Dir = FMath::Normalize(StartLocation);
			Vector3 Cp0{};
			Vector3 Cp1{};
			Vector3 End{};

			Cp0 = StartLocation + FMath::RandomVector(FMath::Random(0.5f,1.f));
			Cp1 = Cp0 + FMath::RandomVector(FMath::Random(0.5f, 1.f));
			End = Cp1 + FMath::RandomVector(FMath::Random(0.5f, 1.f));


			const Vector3 StartRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp0 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp1 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 EndRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };

			const float RScale = FMath::Random(0.0066f, 0.018f) * GScale;

			ParticleInstance::Electric _ElectricValue{};

			_ElectricValue.ColorIntencity = FMath::Random(0.44f, 1.f);
			_ElectricValue.Color = 
				FMath::Lerp(Vector3{ 99.f/ 255.f,121.f/ 255.f, 148.f/ 255.f }, Vector3{ 1.f,1.f,1.f }, FMath::Random(0.f, 1.f));
			const float LifeTime = FMath::Random(0.1f, 3.f);

			_ParticleInstance.PreSetup({ StartLocation ,Cp0,Cp1,End },
				{ StartRot,RotCp0,RotCp1,EndRot },
				{ RScale,RScale,RScale }, LifeTime, 0.0f, _ElectricValue, std::nullopt);
		}
	}
}

void PreLoader::ThunderBoltSecondEndParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>("..\\..\\Usable\\Smoke\\11.tga");

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "ElectricParticle";
	// 공유 정보 바인드
	_PushParticle.SharedResourceBind = [_Tex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		Fx->SetTexture("MskMap", _Tex->GetTexture());
	};

	_PushParticle.InstanceBind = [](const std::any& _InstanceVariable, ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Electric&>(_InstanceVariable);
		Fx->SetFloatArray("ElectricColor", _Value.Color, 3u);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 10000ul;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated("ThunderBoltSecondEndParticle",
			std::move(_PushParticle), PoolSize, true);

	_Info.bLocalVertexLocationsStorage = true;
	auto _TargetMesh = Resources::Load<StaticMesh>(
		"..\\..\\Usable\\Electric\\Spark\\parts.fbx", _Info);

	if (_TargetMesh->m_spVertexLocations)
	{
		for (auto& _ParticleInstance : *ParticlePool)
		{
			const uint32 Idx = FMath::Random(0u, uint32(_TargetMesh->m_spVertexLocations->size() - 1));

			const Vector3 StartLocation =
				(*_TargetMesh->m_spVertexLocations)[Idx];

			const Vector3 Dir = FMath::Normalize(StartLocation);
			Vector3 Cp0{};
			Vector3 Cp1{};
			Vector3 End{};

			{
				Cp0 = StartLocation + -Dir * FMath::Random(1.f, 2.f);
				Cp1 = Cp0 + -Dir * FMath::Random(1.f, 2.f);
				End = Cp1 + -Dir * FMath::Random(1.f, 2.f);
			}

			const Vector3 StartRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp0 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp1 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 EndRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };

			const float RScale = FMath::Random(0.0066f, 0.018f) * GScale;

			ParticleInstance::Electric _ElectricValue{};

			_ElectricValue.ColorIntencity = FMath::Random(0.44f, 1.f);
			_ElectricValue.Color = FMath::Lerp(Vector3{ 99.f / 255.f,121.f / 255.f, 148.f / 255.f }, Vector3{ 1.f,1.f,1.f }, FMath::Random(0.f, 1.f));
			const float LifeTime = FMath::Random(0.1f, 3.f);

			_ParticleInstance.PreSetup({ StartLocation ,Cp0,Cp1,End },
				{ StartRot,RotCp0,RotCp1,EndRot },
				{ RScale,RScale,RScale }, LifeTime, 0.0f, _ElectricValue, std::nullopt);
		}
	}
}

void PreLoader::ElectricBranchParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>("..\\..\\Usable\\Smoke\\11.tga");

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "ElectricParticle";
	// 공유 정보 바인드
	_PushParticle.SharedResourceBind = [_Tex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		Fx->SetTexture("MskMap", _Tex->GetTexture());
	};

	_PushParticle.InstanceBind = [](const std::any& _InstanceVariable, ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Electric&>(_InstanceVariable);
		Fx->SetFloatArray("ElectricColor", _Value.Color, 3u);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 1000ul;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated("ElectricBranchSecondParticle",
			std::move(_PushParticle), PoolSize, true);

	_Info.bLocalVertexLocationsStorage = true;
	auto _TargetMesh = Resources::Load<StaticMesh>(
		"..\\..\\Usable\\Electric\\Spark\\branch.fbx", _Info);

	if (_TargetMesh->m_spVertexLocations)
	{
		for (auto& _ParticleInstance : *ParticlePool)
		{
			const uint32 Idx = FMath::Random(0u, uint32(_TargetMesh->m_spVertexLocations->size() - 1));

			const Vector3 StartLocation =
				(*_TargetMesh->m_spVertexLocations)[Idx];

			const Vector3 Dir = FMath::Normalize(StartLocation);
			Vector3 Cp0{};
			Vector3 Cp1{};
			Vector3 End{};

			Cp0 = StartLocation + FMath::RandomVector(FMath::Random(66.f, 66.f));
			Cp1 = Cp0 + FMath::RandomVector(FMath::Random(33.f, 66.f));
			End = Cp1 + FMath::RandomVector(FMath::Random(33.f, 66.f));


			const Vector3 StartRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp0 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp1 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 EndRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };

			const float RScale = FMath::Random(0.0022f, 0.006f) * GScale;

			ParticleInstance::Electric _ElectricValue{};

			_ElectricValue.ColorIntencity = FMath::Random(0.44f, 1.f);
			_ElectricValue.Color = FMath::Lerp(Vector3{ 99.f / 255.f,121.f / 255.f, 148.f / 255.f }, Vector3{ 1.f,1.f,1.f }, FMath::Random(0.f, 1.f));
			const float LifeTime = FMath::Random(0.1f, 3.f);

			_ParticleInstance.PreSetup({ StartLocation ,Cp0,Cp1,End },
				{ StartRot,RotCp0,RotCp1,EndRot },
				{ RScale,RScale,RScale }, LifeTime, 0.0f, _ElectricValue, std::nullopt);
		}
	}
}

void PreLoader::ElectricBranchEndParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>("..\\..\\Usable\\Smoke\\11.tga");

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "ElectricParticle";
	// 공유 정보 바인드
	_PushParticle.SharedResourceBind = [_Tex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		Fx->SetTexture("MskMap", _Tex->GetTexture());
	};

	_PushParticle.InstanceBind = [](const std::any& _InstanceVariable, ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Electric&>(_InstanceVariable);
		Fx->SetFloatArray("ElectricColor", _Value.Color, 3u);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 500ul;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated("ElectricBranchEndParticle",
			std::move(_PushParticle), PoolSize, true);

	_Info.bLocalVertexLocationsStorage = true;
	auto _TargetMesh = Resources::Load<StaticMesh>(
		"..\\..\\Usable\\Electric\\Spark\\branch.fbx", _Info);

	if (_TargetMesh->m_spVertexLocations)
	{
		for (auto& _ParticleInstance : *ParticlePool)
		{
			const uint32 Idx = FMath::Random(0u, uint32(_TargetMesh->m_spVertexLocations->size() - 1));

			const Vector3 StartLocation =
				(*_TargetMesh->m_spVertexLocations)[Idx];

			const Vector3 Dir = FMath::Normalize(StartLocation);
			Vector3 Cp0{};
			Vector3 Cp1{};
			Vector3 End{};

			{
				Cp0 = StartLocation + -Dir * FMath::Random(99.f, 180.f);
				Cp1 = Cp0 + -Dir * FMath::Random(99.f, 180.f);
				End = Cp1 + Dir * FMath::Random(99.f, 180.f);
			}

			const Vector3 StartRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp0 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp1 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 EndRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };

			const float RScale = FMath::Random(0.0077f, 0.01f) * GScale;

			ParticleInstance::Electric _ElectricValue{};

			_ElectricValue.ColorIntencity = FMath::Random(0.44f, 1.f);
			_ElectricValue.Color = FMath::Lerp(Vector3{ 99.f / 255.f,121.f / 255.f, 148.f / 255.f }, Vector3{ 1.f,1.f,1.f }, FMath::Random(0.f, 1.f));
			const float LifeTime = FMath::Random(0.1f, 3.f);

			_ParticleInstance.PreSetup({ StartLocation ,Cp0,Cp1,End },
				{ StartRot,RotCp0,RotCp1,EndRot },
				{ RScale,RScale,RScale }, LifeTime, 0.0f, _ElectricValue, std::nullopt);
		}
	}

}

void PreLoader::AirHikeParticleEndPoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>("..\\..\\Usable\\Smoke\\11.tga");

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "AirHikeParticle";
	// 공유 정보 바인드
	_PushParticle.SharedResourceBind = [_Tex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		Fx->SetTexture("MskMap", _Tex->GetTexture());
	};

	_PushParticle.InstanceBind = [](const std::any& _InstanceVariable, ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Electric&>(_InstanceVariable);
		Fx->SetFloatArray("_Color", _Value.Color, 3u);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 2000ul;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated("AirHikeEndParticle",
			std::move(_PushParticle), PoolSize, true);

	_Info.bLocalVertexLocationsStorage = true;
	auto _TargetMesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Effect\\AirHike\\AirHike.fbx", _Info);

	if (_TargetMesh->m_spVertexLocations)
	{
		for (auto& _ParticleInstance : *ParticlePool)
		{
			const uint32 Idx = FMath::Random(0u, uint32(_TargetMesh->m_spVertexLocations->size() - 1));

			Vector3 StartLocation =
				(*_TargetMesh->m_spVertexLocations)[Idx];

			const Vector3 Dir = FMath::Normalize(StartLocation);
			Vector3 Cp0{};
			Vector3 Cp1{};
			Vector3 End{};

			{
				Cp0 = StartLocation + Dir * FMath::Random(0.f, 2.f);
				Cp1 = Cp0 + Dir * FMath::Random(0.f, 2.f);
				End = Cp1 + Dir * FMath::Random(0.f, 2.f);
			}

			const Vector3 StartRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp0 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp1 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 EndRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };

			const float RScale = FMath::Random(0.077f *0.5f, 0.05f) * GScale;

			ParticleInstance::Electric _ElectricValue{};

			_ElectricValue.ColorIntencity = FMath::Random(0.44f, 1.f);
			_ElectricValue.Color = FMath::Lerp(Vector3{ 71.f / 255.f,123.f / 255.f, 220.f/ 255.f }, Vector3{ 1.f,1.f,1.f }, FMath::Random(0.f, 1.f));
			const float LifeTime = FMath::Random(0.1f, 1.f);
			
			StartLocation = FMath::RotationVecCoord(StartLocation, { 1.f,0.f,0.f }, FMath::PI / 2.f);
			Cp0 =FMath::RotationVecCoord(Cp0, { 1.f,0.f,0.f }, FMath::PI / 2.f);
			Cp1  = FMath::RotationVecCoord(Cp1, { 1.f,0.f,0.f }, FMath::PI / 2.f);
			End = FMath::RotationVecCoord(End, { 1.f,0.f,0.f }, FMath::PI / 2.f);

			_ParticleInstance.PreSetup(
				{ StartLocation ,Cp0,Cp1,End },
				{ StartRot,RotCp0,RotCp1,EndRot },
				{ RScale,RScale,RScale }, LifeTime, 0.0f, _ElectricValue, std::nullopt);
		}
	}
}

void PreLoader::AirHikeParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>("..\\..\\Usable\\Smoke\\11.tga");

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "AirHikeParticle";
	// 공유 정보 바인드
	_PushParticle.SharedResourceBind = [_Tex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		Fx->SetTexture("MskMap", _Tex->GetTexture());
	};

	_PushParticle.InstanceBind = [](const std::any& _InstanceVariable, ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Electric&>(_InstanceVariable);
		Fx->SetFloatArray("_Color", _Value.Color, 3u);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 10000ul;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated("AirHikeParticle",
			std::move(_PushParticle), PoolSize, true);

	_Info.bLocalVertexLocationsStorage = true;
	auto _TargetMesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Effect\\AirHike\\AirHike.fbx", _Info);
	if (_TargetMesh->m_spVertexLocations)
	{
		for (auto& _ParticleInstance : *ParticlePool)
		{
			const uint32 Idx = FMath::Random(0u, uint32(_TargetMesh->m_spVertexLocations->size() - 1));

			Vector3 StartLocation =
				(*_TargetMesh->m_spVertexLocations)[Idx];

			const Vector3 Dir = FMath::Normalize(StartLocation);
			Vector3 Cp0{};
			Vector3 Cp1{};
			Vector3 End{};
			const uint32 TargetIdx = (Idx % 4);

			if ((TargetIdx % 2) == 0)
			{
				Cp0 = StartLocation + Dir * FMath::Random(0.f, 1.f);
				Cp1 = Cp0 + Dir * FMath::Random(0.f, 1.f);;
				End = Cp1 + Dir * FMath::Random(0.f, 1.f);;
			}
			else
			{
				Cp0 = StartLocation + FMath::Random(Vector3{ -0.f,-50.f ,-0.f }, Vector3{ 0.f,0.f,0.f });
				Cp1 = Cp0 + FMath::Random(Vector3{ -0.f,-50.f ,-0.f }, Vector3{ 0.f,0.f,0.f });
				End = Cp1 + FMath::Random(Vector3{ -0.f,-50.f ,-0.f }, Vector3{ 0.f,0.f,0.f });
			}


			const Vector3 StartRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp0 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp1 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 EndRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };

			const float RScale = FMath::Random(0.011f, 0.03f) * GScale;

			ParticleInstance::Electric _ElectricValue{};

			_ElectricValue.ColorIntencity = FMath::Random(0.44f, 1.f);
			_ElectricValue.Color = FMath::Lerp(Vector3{ 71.f / 255.f,123.f / 255.f, 220.f / 255.f }, Vector3{ 1.f,1.f,1.f }, FMath::Random(0.f, 1.f));
			const float LifeTime = FMath::Random(0.1f, 1.f);

			StartLocation = FMath::RotationVecCoord(StartLocation, { 1.f,0.f,0.f }, FMath::PI / 2.f);
			Cp0 = FMath::RotationVecCoord(Cp0, { 1.f,0.f,0.f }, FMath::PI / 2.f);
			Cp1 = FMath::RotationVecCoord(Cp1, { 1.f,0.f,0.f }, FMath::PI / 2.f);
			End = FMath::RotationVecCoord(End, { 1.f,0.f,0.f }, FMath::PI / 2.f);

			_ParticleInstance.PreSetup({ StartLocation ,Cp0,Cp1,End },
				{ StartRot,RotCp0,RotCp1,EndRot },
				{ RScale,RScale,RScale }, LifeTime, 0.0f, _ElectricValue, std::nullopt);
		}
	}
};



void PreLoader::SecretVisionDisappearParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>("..\\..\\Usable\\Smoke\\11.tga");

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "SecretVisionDisappear";
	// 공유 정보 바인드
	_PushParticle.SharedResourceBind = [_Tex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		Fx->SetTexture("MskMap", _Tex->GetTexture());
	};

	_PushParticle.InstanceBind = [](const std::any& _InstanceVariable, ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Disappear&>(_InstanceVariable);
		Fx->SetFloatArray("_Color", _Value.Color, 3u);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 15000ul;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated("SecretVisionDisappear",
			std::move(_PushParticle), PoolSize, true);

	_Info.bLocalVertexLocationsStorage = true;
	auto _TargetMesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Effect\\AirHike\\AirHike.fbx", _Info);

	if (_TargetMesh->m_spVertexLocations)
	{
		for (auto& _ParticleInstance : *ParticlePool)
		{
			const uint32 Idx = FMath::Random(0u, uint32(_TargetMesh->m_spVertexLocations->size() - 1));
			Vector3 StartLocation = (*_TargetMesh->m_spVertexLocations)[Idx];
			Vector3 Cp0 = StartLocation + FMath::RandomVector(3.3f);
			Vector3 Cp1 = Cp0 + FMath::RandomVector(30.f);
			Vector3 End = Cp1 + FMath::RandomVector(50.f);

			const Vector3 StartRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp0 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp1 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 EndRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };

			const float RScale = FMath::Random(0.015f, 0.045f) * GScale;

			ParticleInstance::Disappear _DisappearValue{};

			_DisappearValue.ColorIntencity = FMath::Random(0.44f, 1.f);
			_DisappearValue.Color = FMath::Lerp(Vector3{ 1.f,82.f / 255.f,35.f / 255.f }, Vector3{ 1.f,1.f,1.f }, FMath::Random(0.f, 1.f));
			const float LifeTime = FMath::Random(0.5f, SecretVisionDisappearParticleLifeEnd);
			static const float StartT = 0.0f;


			StartLocation = FMath::RotationVecCoord(StartLocation, { 1.f,0.f,0.f }, FMath::PI / 2.f);
			Cp0 = FMath::RotationVecCoord(Cp0, { 1.f,0.f,0.f }, FMath::PI / 2.f);
			Cp1 = FMath::RotationVecCoord(Cp1, { 1.f,0.f,0.f }, FMath::PI / 2.f);
			End = FMath::RotationVecCoord(End, { 1.f,0.f,0.f }, FMath::PI / 2.f);

			_ParticleInstance.PreSetup(
				{ StartLocation ,Cp0,Cp1,End },
				{ StartRot,RotCp0,RotCp1,EndRot },
				{ RScale,RScale,RScale },
				LifeTime,
				StartT,
				_DisappearValue,
				std::nullopt);
		}
	}
};

void PreLoader::DissolveNhDoorParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>("..\\..\\Usable\\Smoke\\11.tga");

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "DissolveNhDoor";
	// 공유 정보 바인드
	_PushParticle.SharedResourceBind = [_Tex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		Fx->SetTexture("MskMap", _Tex->GetTexture());
	};

	_PushParticle.InstanceBind = [](const std::any& _InstanceVariable, ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Disappear&>(_InstanceVariable);
		Fx->SetFloatArray("_Color", _Value.Color, 3u);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 30000ul;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated("DissolveNhDoor",
			std::move(_PushParticle), PoolSize, true);

	_Info.bLocalVertexLocationsStorage = true;
	auto _TargetMesh = Resources::Load<StaticMesh>(
		"..\\..\\Usable\\LocationMesh\\NhDoor.fbx", _Info);

	if (_TargetMesh->m_spVertexLocations)
	{
		for (auto& _ParticleInstance : *ParticlePool)
		{
			const uint32 Idx = FMath::Random(0u, uint32(_TargetMesh->m_spVertexLocations->size() - 1));
			Vector3 StartLocation = (*_TargetMesh->m_spVertexLocations)[Idx];
			Vector3 Cp0 = StartLocation + FMath::RandomVector(3.3f);
			Vector3 Cp1 = Cp0 + FMath::RandomVector(30.f);
			Vector3 End = Cp1 + FMath::RandomVector(50.f);

			const Vector3 StartRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp0 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp1 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 EndRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };

			const float RScale = FMath::Random(0.015f, 0.045f) * GScale;

			ParticleInstance::Disappear _DisappearValue{};

			_DisappearValue.ColorIntencity = FMath::Random(0.44f, 1.f);
			_DisappearValue.Color = FMath::Lerp(Vector3{ 14.f / 255.f,1.f,0.f }, Vector3{ 1.f,1.f,1.f }, FMath::Random(0.f, 1.f));
			const float LifeTime = FMath::Random(0.5f, 1.33f);
			static const float StartT = 0.0f;

			/*StartLocation = FMath::RotationVecCoord(StartLocation, { 1.f,0.f,0.f }, FMath::PI / 2.f);
			Cp0 = FMath::RotationVecCoord(Cp0, { 1.f,0.f,0.f }, FMath::PI / 2.f);
			Cp1 = FMath::RotationVecCoord(Cp1, { 1.f,0.f,0.f }, FMath::PI / 2.f);
			End = FMath::RotationVecCoord(End, { 1.f,0.f,0.f }, FMath::PI / 2.f);*/

			_ParticleInstance.PreSetup(
				{ StartLocation ,Cp0,Cp1,End },
				{ StartRot,RotCp0,RotCp1,EndRot },
				{ RScale,RScale,RScale },
				LifeTime,
				StartT,
				_DisappearValue,
				std::nullopt);
		}
	};
}
void PreLoader::SVMCParticleEndPoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>("..\\..\\Usable\\Smoke\\11.tga");

	_PushParticle.bLerpTimeNormalized = false;
	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "AirHikeParticle";
	// 공유 정보 바인드
	_PushParticle.SharedResourceBind = [_Tex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		Fx->SetTexture("MskMap", _Tex->GetTexture());
	};

	_PushParticle.InstanceBind = [](const std::any& _InstanceVariable, ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Electric&>(_InstanceVariable);
		Fx->SetFloatArray("_Color", _Value.Color, 3u);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	
	const uint64 PoolSize = 2000ul;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated("SVMCEndParticle",
			std::move(_PushParticle), PoolSize, true);

	_Info.bLocalVertexLocationsStorage = true;
	auto _TargetMesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Effect\\AirHike\\AirHike.fbx", _Info);

	if (_TargetMesh->m_spVertexLocations)
	{
		for (auto& _ParticleInstance : *ParticlePool)
		{
			const uint32 Idx = FMath::Random(0u, uint32(_TargetMesh->m_spVertexLocations->size() - 1));

			Vector3 StartLocation =
				(*_TargetMesh->m_spVertexLocations)[Idx];

			const Vector3 Dir = FMath::Normalize(StartLocation);
			Vector3 Cp0{};
			Vector3 Cp1{};
			Vector3 End{};

			{
				Cp0 = StartLocation + Dir * FMath::Random(0.f, 2.f);
				Cp1 = Cp0 + Dir * FMath::Random(0.f, 2.f);
				End = Cp1 + Dir * FMath::Random(0.f, 2.f);
			}

			const Vector3 StartRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp0 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp1 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 EndRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };

			const float RScale = FMath::Random(0.077f * 0.5f, 0.05f) * GScale;

			ParticleInstance::Electric _ElectricValue{};

			_ElectricValue.ColorIntencity = FMath::Random(0.44f, 1.f);
			_ElectricValue.Color = FMath::Lerp(Vector3{ 191.f / 255.f, 20.f / 255.f, 7.f / 255.f }, 
				Vector3{ 253.f / 255.f, 74.f / 255.f, 31.f / 255.f }, 
				FMath::Random(0.f, 1.f));
			const float LifeTime = FMath::Random(0.1f, 1.f);

			StartLocation = FMath::RotationVecCoord(StartLocation, { 1.f,0.f,0.f }, FMath::PI / 2.f);
			Cp0 = FMath::RotationVecCoord(Cp0, { 1.f,0.f,0.f }, FMath::PI / 2.f);
			Cp1 = FMath::RotationVecCoord(Cp1, { 1.f,0.f,0.f }, FMath::PI / 2.f);
			End = FMath::RotationVecCoord(End, { 1.f,0.f,0.f }, FMath::PI / 2.f);

			_ParticleInstance.PreSetup(
				{ StartLocation ,Cp0,Cp1,End },
				{ StartRot,RotCp0,RotCp1,EndRot },
				{ RScale,RScale,RScale }, LifeTime, 0.0f, _ElectricValue, std::nullopt);
		}
	}
}

void PreLoader::DissolveParticlePoolLoad(
	const std::filesystem::path& _Path,
	const Vector3& StartColor )
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>("..\\..\\Usable\\Smoke\\11.tga");

	_PushParticle.bLerpTimeNormalized = false;

	// Particle 정보 채워주기 
	_PushParticle._ShaderKey = "DissolveParticle";

	// 공유 정보 바인드
	_PushParticle.SharedResourceBind = [_Tex](
		ENGINE::ParticleSystem::Particle& TargetParticle,
		ID3DXEffect* const Fx)
	{
		Fx->SetTexture("MskMap", _Tex->GetTexture());
	};

	_PushParticle.InstanceBind = [](const std::any& _InstanceVariable, ID3DXEffect* const Fx)
	{
		const auto& _Value = std::any_cast<const ParticleInstance::Dissolve&>(_InstanceVariable);
		Fx->SetFloatArray("_Color", _Value.Color, 3u);
		Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
		return;
	};

	const uint64 PoolSize = 30000u;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated(_Path.string(),
			std::move(_PushParticle), PoolSize, true);

	_Info.bLocalVertexLocationsStorage = true;
	auto _TargetMesh = Resources::Load<StaticMesh>(_Path, _Info);

	if (_TargetMesh->m_spVertexLocations)
	{
		for (auto& _ParticleInstance : *ParticlePool)
		{
			const uint32 Idx = FMath::Random(0u, uint32(_TargetMesh->m_spVertexLocations->size() - 1));
			Vector3 StartLocation = (*_TargetMesh->m_spVertexLocations)[Idx];

			 Vector3 Cp0 = StartLocation + FMath::RandomVector(3.3f);
			  Vector3 Cp1 = Cp0 + FMath::RandomVector(30.f);
			  Vector3 End = Cp1 + FMath::RandomVector(50.f); 


			const Vector3 StartRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp0 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 RotCp1 = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };
			const Vector3 EndRot = Vector3{ 0.f,0.f,FMath::Random(0.0f,FMath::PI) };

			const float RScale = FMath::Random(0.015f, 0.045f) * GScale;

			ParticleInstance::Dissolve _DisappearValue{};

			_DisappearValue.ColorIntencity = FMath::Random(0.44f, 1.f);
			_DisappearValue.Color = FMath::Lerp(
				StartColor, Vector3{ 1.f,1.f,1.f }, FMath::Random(0.f, 1.f));
			const float LifeTime = FMath::Random(0.5f, 1.33f);
			static const float StartT = 0.0f;

			Cp0.y += FMath::Random(0.f, 40.f);
			Cp1.y += FMath::Random(0.f, 400.f);
			End.y += FMath::Random(0.f, 1000.f);

			_ParticleInstance.PreSetup(
				{ StartLocation ,Cp0,Cp1,End },
				{ StartRot,RotCp0,RotCp1,EndRot },
				{ RScale,RScale,RScale },
				LifeTime,
				StartT,
				_DisappearValue,
				std::nullopt);
		}
	};
};

