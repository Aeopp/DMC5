#include "stdafx.h"
#include "..\Header\StoneDebrisMulti.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"


void StoneDebrisMulti::Free()
{
	GameObject::Free();
}

std::string StoneDebrisMulti::GetName()
{
	return "StoneDebrisMulti";
}

void StoneDebrisMulti::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const Vector3 Scale = _SpTransform->GetScale();
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();

		const auto& _Subset = _DebrisMesh->GetSubset(MAX_SUBSET_IDX);
		const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

		_RenderUpdateInfo.SubsetCullingSphere.resize(1);
		_RenderUpdateInfo.SubsetCullingSphere[0] = _CurBS.Transform(_RenderUpdateInfo.World, Scale.x);
	}
}

void StoneDebrisMulti::Reset()
{
	_SubsetIdx = 0u;

	Effect::Reset();
}

void StoneDebrisMulti::Imgui_Modify()
{
	if (auto Sptransform = GetComponent<ENGINE::Transform>().lock();
		Sptransform)
	{
		ImGui::Text("Eff_StoneDebrisMulti");

		{
			static Vector3 SliderPosition = Sptransform->GetPosition();
			ImGui::SliderFloat3("Pos##StoneDebrisMulti", SliderPosition, -10.f, 10.f);
			Sptransform->SetPosition(SliderPosition);
		}

		{
			static float Scale = Sptransform->GetScale().x * 10000.f;
			ImGui::SliderFloat("Scale##StoneDebrisMulti", &Scale, 1.f, 100.f);
			//Sptransform->SetScale({ Scale, Scale, Scale });	// x¸¸ À¯È¿
			SetScale(Scale * 0.00001f);
		}

		{
			static Vector3 SliderRotation{ 0,0,0 };
			ImGui::SliderFloat3("Rot##StoneDebrisMulti", SliderRotation, 0.f, 360.f);
			Sptransform->SetRotation(SliderRotation);
		}

		{
			static float PlayingSpeed = _PlayingSpeed;
			ImGui::SliderFloat("PlayingSpeed##StoneDebrisMulti", &PlayingSpeed, 1.f, 100.f);
			_PlayingSpeed = PlayingSpeed;
		}
	}
}

void StoneDebrisMulti::RenderInit()
{
	SetRenderEnable(true);

	ENGINE::RenderProperty _InitRenderProp;
	_InitRenderProp.bRender = false;
	_InitRenderProp.RenderOrders[RenderProperty::Order::GBuffer] =
	{
		{"GBufferEffect",
		[this](const DrawInfo& _Info)
			{
				this->RenderGBuffer(_Info);
			}
		},
	};
	RenderInterface::Initialize(_InitRenderProp);
}

void StoneDebrisMulti::RenderGBuffer(const DrawInfo& _Info)
{
	if (!_Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[0]))
		return;

	_Info.Fx->SetMatrix("matWorld", &_RenderUpdateInfo.World);
	_Info._Device->SetTexture(0, _DebrisALBMTex->GetTexture());
	_Info._Device->SetTexture(1, _DebrisNRMRTex->GetTexture());
	_Info.Fx->SetFloat("brightScale", _BrightScale);

	auto WeakSubset = _DebrisMesh->GetSubset(_SubsetIdx);
	if (auto SharedSubset = WeakSubset.lock();
		SharedSubset)
	{
		SharedSubset->Render(_Info.Fx);
	}
}

StoneDebrisMulti* StoneDebrisMulti::Create()
{
	return new StoneDebrisMulti{};
}

HRESULT StoneDebrisMulti::Ready()
{
	RenderInit();

	m_nTag = GAMEOBJECTTAG::Eff_StoneDebrisMulti;

	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.0003f, 0.0003f, 0.0003f });

	_DebrisMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Stone\\mesh_capcom_debris_stone00_multi.fbx");
	_DebrisALBMTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\mesh_capcom_debris_stone00_ALBM.tga");
	_DebrisNRMRTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\mesh_capcom_debris_stone00_NRMR.tga");

	_PlayingSpeed = 45.f;
	_BrightScale = 0.1f;

	return S_OK;
};

HRESULT StoneDebrisMulti::Awake()
{
	return S_OK;
}

HRESULT StoneDebrisMulti::Start()
{
	return S_OK;
}

UINT StoneDebrisMulti::Update(const float _fDeltaTime)
{
	Effect::Update(_fDeltaTime);

	if (!_IsPlaying)
		return 0;

	//
	_SubsetIdx = static_cast<uint32>(_AccumulateTime);
	if (MAX_SUBSET_IDX < _SubsetIdx)
		Reset();

	return 0;
}

UINT StoneDebrisMulti::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void StoneDebrisMulti::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		Imgui_Modify();
	}
}

void StoneDebrisMulti::OnEnable()
{
	GameObject::OnEnable();
}

void StoneDebrisMulti::OnDisable()
{
	GameObject::OnDisable();
}