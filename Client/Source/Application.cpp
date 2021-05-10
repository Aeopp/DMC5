#include "stdafx.h"
#include "Application.h"

#include "TestScene.h"
#include "ParticleSystem.h"
#include "ParticleInstanceDesc.hpp"
#include "Subset.h"

Application::Application()
{

}

HRESULT Application::ReadyApplication(const bool bWindowed,
	const bool bMultiSample)
{
	if (FAILED(Engine::ReadyEngine(bWindowed, bMultiSample)))
		return E_FAIL;

	StaticResourceLoad();

	SceneManager::LoadScene(TestScene::Create());
	return S_OK;
}

HRESULT Application::UpdateApplication(const float Delta)
{
	if (FAILED(Engine::UpdateEngine(Delta)))
		return E_FAIL;
	return S_OK;
}

HRESULT Application::ReleaseApplication()
{
	if (FAILED(Engine::ReleaseEngine()))
		return E_FAIL;

	return S_OK;
}

void Application::StaticResourceLoad()
{
	ParticlePoolLoad();

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
};

void Application::ParticlePoolLoad()
{
	IceParticlePoolLoad();
};

// IceCbsMid;

void Application::IceParticlePoolLoad()
{
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

	ElectricVoertexParticleLoad();
};

void Application::IceCbsMidParticlePoolLoad()
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
		ParticleSystem::GetInstance()->PreGenerated("IceCbsMid", std::move(_PushParticle), PoolSize,false);

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

void Application::IceAgeParticlePoolLoad()
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

	const uint64 PoolSize = 1000;

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

void Application::FireParticlePoolLoad()
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
};

void Application::ElectricEffectParticlePoolLoad(
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

void Application::ElectricParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);
	
	auto _Tex = Resources::Load<Texture>("..\\..\\Usable\\Electric\\mesh_03_magic_electric01_00_00_ALBA.dds");

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

void Application::ElectricVoertexParticleLoad()
{

	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>("..\\..\\Usable\\Electric\\mesh_03_magic_electric01_00_00_ALBA.dds");

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

			const uint32 TargetIdx = (Idx % 3);

			/*if (TargetIdx == 0)
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
			}*/

			if (TargetIdx == 0)
			{
				Cp0 = StartLocation + FMath::RandomVector(FMath::Random(0.f, 0.f));
				Cp1 = Cp0 + FMath::RandomVector(FMath::Random(0.f, 0.f));
				End = Cp1 + FMath::RandomVector(FMath::Random(0.f, 0.f));
			}
			else if (TargetIdx == 1)
			{
				Cp0 = StartLocation + FMath::RandomVector(FMath::Random(0.f, 0.f));
				Cp1 = Cp0 + FMath::RandomVector(FMath::Random(0.f, 0.f));
				End = Cp1 + FMath::RandomVector(FMath::Random(0.f, 0.f));
			}
			else
			{
				Cp0 = StartLocation + Dir * FMath::Random(0.f, 0.f);
				Cp1 = Cp0 + Dir * FMath::Random(0.f, 0.f);
				End = Cp1 + Dir * FMath::Random(0.f, 0.f);
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

void Application::ThunderBoltParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>("..\\..\\Usable\\Electric\\mesh_03_magic_electric01_00_00_ALBA.dds");

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

	const uint64 PoolSize = 250ul;

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

void Application::ThunderBoltEndParticlePoolLoad()
{
	ENGINE::ParticleSystem::Particle _PushParticle{};

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = false;
	_PushParticle._Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _Info);

	auto _Tex = Resources::Load<Texture>("..\\..\\Usable\\Electric\\mesh_03_magic_electric01_00_00_ALBA.dds");

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

