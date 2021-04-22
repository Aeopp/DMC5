#include "stdafx.h"
#include "..\Header\OvertureHand.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"


void OvertureHand::Free()
{
	GameObject::Free();
}

std::string OvertureHand::GetName()
{
	return "OvertureHand";
}

void OvertureHand::Reset()
{
	_RandTexV0 = FMath::Random<float>(0.75f, 0.9f);
	_RandTexV1 = FMath::Random<float>(0.4f, 0.8f);

	Effect::Reset();
}

void OvertureHand::Imgui_Modify()
{
	if (auto Sptransform = GetComponent<ENGINE::Transform>().lock();
		Sptransform)
	{
		//
		ImGui::Text("Eff_OvertureHand");
		{
			Vector3 SliderPosition = Sptransform->GetPosition();
			ImGui::SliderFloat3("Pos##OvertureHand", SliderPosition, -10.f, 10.f);
			Sptransform->SetPosition(SliderPosition);
		}

		{
			float AllScale = Sptransform->GetScale().x;
			ImGui::SliderFloat("Scale##OvertureHand", &AllScale, 0.01f, 1.f);
			Sptransform->SetScale({ AllScale,AllScale,AllScale });
		}

		{
			Vector3 SliderRotation{ 0,0,0 };
			ImGui::SliderFloat3("Rot##OvertureHand", SliderRotation, 0.f, 360.f);
			Sptransform->SetRotation(SliderRotation);
		}
	}
}

OvertureHand* OvertureHand::Create()
{
	return new OvertureHand{};
}


void OvertureHand::RenderAlphaBlendEffect(const DrawInfo& _ImplInfo)
{
	auto WeakSubset = _HandMesh->GetSubset(0u);
	if (auto SharedSubset = WeakSubset.lock();
		SharedSubset)
	{
		if (0 == _ImplInfo.PassIndex)
		{
			if (0.2f > _AccumulateTime)
			{
				_ImplInfo.Fx->SetMatrix("World", &_RenderUpdateInfo.World);
				_ImplInfo.Fx->SetTexture("NoiseMap", _NoiseTex->GetTexture());
				_ImplInfo.Fx->SetTexture("ALB0Map", _GlowTex->GetTexture());
				_ImplInfo.Fx->SetFloat("_TexV", _RandTexV1);
				_ImplInfo.Fx->SetFloat("_SliceAmount", 1.f - _AccumulateTime * 5.f);

				SharedSubset->Render(_ImplInfo.Fx);
			}
			else if (0.6f > _AccumulateTime)
			{
				_ImplInfo.Fx->SetMatrix("World", &_RenderUpdateInfo.World);
				_ImplInfo.Fx->SetTexture("NoiseMap", _NoiseTex->GetTexture());
				_ImplInfo.Fx->SetTexture("ALB0Map", _GlowTex->GetTexture());
				_ImplInfo.Fx->SetFloat("_TexV", _RandTexV1);
				_ImplInfo.Fx->SetFloat("_SliceAmount", _AccumulateTime);

				SharedSubset->Render(_ImplInfo.Fx);
			}
		}
		else if (1 == _ImplInfo.PassIndex)
		{
			if (0.3f < _AccumulateTime)
			{
				_ImplInfo.Fx->SetMatrix("World", &_RenderUpdateInfo.World);
				_ImplInfo.Fx->SetTexture("ALB0Map", _LightningColorTex->GetTexture());
				_ImplInfo.Fx->SetTexture("AlphaMap", _LightningTex->GetTexture());
				_ImplInfo.Fx->SetFloat("_TexV", _RandTexV0);
				_ImplInfo.Fx->SetFloat("_SliceAmount", (_AccumulateTime - 0.3f) * 0.8f);

				SharedSubset->Render(_ImplInfo.Fx);
			}
		}
	}
}


HRESULT OvertureHand::Ready()
{
	SetRenderEnable(true);

	m_nTag = GAMEOBJECTTAG::Eff_OvertureHand;

	ENGINE::RenderProperty _InitRenderProp;
	_InitRenderProp.bRender = false;
	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{
		{"OvertureHand",
		[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		},
	};
	RenderInterface::Initialize(_InitRenderProp);

	auto InitTransform =  GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.01f, 0.01f, 0.01f });

	_HandMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\wp00_010_0000.fbx");
	_LightningTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\lightning.dds");
	_GlowTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Light\\tex_capcom_light_glow_0002_alpg.tga");
	_LightningColorTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\lightning_alb.png");
	_NoiseTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\noiseInput_ATOS.tga");

	_PlayingSpeed = 1.f;

	Reset();	// PlayStart()·Î Àç»ý

	//PushEditEntity(_HandMesh.get());
	//PushEditEntity(_LightningTex.get());
	//PushEditEntity(_GlowTex.get());
	//PushEditEntity(_LightningColorTex.get());
	//PushEditEntity(_NoiseTex.get());

	return S_OK;
}

HRESULT OvertureHand::Awake()
{
	return S_OK;
}

HRESULT OvertureHand::Start()
{
	return S_OK;
}

UINT OvertureHand::Update(const float _fDeltaTime)
{
	Effect::Update(_fDeltaTime);

	if (1.5f < _AccumulateTime)
		Reset();

	//
	//Imgui_Modify();

	return 0;
}

UINT OvertureHand::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void OvertureHand::Editor()
{
	GameObject::Editor();
}

void OvertureHand::OnEnable()
{
	GameObject::OnEnable();
}

void OvertureHand::OnDisable()
{
	GameObject::OnDisable();
}