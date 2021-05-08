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
}



void Application::ParticlePoolLoad()
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

	const uint64 PoolSize = 10000;
	ParticleSystem::GetInstance()->PreGenerated("Ice", std::move(_PushParticle), PoolSize);


}
