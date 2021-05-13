#include "stdafx.h"
#include "..\Header\IceAge.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "GraphicSystem.h"
#include "RedQueen.h"
#include "ParticleSystem.h"
#include "ParticleInstanceDesc.hpp"

IceAge::IceAge()
{};

void IceAge::Free()
{
	GameObject::Free();
};

std::string IceAge::GetName()
{
	return "IceAge";
};

IceAge* IceAge::Create()
{
	return new IceAge{};
};

void IceAge::RenderReady()
{
	// �̹� ���ؽ� ��ü�� ���� ��ġ�� . 
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const Vector3 Scale = _SpTransform->GetScale();
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
		if (Inner)
		{
			const uint32  Numsubset = Inner->GetNumSubset();
			_RenderUpdateInfo.SubsetCullingSphere.resize(Numsubset);

			for (uint32 i = 0; i < Numsubset; ++i)
			{
				const auto& _Subset = Inner->GetSubset(i);
				const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

				_RenderUpdateInfo.SubsetCullingSphere[i] = _CurBS.Transform(_RenderUpdateInfo.World, Scale.x);
			}
		}
	}
};

void IceAge::RenderInit()
{
	m_nTag = Eff_IceAge;
	// ������ �����ؾ��ϴ� ������Ʈ��� (�������� ��� ����) �˸�.
	// ���� �������̽� ��ӹ��� �ʾҴٸ� Ű��������.
	SetRenderEnable(true);

	// ���� ���� �ʱ�ȭ ...
	ENGINE::RenderProperty _InitRenderProp;
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
	// ���� �Ӽ� ��ü �ʱ�ȭ 
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
	_InitRenderProp.bRender = false;

	_InitRenderProp.RenderOrders[RenderProperty::Order::Debug]=
	{
		{"Debug" ,[this](const DrawInfo& _Info)
			{
				RenderDebug(_Info);
			}
	} };

	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{
		{
			"IceAge",[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		}

	};

	RenderInterface::Initialize(_InitRenderProp);

	// �޽�
	{
		Mesh::InitializeInfo _Info{};
		_Info.bLocalVertexLocationsStorage = true;
		Inner = Resources::Load<StaticMesh>(
			"..\\..\\Resource\\Mesh\\Static\\Primitive\\nsg.fbx" , _Info);
	}

	// �ؽ��� 
	Albedo = Resources::Load<Texture>(
		"..\\..\\Resource\\Texture\\Effect\\mesh_03_debris_ice00_00_albm.tga");
	TrailMap = Resources::Load<Texture>(
		"..\\..\\Usable\\tex_03_common_000_0002_alpg.tga");
	EmssiveMskMap = Resources::Load<Texture>(
		"..\\..\\Resource\\Texture\\Effect\\emissive_msk.tga");
	NoiseMap = Resources::Load<Texture>(
		"..\\..\\Resource\\Texture\\Effect\\water_new_height.png");

	PushEditEntity(Inner.get());
	PushEditEntity(Albedo.get());
	PushEditEntity(TrailMap.get());
	PushEditEntity(EmssiveMskMap.get());
	PushEditEntity(NoiseMap.get());

	NoiseScale = { 0.005f,0.02f,0.0f};
	NoiseScrollSpeed = { 0.0005f,0.005f,0.00f};
	EditRotationSpeed = 1000.f;
	RollRotationSpeed = FMath::PI;
	EmissiveIntencity = 0.01f;
	ColorIntencity = 0.059f;
	DistortionIntencity = 1.2f;
};

void IceAge::PlayStart(
	const std::optional<Vector3>& Location,
	const Vector3 Scale,
	const float RollRotateSpeed,
	const float PlayTime)
{
	Vector3 MyLocation = { 0,0,0 };

	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		MyLocation = SpTransform->GetPosition();

		if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
			SpTransform)
		{
			SpTransform->SetScale(Scale);
			SpTransform->SetPosition(Location.value()); 
			// ��� ..
		}
	}

	CurParticleTime = ParticleCycle;
	this->PlayTime = PlayTime;
	this->RollRotationSpeed = RollRotateSpeed;
	_RenderProperty.bRender = true;
	T = 0.0f;
};



void IceAge::PlayEnd()
{
	_RenderProperty.bRender = false;
	T = 0.0f;
};

void IceAge::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("matWorld", &_RenderUpdateInfo.World);

	_Info.Fx->SetFloat("ColorIntencity", ColorIntencity);
	_Info.Fx->SetFloat("EmissiveIntencity", EmissiveIntencity);
	_Info.Fx->SetFloat("DistortionIntencity", DistortionIntencity);

	const float AlphaFactor = std::clamp(PlayTime - T, 0.0f, 1.f);

	_Info.Fx->SetFloat("AlphaFactor", AlphaFactor);
	_Info.Fx->SetFloatArray("NoiseScale", NoiseScale, 3u);
	const Vector3 Speed = NoiseScrollSpeed * T;
	_Info.Fx->SetFloatArray("NoiseScrollSpeed", Speed, 3u);

	_Info.Fx->SetFloatArray("NoiseDistortion0", NoiseDistortion0, 2u);
	_Info.Fx->SetFloatArray("NoiseDistortion1", NoiseDistortion1, 2u);
	_Info.Fx->SetFloatArray("NoiseDistortion2", NoiseDistortion2, 2u);
	_Info.Fx->SetTexture("NoiseMap", NoiseMap->GetTexture());

	_Info.Fx->SetTexture("AlbedoMap", Albedo->GetTexture());
	_Info.Fx->SetTexture("TrailMap", TrailMap->GetTexture());
	_Info.Fx->SetTexture("EmissiveMskMap", EmssiveMskMap->GetTexture());

	{
		const uint32 Numsubset = Inner->GetNumSubset();
		for (uint32 i = 0; i < Numsubset; ++i)
		{
			if (auto SpSubset = Inner->GetSubset(i).lock();
				SpSubset)
			{
				if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
				{
					continue;
				}

				SpSubset->Render(_Info.Fx);
			};
		};
	}
};



