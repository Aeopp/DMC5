#include "stdafx.h"
#include "..\Header\Energism.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "ParticleSystem.h"
#include "NuclearLensFlare.h"
#include "ShockWave.h"
#include "LensFlare.h"

void Energism::Free()
{
	GameObject::Free();
};

std::string Energism::GetName()
{
	return "Energism";
};

Energism* Energism::Create()
{
	return new Energism{};
};

void Energism::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();

	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
	}
};

void Energism::RenderInit()
{
	m_nTag = Eff_Energism;
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

		{"Energism",
		[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		}
	};

	_InitRenderProp.RenderOrders[RenderProperty::Order::Collider]
		=
	{
		{"Debug" ,
		[this](const DrawInfo& _Info)
		{
			DrawCollider(_Info);
		}
	} };

	RenderInterface::Initialize(_InitRenderProp);

	// 
	// ����ƽ �޽� �ε�
	Mesh::InitializeInfo _InitInfo{};
	_InitInfo.bLocalVertexLocationsStorage = false;

	_Mesh = Resources::Load<ENGINE::StaticMesh>
		(L"..\\..\\Usable\\Artemis\\Energism.fbx", _InitInfo);


	_AlbmMap =  Resources::Load<ENGINE::Texture>(
		"..\\..\\Resource\\Texture\\Effect\\lightning_alb.png"
		);

	_NoiseMap = Resources::Load<ENGINE::Texture>(
		"..\\..\\Usable\\Artemis\\1.tga"
		);

	PushEditEntity(_Mesh.get());

	for (auto& _Reverberation : ReverberationArr)
	{
		_Reverberation = AddGameObject<Reverberation>();
	}

	_ShockWave = AddGameObject<ShockWave>();
	_LensFlare = AddGameObject<LensFlare>();
};

void Energism::PlayStart(const Vector3& Location, const float Yaw)
{
	if (auto SpTransform = GetComponent<Transform>().lock();
		SpTransform)
	{
		if (Location)
		{
			SpTransform->SetPosition(Location);
			SpTransform->SetRotation(Vector3{ 0.f,Yaw,0.f });
			_ShockWave.lock()->PlayStart(Location,
				ShockWave::Option::ArtemisEnergism);
			_LensFlare.lock()->Option = 1;
			_LensFlare.lock()->PlayStart(Location);
		}
	}

	T = 0.0f;
	_RenderProperty.bRender = true;
};

void Energism::UpdateVariable(const Vector3& Location , const float Yaw)
{
	if (auto SpTransform = GetComponent<Transform>().lock();
		SpTransform)
	{
		SpTransform->SetPosition(Location);
		SpTransform->Rotate(Vector3{ 0.f,Yaw,0.f } );

		_ShockWave.lock()->GetComponent<Transform>().lock()->SetPosition(Location);
		_LensFlare.lock()->GetComponent<Transform>().lock()->SetPosition(Location);
	};
} 

void Energism::PlayEnd()
{
	T = 0.0f;
	_RenderProperty.bRender = false;
};

void Energism::UpdateReverberation(const float DeltaTime)
{
	CurReverberationDelta -= DeltaTime;
	if (CurReverberationDelta < 0.0f)
	{
		CurReverberationDelta += ReverberationDelta;
		PlayReverberation();
	}
}
void Energism::Hit(BT_INFO _BattleInfo, void* pArg)
{
}
void Energism::PlayReverberation()
{
	auto SpTransform = GetComponent<Transform>().lock();
	const Vector3 Look = -FMath::Normalize(SpTransform->GetLook());
	const float OffsetScale = SpTransform->GetScale().x * ReverberationOffsetScale; 

	const Vector3 CurLocation = SpTransform->GetPosition() + FMath::Random(0.0f, OffsetScale) * Look;

	const float ReverationStartScale = FMath::Random(ReverationStartRange.first, ReverationStartRange.second); 
	const float ReverationEndScale = FMath::Random(ReverationEndRange.first, ReverationEndRange.second);
	auto SpReverberation = ReverberationArr[CurReverberationIdx].lock();
	SpReverberation->PlayStart(CurLocation,Look,ReverationStartScale,ReverationEndScale);
	CurReverberationIdx = (CurReverberationIdx + 1) % ReverberationCount;
};

