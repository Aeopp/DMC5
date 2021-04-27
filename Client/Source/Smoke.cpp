#include "stdafx.h"
#include "..\Header\Smoke.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"


void Smoke::SetVariationIdx(Smoke::VARIATION Idx)
{
	if (Idx > MAX_VARIATION_IDX)
		return;

	Reset();

	_VariationIdx = Idx;
}

void Smoke::Free()
{
	GameObject::Free();
}

std::string Smoke::GetName()
{
	return "Smoke";
};

void Smoke::Reset()
{
	Effect::Reset();
}

void Smoke::Imgui_Modify()
{
	if (auto Sptransform = GetComponent<ENGINE::Transform>().lock();
		Sptransform)
	{
		ImGui::Text("Eff_Smoke");

		{
			static Vector3 Position = Sptransform->GetPosition();
			ImGui::InputFloat3("Pos##Smoke", Position);
			Sptransform->SetPosition(Position);
		}

		{
			static float Scale = Sptransform->GetScale().x;
			ImGui::SliderFloat("Scale##Smoke", &Scale, 0.01f, 0.3f);
			//Sptransform->SetScale({ Scale, Scale, Scale });	// x만 유효
			SetScale(Scale);
		}

		{
			static Vector3 SliderRotation{ 0,0,0 };
			ImGui::SliderFloat3("Rot##Smoke", SliderRotation, 0.f, 360.f);
			Sptransform->SetRotation(SliderRotation);
		}

		{
			static float PlayingSpeed = _PlayingSpeed;
			ImGui::SliderFloat("PlayingSpeed##Smoke", &PlayingSpeed, 0.1f, 10.f);
			_PlayingSpeed = PlayingSpeed;
		}
	}
}

Smoke* Smoke::Create()
{
	return new Smoke{};
}

void Smoke::RenderInit()
{
	SetRenderEnable(true);

	ENGINE::RenderProperty _InitRenderProp;
	_InitRenderProp.bRender = false;
	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{
		{"Sprite",
		[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		},
	};

	RenderInterface::Initialize(_InitRenderProp);
}

void Smoke::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	//auto RefEffInfo = std::any_cast<EffectInfo>((_Info.BySituation));

	auto WeakSubset = _SmokeMesh->GetSubset(0u);
	if (auto SharedSubset = WeakSubset.lock();
		SharedSubset)
	{
		_Info.Fx->SetMatrix("World", &_RenderUpdateInfo.World);
		_Info.Fx->SetTexture("ALB0Map", _SmokeALB0Tex->GetTexture());
		//_Info.Fx->SetFloat("SoftParticleDepthScale", _SoftParticleDepthScale);
		_Info.Fx->SetFloat("_BrightScale", _BrightScale);
		_Info.Fx->SetFloatArray("_MinTexUV", _SmokeMinTexUV, 2u);
		_Info.Fx->SetFloatArray("_MaxTexUV", _SmokeMaxTexUV, 2u);

		SharedSubset->Render(_Info.Fx);
	}
}


HRESULT Smoke::Ready()
{
	RenderInit();

	m_nTag = GAMEOBJECTTAG::Eff_Smoke;

	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.1f, 0.1f, 0.1f });	// _SmokeMesh 원점이 중앙이 아니라 스케일 늘리면 x축 이동도 필요
	
	//_PlaneMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx");
	_SmokeMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\mesh_03_enviroment_smoke00_03.fbx");

	_SmokeALB0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\tex_capcom_smoke_00_0016_alpg.tga");

	_PlayingSpeed = 1.f;
	_BrightScale = 0.00005f;

	Reset();

	return S_OK;
};

HRESULT Smoke::Awake()
{
	return S_OK;
}

HRESULT Smoke::Start()
{
	return S_OK;
}

UINT Smoke::Update(const float _fDeltaTime)
{
	Effect::Update(_fDeltaTime);

	// Reset() 호출까지 계속 재생

	//
	// + if (SMOKE_0 == _VariationIdx)
	float cx = 8.f;	// 가로 갯수
	float cy = 8.f; // 세로 갯수

	if (cx * cy < _AccumulateTime)
		_AccumulateTime = 0.f;

	int Frame = static_cast<int>(_AccumulateTime);
	int w = Frame % static_cast<int>(cx);
	int h = Frame / static_cast<int>(cx);
	_SmokeMinTexUV.x = 1.f / cx * static_cast<float>(w);
	_SmokeMinTexUV.y = 1.f / cy * static_cast<float>(h);
	_SmokeMaxTexUV.x = _SmokeMinTexUV.x + 1.f / cx;
	_SmokeMaxTexUV.y = _SmokeMinTexUV.y + 1.f / cy;

	// y축 빌보드
	//Matrix TempMat, BillMat;
	//TempMat = Renderer::GetInstance()->_RenderInfo.View;
	//D3DXMatrixIdentity(&BillMat);
	//BillMat._11 = TempMat._11;
	//BillMat._13 = TempMat._13;
	//BillMat._31 = TempMat._31;
	//BillMat._33 = TempMat._33;
	//D3DXMatrixInverse(&BillMat, 0, &BillMat);
	////BillMat._42 = -1.2f;
	//
	//if (auto Sptransform = GetComponent<ENGINE::Transform>().lock();
	//	Sptransform)
	//	Sptransform->SetBillBoard(BillMat);

	//
	//Imgui_Modify();

	return 0;
}

UINT Smoke::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void Smoke::Editor()
{
	GameObject::Editor();
}

void Smoke::OnEnable()
{
	GameObject::OnEnable();
}

void Smoke::OnDisable()
{
	GameObject::OnDisable();
}