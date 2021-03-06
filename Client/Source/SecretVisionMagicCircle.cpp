#include "stdafx.h"
#include "..\Header\SecretVisionMagicCircle.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "ParticleSystem.h"

void SecretVisionMagicCircle::SetTexID(SecretVisionMagicCircle::TexID Idx)
{
	if (Idx > MAX_VARIATION_IDX)
		return;

	_TexID = Idx;
}

void SecretVisionMagicCircle::Free()
{
	GameObject::Free();
};

std::string SecretVisionMagicCircle::GetName()
{
	return "SecretVisionMagicCircle";
};

SecretVisionMagicCircle* SecretVisionMagicCircle::Create()
{
	return new SecretVisionMagicCircle{};
};

void SecretVisionMagicCircle::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();

	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const float CurScale = FMath::Lerp(StartScale, FinalScale, Sin);
		if (auto SpTransform = GetComponent<Transform>().lock();
			SpTransform)
		{
			SpTransform->SetScale({ CurScale,CurScale,CurScale });
			_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
			// 컬링 X
			//if (_StaticMesh)
			//{
			//	const uint32  Numsubset = _StaticMesh->GetNumSubset();
			//	_RenderUpdateInfo.SubsetCullingSphere.resize(Numsubset);

			//	for (uint32 i = 0; i < Numsubset; ++i)
			//	{
			//		const auto& _Subset = _StaticMesh->GetSubset(i);
			//		const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

			//		_RenderUpdateInfo.SubsetCullingSphere[i] = _CurBS.Transform(_RenderUpdateInfo.World, CurScale);
			//	}
			//}
		}
	}

};

void SecretVisionMagicCircle::RenderInit()
{
	m_nTag  = Eff_SecretVisionMagicCircle;
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
		{"Debug",
		[this](const DrawInfo& _Info)
		{
			RenderDebug(_Info);
		}
	} };
	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] 
		=
	{
		{"AirHike",
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
	_InitInfo.bLocalVertexLocationsStorage = false;

	_StaticMesh = Resources::Load<ENGINE::StaticMesh>
			(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx" , _InitInfo);

	_SVTex0 =
		Resources::Load<ENGINE::Texture>(L"..\\..\\Usable\\SecretVision\\3.tga");
	_SVTex1 =
		Resources::Load<ENGINE::Texture>(L"..\\..\\Usable\\SecretVision\\7.tga");
	_SVTex2 =
		Resources::Load<ENGINE::Texture>(L"..\\..\\Usable\\SecretVision\\9.tga");

	PushEditEntity(_StaticMesh.get());
};

void SecretVisionMagicCircle::PlayStart(
	const std::optional<Vector3>& Location,
	const float StartScale,
	const float FinalScale,
	const float PlayTime)
{
	if (auto SpTransform = GetComponent<Transform>().lock();
		SpTransform)
	{
		if (Location)
		{
			SpTransform->SetPosition(Location.value());
			PlayPos = Location.value();
		}
	}

	CurParticleTime = 0.0f;
	this->StartScale = StartScale;
	this->FinalScale = FinalScale;
	this->PlayTime = PlayTime;
	T = 0.0f;
	_RenderProperty.bRender = true;
	bPlayedEndParticle = false;
};

void SecretVisionMagicCircle::PlayEnd()
{
	_RenderProperty.bRender = false;
    T = 0.0f;
};

void SecretVisionMagicCircle::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	Matrix World = _RenderUpdateInfo.World;
	*reinterpret_cast<Vector3*>(&World.m[3][0]) = PlayPos;	// ㅠㅠ

	const float CurIntencity = FMath::Lerp(StartIntencity, FinalIntencity, Sin);
	const Vector4 CurColor = FMath::Lerp(StartColor, FinalColor, Sin);
	const uint32 Numsubset = static_cast<uint32>(_StaticMesh->GetNumSubset());

	if (Numsubset > 0)
	{
		_Info.Fx->SetMatrix("matWorld", &World);
		_Info.Fx->SetVector("CurColor", &CurColor);
		_Info.Fx->SetFloat("Intencity", CurIntencity);

		switch (_TexID)
		{
		case SECRETVISION_0: default:
			_Info.Fx->SetTexture("MagicMap", _SVTex0->GetTexture());
			_Info.Fx->SetTexture("MagicMsk", _SVTex0->GetTexture());
			_Info.Fx->SetTexture("MagicAlb", _SVTex0->GetTexture());
			break;
		case SECRETVISION_1:
			_Info.Fx->SetTexture("MagicMap", _SVTex1->GetTexture());
			_Info.Fx->SetTexture("MagicMsk", _SVTex1->GetTexture());
			_Info.Fx->SetTexture("MagicAlb", _SVTex1->GetTexture());
			break;
		case SECRETVISION_2:
			_Info.Fx->SetTexture("MagicMap", _SVTex2->GetTexture());
			_Info.Fx->SetTexture("MagicMsk", _SVTex2->GetTexture());
			_Info.Fx->SetTexture("MagicAlb", _SVTex2->GetTexture());
			break;
		}
	}

	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = _StaticMesh->GetSubset(i).lock();
			SpSubset)
		{
			// 컬링 X
			//if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
			//{
			//	continue;
			//}

			SpSubset->Render(_Info.Fx);
		};
	};
}


