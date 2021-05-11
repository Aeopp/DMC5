#include "stdafx.h"
#include "..\Header\FireCircle.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "GraphicSystem.h"
#include "RedQueen.h"
#include "ParticleSystem.h"

FireCircle::FireCircle()
{

};

void FireCircle::Free()
{
	GameObject::Free();
};

std::string FireCircle::GetName()
{
	return "FireCircle";
};

FireCircle* FireCircle::Create()
{
	return new FireCircle{};
};

void FireCircle::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const Vector3 Scale = _SpTransform->GetScale();
		if (Inner)
		{
			const uint32 Numsubset = Inner->GetNumSubset();
			
			if (Numsubset > 0)
			{
				if (auto SpUser = User.lock();
					SpUser)
				{
					_RenderUpdateInfo.SubsetCullingSphere.resize(Numsubset);

					if (auto spTransform = GetComponent<ENGINE::Transform>().lock();
						spTransform)
					{
						_RenderUpdateInfo.World =
							FMath::Scale(spTransform->GetScale()) *
							FMath::Rotation({ FMath::HalfPI - FLT_MIN, 0.f,CurRoll }) *
							FMath::Rotation(PlayRotation) *
							(UserRotation)*
							FMath::Translation(spTransform->GetPosition());

						spTransform->SetWorldMatrix(_RenderUpdateInfo.World);
					}

					for (uint32 i = 0; i < Numsubset; ++i)
					{
						const auto& _Subset = Inner->GetSubset(i);
						const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

						_RenderUpdateInfo.SubsetCullingSphere[i] = _CurBS.Transform(_RenderUpdateInfo.World, Scale.x);
					}
				}
			}
		}
	}
};

void FireCircle::RenderInit()
{
	m_nTag = Eff_FireCircle;
	// ������ �����ؾ��ϴ� ������Ʈ��� (�������� ��� ����) �˸�.
	// ���� �������̽� ��ӹ��� �ʾҴٸ� Ű��������.
	SetRenderEnable(true);

	// ���� ���� �ʱ�ȭ ...
	ENGINE::RenderProperty _InitRenderProp;
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
	// ���� �Ӽ� ��ü �ʱ�ȭ 
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
	_InitRenderProp.bRender = false;

	_InitRenderProp.RenderOrders[RenderProperty::Order::Debug]=
	{
		{"Debug" ,[this](const DrawInfo& _Info)
			{
				RenderDebug(_Info);
			}
	} };

	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{
		{"FireCircle",[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		}
	};

	RenderInterface::Initialize(_InitRenderProp);

	// �޽�
	Outer = Resources::Load<StaticMesh>("..\\..\\Resource\\Mesh\\Static\\Primitive\\pipe01.fbx");
	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = true;
	Inner = Resources::Load<StaticMesh>("..\\..\\Resource\\Mesh\\Static\\Primitive\\pipe00.fbx");
	
	SpriteMap = Resources::Load<Texture>("..\\..\\Resource\\Texture\\Effect\\Fire\\7.tga");
	TrailMap = Resources::Load<Texture>("..\\..\\Resource\\Texture\\Effect\\Fire.tga");
	EmssiveMskMap = Resources::Load<Texture>("..\\..\\Resource\\Texture\\Effect\\emissive_msk.tga");
	NoiseMap = Resources::Load<Texture>("..\\..\\Resource\\Texture\\Effect\\noiseInput_ATOS.tga");

	NoiseScrollSpeed = { 0.55f,0.0f,2.09f };
	NoiseScale = { 0.917f,1.009f,1.09f};

	EditPlayRollRotateSpeed  = 	RollRotationSpeed = 400.f;
	EmissiveIntencity = 0.01f;
	ColorIntencity = 0.2f;
	SpriteUpdateCycle = 0.01f;
	DistortionIntencity = 1.f;
	bOuterRender = false;
	OuterDistortionIntencity =  DistortionIntencity = 10000000.f;
};

void FireCircle::PlayStart(
	const Vector3& Rotation,
	// ��� ��ų ��ġ 
	const Vector3& Location ,
	// ��� ��ų ȸ�� 
	const float CurRoll ,
	// ȸ�� �ӵ� . 
	const float RollRotateSpeed,
	const int32 StartSpriteRow ,
	const float PlayTime ,
	const int32 StartSpriteCol ,
	const Vector3& Scale)
{
	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		SpTransform->SetScale(Scale);
		SpTransform->SetPosition(Location);
	}

	if (auto SpUser = User.lock();
		SpUser)
	{
		if (auto SpUserTransform = SpUser->GetComponent<Transform>().lock();
			SpUserTransform)
		{
			UserRotation = SpUserTransform->GetRotationMatrix();
		}
	}

	PlayRotation = Rotation;
	this->CurRoll = CurRoll;
	this->RollRotationSpeed = RollRotateSpeed;
	_RenderProperty.bRender = true;
	T = 0.0f;
	this->PlayTime = PlayTime;
	SpriteUpdateCycle = PlayTime / static_cast<float>(SpriteCol);
	SpriteCurUpdateTime = SpriteUpdateCycle;
	SpriteProgressTime = SpriteCurUpdateTime / SpriteUpdateCycle;

	SpritePrevRowIdx = SpriteRowIdx = static_cast<float>(StartSpriteRow);
	SpritePrevColIdx = SpriteColIdx = static_cast<float>(StartSpriteCol);

	CurParticleTime= 0.0f;
};

