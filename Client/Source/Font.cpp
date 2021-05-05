#include "stdafx.h"
#include "..\Header\Font.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"


void Font::Free()
{
	GameObject::Free();
}

std::string Font::GetName()
{
	return "Font";
}

Font* Font::Create()
{
	return new Font{};
}

void Font::RenderFont(const DrawInfo& _Info)
{
	if (_Info.IsAfterPostProcessing)
	{
		auto WeakSubset = _PlaneMesh->GetSubset(0u);
		if (auto SharedSubset = WeakSubset.lock();
			SharedSubset)
		{
			_Info.Fx->SetTexture("ALB0Map", _Dmc5FontTex->GetTexture());
			_Info.Fx->SetFloat("_UsingNoise", _UsingNoise);
			if (_UsingNoise)
				_Info.Fx->SetTexture("NoiseMap", _NoiseTex->GetTexture());
			_Info.Fx->SetFloat("_BrightScale", 1.f);
			_Info.Fx->SetFloat("_SliceAmount", 0.f);

			if (!_UsingPerspective)
			{
				CreateScreenMat();
				_Info.Fx->SetMatrix("ScreenMat", &_ScreenMat);

				SetFontUV();
				_Info.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
				_Info.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);

				_Info.Fx->BeginPass(0);
				SharedSubset->Render(_Info.Fx);
				_Info.Fx->EndPass();
			}
			else if (_UsingPerspective)
			{
				_Info.Fx->SetMatrix("Perspective", &_PerspectiveProjMatrix);
			
				// ...
			}
		}
	}
}

void Font::CreateScreenMat()
{
	Matrix RotMat;
	D3DXMatrixIdentity(&_ScreenMat);

	_ScreenMat._11 = _Scale.x;
	_ScreenMat._22 = _Scale.y;
	_ScreenMat._33 = _Scale.z;
	if (_UsingPerspective)
	{
		D3DXMatrixRotationX(&RotMat, D3DXToRadian(_Rot.x));
		_ScreenMat *= RotMat;
		D3DXMatrixRotationY(&RotMat, D3DXToRadian(_Rot.y));
		_ScreenMat *= RotMat;
	}
	D3DXMatrixRotationZ(&RotMat, D3DXToRadian(_Rot.z));
	_ScreenMat *= RotMat;
	_ScreenMat._41 = _Pos.x - (g_nWndCX >> 1);
	_ScreenMat._42 = -(_Pos.y - (g_nWndCY >> 1));
	_ScreenMat._43 = _Pos.z;
}

void Font::SetFontUV()
{
	_MinTexUV = Vector2(0.f, 0.f);
	_MaxTexUV = Vector2(1.f, 1.f);
}

void Font::RenderReady()
{

}

HRESULT Font::Ready()
{
	SetRenderEnable(true);

	m_nTag = GAMEOBJECTTAG::UI_Font;

	ENGINE::RenderProperty _InitRenderProp;
	_InitRenderProp.bRender = true;
	_InitRenderProp.RenderOrders[RenderProperty::Order::UI] =
	{
		{"Font",
		[this](const DrawInfo& _Info)
			{
				this->RenderFont(_Info);
			}
		},
	};
	RenderInterface::Initialize(_InitRenderProp);

	_PlaneMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx");
	_NoiseTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\noiseInput_ATOS.tga");
	_Dmc5FontTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Font\\Texture\\dmc5font.tga");

	D3DXMatrixPerspectiveFovLH(&_PerspectiveProjMatrix, D3DXToRadian(2.5f), (float)g_nWndCX / g_nWndCY, 0.1f, 1.f);
		 
	return S_OK;
}

HRESULT Font::Awake()
{
	return S_OK;
}

HRESULT Font::Start()
{
	return S_OK;
}

UINT Font::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);

	return 0;
}          

UINT Font::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void Font::Editor()
{
	GameObject::Editor();
}

void Font::OnEnable()
{
	GameObject::OnEnable();
}

void Font::OnDisable()
{
	GameObject::OnDisable();
}