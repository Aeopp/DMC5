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
	const Vector3& _Color ,
	const Vector3 BurnColor ,
	const float BurnSize )
{
	_DissolveMap = Resources::Load<ENGINE::Texture>(
		"..\\..\\Usable\\mesh_03_cs_noise_00_01.tga");

	_BurnMap = Resources::Load<ENGINE::Texture>("..\\..\\Usable\\burn.tga");
	DissolveParticleName = MeshPath.string();
	PreLoader::DissolveParticlePoolLoad(DissolveParticleName, _Color);
	this->BurnColor = BurnColor;
	this->BurnSize = BurnSize;
};

void DissolveInfo::DissolveStart(const bool bReverse,
	const bool bParticle,
	const float DissolveTime)
{
	if (!bDissolve)
	{
		const float DissolveAcc = (1.f / DissolveTime);
		bDissolve = true;
		this->bParticle = bParticle;
		this->bReverse = bReverse;
		CurDissolveParticleDelta = 0.0f;

		if (bReverse == false)
		{
			SliceAmount = 0.0f;
			this->DissolveAcc = DissolveAcc;
		}
		else
		{
			SliceAmount = 1.0f;
			this->DissolveAcc = (std::fabsf(DissolveAcc) * -1.f);
		}
	}
};

void DissolveInfo::DissolveEnd()
{
	bDissolve = false;

	if (bReverse == false)
	{
		SliceAmount = 1.1f;
	}
	else
	{
		SliceAmount = 0.0f;
	};
};

void DissolveInfo::DissolveVariableBind(ID3DXEffect* const Fx)
{
	// if (bDissolve)
	{
		Fx->SetTexture("DissolveMap", _DissolveMap->GetTexture());
		Fx->SetTexture("BurnMap", _BurnMap->GetTexture());
		Fx->SetFloatArray("BurnColor", BurnColor, 3u);
		Fx->SetFloat("BurnSize", BurnSize);
		Fx->SetFloat("EmissionAmount", EmissionAmount);
		Fx->SetFloat("SliceAmount", SliceAmount);
	}
}

bool DissolveInfo::DissolveUpdate(const float DeltaTime,const Matrix& RenderMatrix)
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

		if (false == FMath::IsRange(0.0f, 1.f, SliceAmount))
		{
			DissolveEnd();
			return true;
		}
	}

	return false;
}

void DissolveInfo::DissolveEditor()
{
	if (ImGui::Button("Dissolve Play"))
	{
		DissolveStart(bEditDissolveReverse, bEditDissolveParticle, EditDissolveTime);
	}

	if (ImGui::Button("Dissolve Particle Play"))
	{
		DissolveParticle();
	}

	ImGui::SliderFloat("EditDissolveTime", &EditDissolveTime,0.0f,1.f);
	ImGui::Checkbox("bEditDissolveParticle", &bEditDissolveParticle);
	ImGui::Checkbox("bEditDissolveReverse", &bEditDissolveReverse);

	ImGui::SliderFloat("SliceAmount", &SliceAmount, 0.0f, 10.f);
	ImGui::SliderFloat("BurnSize", &BurnSize, 0.0f, 10.f);
	ImGui::SliderFloat("EmissionAmount", &EmissionAmount, 0.0f, 100.f);
	ImGui::ColorEdit3("BurnColor", BurnColor);
};

bool DissolveInfo::IsDissolve()const
{
	return bDissolve;
};

void DissolveInfo::DissolveParticle()
{
	if (bParticle)
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
}
