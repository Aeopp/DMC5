#include "stdafx.h"
#include "..\Header\DissolveInfo.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "RenderInterface.h"
#include "Renderer.h"
#include "ParticleSystem.h"
#include "PreLoader.h"

void DissolveInfo::Initialize(const std::filesystem::path& MeshPath,
	const Vector3& _Color)
{
	_DissolveMap = Resources::Load<ENGINE::Texture>(
		"..\\..\\Usable\\mesh_03_cs_noise_00_01.tga");

	_BurnMap = Resources::Load<ENGINE::Texture>("..\\..\\Usable\\burn.tga");
	DissolveParticleName = MeshPath.string();
	PreLoader::DissolveParticlePoolLoad(DissolveParticleName, _Color);
};

void DissolveInfo::DissolveStart()
{
	bDissolve = true; 
	SliceAmount = 0.0f;
	CurDissolveParticleDelta = 0.0f;
}

void DissolveInfo::DissolveEnd()
{
	bDissolve = false;
}

void DissolveInfo::DissolveVariableBind(ID3DXEffect* const Fx)
{
	if (bDissolve)
	{
		Fx->SetTexture("DissolveMap", _DissolveMap->GetTexture());
		Fx->SetTexture("BurnMap", _BurnMap->GetTexture());
		Fx->SetFloatArray("BurnColor", BurnColor, 3u);
		Fx->SetFloat("BurnSize", BurnSize);
		Fx->SetFloat("EmissionAmount", EmissionAmount);
		Fx->SetFloat("SliceAmount", SliceAmount);
	}
}

void DissolveInfo::DissolveUpdate(const float DeltaTime , const Matrix& RenderMatrix)
{
	if (bDissolve)
	{
		this->RenderMatrix = RenderMatrix;

		CurDissolveParticleDelta -= DissolveParticleDelta;

		if (CurDissolveParticleDelta < 0.0f)
		{
			CurDissolveParticleDelta += DissolveParticleDelta;
			DissolveParticle();
		}

		SliceAmount += DeltaTime * DissolveAcc;

		if (SliceAmount > 1.f)
		{
			DissolveEnd();
		}
	}
}

void DissolveInfo::DissolveEditor()
{
	if (ImGui::Button("Dissolve Play"))
	{
		DissolveStart();
	}

	if (ImGui::Button("Dissolve Particle Play"))
	{
		DissolveParticle();
	}

	ImGui::SliderFloat("SliceAmount", &SliceAmount, 0.0f, 10.f);
	ImGui::SliderFloat("BurnSize", &BurnSize, 0.0f, 10.f);
	ImGui::SliderFloat("EmissionAmount", &EmissionAmount, 0.0f, 100.f);
	ImGui::ColorEdit3("BurnColor", BurnColor);
};

void DissolveInfo::DissolveParticle()
{
	if (auto _Particle =
		ParticleSystem::GetInstance()->PlayParticle(
			DissolveParticleName, DissolveParticleCount, true);
		_Particle.empty() == false)
	{
		for (int32 i = 0; i < _Particle.size(); ++i)
		{
			auto& _PlayInstance = _Particle[i];
			_PlayInstance->PlayDescBind(RenderMatrix);
		}
	}
}
