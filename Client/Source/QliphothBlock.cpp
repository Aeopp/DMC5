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
	auto WeakSubset = _Pipe0Mesh->GetSubset(0u);
	if (auto SharedSubset = WeakSubset.lock();
		SharedSubset)
	{
		_Info.Fx->SetMatrix("World", &_RenderUpdateInfo.World);
		_Info.Fx->SetTexture("ALB0Map", _QliphothALB0Tex->GetTexture());
		_Info.Fx->SetTexture("NRMR0Map", _QliphothNRMR0Tex->GetTexture());
		_Info.Fx->SetTexture("NoiseMap", _NoiseTex->GetTexture());
		// + 빛방향
		_Info.Fx->SetFloat("_SliceAmount", _SliceAmount);

		SharedSubset->Render(_Info.Fx);
	}
}


HRESULT QliphothBlock::Ready()
{
	RenderInit();

	m_nTag = GAMEOBJECTTAG::Eff_QliphothBlock;

	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.1f, 0.1f, 0.1f });
	
	_Pipe0Mesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\pipe00.fbx");

	_QliphothALB0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\em5100_10_ALBA.tga");
	_QliphothNRMR0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\qliphoth_perfection_01_NRMR.tga");
	_QliphothNRMR1Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\em5100_Liquid_NRMR.tga");

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
	Imgui_Modify();

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