void FireCircle::PlayEnd()
{
	_RenderProperty.bRender = false;
}

void FireCircle::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("matWorld", &_RenderUpdateInfo.World);

	_Info.Fx->SetFloat("ColorIntencity", ColorIntencity);
	_Info.Fx->SetFloat("EmissiveIntencity", EmissiveIntencity);
	_Info.Fx->SetFloat("DistortionIntencity", DistortionIntencity);
	_Info.Fx->SetFloat("SpriteProgressTime", SpriteProgressTime);
	_Info.Fx->SetFloat("ClipRange", ClipRange);

	_Info.Fx->SetFloatArray("NoiseScale", NoiseScale, 3u);
	const Vector3 Speed = NoiseScrollSpeed * T;
	_Info.Fx->SetFloatArray("NoiseScrollSpeed", Speed, 3u);

	_Info.Fx->SetFloatArray("NoiseDistortion0", NoiseDistortion0, 2u);
	_Info.Fx->SetFloatArray("NoiseDistortion1", NoiseDistortion1, 2u);
	_Info.Fx->SetFloatArray("NoiseDistortion2", NoiseDistortion2, 2u);
	_Info.Fx->SetTexture("NoiseMap", NoiseMap->GetTexture());

	_Info.Fx->SetFloat("SpritePrevXStart", SpritePrevColIdx / SpriteCol);
	_Info.Fx->SetFloat("SpritePrevXEnd", (SpritePrevColIdx + 1.f) / SpriteCol);
	_Info.Fx->SetFloat("SpritePrevYStart", SpritePrevRowIdx / SpriteRow);
	_Info.Fx->SetFloat("SpritePrevYEnd", (SpritePrevRowIdx + 1.f) / SpriteRow);

	_Info.Fx->SetFloat("SpriteXStart", SpriteColIdx / SpriteCol);
	_Info.Fx->SetFloat("SpriteXEnd", (SpriteColIdx + 1.f) / SpriteCol);
	_Info.Fx->SetFloat("SpriteYStart", SpriteRowIdx / SpriteRow);
	_Info.Fx->SetFloat("SpriteYEnd", (SpriteRowIdx + 1.f) / SpriteRow);

	_Info.Fx->SetTexture("SpriteMap", SpriteMap->GetTexture());
	_Info.Fx->SetTexture("TrailMap", TrailMap->GetTexture());
	_Info.Fx->SetTexture("EmissiveMskMap", EmssiveMskMap->GetTexture());

	const float PlayTimehalf = PlayTime * 0.5f;
	if (T >= PlayTimehalf)
	{
		_Info.Fx->SetFloat("AlphaFactor", 1.0f - ((T-PlayTimehalf) / PlayTimehalf));
		
	}
	else
	{
		_Info.Fx->SetFloat("AlphaFactor", 1.0f);
	}

	{
		const uint32 Numsubset = Inner->GetNumSubset();
		for (uint32 i = 0; i < Numsubset; ++i)
		{
			if (auto SpSubset = Inner->GetSubset(i).lock();
				SpSubset)
			{
				if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
				{
					continue;
				}

				SpSubset->Render(_Info.Fx);
			};
		};
	}

	/*if(bOuterRender)
	{
		const uint32 Numsubset = Outer->GetNumSubset();
		if (Numsubset > 0)
		{
			_Info.Fx->SetFloat("DistortionIntencity", OuterDistortionIntencity);
		}
		for (uint32 i = 0; i < Numsubset; ++i)
		{
			if (auto SpSubset = Outer->GetSubset(i).lock();
				SpSubset)
			{
				if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
				{
					continue;
				}

				SpSubset->Render(_Info.Fx);
			};
		};
	}*/
};

void FireCircle::PlayParticle()
{
	return;

	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		const Matrix Mat = SpTransform->GetRenderMatrix();
		const uint32 RangeEnd = Inner->m_spVertexLocations->size() - 1u;
		const uint32 JumpOffset = 2u;

		{
			auto _PlayableParticle = ParticleSystem::GetInstance()->PlayParticle(
				"FireParticle", 1000ul,true);
			for (int32 i = 0; i < _PlayableParticle.size();
				i += JumpOffset)
			{
				auto& _PlayInstance = _PlayableParticle[i];
				_PlayInstance->PlayDescBind(SpTransform->GetRenderMatrix());
			}
		}
	};
}

