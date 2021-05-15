#include "stdafx.h"
#include "..\Header\FadeOut.h"
#include "Transform.h"
#include "Subset.h"
#include "TimeSystem.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "ParticleSystem.h"
#include "Renderer.h"

void FadeOut::Free()
{
	GameObject::Free();
};

std::string FadeOut::GetName()
{
	return "FadeOut";
};

FadeOut* FadeOut::Create()
{
	return new FadeOut{};
};

void FadeOut::RenderReady()
{
};

void FadeOut::RenderInit()
{
	m_nTag  = Eff_FadeOut;
	// ������ �����ؾ��ϴ� ������Ʈ��� (�������� ��� ���� ) �˸�.
	// ���� �������̽� ��ӹ��� �ʾҴٸ� Ű��������.
	SetRenderEnable(true);

	// ���� ���� �ʱ�ȭ ...
	ENGINE::RenderProperty _InitRenderProp;
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
	// ���� �Ӽ� ��ü �ʱ�ȭ 
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
	_InitRenderProp.bRender = false;
	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{
		{"FadeOut",
		[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		}
	};

	RenderInterface::Initialize(_InitRenderProp);
	NoiseMap  = Resources::Load<ENGINE::Texture>("..\\..\\Resource\\Texture\\Sky\\mission02\\smoke_01_iam.tga");
	AlbmMap = Resources::Load<ENGINE::Texture>("..\\..\\Usable\\SecretVision\\13.tga");
	_RenderProperty.bRender = false;
};

void FadeOut::PlayStart()
{
	T = 0.0f;
	_RenderProperty.bRender = true;
};

void FadeOut::PlayEnd()
{
	_RenderProperty.bRender = false;
	T = 0.0f;
};

void FadeOut::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	auto _Renderer = Renderer::GetInstance();
	
	if (auto _Quad = _Renderer->GetQuad().lock();
		_Quad)
	{
		Vector2  PixelSize{ 1.f, 1.f };
		PixelSize.x = 1.0f / static_cast<float > ( _Renderer->_RenderInfo.Viewport.Width );
		PixelSize.y = -1.0f /static_cast<float > ( _Renderer->_RenderInfo.Viewport.Height);
		_Info.Fx->SetTexture("AlbmMap",AlbmMap->GetTexture());
		_Info.Fx->SetTexture("NoiseMap",NoiseMap->GetTexture());
		//const float Time = TimeSystem::GetInstance()->AccTime();
		_Info.Fx->SetFloat("Time", T);
		_Info.Fx->SetFloat("Noisewrap",Noisewrap);
		_Info.Fx->SetFloat("Timecorr",Timecorr);
		_Info.Fx->SetFloat("DistortionIntencity", DistortionIntencity);
		_Info.Fx->SetFloat("Intencity", Intencity);
		_Info.Fx->SetFloatArray("PixelSize", PixelSize, 2u);
		_Info.Fx->SetVector("DistortionColor", &DistortionColor);
		// ���⼭ ���� ���ε� . 
		_Quad->Render(_Info._Device, 1.f, 1.f, _Info.Fx);
	}
}


HRESULT FadeOut::Ready()
{
	RenderInit();
	// �������� ������ �ް���� ������Ʈ�� Raw �����ͷ� Ǫ��.
	return S_OK;
};

HRESULT FadeOut::Awake()
{
	GameObject::Awake();
	return S_OK;
}

HRESULT FadeOut::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT FadeOut::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	if (_RenderProperty.bRender == false)return 0;
	T += _fDeltaTime;
	return 0;
};


UINT FadeOut::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);
	if (_RenderProperty.bRender == false)return 0;
	return 0;
}

void FadeOut::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		if (ImGui::SmallButton("Play"))
		{
			PlayStart();
		}

		ImGui::Text("T : %2.6f", T);
		ImGui::SliderFloat("T", &T, 0.0f, 1.f);
		ImGui::SliderFloat("Intencity", &Intencity, 0.0f, 10.f, "%2.6f");
		ImGui::SliderFloat("Noisewrap", &Noisewrap, 0.0f, 20.f, "%2.6f");
		ImGui::SliderFloat("Timecorr", &Timecorr, 0.0f, 10.f, "%2.6f");
		ImGui::ColorEdit4("DistortionColor", DistortionColor);
		ImGui::SliderFloat("DistortionIntencity", &DistortionIntencity, 0.0f, 100.f, "%2.6f");
	}
}


void FadeOut::OnEnable()
{
	GameObject::OnEnable();
}

void FadeOut::OnDisable()
{
	GameObject::OnDisable();
}