void Energism::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	const uint32 Numsubset = _Mesh->GetNumSubset();

	if (Numsubset > 0)
	{
		_Info.Fx->SetMatrix("matWorld", &World);
		_Info.Fx->SetTexture("AlbmMap", _AlbmMap->GetTexture());
		_Info.Fx->SetTexture("NoiseMap", _NoiseMap->GetTexture());
		_Info.Fx->SetFloat("NoiseFactor", NoiseFactor);
		
		_Info.Fx->SetFloat("Time",  T*TimeCorr);
		_Info.Fx->SetFloat("ColorIntencity", ColorIntencity);
		_Info.Fx->SetFloat("CurveScale", CurveScale);
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


void Energism::RenderDebug(const DrawInfo& _Info)
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


HRESULT Energism::Ready()
{
	// Ʈ������ �ʱ�ȭ .. 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	PushEditEntity(InitTransform.lock().get());
	RenderInit();

	m_nTag = MonsterWeapon;
	m_BattleInfo.iAttack = 50;
	// �������� ������ �ް���� ������Ʈ�� Raw �����ͷ� Ǫ��.
	return S_OK;
};

HRESULT Energism::Awake()
{
	
	GameObject::Awake();
	m_pCollider = AddComponent<BoxCollider>();
	m_pCollider.lock()->SetTrigger(true);
	m_pCollider.lock()->SetGravity(false);;
	PushEditEntity(m_pCollider.lock().get());
	m_pCollider.lock()->SetCenter({ 0.f, -0.05f, -2.5f });
	m_pCollider.lock()->SetSize({ 0.15f, 0.35f, 5.f });


	m_pTransform.lock()->SetScale(Vector3{ 0.040f,0.040f,0.040f } *ScaleFactor);
	m_pTransform.lock()->SetPosition(Vector3{ -37.411f,0.821f,30.663f });
	m_pTransform.lock()->SetRotation(Vector3{ 0.0f,0.f ,0.0f });

	return S_OK;
}

HRESULT Energism::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT Energism::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	if (_RenderProperty.bRender == false) return 0;

	T += _fDeltaTime;

	if (T > PlayTime)
	{
		PlayEnd();
	};

	UpdateReverberation(_fDeltaTime);

	return 0;
}


UINT Energism::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
}

void Energism::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		const std::string ChildName = GetName() + "_Play";
		ImGui::Begin(ChildName.c_str());

		static float EditPlayYaw = 0.0f;
		ImGui::SliderFloat("EditPlayYaw", &EditPlayYaw, -360.f, 360.f);

		if (ImGui::SmallButton("Play"))
		{
			PlayStart(GetComponent<Transform>().lock()->GetPosition() ,EditPlayYaw);
		}

		ImGui::Text("T : %2.6f", T);

		ImGui::SliderFloat("ReverberationOffsetScale", &ReverberationOffsetScale, 0.0f, 10000.f);

		ImGui::SliderFloat("ReverationStartRange First", &ReverationStartRange.first, 0.0f, 1.f);
		ImGui::SliderFloat("ReverationStartRange Second", &ReverationStartRange.second, 0.0f, 1.f);
		ImGui::SliderFloat("ReverationEndScale First", &ReverationEndRange.first, 0.0f, 1.f);
		ImGui::SliderFloat("ReverationEndScale Second", &ReverationEndRange.second, 0.0f, 1.f);

		ImGui::SliderFloat("TimeCorr", &TimeCorr, 0.0f, 10.f);
		ImGui::SliderFloat("ColorIntencity", &ColorIntencity, 
			0.0f, 10.f);
		ImGui::SliderFloat("CurveScale", &CurveScale,
			0.0f, 100.f);

		ImGui::SliderFloat("NoiseFactor", &NoiseFactor,
			0.0f, 100.f);

		ImGui::End();
	}
}


void Energism::OnEnable()
{
	GameObject::OnEnable();
}

void Energism::OnDisable()
{
	GameObject::OnDisable();
}



