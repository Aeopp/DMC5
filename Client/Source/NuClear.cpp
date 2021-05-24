#include "stdafx.h"
#include "..\Header\NuClear.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "ParticleSystem.h"
#include "NuclearLensFlare.h"

void NuClear::Free()
{
	GameObject::Free();
};

std::string NuClear::GetName()
{
	return "NuClear";
};

NuClear* NuClear::Create()
{
	return new NuClear{};
};

void NuClear::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();

	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const float LerpT = FMath::Clamp(T / GrowEndT, 0.0f, 1.f);
		const float CurScale = FMath::Lerp(0.0f, GrowEndScale, LerpT);

		_SpTransform->SetScale(
			Vector3{ CurScale ,CurScale ,CurScale });
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
	}
};

void NuClear::RenderInit()
{
	m_nTag = Eff_NuClear;
	// ������ �����ؾ��ϴ� ������Ʈ��� (�������� ��� ���� ) �˸�.
	// ���� �������̽� ��ӹ��� �ʾҴٸ� Ű��������.
	SetRenderEnable(true);

	// ���� ���� �ʱ�ȭ ...
	ENGINE::RenderProperty _InitRenderProp;
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
	// ���� �Ӽ� ��ü �ʱ�ȭ 
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
	_InitRenderProp.bRender = false;

	_InitRenderProp.RenderOrders[RenderProperty::Order::Debug]
		=
	{
		{"Debug" ,
		[this](const DrawInfo& _Info)
		{
			RenderDebug(_Info);
		}
	} };


	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{

		{"NuClear",
		[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		}
	};

	RenderInterface::Initialize(_InitRenderProp);

	// 
	// ����ƽ �޽� �ε�
	Mesh::InitializeInfo _InitInfo{};
	_InitInfo.bLocalVertexLocationsStorage = false;

	_Mesh = Resources::Load<ENGINE::StaticMesh>
		(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\sphere00.fbx", _InitInfo);

	_LightMsk = Resources::Load<ENGINE::Texture>(
		L"..\\..\\Usable\\Artemis\\LightSphere.tga"
		);

	_AlbmMap = Resources::Load<ENGINE::Texture>(
		"..\\..\\Resource\\Texture\\Effect\\lightning_alb.png"
		);

	PushEditEntity(_Mesh.get());
	
	_NuclearLensFlare = AddGameObject<NuclearLensFlare>();
		

	_DynamicLight.Color = 
	{
		Vector4 
		{108.f/255.f, 82.f/255.f, 158.f/255.f ,1.f},
		Vector4 
		{194.f/255.f, 181.f/255.f, 247.f/255.f ,1.f}
	};

	_DynamicLight.Flux = 
	{
		ExplosionReadyFluxLow,ExplosionReadyFluxHigh
	};

	_DynamicLight.PointRadius=
	{
		ExplosionReadyRadiusLow,ExplosionReadyRadiusHigh
	};
};

void NuClear::PlayStart(const Vector3& Location)
{
	if (auto SpTransform = GetComponent<Transform>().lock();
		SpTransform)
	{
		if (Location)
		{
			SpTransform->SetPosition(Location);
			_DynamicLight.PlayStart(Location, GrowEndT + ExplosionReadyTime + ExplosionTime);
			_NuclearLensFlare.lock()->UpdatePlayVariable(SpTransform->GetScale().x, SpTransform->GetPosition());
		}
	}

	T = 0.0f;
	_RenderProperty.bRender = true;
	CurParticleDelta = 0.0f;

	PlayParticle();
};

void NuClear::PlayEnd()
{
	_RenderProperty.bRender = false;
	T = 0.0f;
	_DynamicLight.PlayEnd();
};
void NuClear::ParticleUpdate(const float DeltaTime)
{
	if (T > ExplosionReadyTime)
		return;

	CurParticleDelta -= DeltaTime;
	if (CurParticleDelta < 0.0f)
	{
		CurParticleDelta += ParticleDelta;
		PlayParticle();
	}
}
void NuClear::PlayParticle()
{
	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		if (auto _Particle =
			ParticleSystem::GetInstance()->PlayParticle(
				"NuClearParticle", 22u, true);
			_Particle.empty() == false)
		{

			for (int32 i = 0; i < _Particle.size(); ++i)
			{
				auto& _PlayInstance = _Particle[i];
				_PlayInstance->PlayDescBind(SpTransform->GetRenderMatrix());
			}
		}
	};
};

void NuClear::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	const uint32 Numsubset = _Mesh->GetNumSubset();

	if (Numsubset > 0)
	{
		_Info.Fx->SetMatrix("matWorld", &World);
		_Info.Fx->SetTexture("LightMskMap", _LightMsk->GetTexture());
		_Info.Fx->SetTexture("AlbmMap", _AlbmMap->GetTexture());

		const float LerpT = 
			FMath::Clamp(T / GrowEndT, 0.0f, 1.f);

		const float CurColor =
			FMath::Lerp(0.0f, ColorIntencity, LerpT);

		_Info.Fx->SetFloat("ColorIntencity", CurColor);
		_Info.Fx->SetFloat("AlphaFactor", LerpT);
	}

	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset =
			_Mesh->GetSubset(i).lock();
			SpSubset)
		{

			SpSubset->Render(_Info.Fx);
		};
	};
}


