#include "stdafx.h"
#include "..\Header\Blood.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"


void Blood::Free()
{
	GameObject::Free();
}

std::string Blood::GetName()
{
	return "Blood";
};

void Blood::Reset()
{
	_SubsetIdx = 0u;

	Effect::Reset();
}

void Blood::Imgui_Modify()
{
	if (auto Sptransform = GetComponent<ENGINE::Transform>().lock();
		Sptransform)
	{
		ImGui::Text("Eff_Blood : %d", _VariationIdx);
		{
			Vector3 SliderPosition = Sptransform->GetPosition();
			ImGui::SliderFloat3("Pos##Blood", SliderPosition, -10.f, 10.f);
			Sptransform->SetPosition(SliderPosition);
		}

		{
			float Scale = Sptransform->GetScale().x;
			ImGui::SliderFloat("Scale##Blood", &Scale, 0.1f, 1.f);
			//Sptransform->SetScale({ Scale, Scale, Scale });	// x¸¸ À¯È¿
			SetScale(Scale);
		}

		{
			float PlayingSpeed = _PlayingSpeed;
			ImGui::SliderFloat("PlayingSpeed##Blood", &PlayingSpeed, 0.1f, 10.f);
			_PlayingSpeed = PlayingSpeed;
		}
	}
}

void Blood::RenderInit()
{
	SetRenderEnable(true);

	ENGINE::RenderProperty _InitRenderProp;
	_InitRenderProp.bRender = false;
	_InitRenderProp.RenderOrders[RenderProperty::Order::GBuffer] =
	{
		{"Blood",
		[this](const DrawInfo& _Info)
			{
				this->RenderGBuffer(_Info);
			}
		},
	};
	RenderInterface::Initialize(_InitRenderProp);
}

void Blood::RenderGBuffer(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);

	weak_ptr<ENGINE::Subset> WeakSubset;
	switch (_VariationIdx)
	{
	case 0u:
		WeakSubset = _Blood0Mesh->GetSubset(_SubsetIdx);
		break;
	case 1u:
		WeakSubset = _Blood1Mesh->GetSubset(_SubsetIdx);
		break;
	case 2u:
		WeakSubset = _Blood2Mesh->GetSubset(_SubsetIdx);
		break;
	case 3u:
		WeakSubset = _Blood3Mesh->GetSubset(_SubsetIdx);
		break;
	}

	if (auto SharedSubset = WeakSubset.lock();
		SharedSubset)
	{
		_Info._Device->SetTexture(0, _BloodALB0Tex->GetTexture());
		_Info._Device->SetTexture(1, _BloodNRMR0Tex->GetTexture());
		SharedSubset->Render(_Info.Fx);
	}
}

Blood* Blood::Create()
{
	return new Blood{};
}

HRESULT Blood::Ready()
{
	RenderInit();

	m_nTag = GAMEOBJECTTAG::Eff_Blood;

	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.3f, 0.3f, 0.3f });

	_Blood0Mesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Blood\\Blood1.fbx");
	_Blood1Mesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Blood\\Blood2.fbx");
	_Blood2Mesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Blood\\Blood3.fbx");
	_Blood3Mesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Blood\\Blood4.fbx");
	_BloodALB0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\mesh_capcom_liquid_splash00_00_ALBM.tga");
	_BloodNRMR0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\mesh_03_liquid_splash00_00_00_NRMR.tga");

	_PlayingSpeed = 1.f;

	SetVariationIdx(_VariationIdx);

	return S_OK;
};

HRESULT Blood::Awake()
{
	return S_OK;
}

HRESULT Blood::Start()
{
	return S_OK;
}

UINT Blood::Update(const float _fDeltaTime)
{
	Effect::Update(_fDeltaTime);

	//
	_SubsetIdx = static_cast<uint32>(_AccumulateTime);
	if (_SubsetMaxIdx < _SubsetIdx)
		Reset();

	//
	Imgui_Modify();

	return 0;
}

UINT Blood::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void Blood::Editor()
{
	GameObject::Editor();
}

void Blood::OnEnable()
{
	GameObject::OnEnable();
}

void Blood::OnDisable()
{
	GameObject::OnDisable();
}

void Blood::SetVariationIdx(const uint32 Idx)
{
	if (Idx < 0u || Idx > 3u)
		return;

	Reset();

	switch (Idx)
	{
	case 0u:
		_SubsetMaxIdx = 15u;
		break;
	case 1u:
		_SubsetMaxIdx = 17u;
		break;
	case 2u:
		_SubsetMaxIdx = 25u;
		break;
	case 3u:
		_SubsetMaxIdx = 21u;
		break;
	}

	_VariationIdx = Idx;
}