void IceAge::RenderDebug(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("World", &_RenderUpdateInfo.World);

	{
		const uint32 Numsubset = Inner->GetNumSubset();
		for (uint32 i = 0; i < Numsubset; ++i)
		{
			if (auto SpSubset = Inner->GetSubset(i).lock();
				SpSubset)
			{
				if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
				{
					continue;
				}

				SpSubset->Render(_Info.Fx);
			};
		};
	}
};


HRESULT IceAge::Ready()
{
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetPosition(Vector3{ 0.f,0.f,0.f });
	InitTransform.lock()->SetScale( Vector3{ 0.005f,0.005f,0.005f } );
	InitTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	return S_OK;
};

HRESULT IceAge::Awake()
{
	GameObject::Awake();
	m_pTransform.lock()->SetPosition(Vector3{ 0.f,0.f,0.f });
	m_pTransform.lock()->SetScale(Vector3{ 0.005f,0.005f,0.005f });
	m_pTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	return S_OK;
}

HRESULT IceAge::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT IceAge::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);

	if (_RenderProperty.bRender == false) return 0;

	T += _fDeltaTime;

	if (T > PlayTime)
	{
		PlayEnd();
		return 0;
	}

	CurParticleTime -= _fDeltaTime;

	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		const Vector3 CurRotation = Vector3{ 0.f,RollRotationSpeed * _fDeltaTime , 0.f };
		SpTransform->Rotate(CurRotation);

		if (CurParticleTime < 0.0f)
		{
			CurParticleTime += ParticleCycle;

			const Matrix Mat = SpTransform->GetRenderMatrix();
			const uint32 RangeEnd = Inner->m_spVertexLocations->size() - 1u;

			{
				auto _PlayableParticle = ParticleSystem::GetInstance()->
					PlayParticle("Ice",500u, true);
				for (int32 i = 0; i < _PlayableParticle.size(); 
					++i)
				{
					auto& _PlayInstance = _PlayableParticle[i];
					_PlayInstance->PlayDescBind(SpTransform->GetRenderMatrix());
				}
			}
		}
	}


	return 0;
}

UINT IceAge::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
};

void IceAge::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		const std::string ChildName = GetName() + "_Play";
		ImGui::BeginChild(ChildName.c_str());
		{
			if (ImGui::SmallButton("Play"))
			{
				Vector3 Point{ 0.f,0.f ,0.f };
				if (auto SpTransform = m_pTransform.lock();
					SpTransform)
				{
					Point = SpTransform->GetPosition();
				};

				PlayStart(Point, Vector3{ 0.005f ,0.005f ,0.005f }, EditRotationSpeed ,EditPlayTime);
			}
			if (ImGui::SmallButton("PlayEnd"))
			{
				PlayEnd();
			}

			ImGui::SliderFloat3("NoiseScrollSpeed", NoiseScrollSpeed, FLT_MIN, 10.f, "%9.6f");
			ImGui::InputFloat3("In NoiseScrollSpeed", NoiseScrollSpeed, "%9.6f");

			ImGui::SliderFloat3("NoiseScale", NoiseScale, FLT_MIN, 10.f, "%9.6f");
			ImGui::InputFloat3("In NoiseScale", NoiseScale, "%9.6f");


			ImGui::SliderFloat("EditPlayTime", &EditPlayTime, FLT_MIN, 10.f, "%9.6f");
			ImGui::InputFloat("In EditPlayTime", &EditPlayTime, 0.f, 0.f, "%9.6f");

			ImGui::SliderFloat("EditPlayRollRotateSpeed", &EditRotationSpeed, FLT_MIN, 10000.f, "%9.6f");
			ImGui::InputFloat("In EditPlayRollRotateSpeed", &EditRotationSpeed,0.f,0.f ,"%9.6f");

			ImGui::SliderFloat("DistortionIntencity", &DistortionIntencity, FLT_MIN, 10000.f, "%9.6f");
			ImGui::InputFloat("In DistortionIntencity", &DistortionIntencity, 0.f, 0.f, "%9.6f");

			ImGui::SliderFloat("EmissiveIntencity", &EmissiveIntencity, FLT_MIN, 10000.f, "%9.6f");
			ImGui::InputFloat("In EmissiveIntencity", &EmissiveIntencity, 0.f,0.f,"%9.6f");

			ImGui::SliderFloat("ColorIntencity", &ColorIntencity, FLT_MIN, 10000.f, "%9.6f");
			ImGui::InputFloat("In ColorIntencity", &ColorIntencity, 0.f, 0.f, "%9.6f");

		}
		ImGui::EndChild();
	}
};

void IceAge::OnEnable()
{
	GameObject::OnEnable();
};

void IceAge::OnDisable()
{
	GameObject::OnDisable();
};



