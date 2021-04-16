#include "stdafx.h"
#include "..\Header\DashImpact.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"


void DashImpact::Free()
{
}
std::string DashImpact::GetName()
{
	return "DashImpact";
};

DashImpact* DashImpact::Create()
{
	return new DashImpact{};
}


void DashImpact::RenderAlphaBlendEffectImplementation(
	const DrawInfo& _ImplInfo)
{
	auto WeakSubset = m_ImpactMesh->GetSubset(0u);
	
	if (auto SharedSubset = WeakSubset.lock();
		SharedSubset)
	{

		_ImplInfo.Fx->SetTexture("ColorMap", m_ImpactTexture->GetTexture());
		_ImplInfo.Fx->SetFloat("fAlpha", m_fAlpha);

		_ImplInfo.Fx->BeginPass(0);
		SharedSubset->Render(_ImplInfo.Fx);
		_ImplInfo.Fx->EndPass();

	}
};


HRESULT DashImpact::Ready()
{
	SetRenderEnable(true);

	ENGINE::RenderProperty _InitRenderProp;
	_InitRenderProp.bRender = true;
	/*_InitRenderProp.RenderOrders =
	{
		ENGINE::RenderProperty::Order::AlphaBlendEffect,
		ENGINE::RenderProperty::Order::Debug
	};
	RenderInterface::Initialize(_InitRenderProp);

	_ShaderInfo.RegistShader(ENGINE::RenderProperty::Order::AlphaBlendEffect,
		L"..\\..\\Resource\\Shader\\Effect\\DashImpact.hlsl", {});*/

	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.001f, 0.001f, 0.001f });


	m_ImpactMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\mesh_primitive\\plane00.fbx");
	m_ImpactTexture = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\Dash_Impact\\DashImpact.tga");

	return S_OK;
};

HRESULT DashImpact::Awake()
{
	return S_OK;
}

HRESULT DashImpact::Start()
{
	return S_OK;
}

UINT DashImpact::Update(const float _fDeltaTime)
{


	if (auto Sptransform = GetComponent<ENGINE::Transform>().lock();
		Sptransform)
	{
		{
			Vector3 SliderPosition = Sptransform->GetPosition();
			ImGui::SliderFloat3("Position", SliderPosition, -10.f, 10.f);
			Sptransform->SetPosition(SliderPosition);
		}

		{
			float AllScale = Sptransform->GetScale().x;
			ImGui::SliderFloat("All Scale", &AllScale, 0.001f, 1.f);
			Sptransform->SetScale({ AllScale,AllScale,AllScale });
		}
		static Vector3 Rotation{ 0,0,0 };
		if (ImGui::SliderAngle("Light Pitch", &Rotation.x) ||
			ImGui::SliderAngle("Light Yaw", &Rotation.y) ||
			ImGui::SliderAngle("Light Roll", &Rotation.z))
		{
			Sptransform->SetRotation({ Rotation.x, Rotation.y, Rotation.z });
		}
	}


	auto Transform = GetComponent<ENGINE::Transform>().lock();

	if (Input::GetKeyDown(DIK_T))
	{
		if (m_bTest == true)
		{
			m_bTest = false;
		}
		else
			m_bTest = true;
	}

	if (m_bTest == true)
	{
		m_fAlpha -= _fDeltaTime ;
		m_vTest.x += _fDeltaTime * 0.5f;
		m_vTest.y += _fDeltaTime * 0.5f;
		m_vTest.z += _fDeltaTime * 0.5f;
		
		Transform->SetScale(m_vTest);
	}

	if (m_fAlpha <= -0.7f)
	{
		m_fAlpha = -0.2f;
		m_vTest = { 0.01f, 0.01f, 0.01f };
		Transform->SetScale(m_vTest);
		m_bTest = false;
	}

	return 0;
}

UINT DashImpact::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void DashImpact::Editor()
{
	GameObject::Editor();
}

void DashImpact::OnEnable()
{

}

void DashImpact::OnDisable()
{

}