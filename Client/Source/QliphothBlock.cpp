#include "stdafx.h"
#include "..\Header\QliphothBlock.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include "MainCamera.h"

void QliphothBlock::Free()
{
	GameObject::Free();
}

std::string QliphothBlock::GetName()
{
	return "QliphothBlock";
}

void QliphothBlock::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const Vector3 Scale = _SpTransform->GetScale();
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();

		const auto& _Subset = _BaseMesh->GetSubset(0);
		const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

		_RenderUpdateInfo.SubsetCullingSphere.resize(1);
		_RenderUpdateInfo.SubsetCullingSphere[0] = _CurBS.Transform(_RenderUpdateInfo.World, Scale.x);	// _Subset을 회전만 하기 떄문에 부모꺼 그대로 씀
	}
}

void QliphothBlock::PlayStart(const float PlayingSpeed)
{
	Effect::PlayStart(PlayingSpeed);
	_IsAlive = true;

	if (auto SpCollider = _Collider.lock();
		SpCollider)
	{
		SpCollider->SetActive(true);
	}
}

void QliphothBlock::Reset()
{
	_IsAlive = false;

	if (auto SpCollider = _Collider.lock();
		SpCollider)
	{
		SpCollider->SetActive(false);
	}
}

void QliphothBlock::Imgui_Modify()
{
	if (auto Sptransform = GetComponent<ENGINE::Transform>().lock();
		Sptransform)
	{
		ImGui::Text("Eff_QliphothBlock");

		{
			static Vector3 SliderPosition = Sptransform->GetPosition();
			//ImGui::SliderFloat3("Pos##QliphothBlock", SliderPosition, -10.f, 10.f);
			ImGui::InputFloat3("Pos##QliphothBlock", SliderPosition);
			Sptransform->SetPosition(SliderPosition);
		}

		{
			static float Scale = Sptransform->GetScale().x;
			ImGui::SliderFloat("Scale##QliphothBlock", &Scale, 0.001f, 0.1f);
			Sptransform->SetScale({ Scale, Scale, Scale });	// x만 유효
		}

		{
			static Vector3 SliderRotation{ 0,0,0 };
			ImGui::SliderFloat3("Rot##QliphothBlock", SliderRotation, 0.f, 360.f);
			Sptransform->SetRotation(SliderRotation);
		}

		{
			static float PlayingSpeed = _PlayingSpeed;
			ImGui::SliderFloat("PlayingSpeed##QliphothBlock", &PlayingSpeed, 0.1f, 10.f);
			_PlayingSpeed = PlayingSpeed;
		}

		//{
		//	static float SliceAmount = _SliceAmount;
		//	ImGui::SliderFloat("SliceAmount##QliphothBlock", &SliceAmount, 0.f, 1.f);
		//	_SliceAmount = SliceAmount;
		//}

		{
			if (ImGui::Button("PlayStart##QliphothBlock"))
				PlayStart(_PlayingSpeed);
			ImGui::SameLine();
			if (ImGui::Button("Reset##QliphothBlock"))
				Reset();
		}
	}
}

QliphothBlock* QliphothBlock::Create()
{
	return new QliphothBlock{};
}

