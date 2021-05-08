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

	const uint64 PoolSize = 1000;

	auto* const ParticlePool =
		ParticleSystem::GetInstance()->PreGenerated("IceCbsMid", std::move(_PushParticle), PoolSize);

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
		static const float ParticleScale = FMath::Random(0.018f,0.036f) * GScale;

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
			{ ParticleScale,ParticleScale,ParticleScale }, 
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
		ParticleSystem::GetInstance()->PreGenerated("Ice", std::move(_PushParticle), PoolSize);

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
		const float PScale = FMath::Random(ScaleRange.x, ScaleRange.y) * GScale;

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
			{ PScale,PScale,PScale }, LifeTime, 0.0f, _IceValue, std::nullopt);
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
	ParticleSystem::GetInstance()->PreGenerated("Ice", std::move(_PushParticle), PoolSize);
}

