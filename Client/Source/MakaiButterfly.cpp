#include "stdafx.h"
#include "..\Header\MakaiButterfly.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include "SecretVision.h"
#include "BtlPanel.h"
#include "SecretVisionMagicCircle.h"


uint32 MakaiButterfly::_TotalCnt = 0u;

void MakaiButterfly::SetVariationIdx(MakaiButterfly::VARIATION Idx)
{
	if (Idx > MAX_VARIATION_IDX)
		return;

	Reset();

	_VariationIdx = Idx;
}

void MakaiButterfly::Free()
{
	Destroy(_SVMC);

	GameObject::Free();
}

std::string MakaiButterfly::GetName()
{
	return "MakaiButterfly";
}

void MakaiButterfly::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const Vector3 Scale = _SpTransform->GetScale();
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();

		const auto& _Subset = _Mesh->GetSubset(static_cast<uint32>(_SubsetIdx));
		const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

		_RenderUpdateInfo.SubsetCullingSphere.resize(1);
		_RenderUpdateInfo.SubsetCullingSphere[0] = _CurBS.Transform(_RenderUpdateInfo.World, Scale.x);
	}
}

void MakaiButterfly::PlayStart(const float PlayingSpeed/*= 25.f*/)
{
	Effect::PlayStart(PlayingSpeed);
	_IsAlive = true;
	_SliceAmount = 1.f;
	_BrightScale = 0.2f;
	_Collider.lock()->SetActive(true);
}

void MakaiButterfly::Reset()
{
	_IsAlive = false;
	_Collider.lock()->SetActive(false);
}

void MakaiButterfly::Imgui_Modify()
{
	if (auto Sptransform = GetComponent<ENGINE::Transform>().lock();
		Sptransform)
	{
		ImGui::Text("Eff_MakaiButterfly");

		{
			static Vector3 SliderPosition = Sptransform->GetPosition();
			ImGui::SliderFloat3("Pos##MakaiButterfly", SliderPosition, -10.f, 10.f);
			Sptransform->SetPosition(SliderPosition);
		}

		{
			static float Scale = Sptransform->GetScale().x;
			ImGui::SliderFloat("Scale##MakaiButterfly", &Scale, 0.001f, 0.003f);
			//Sptransform->SetScale({ Scale, Scale, Scale });	// x¸¸ À¯È¿
			SetScale(Scale);
		}

		{
			static Vector3 SliderRotation{ 0,0,0 };
			ImGui::SliderFloat3("Rot##MakaiButterfly", SliderRotation, 0.f, 360.f);
			Sptransform->SetRotation(SliderRotation);
		}

		//{
		//	static float BrightScale = _BrightScale;
		//	ImGui::SliderFloat("BrightScale##MakaiButterfly", &BrightScale, 0.001f, 0.3f);
		//	_BrightScale = BrightScale;
		//}

		{
			static float PlayingSpeed = _PlayingSpeed;
			ImGui::SliderFloat("PlayingSpeed##MakaiButterfly", &PlayingSpeed, 1.f, 100.f);
			_PlayingSpeed = PlayingSpeed;
		}

		{
			if (ImGui::Button("PlayStart##MakaiButterfly"))
				PlayStart(_PlayingSpeed);
			ImGui::SameLine();
			if (ImGui::Button("Reset##MakaiButterfly"))
				Reset();
		}
	}
}

