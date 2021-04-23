#include "stdafx.h"
#include "..\Header\Dust.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"


void Dust::Free()
{
	GameObject::Free();
}

std::string Dust::GetName()
{
	return "Dust";
};

void Dust::Reset()
{
	Effect::Reset();
}

void Dust::Imgui_Modify()
{
	if (auto Sptransform = GetComponent<ENGINE::Transform>().lock();
		Sptransform)
	{
		ImGui::Text("Eff_Dust");

		{
			static Vector3 SliderPosition = Sptransform->GetPosition();
			ImGui::SliderFloat3("Pos##Dust", SliderPosition, -10.f, 10.f);
			Sptransform->SetPosition(SliderPosition);
		}

		{
			static float Scale = Sptransform->GetScale().x;
			ImGui::SliderFloat("Scale##Dust", &Scale, 0.1f, 1.f);
			//Sptransform->SetScale({ Scale, Scale, Scale });	// x만 유효
			SetScale(Scale);
		}

		{
			static Vector3 SliderRotation{ 0,0,0 };
			ImGui::SliderFloat3("Rot##Dust", SliderRotation, 0.f, 360.f);
			Sptransform->SetRotation(SliderRotation);
		}

		{
			static float PlayingSpeed = _PlayingSpeed;
			ImGui::SliderFloat("PlayingSpeed##Dust", &PlayingSpeed, 0.1f, 10.f);
			_PlayingSpeed = PlayingSpeed;
		}
	}
}

Dust* Dust::Create()
{
	return new Dust{};
}

void Dust::RenderInit()
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

void Dust::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	auto WeakSubset = _PlaneMesh->GetSubset(0u);
	if (auto SharedSubset = WeakSubset.lock();
		SharedSubset)
	{
		_Info.Fx->SetMatrix("World", &_RenderUpdateInfo.World);

		_Info.Fx->SetTexture("ALB0Map", _SmokeALB0Tex->GetTexture());
		_Info.Fx->SetFloatArray("_MinTexUV", _SmokeMinTexUV, 2u);
		_Info.Fx->SetFloatArray("_MaxTexUV", _SmokeMaxTexUV, 2u);

		SharedSubset->Render(_Info.Fx);
	}
}


HRESULT Dust::Ready()
{
	RenderInit();

	m_nTag = GAMEOBJECTTAG::Eff_Dust;

	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.1f, 0.1f, 0.1f });
	
	_PlaneMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx");

	_SmokeALB0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\tex_capcom_smoke_00_0016_alpg.tga");

	_PlayingSpeed = 1.f;

	Reset();

	return S_OK;
};

HRESULT Dust::Awake()
{
	return S_OK;
}

HRESULT Dust::Start()
{
	return S_OK;
}

UINT Dust::Update(const float _fDeltaTime)
{
	Effect::Update(_fDeltaTime);

	//
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

std::cout << _SmokeMinTexUV.x << " " << _SmokeMinTexUV.y << std::endl;
std::cout << _SmokeMaxTexUV.x << " " << _SmokeMaxTexUV.y << std::endl << std::endl;

	Matrix TempMat, BillMat;
	TempMat = Renderer::GetInstance()->_RenderInfo.View;
	D3DXMatrixIdentity(&BillMat);
	BillMat._11 = TempMat._11;
	BillMat._13 = TempMat._13;
	BillMat._31 = TempMat._31;
	BillMat._33 = TempMat._33;
	D3DXMatrixInverse(&BillMat, 0, &BillMat);
	//BillMat._42 = -1.2f;

	if (auto Sptransform = GetComponent<ENGINE::Transform>().lock();
		Sptransform)
		Sptransform->SetBillBoard(BillMat);

	//
	//Imgui_Modify();

	return 0;
}

UINT Dust::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void Dust::Editor()
{
	GameObject::Editor();
}

void Dust::OnEnable()
{
	GameObject::OnEnable();
}

void Dust::OnDisable()
{
	GameObject::OnDisable();
}