void NuClear::RenderDebug(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);

	const uint32 Numsubset = _Mesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = _Mesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};

};


HRESULT NuClear::Ready()
{
	// Ʈ������ �ʱ�ȭ .. 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	// �������� ������ �ް���� ������Ʈ�� Raw �����ͷ� Ǫ��.
	return S_OK;
};

HRESULT NuClear::Awake()
{
	GameObject::Awake();

	m_pTransform.lock()->SetScale({ 0.0020f,0.0020f,0.0020f });
	m_pTransform.lock()->SetPosition(Vector3{ -37.411f,0.821f,30.663f });
	m_pTransform.lock()->SetRotation(Vector3{ 0.0f,0.f ,0.0f });

	return S_OK;
}

HRESULT NuClear::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT NuClear::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	if (_RenderProperty.bRender == false) return 0;

	T += _fDeltaTime;

	// ���� ���� .
	if (T > PlayTime)
	{
		PlayEnd();
	};

	ParticleUpdate(_fDeltaTime);

	_DynamicLight.Flux = {FluxLow,FluxHigh};
	_DynamicLight.PointRadius = { RadiusLow,RadiusHigh };

	_DynamicLight.Update(
		_fDeltaTime, 
		GetComponent<Transform>().lock()->GetPosition());

	return 0;
}


UINT NuClear::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
}

void NuClear::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		const std::string ChildName = GetName() + "_Play";
		ImGui::BeginChild(ChildName.c_str());
		if (ImGui::SmallButton("Play"))
		{
			PlayStart(
				GetComponent<Transform>().lock()->GetPosition());
		}

		ImGui::Text("T : %2.6f", T);

		ImGui::SliderFloat("ParticleDelta", &ParticleDelta, 0.0f, 1.f);
		ImGui::SliderFloat("PlayTime", &PlayTime, 0.0f, 10.f);
		ImGui::SliderFloat("GrowEndT", &GrowEndT, 0.0f, PlayTime);

		ImGui::SliderFloat("ColorIntencity", 
			&ColorIntencity, 0.0f, 10.f);
		ImGui::SliderFloat("GrowEndScale", &GrowEndScale, 0.0f, 1.f);

		ImGui::SliderFloat("FluxLow",
			&FluxLow, 0.0f, 50.f );
		ImGui::SliderFloat("FluxHigh", 
			&FluxHigh, 0.0f, 50.f);

		ImGui::SliderFloat("RadiusLow",
			&RadiusLow, 0.0f, 10.f);

		ImGui::SliderFloat("RadiusHigh",
			&RadiusHigh, 0.0f, 10.f);

		 
		ImGui::SliderFloat("ParticleEndT", 
			&ParticleEndT, 0.0f, PlayTime);

		ImGui::EndChild();
	}
}


void NuClear::OnEnable()
{
	GameObject::OnEnable();
}

void NuClear::OnDisable()
{
	GameObject::OnDisable();
}



