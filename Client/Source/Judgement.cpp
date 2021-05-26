#include "stdafx.h"
#include "..\Header\Judgement.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "ParticleSystem.h"

void Judgement::Free()
{
	GameObject::Free();
};

std::string Judgement::GetName()
{
	return "Judgement";
};

Judgement* Judgement::Create()
{
	return new Judgement{};
};

void Judgement::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	const float CurScale =
		FMath::Lerp(0.0f, GrowEndScale, GrowLerp());

	if (auto SpTransform = GetComponent<Transform>().lock();
		SpTransform)
	{
		SpTransform->SetScale({ CurScale ,CurScale ,CurScale });
		_RenderUpdateInfo.World = SpTransform->GetRenderMatrix();
		if (_StaticMesh)
		{
			const uint32  Numsubset = _StaticMesh->GetNumSubset();
			_RenderUpdateInfo.SubsetCullingSphere.resize(Numsubset);

			for (uint32 i = 0; i < Numsubset; ++i)
			{
				const auto& _Subset = _StaticMesh->GetSubset(i);
				const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

				_RenderUpdateInfo.SubsetCullingSphere[i] = _CurBS.Transform(_RenderUpdateInfo.World, CurScale);
			}
		}
	}
};

void Judgement::RenderInit()
{
	m_nTag = Eff_Judgement;
	// 렌더를 수행해야하는 오브젝트라고 (렌더러에 등록 가능 ) 알림.
	// 렌더 인터페이스 상속받지 않았다면 키지마세요.
	SetRenderEnable(true);

	// 렌더 정보 초기화 ...
	ENGINE::RenderProperty _InitRenderProp;
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	// 렌더 속성 전체 초기화 
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
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

		{"Judgement",
		[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		}
	};

	RenderInterface::Initialize(_InitRenderProp);

	// 
	// 스태틱 메쉬 로딩
	Mesh::InitializeInfo _InitInfo{};
	_InitInfo.bLocalVertexLocationsStorage = true;

	_StaticMesh = Resources::Load<ENGINE::StaticMesh>
		(L"..\\..\\Resource\\Mesh\\Static\\Effect\\AirHike\\AirHike.fbx", _InitInfo);

	_MagicTexture =
		Resources::Load<ENGINE::Texture>(L"..\\..\\Usable\\Judgement\\Magic.tga");
	_MagicAlb =
		Resources::Load<ENGINE::Texture>(L"..\\..\\Usable\\Judgement\\alb.tga");
	_MagicMsk =
		Resources::Load<ENGINE::Texture>(L"..\\..\\Usable\\Judgement\\msk.tga");
	_NrmrMap = Resources::Load<ENGINE::Texture>(
		L"..\\..\\Usable\\Judgement\\Nrmr.tga");

	_ShockWave = AddGameObject<ShockWave>();
	_JudgementCut = AddGameObject<JudgementCut>();
	_CircleWave = AddGameObject<CircleWave>();
	_CircleWave.lock()->bWaveDistortion = true;


	PushEditEntity(_StaticMesh.get());
	PushEditEntity(_MagicTexture.get());
	PushEditEntity(_MagicAlb.get());
	PushEditEntity(_MagicMsk.get());
	PushEditEntity(_NrmrMap.get());
};

void Judgement::PlayStart(
	const Vector3& Location)
{
	if (auto SpTransform = GetComponent<Transform>().lock();
		SpTransform)
	{
		if (Location)
		{
			SpTransform->SetPosition(Location);
		}
	}

	bCircleGrowEndParticle = false;
	CurCircleGrowParticleTime = 0.0f;
	CurShockDelta = 0.0f;
	T = 0.0f;
	_RenderProperty.bRender = true;

	auto SpTransform = GetComponent<Transform>().lock();
	_CircleWave.lock()->PlayStart(SpTransform->GetScale().x * 3.5f, SpTransform->GetPosition());
};

