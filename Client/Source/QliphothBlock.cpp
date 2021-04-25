#include "stdafx.h"
#include "..\Header\QliphothBlock.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"


void QliphothBlock::Free()
{
	GameObject::Free();
}

std::string QliphothBlock::GetName()
{
	return "QliphothBlock";
};

void QliphothBlock::Reset()
{
	_SliceAmount = 0.f;

	Effect::Reset();
}

void QliphothBlock::Imgui_Modify()
{
	if (auto Sptransform = GetComponent<ENGINE::Transform>().lock();
		Sptransform)
	{
		ImGui::Text("Eff_QliphothBlock");

		{
			static Vector3 SliderPosition = Sptransform->GetPosition();
			ImGui::SliderFloat3("Pos##QliphothBlock", SliderPosition, -10.f, 10.f);
			Sptransform->SetPosition(SliderPosition);
		}

		{
			static float Scale = Sptransform->GetScale().x;
			ImGui::SliderFloat("Scale##QliphothBlock", &Scale, 0.1f, 1.f);
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

	RenderInterface::Initialize(_InitRenderProp);
}

void QliphothBlock::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	auto RefEffInfo = std::any_cast<EffectInfo>((_Info.BySituation));

	auto WeakSubset = _BaseMesh->GetSubset(0u);
	if (auto SharedSubset = WeakSubset.lock();
		SharedSubset)
	{
		if (0 == _Info.PassIndex)
		{
			_Info.Fx->SetTexture("NoiseMap", _NoiseTex->GetTexture());
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
		}
		else if (1 == _Info.PassIndex)
		{

		}

		// 깊이스케일 조절 했으면 원래대로 복구 . 해주세요 ~  
	}
}


HRESULT QliphothBlock::Ready()
{
	RenderInit();

	m_nTag = GAMEOBJECTTAG::Eff_QliphothBlock;

	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.05f, 0.05f, 0.05f });
	
	_BaseMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\mesh_03_props_sm7001_001_00.fbx");

	_NoiseTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\noiseInput_ATOS.tga");

	_PlayingSpeed = 1.f;

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

	//
	//if (5.f < _AccumulateTime)
	//	Reset();
	//else if (4.f < _AccumulateTime)
	//	_SliceAmount =  (_AccumulateTime - 4.f) * 0.6f;
	//else
	//	_SliceAmount = 1.f - _AccumulateTime * 0.6f;

	//
	//Imgui_Modify();

	return 0;
}

UINT QliphothBlock::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void QliphothBlock::Editor()
{
	GameObject::Editor();
}

void QliphothBlock::OnEnable()
{
	GameObject::OnEnable();
}

void QliphothBlock::OnDisable()
{
	GameObject::OnDisable();
}