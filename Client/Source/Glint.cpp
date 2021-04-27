#include "stdafx.h"
#include "..\Header\Glint.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"


void Glint::Free()
{
	GameObject::Free();
}

std::string Glint::GetName()
{
	return "Glint";
}

void Glint::Reset()
{
	for (uint32 i = 0u; i < 3u; ++i)
	{
		_SliceAmount[i] = 1.f;
		_Scale[i] = 0.1f;
		D3DXMatrixIdentity(&_WorldMatrix[i]);
	}
	_Interval = 0.f;

	Effect::Reset();
}

void Glint::Imgui_Modify()
{
	if (auto Sptransform = GetComponent<ENGINE::Transform>().lock();
		Sptransform)
	{
		ImGui::Text("Eff_Glint");
		{
			static Vector3 SliderPosition = Sptransform->GetPosition();
			ImGui::SliderFloat3("Pos##Glint", SliderPosition, -10.f, 10.f);
			Sptransform->SetPosition(SliderPosition);
		}

		{
			static float Scale = Sptransform->GetScale().x;
			ImGui::SliderFloat("Scale##Glint", &Scale, 0.1f, 10.f);
			//Sptransform->SetScale({ Scale, Scale, Scale });	// x만 유효
			SetScale(Scale);
		}

		{
			static float PlayingSpeed = _PlayingSpeed;
			ImGui::SliderFloat("PlayingSpeed##Glint", &PlayingSpeed, 0.1f, 10.f);
			_PlayingSpeed = PlayingSpeed;
		}
	}
}

Glint* Glint::Create()
{
	return new Glint{};
}


void Glint::RenderAlphaBlendEffect(const DrawInfo& _ImplInfo)
{
	auto WeakSubset_Plane = _PlaneMesh->GetSubset(0u);
	if (auto SharedSubset = WeakSubset_Plane.lock();
		SharedSubset)
	{
		for (UINT i = 0u; i < 3u; ++i)
		{
			if (_SliceAmount[i] < 1.f)
			{
				_ImplInfo.Fx->SetMatrix("World", &_WorldMatrix[i]);
				_ImplInfo.Fx->SetFloat("_SliceAmount", _SliceAmount[i]);
				_ImplInfo.Fx->SetTexture("BaseMap", _GlintTex->GetTexture());
				
				SharedSubset->Render(_ImplInfo.Fx);
			}
		}
	}
}

HRESULT Glint::Ready()
{
	SetRenderEnable(true);

	m_nTag = GAMEOBJECTTAG::Eff_Glint;

	ENGINE::RenderProperty _InitRenderProp;
	_InitRenderProp.bRender = false;
	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{
		{"Glint",
		[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		},
	};
	RenderInterface::Initialize(_InitRenderProp);

	_PlaneMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx");
	_GlintTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Light\\tex_capcom_light_glint_0017_alpg.tga");

	_PlayingSpeed = 3.5f;
	_BrightScale = 0.02f;

	Reset();	// PlayStart()로 재생

	return S_OK;
}

HRESULT Glint::Awake()
{
	return S_OK;
}

HRESULT Glint::Start()
{
	return S_OK;
}

UINT Glint::Update(const float _fDeltaTime)
{
	Effect::Update(_fDeltaTime);

	//std::cout << _AccumulateTime << endl;

	if (1.5f < _AccumulateTime)
		Reset();

	if (auto Sptransform = GetComponent<ENGINE::Transform>().lock();
		Sptransform)
	{
		float ScaleSpeed = 0.018f;
		for (uint32 i = 0u; i < 3u; ++i)
		{
			_SliceAmount[i] = _AccumulateTime - _Interval > 1.f ? 1.f : _AccumulateTime - _Interval;
			if (0.f > _SliceAmount[i])
			{
				_SliceAmount[i] = 1.f;
				_Scale[i] = 0.f;
			}
			else
			{
				_Scale[i] = 0.1f + ScaleSpeed * _SliceAmount[0];
				if (1.f < _Scale[i])
					_Scale[i] = 1.f;

				Matrix TempMat, BillMat;
				D3DXMatrixIdentity(&_WorldMatrix[i]);
				float Scale = Sptransform->GetScale().x * _Scale[i] * 0.1f;
				float Rot = 0.f;
				Vector3 Pos = Sptransform->GetPosition();
				if (1u == i)
				{
					Rot = D3DXToRadian(35.f);
					D3DXMatrixScaling(&TempMat, Scale * 1.3f, Scale * _Aspect, 1.f);
					//Pos += Vector3(0.01f, 0.01f, 0.01f);
				}
				else if (2u == i)
				{
					Rot = D3DXToRadian(-35.f);
					D3DXMatrixScaling(&TempMat, Scale * 0.7f, Scale * _Aspect, 1.f);
					//Pos -= Vector3(0.01f, 0.01f, 0.01f);
				}
				else // 0
				{
					Rot = 0.f;
					D3DXMatrixScaling(&TempMat, Scale, Scale * _Aspect, 1.f);
				}
				_WorldMatrix[i] *= TempMat;
				D3DXMatrixRotationZ(&TempMat, Rot);
				_WorldMatrix[i] *= TempMat;
				TempMat = Renderer::GetInstance()->_RenderInfo.View;
				D3DXMatrixIdentity(&BillMat);
				memcpy(&BillMat.m[0][0], &TempMat.m[0][0], sizeof(Vector3));
				memcpy(&BillMat.m[1][0], &TempMat.m[1][0], sizeof(Vector3));
				memcpy(&BillMat.m[2][0], &TempMat.m[2][0], sizeof(Vector3));
				D3DXMatrixInverse(&BillMat, 0, &BillMat);
				_WorldMatrix[i] *= BillMat;
				D3DXMatrixTranslation(&TempMat, Pos.x, Pos.y, Pos.z);
				_WorldMatrix[i] *= TempMat;
			}

			if (0u == i)
				_Interval = 0.2f;
			else if (1u == i)
				_Interval = 0.4f;
			else
				_Interval = 0.f;
		}
	}

	//
	//Imgui_Modify();

	return 0;
}

UINT Glint::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void Glint::Editor()
{
	GameObject::Editor();
}

void Glint::OnEnable()
{
	GameObject::OnEnable();
}

void Glint::OnDisable()
{
	GameObject::OnDisable();
}