void Judgement::PlayEnd()
{
	_RenderProperty.bRender = false;
	T = 0.0f;

	auto SpTransform = GetComponent<Transform>().lock();
	_CircleWave.lock()->PlayStart(SpTransform->GetScale().x *3.5f, SpTransform->GetPosition());
	_JudgementCut.lock()->PlayStart(SpTransform->GetPosition());
};

void Judgement::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;

	float CurIntencity = 0.0f;
	Vector3 CurColor = { 0.f,0.f,0.f };
	bool bDecal = false;

	if (FMath::IsRange(0.0f, CircleGrowTime, T))
	{
		bDecal = false;

		CurIntencity = FMath::Lerp(0.0f, GrowEndIntencity,
			GrowLerp());
		CurColor = FMath::Lerp(GrowStartColor, GrowEndColor,GrowLerp());
	}
	else if (FMath::IsRange(CircleGrowTime, JudgementReadyTime, T))
	{
		bDecal = false;

		CurIntencity = FMath::Lerp(GrowEndIntencity,JudgementReadyIntencity,GrowEndLerp());
		CurColor = FMath::Lerp(GrowEndColor, JudgementReadyColor, GrowEndLerp());
	}
	else
	{
		bDecal = true;
		CurIntencity = DecalIntencity;
		CurColor = DecalColor;
	}

	const uint32 Numsubset = _StaticMesh->GetNumSubset();

	if (Numsubset > 0)
	{
		_Info.Fx->SetMatrix("matWorld", &World);
		_Info.Fx->SetFloatArray("CurColor", CurColor,3u);
		_Info.Fx->SetFloat("Intencity", CurIntencity);

		_Info.Fx->SetTexture("MagicMap",
			_MagicTexture->GetTexture());

		_Info.Fx->SetTexture("MagicMsk",
			_MagicMsk->GetTexture());

		_Info.Fx->SetTexture("MagicAlb",
			_MagicAlb->GetTexture());

		_Info.Fx->SetTexture("NrmrMap", 
			_NrmrMap->GetTexture());

		_Info.Fx->SetFloat("Lerp", GrowLerp());

		_Info.Fx->SetBool("bDecal", bDecal);
	}

	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = _StaticMesh->GetSubset(i).lock();
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


void Judgement::RenderDebug(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);

	const uint32 Numsubset = _StaticMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = _StaticMesh->GetSubset(i).lock();
			SpSubset)
		{
			if (false ==
				_Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
			{
				continue;
			}

			SpSubset->Render(_Info.Fx);
		};
	};

};


HRESULT Judgement::Ready()
{
	// 트랜스폼 초기화 .. 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.
	return S_OK;
};

HRESULT Judgement::Awake()
{
	GameObject::Awake();

	m_pTransform.lock()->SetScale({ 0.0015f,0.0015f,0.0015f });
	m_pTransform.lock()->SetPosition(Vector3{ 0.f,0.12f,0.f });
	m_pTransform.lock()->SetRotation(Vector3{ 0.f ,0.f ,0.0f });

	return S_OK;
}

HRESULT Judgement::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT Judgement::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	if (_RenderProperty.bRender == false) return 0;

	T += _fDeltaTime;

	UpdateParticle(_fDeltaTime);
	UpdateShockWave(_fDeltaTime);

	if (T > JudgementReadyTime)
	{
		PlayCircleGrowEndParticle();
	}

	// 끝날 쯔음 .
	if (T >= PlayTime)
	{
		PlayEnd();
	};

	return 0;
};

void Judgement::PlayCircleGrowEndParticle()
{
	if (!bCircleGrowEndParticle)
	{
		bCircleGrowEndParticle = true;
		if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
			SpTransform)
		{
			if (auto _Particle =
				ParticleSystem::GetInstance()->PlayParticle(
					"JudgementReady", 2500ul, true);
				_Particle.empty() == false)
			{
				for (int32 i = 0; i < _Particle.size(); ++i)
				{
					auto& _PlayInstance = _Particle[i];
					_PlayInstance->PlayDescBind(SpTransform->GetRenderMatrix());
				}
			}
		};
	}

	auto SpTransform = GetComponent<Transform>().lock();
	_CircleWave.lock()->PlayStart(SpTransform->GetScale().x * 3.f, SpTransform->GetPosition());

};