void QliphothBlock::RenderInit()
{
	SetRenderEnable(true);

	ENGINE::RenderProperty _InitRenderProp;
	_InitRenderProp.bRender = false;
	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{
		{"QliphothBlock",
		[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		},
	};
	_InitRenderProp.RenderOrders[RenderProperty::Order::Collider] =
	{
		{"Collider" ,
		[this](const DrawInfo& _Info)
			{
			this->DrawCollider(_Info);
			}
		} 
	};

	RenderInterface::Initialize(_InitRenderProp);
}

void QliphothBlock::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	if (!_Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[0]))
		return;

	//auto RefEffInfo = std::any_cast<EffectInfo>((_Info.BySituation));

	if (0 == _Info.PassIndex)
	{
		auto WeakSubset = _BaseMesh->GetSubset(0u);
		if (auto SharedSubset = WeakSubset.lock();
			SharedSubset)
		{
			_Info.Fx->SetTexture("NoiseMap", _NoiseTex->GetTexture());
			_Info.Fx->SetFloat("_BrightScale", _BrightScale);
			_Info.Fx->SetFloat("_AccumulationTexU", _AccumulateTime * 0.2f);
			_Info.Fx->SetFloat("_AccumulationTexV", _AccumulateTime * 0.2f);
			_Info.Fx->SetFloat("_SliceAmount", _SliceAmount);

			// 깊이스케일 조절 하고 싶으면 바인드 .
			 // Fx -> ???????? RefEffInfo.SoftParticleDepthBiasScale 

			for (int i = 0; i < 3; ++i)
			{
				Matrix Rot;
				D3DXMatrixRotationZ(&Rot, D3DXToRadian(45.f * i));
				Rot *= _RenderUpdateInfo.World;
				_Info.Fx->SetMatrix("World", &Rot);

				SharedSubset->Render(_Info.Fx);
			}

			// 깊이스케일 조절 했으면 원래대로 복구 . 해주세요 ~  
		}

		return;
	}
	else if (1 == _Info.PassIndex)
	{
		auto WeakSubset = _BaseInnerMesh->GetSubset(0u);
		if (auto SharedSubset = WeakSubset.lock();
			SharedSubset)
		{
			_Info.Fx->SetTexture("ALP0Map", _BaseInnerTex->GetTexture());
			_Info.Fx->SetTexture("NoiseMap", _NoiseTex->GetTexture());
			_Info.Fx->SetFloat("_BrightScale", _BrightScale);
			_Info.Fx->SetFloat("_AccumulationTexU", _AccumulateTime * 0.15f);
			_Info.Fx->SetFloat("_AccumulationTexV", _AccumulateTime * 0.15f);
			_Info.Fx->SetFloat("_SliceAmount", _SliceAmount);

			// 깊이스케일 조절 하고 싶으면 바인드 .
			 // Fx -> ???????? RefEffInfo.SoftParticleDepthBiasScale 

			Matrix temp;
			D3DXMatrixScaling(&temp, 0.8f, 0.8f, 0.8f);
			temp *= _RenderUpdateInfo.World;
			_Info.Fx->SetMatrix("World", &_RenderUpdateInfo.World);

			SharedSubset->Render(_Info.Fx);

			// 깊이스케일 조절 했으면 원래대로 복구 . 해주세요 ~  
		}

		return;
	}
}


HRESULT QliphothBlock::Ready()
{
	RenderInit();

	m_nTag = GAMEOBJECTTAG::Eff_QliphothBlock;

	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.05f, 0.05f, 0.05f });
	
	_BaseMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\mesh_03_props_sm7001_001_00.fbx");
	_BaseInnerMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\mesh_03_environment_lc_00.fbx");

	_BaseInnerTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\ex_capcom_smoke_00_0037_alpg.tga");
	_NoiseTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\noiseInput_ATOS.tga");

	_PlayingSpeed = 1.f;
	_BrightScale = 0.015f;

	_Collider = AddComponent<BoxCollider>();
	if (auto SpCollider = _Collider.lock(); SpCollider)
	{
		SpCollider->ReadyCollider();
		SpCollider->SetSize(Vector3{ 2.2f, 2.2f, 0.125f });	// 무지성 2.2배. 나중에 문제생기면 수정 ㅎㅎ
		PushEditEntity(SpCollider.get());
	}

	Reset();

	return S_OK;
};

HRESULT QliphothBlock::Awake()
{
	return S_OK;
}

HRESULT QliphothBlock::Start()
{
	return S_OK;
}

UINT QliphothBlock::Update(const float _fDeltaTime)
{
	Effect::Update(_fDeltaTime);

	if (!_IsPlaying)
		return 0;
	else
	{
		if (_IsAlive)
		{
			if (0.f < _SliceAmount)
			{
				_SliceAmount -= _fDeltaTime * 1.f;
				if (0.f > _SliceAmount)
					_SliceAmount = 0.f;
			}
		}
		else
		{
			_SliceAmount += _fDeltaTime * 0.5f;
			if (1.f <= _SliceAmount)
			{
				_SliceAmount = 1.f;

				Effect::Reset();
			}
		}
	}

	return 0;
}

UINT QliphothBlock::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void QliphothBlock::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		Imgui_Modify();
	}
}

void QliphothBlock::OnEnable()
{
	GameObject::OnEnable();
}

void QliphothBlock::OnDisable()
{
	GameObject::OnDisable();
}