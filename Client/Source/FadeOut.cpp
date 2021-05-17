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
	// 렌더를 수행해야하는 오브젝트라고 (렌더러에 등록 가능 ) 알림.
	// 렌더 인터페이스 상속받지 않았다면 키지마세요.
	SetRenderEnable(true);

	// 렌더 정보 초기화 ...
	ENGINE::RenderProperty _InitRenderProp;
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	// 렌더 속성 전체 초기화 
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
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
	NoiseMap  = Resources::Load<ENGINE::Texture>("..\\..\\Usable\\cs_threshold_50_000_msk3.tga");

	AlbmMap[0] = Resources::Load<Texture>("..\\..\\Usable\\SecretVision\\3.tga");
	AlbmMap[1] = Resources::Load<Texture>("..\\..\\Usable\\SecretVision\\5.tga");
	AlbmMap[2] = Resources::Load<Texture>("..\\..\\Usable\\SecretVision\\7.tga");
	AlbmMap[3] = Resources::Load<Texture>("..\\..\\Usable\\SecretVision\\9.tga");
	AlbmMap[4] = Resources::Load<Texture>("..\\..\\Usable\\SecretVision\\10.tga");
	AlbmMap[5] = Resources::Load<Texture>("..\\..\\Usable\\SecretVision\\14.tga");
	AlbmMap[6] = Resources::Load<Texture>("..\\..\\Usable\\SecretVision\\serectvision00_mark_alba.tga");
	AlbmMap[7] = Resources::Load<Texture>("..\\..\\Usable\\SecretVision\\sv00_alba.tga");

	_RenderProperty.bRender = false;
};

void FadeOut::PlayStart(const uint32 AlbIdx , const std::function<void()>& PlayEndCallBack)
{
	T = 0.0f;
	_RenderProperty.bRender = true;
	this->AlbIdx = AlbIdx;
	this->PlayEndCallBack = PlayEndCallBack;
};

void FadeOut::PlayEnd()
{
	_RenderProperty.bRender = false;
	T = 0.0f;
};

void FadeOut::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	auto _Renderer = Renderer::GetInstance();

	// 검은색 블랙 아웃 
	if (_Info.PassIndex == 1)
	{
		if (auto _Quad = _Renderer->GetQuad().lock();
			_Quad)
		{
			Vector2  PixelSize{ 1.f, 1.f };
			PixelSize.x = 1.0f / static_cast<float> (_Renderer->_RenderInfo.Viewport.Width);
			PixelSize.y = -1.0f / static_cast<float> (_Renderer->_RenderInfo.Viewport.Height);
			_Info.Fx->SetFloat("Time", (T / EndTime));
			_Info.Fx->SetFloatArray("PixelSize", PixelSize, 2u);
			// 여기서 변수 바인딩 . 
			_Quad->Render(_Info._Device, 1.f, 1.f, _Info.Fx);		
		}
		return;
	}
	// 페이드 아웃
	else if (_Info.PassIndex == 0)
	{
		if (auto _Quad = _Renderer->GetQuad().lock();
			_Quad)
		{
			Vector2  PixelSize{ 1.f, 1.f };
			PixelSize.x = 1.0f / static_cast<float> (_Renderer->_RenderInfo.Viewport.Width);
			PixelSize.y = -1.0f / static_cast<float> (_Renderer->_RenderInfo.Viewport.Height);
			_Info.Fx->SetTexture("AlbmMap", AlbmMap[AlbIdx]->GetTexture());
			_Info.Fx->SetTexture("NoiseMap", NoiseMap->GetTexture());
			const float LerpT = FMath::Clamp(T, 0.f, 1.f);
			//const float Time = TimeSystem::GetInstance()->AccTime();
			_Info.Fx->SetFloat("Time", T);
			_Info.Fx->SetFloat("Noisewrap", FMath::Lerp(0.f, Noisewrap, LerpT));
			_Info.Fx->SetFloat("Timecorr", FMath::Lerp(0.f, Timecorr, LerpT));
			_Info.Fx->SetFloat("NoiseIntencity", FMath::Lerp(0.f, NoiseIntencity, LerpT));
			_Info.Fx->SetFloat("DistortionIntencity", FMath::Lerp(0.f, DistortionIntencity, LerpT));
			_Info.Fx->SetFloat("Intencity", FMath::Lerp(0.f, Intencity, LerpT));
			const float ColorLerpT = FMath::Clamp(T - 0.5f, 0.f, 1.f);
			const Vector3 LerpDistortionColor = FMath::Lerp(DistortionColor[0], DistortionColor[1], ColorLerpT);
			_Info.Fx->SetFloatArray("DistortionColor", LerpDistortionColor, 3u);

			_Info.Fx->SetFloatArray("PixelSize", PixelSize, 2u);
			// 여기서 변수 바인딩 . 
			_Quad->Render(_Info._Device, 1.f, 1.f, _Info.Fx);
		}
		return;
	}
}


HRESULT FadeOut::Ready()
{
	RenderInit();
	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.
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
	T += _fDeltaTime * Acceleration;
	if ( ( T/EndTime) > 1.f )
	{
		PlayEnd();
		if (PlayEndCallBack)
		{
			PlayEndCallBack();
		}
	}
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
			PlayStart(AlbIdx, {});
		}

		ImGui::Text("T : %2.6f", T);

		ImGui::SliderFloat("T", &T, 0.0f, 1.f);
		ImGui::SliderFloat("Acceleration", &Acceleration, 0.0f, 10.f);
		ImGui::SliderFloat("EndTime", &EndTime, 0.0f, 10.f);
		ImGui::SliderFloat("NoiseIntencity", &NoiseIntencity, 0.0f, 10.f, "%2.6f");
		ImGui::SliderFloat("Intencity", &Intencity, 0.0f, 10.f, "%2.6f");
		ImGui::SliderFloat("Noisewrap", &Noisewrap, 0.0f, 20.f, "%2.6f");
		ImGui::SliderFloat("Timecorr", &Timecorr, 0.0f, 10.f, "%2.6f");
		ImGui::ColorEdit4("DistortionColor", DistortionColor[1]);
		ImGui::SliderFloat("DistortionIntencity", &DistortionIntencity, 0.0f, 100.f, "%2.6f");
		ImGui::SliderInt("AlbIdx", &AlbIdx, 0, AlbmMap.size()-1, "%d");
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