void Judgement::PlayCircleGrowParticle()
{
	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		if (auto _Particle =
			ParticleSystem::GetInstance()->PlayParticle(
				"CircleGrowParticle", 455ul, true);
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

void Judgement::PlayJudgementDayParticle()
{
	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		if (auto _Particle =
			ParticleSystem::GetInstance()->PlayParticle(
				"JudgementDay", 111ul, true);
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

void Judgement::PlayShockWave()
{
	auto SpTransform = GetComponent<Transform>().lock();

	_ShockWave.lock()->PlayStart(
		SpTransform->GetPosition(),
		ShockWave::Option::Judgement ,false,
		SpTransform->GetScale().x * ShockScaleFactor);
};


void Judgement::UpdateParticle(const float DeltaTime)
{
	if (T <= CircleGrowTime)
	{
		CurCircleGrowParticleTime -= DeltaTime;
		if (CurCircleGrowParticleTime < 0.0f)
		{
			CurCircleGrowParticleTime += CircleGrowParticleTime;
			PlayCircleGrowParticle();
		}
	}
	else if (T >= JudgementReadyTime)
	{
		CurJudgementDayParticleTime -= DeltaTime;
		if (CurJudgementDayParticleTime < 0.0f)
		{
			CurJudgementDayParticleTime += JudgementDayParticleTime;
			PlayJudgementDayParticle();
		}
	}
};

void Judgement::UpdateShockWave(const float DeltaTime)
{
	CurShockDelta -= DeltaTime;
	if (CurShockDelta < 0.0f)
	{
		CurShockDelta += ShockDelta;
		PlayShockWave();
		auto SpTransform = GetComponent<Transform>().lock();
		
	}
};

float Judgement::GrowLerp() const
{
	return FMath::Clamp(T / CircleGrowTime, 0.0f, 1.f);
};

float Judgement::GrowEndLerp() const
{
	return FMath::Clamp(
		(T - CircleGrowTime) / 
		(JudgementReadyTime - CircleGrowTime), 0.0f, 1.f);
};

UINT Judgement::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
};

void Judgement::Editor()
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

		ImGui::SliderFloat("PlayTime", &PlayTime, 0.f, 10.f, "%2.6f");
		ImGui::SliderFloat("CircleGrowParticleTime", &CircleGrowParticleTime, 0.f, 1.f, "%2.6f");
		ImGui::SliderFloat("JudgementDayParticleTime", &JudgementDayParticleTime, 0.f, 1.f, "%2.6f");
		;
		ImGui::SliderFloat("CircleGrowTime", &CircleGrowTime, 0.f, 10.f, "%2.6f");
		ImGui::SliderFloat("JudgementReadyTime", &JudgementReadyTime, CircleGrowTime, 10.f, "%2.6f");

		ImGui::SliderFloat("ShockDelta", 
			&ShockDelta, ShockDelta, 2.f, "%2.6f");
		ImGui::SliderFloat("ShockScaleFactor",
			&ShockScaleFactor, 1.f, 1000.f, "%2.6f");

		ImGui::SliderFloat("GrowEndScale", &GrowEndScale, 0.f, 1.f, "%2.6f");
		ImGui::SliderFloat("FinalIntencity", &GrowEndIntencity, 0.f, 1.f, "%2.6f");
		ImGui::SliderFloat("JudgementIntencity", &JudgementReadyIntencity, 0.f, 1.f, "%2.6f");
		ImGui::SliderFloat("DecalIntencity", &DecalIntencity, -1.f, 1.f, "%2.6f");

		ImGui::ColorEdit3("GrowStartColor", GrowStartColor);
		ImGui::ColorEdit3("GrowEndColor", GrowEndColor);
		ImGui::ColorEdit3("JudgementReadyColor", JudgementReadyColor);

		ImGui::EndChild();
	}
}


void Judgement::OnEnable()
{
	GameObject::OnEnable();
}

void Judgement::OnDisable()
{
	GameObject::OnDisable();
}