void MakaiButterfly::RenderInit()
{
	SetRenderEnable(true);

	ENGINE::RenderProperty _InitRenderProp;
	_InitRenderProp.bRender = false;
	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{
		{"MakaiButterfly",
		[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		}
	};
	_InitRenderProp.RenderOrders[RenderProperty::Order::Debug] =
	{
		{"Debug" ,
		[this](const DrawInfo& _Info)
			{
				this->RenderDebug(_Info);
			}
		}
	};
	_InitRenderProp.RenderOrders[RenderProperty::Order::Collider] =
	{
		{"Collider" ,
		[this](const DrawInfo& _Info)
		{
			DrawCollider(_Info);
		}
	} };
	RenderInterface::Initialize(_InitRenderProp);
}

void MakaiButterfly::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	if (!_Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[0]))
		return;

	auto WeakSubset = _Mesh->GetSubset(static_cast<uint32>(_SubsetIdx));
	if (auto SharedSubset = WeakSubset.lock();
		SharedSubset)
	{
		Matrix World = _RenderUpdateInfo.World;
		*reinterpret_cast<Vector3*>(&World.m[3][0]) += _BezierDeltaOffsetPos;

		_Info.Fx->SetMatrix("World", &World);
		_Info.Fx->SetTexture("ALB0Map", _ALBMTex->GetTexture());
		_Info.Fx->SetTexture("ALP0Map", _ALPTex->GetTexture());
		_Info.Fx->SetTexture("NRM0Map", _NRMTex->GetTexture());

		_Info.Fx->SetFloat("_BrightScale", _BrightScale);
		_Info.Fx->SetFloat("_SliceAmount", _SliceAmount);

		SharedSubset->Render(_Info.Fx);
	}
}

void MakaiButterfly::RenderDebug(const DrawInfo& _Info)
{
	if (!_Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[0]))
		return;

	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);

	auto WeakSubset = _Mesh->GetSubset(static_cast<uint32>(_SubsetIdx));
	if (auto SharedSubset = WeakSubset.lock();
		SharedSubset)
	{
		SharedSubset->Render(_Info.Fx);
	}
}

MakaiButterfly* MakaiButterfly::Create()
{
	return new MakaiButterfly{};
}

HRESULT MakaiButterfly::Ready()
{
	RenderInit();

	m_nTag = GAMEOBJECTTAG::Eff_MakaiButterFly;

	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.002f, 0.002f, 0.002f });

	_Mesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\mesh_03_creature_makaibutterfly00_00.fbx");
	_ALBMTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\mesh_03_creature_makaibutterfly00_00_ALBM.tga");
	_ALPTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\mesh_03_creature_makaibutterfly00_00_ALP.tga");
	_NRMTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\mesh_03_creature_makaibutterfly00_00_NRM.tga");

	_PlayingSpeed = 25.f;
	_BrightScale = 0.2f;

	_BezierStartOffsetPos = Vector3(0.f, 0.f, 0.f);
	_BezierEndOffsetPos = FMath::Random<Vector3>(Vector3(-0.03f, -0.03f, -0.03f), Vector3(0.03f, 0.03f, 0.03f));
	_BezierDeltaOffsetPos = Vector3(0.f, 0.f, 0.f);

	_Collider = AddComponent<CapsuleCollider>();
	_Collider.lock()->ReadyCollider();
	_Collider.lock()->SetRadius(0.05f);
	_Collider.lock()->SetHeight(0.03f);
	_Collider.lock()->SetCenter({ 0.f, 0.f, 0.f });
	_Collider.lock()->SetActive(false);
	PushEditEntity(_Collider.lock().get());

	_SVMC = AddGameObject<SecretVisionMagicCircle>();

	return S_OK;
}

HRESULT MakaiButterfly::Awake()
{
	return S_OK;
}

HRESULT MakaiButterfly::Start()
{
	return S_OK;
}