void FireCircle::SpriteUpdate(const float DeltaTime)
{
	SpriteCurUpdateTime -= DeltaTime;

	if (SpriteCurUpdateTime < 0.0f)
	{
		SpritePrevColIdx = SpriteColIdx;
		SpriteColIdx += 1.f;

		if (SpriteColIdx >= SpriteCol)
		{
			PlayEnd();
			return;

			SpriteColIdx = 0.f;
			SpritePrevRowIdx = SpriteRowIdx;
			SpriteRowIdx += 1.f;
			if (SpriteRowIdx >= SpriteRow)
			{
				SpriteRowIdx = 0.f;
			}
		}
		SpriteCurUpdateTime += SpriteUpdateCycle;
	}

	SpriteProgressTime = SpriteCurUpdateTime / SpriteUpdateCycle;
}

void FireCircle::RenderDebug(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("World", &_RenderUpdateInfo.World);

	{
		const uint32 Numsubset = Inner->GetNumSubset();
		for (uint32 i = 0; i < Numsubset; ++i)
		{
			if (auto SpSubset = Inner->GetSubset(i).lock();
				SpSubset)
			{
				if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
				{
					continue;
				}

				SpSubset->Render(_Info.Fx);
			};
		};
	}


	//{
	//	const uint32 Numsubset = Outer->GetNumSubset();
	//	for (uint32 i = 0; i < Numsubset; ++i)
	//	{
	//		if (auto SpSubset = Outer->GetSubset(i).lock();
	//			SpSubset)
	//		{
	//			if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
	//			{
	//				continue;
	//			}

	//			SpSubset->Render(_Info.Fx);
	//		};
	//	};
	//}
};


HRESULT FireCircle::Ready()
{
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetPosition(Vector3{ 0.f,0.f,0.f });
	InitTransform.lock()->SetScale(Vector3{ 0.01f,0.01f,0.01f });
	InitTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	return S_OK;
};

HRESULT FireCircle::Awake()
{
	GameObject::Awake();
	
	m_pTransform.lock()->SetPosition(Vector3{ 0.f,0.f,0.f });
	m_pTransform.lock()->SetScale(Vector3{ 0.01f,0.01f,0.01f });
	m_pTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	User = FindGameObjectWithTag(Player);

	return S_OK;
}

HRESULT FireCircle::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT FireCircle::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	if (_RenderProperty.bRender == false) return 0;
	T += _fDeltaTime;

	if (T > PlayTime)
	{
		PlayEnd();
		return 0;
	}

	CurRoll += _fDeltaTime * RollRotationSpeed;
	SpriteUpdate(_fDeltaTime);

	CurParticleTime -= _fDeltaTime;
	if (CurParticleTime < 0.0f)
	{
		CurParticleTime += ParticleTime;
		PlayParticle();
	}
	return 0;
}

UINT FireCircle::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
};