void SecretVisionMagicCircle::RenderDebug(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);

	const uint32 Numsubset = static_cast<uint32>(_StaticMesh->GetNumSubset());
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


HRESULT SecretVisionMagicCircle::Ready()
{
	// 트랜스폼 초기화 .. 
	auto InitTransform = GetComponent<ENGINE::Transform>();

	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.

	D3DXMatrixIdentity(&BillBoardMat);

	return S_OK;
};

HRESULT SecretVisionMagicCircle::Awake()
{
	GameObject::Awake();
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.01f,0.01f,0.01f });
	InitTransform.lock()->SetPosition(Vector3{ 0.f,0.f,0.f });
	InitTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	return S_OK;
}

HRESULT SecretVisionMagicCircle::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT SecretVisionMagicCircle::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	if (_RenderProperty.bRender == false) return 0;

	T += _fDeltaTime * Speed;
	Sin = std::sinf(T);

	if (bPlayedEndParticle == false && T >=  (FMath::PI / 4.f) )
	{
		bPlayedEndParticle = true;

		if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
			SpTransform)
		{
			if (auto _Particle =
				ParticleSystem::GetInstance()->PlayParticle(
					"SVMCEndParticle", 2000ul, true);
				_Particle.empty() == false)
			{

				for (int32 i = 0; i < _Particle.size(); ++i)
				{
					auto& _PlayInstance = _Particle[i];

					Matrix World = SpTransform->GetRenderMatrix();
					*reinterpret_cast<Vector3*>(&World.m[3][0]) = PlayPos;	// ㅠㅠ

					_PlayInstance->PlayDescBind(World);
				}
			}
		};
	}

	CurParticleTime -= _fDeltaTime;
	if (CurParticleTime < 0.0f)
	{
		CurParticleTime += ParticleTime;
		//PlayParticle();
	}

	// 빌보드
	Matrix ViewInvMat;
	ViewInvMat = Renderer::GetInstance()->_RenderInfo.ViewInverse;
	D3DXMatrixIdentity(&BillBoardMat);
	memcpy(&BillBoardMat.m[0][0], &ViewInvMat.m[0][0], sizeof(Vector3));
	memcpy(&BillBoardMat.m[1][0], &ViewInvMat.m[1][0], sizeof(Vector3));
	memcpy(&BillBoardMat.m[2][0], &ViewInvMat.m[2][0], sizeof(Vector3));
	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		SpTransform->SetBillBoard(BillBoardMat);
	}

	// 끝날 쯔음 .
	if (T >= PlayTime)
	{
		PlayEnd();
	};

	return 0;
}

void SecretVisionMagicCircle::PlayParticle()
{
	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		if (auto _Particle =
			ParticleSystem::GetInstance()->PlayParticle(
				"AirHikeParticle", 333ul, true);
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

UINT SecretVisionMagicCircle::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
}

void SecretVisionMagicCircle::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		const std::string ChildName = GetName() + "_Play";
		ImGui::BeginChild(ChildName.c_str()); 
		if (ImGui::SmallButton("Play"))
		{
			PlayStart();
		}

		ImGui::Text("T : %2.6f", T);
		ImGui::SliderFloat("Speed", &Speed, 0.f, 10.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);

		ImGui::SliderFloat("StartIntencity", &StartIntencity, 0.f, 10.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("StartScale", &StartScale, 0.f, 1.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
		ImGui::ColorEdit4("StartColor", StartColor);

		ImGui::SliderFloat("FinalIntencity", &FinalIntencity, 0.f, 10.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("FinalScale", &FinalScale, 0.f, 1.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
		ImGui::ColorEdit4("FinalColor", FinalColor);
		ImGui::EndChild();
	}
}


void SecretVisionMagicCircle::OnEnable()
{
	GameObject::OnEnable();
}

void SecretVisionMagicCircle::OnDisable()
{
	GameObject::OnDisable();
}