UINT MakaiButterfly::Update(const float _fDeltaTime)
{
	Effect::Update(_fDeltaTime);

	if (!_IsPlaying)
		return 0;

	if (MakaiButterfly::VARIATION::MOVE_FORWARD == _VariationIdx)
	{
		if (100.f < _AccumulateTime)
			_IsAlive = false;

		auto WeakTransform = GetComponent<ENGINE::Transform>();
		if (auto SpTransform = WeakTransform.lock();
			SpTransform)
		{
			Vector3 vDir = SpTransform->GetLook(); 
			SpTransform->SetPosition(SpTransform->GetPosition() + vDir * 0.001f);
		}
	}

	if (_IsAlive)
	{
		if (0.0025f < _BrightScale)
		{
			_BrightScale -= 0.2f * _fDeltaTime;
			if (0.0025f > _BrightScale)
				_BrightScale = 0.0025f;
		}

		if (0.f < _SliceAmount)
		{
			_SliceAmount -= _fDeltaTime * 1.f;
			if (0.f > _SliceAmount)
				_SliceAmount = 0.f;
		}
	}
	else
	{
		if (0.2f > _BrightScale)
		{
			_BrightScale += 0.5f * _fDeltaTime;
			if (0.2f < _BrightScale)
				_BrightScale = 0.2f;
		}

		_SliceAmount += _fDeltaTime * 1.f;
		if (1.f <= _SliceAmount)
		{
			_SliceAmount = 1.f;
			_SubsetIdx = 0.f;
			_BezierStartOffsetPos = Vector3(0.f, 0.f, 0.f);
			_BezierEndOffsetPos = Vector3(0.f, 0.f, 0.f);
			_BezierDeltaOffsetPos = Vector3(0.f, 0.f, 0.f);
			Effect::Reset();
		}
	}

	//
	_SubsetIdx += _PlayingSpeed * _fDeltaTime;
	if (MAX_SUBSET_IDX < static_cast<uint32>(_SubsetIdx))
	{
		_SubsetIdx = 0.f;
		_BezierStartOffsetPos = _BezierEndOffsetPos;
		_BezierEndOffsetPos = FMath::Random<Vector3>(Vector3(-0.03f, -0.03f, -0.03f), Vector3(0.03f, 0.03f, 0.03f));
	}

	// BezierCurve
	_BezierDeltaOffsetPos = FMath::BezierCurve(
		_BezierStartOffsetPos,
		Vector3(0.f, 0.f, 0.f),
		_BezierEndOffsetPos, _SubsetIdx / static_cast<float>(MAX_SUBSET_IDX));

	return 0;
}

UINT MakaiButterfly::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void MakaiButterfly::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		Imgui_Modify();
	}
}

void MakaiButterfly::OnEnable()
{
	GameObject::OnEnable();
}

void MakaiButterfly::OnDisable()
{
	GameObject::OnDisable();
}

void MakaiButterfly::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
	if (!_IsAlive)
		return;

	if (MakaiButterfly::VARIATION::STAY == _VariationIdx)
	{
		Matrix ViewInverse;
		Vector3 Dir;

		switch (_pOther.lock()->m_nTag)
		{
		case GAMEOBJECTTAG::TAG_RedQueen:
		case GAMEOBJECTTAG::Tag_Cbs_Middle:
		case GAMEOBJECTTAG::Tag_Cbs_Short:
		case GAMEOBJECTTAG::Tag_Cbs_Long:

			if (auto SpSecretVision = std::static_pointer_cast<SecretVision>(FindGameObjectWithTag(TAG_SecretVision).lock());
				SpSecretVision)
			{
				uint32 idx = SpSecretVision->GetInteractionIdx();
				if (_TotalCnt != idx)
					return;

				// To SecretVision
				SpSecretVision->SetInteractionEnable(true);

				// To BtlPannel
				if (auto SpPanel = std::static_pointer_cast<BtlPanel>(FindGameObjectWithTag(UI_BtlPanel).lock());
					SpPanel)
				{
					SpPanel->AddRankScore(10.f);
					SpPanel->AddExGauge(3.f);
					SpPanel->ActivateSecretVision(idx);
				}

				// SVMC
				_SVMC.lock()->SetTexID((SecretVisionMagicCircle::TexID)idx);
				_SVMC.lock()->PlayStart(m_pTransform.lock()->GetPosition());

				//
				Reset();

				++_TotalCnt;
			}
			else
			{
				// To BtlPannel
				if (auto SpPanel = std::static_pointer_cast<BtlPanel>(FindGameObjectWithTag(UI_BtlPanel).lock());
					SpPanel)
				{
					SpPanel->AddRankScore(10.f);
					SpPanel->AddExGauge(3.f);
				}

				// SVMC
				_SVMC.lock()->SetTexID((SecretVisionMagicCircle::TexID)FMath::Random<uint32>(0u, 2u));
				_SVMC.lock()->PlayStart(m_pTransform.lock()->GetPosition());

				//
				Reset();
			}

			break;
		}
	}
}