void FireCircle::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		const std::string ChildName = GetName() + "_Play";
		ImGui::BeginChild(ChildName.c_str());
		{
			if (ImGui::SmallButton("Play"))
			{
				PlayStart(EditPlayStartRotation,
					EditPlayStartLocation,
					EditStartRoll,
					EditPlayRollRotateSpeed,
					EditSpriteRow,
					EditPlayStartPlayTime,
					EditSpriteCol,
					EditPlayStartScale);
			}

			if (ImGui::SmallButton("PlayEnd"))
			{
				PlayEnd();
			}
			
			int32 ISpriteColIdx = SpriteColIdx;
			int32 ISpriteRowIdx = SpriteRowIdx;

			ImGui::Text("Cur Col Idx : %d", (int32)ISpriteColIdx);
			ImGui::Text("Cur Row Idx : %d", (int32)ISpriteRowIdx);
			ImGui::Text("SpriteProgressTime: %2.6f", SpriteProgressTime);
			ImGui::Text("Prev Col Idx : %d", (int32)SpritePrevColIdx);
			ImGui::Text("Prev Row Idx : %d", (int32)SpritePrevRowIdx);

			ImGui::SliderInt("Cur Col", &ISpriteColIdx, 0, SpriteCol-1.f);
			ImGui::SliderInt("Cur Row", &ISpriteRowIdx, 0, SpriteRow-1.f);

			SpriteColIdx = ISpriteColIdx;
			SpriteRowIdx = ISpriteRowIdx;

			ImGui::Text("SpriteXStart %2.6f", SpriteColIdx / SpriteCol);
			ImGui::SameLine();
			ImGui::Text("SpriteXEnd %2.6f", (SpriteColIdx + 1.f) / SpriteCol);

			ImGui::Text("SpriteYStart %2.6f", SpriteRowIdx / SpriteRow);
			ImGui::SameLine();
			ImGui::Text("SpriteYEnd %2.6f", (SpriteRowIdx + 1.f) / SpriteRow);

			ImGui::SliderFloat3("NoiseScrollSpeed", NoiseScrollSpeed, FLT_MIN, 10.f, "%9.6f");
			ImGui::InputFloat3("In NoiseScrollSpeed", NoiseScrollSpeed, "%9.6f");

			ImGui::SliderFloat3("NoiseScale", NoiseScale, FLT_MIN, 10.f, "%9.6f");
			ImGui::InputFloat3("In NoiseScale", NoiseScale, "%9.6f");

			ImGui::SliderFloat("ClipRange", &ClipRange, FLT_MIN, 1.f, "%9.6f");
			ImGui::InputFloat("In ClipRange", &ClipRange, 0.f, 0.f, "%9.6f");

			ImGui::SliderFloat3("EditPlayStartScale", EditPlayStartScale, FLT_MIN, 10000.f, "%9.6f");
			ImGui::InputFloat3("In EditPlayStartScale", EditPlayStartScale, "%9.6f");

			ImGui::SliderFloat("EditStartRoll", &EditStartRoll, FLT_MIN, 10000.f, "%9.6f");
			ImGui::InputFloat("In EditStartRoll", &EditStartRoll, 0.f, 0.f, "%9.6f");

			ImGui::SliderFloat3("EditPlayStartLocation", EditPlayStartLocation, FLT_MIN, 10000.f, "%9.6f");
			ImGui::InputFloat3("In EditPlayStartLocation", EditPlayStartLocation, "%9.6f");

			ImGui::SliderFloat3("EditPlayStartRotation", EditPlayStartRotation, FLT_MIN, FMath::PI, "%9.6f");
			ImGui::InputFloat3("In EditPlayStartRotation", EditPlayStartRotation, "%9.6f");

			ImGui::SliderFloat3("CurRotation", PlayRotation, FLT_MIN, FMath::PI, "%9.6f");
			ImGui::InputFloat3("In CurRotation", PlayRotation, "%9.6f");

			;

			ImGui::SliderFloat("EditPlayRollRotateSpeed", &EditPlayRollRotateSpeed, FLT_MIN, 10000.f, "%9.6f");
			ImGui::InputFloat("In EditPlayRollRotateSpeed", &EditPlayRollRotateSpeed,0.f,0.f ,"%9.6f");

			ImGui::SliderInt("EditSpriteCol",&EditSpriteCol,0,static_cast<int32>(SpriteCol-1));
			ImGui::SliderInt("EditSpriteRow", &EditSpriteRow, 0, static_cast<int32>(SpriteRow- 1));

			ImGui::SliderFloat("DistortionIntencity", &DistortionIntencity, FLT_MIN, 10000.f, "%9.6f");
			ImGui::InputFloat("In DistortionIntencity", &DistortionIntencity, 0.f, 0.f, "%9.6f");

			ImGui::SliderFloat("OuterDistortionIntencity", &OuterDistortionIntencity, FLT_MIN, 10000.f, "%9.6f");
			ImGui::InputFloat("In OuterDistortionIntencity", &OuterDistortionIntencity, 0.f, 0.f, "%9.6f");

			ImGui::SliderFloat("EmissiveIntencity", &EmissiveIntencity, FLT_MIN, 10000.f, "%9.6f");
			ImGui::InputFloat("In EmissiveIntencity", &EmissiveIntencity, 0.f,0.f,"%9.6f");

			ImGui::SliderFloat("ColorIntencity", &ColorIntencity, FLT_MIN, 10000.f, "%9.6f");
			ImGui::InputFloat("In ColorIntencity", &ColorIntencity, 0.f, 0.f, "%9.6f");

			ImGui::SliderFloat("EditPlayStartPlayTime", &EditPlayStartPlayTime, FLT_MIN, 10000.f, "%9.6f");
			ImGui::InputFloat("In EditPlayStartPlayTime", &EditPlayStartPlayTime, 0.f, 0.f, "%9.6f");


			ImGui::SliderFloat3("TestRotation", TestRotation, FLT_MIN, FMath::PI, "%9.6f");
			ImGui::InputFloat3("In TestRotation", TestRotation, "%9.6f");
		}
		ImGui::EndChild();
	}
};

void FireCircle::OnEnable()
{
	GameObject::OnEnable();
};

void FireCircle::OnDisable()
{
	GameObject::OnDisable();
};


