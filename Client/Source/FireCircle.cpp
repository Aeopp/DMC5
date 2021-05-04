#include "stdafx.h"
#include "..\Header\FireCircle.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "GraphicSystem.h"
#include "RedQueen.h"

FireCircle::FireCircle()
{

}

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
	// �̹� ���ؽ� ��ü�� ���� ��ġ�� . 
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const Vector3 Scale = _SpTransform->GetScale();
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
		if (Inner)
		{
			const uint32  Numsubset = Inner->GetNumSubset();
			_RenderUpdateInfo.SubsetCullingSphere.resize(Numsubset);

			for (uint32 i = 0; i < Numsubset; ++i)
			{
				const auto& _Subset = Inner->GetSubset(i);
				const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

				_RenderUpdateInfo.SubsetCullingSphere[i] = _CurBS.Transform(_RenderUpdateInfo.World, Scale.x);
			}
		}

		if (Outer)
		{
			const uint32  Numsubset = Outer->GetNumSubset();
			_RenderUpdateInfo.SubsetCullingSphere.resize(Numsubset);

			for (uint32 i = 0; i < Numsubset; ++i)
			{
				const auto& _Subset = Outer->GetSubset(i);
				const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

				_RenderUpdateInfo.SubsetCullingSphere[i] = _CurBS.Transform(_RenderUpdateInfo.World, Scale.x);
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
	// Outer = Resources::Load<StaticMesh>("..\\..\\Resource\\Mesh\\Static\\Primitive\\pipe01.fbx");
	Outer = Resources::Load<StaticMesh>("..\\..\\Resource\\Mesh\\Static\\Primitive\\pipe01.fbx");
	Inner = Resources::Load<StaticMesh>("..\\..\\Resource\\Mesh\\Static\\Primitive\\pipe00.fbx");

	// Inner = Resources::Load<StaticMesh>("..\\..\\Resource\\Mesh\\Static\\Primitive\\halfpipe.fbx");
	// �ؽ��� 
	SpriteMap = Resources::Load<Texture>("..\\..\\Resource\\Texture\\Effect\\fire_explosive_01.tga");
	TrailMap = Resources::Load<Texture>("..\\..\\Resource\\Texture\\Effect\\Fire.tga");
	EmssiveMskMap = Resources::Load<Texture>("..\\..\\Resource\\Texture\\Effect\\emissive_msk.tga");
	NoiseMap = Resources::Load<Texture>("..\\..\\Resource\\Texture\\Effect\\noiseInput_ATOS.tga");


	/*NoiseScale = { 0.f,0.005f,0.005f};
	NoiseScrollSpeed = { 0.f,0.0005f,0.0005f};*/

	NoiseScale = { 0.f,0.f,0.f };
	NoiseScrollSpeed = { 0.f,0.f,0.f };

	RollRotationSpeed = FMath::PI;
	EmissiveIntencity = 0.01f;
	ColorIntencity = 0.6f;
	SpriteUpdateCycle = 0.01f;
	DistortionIntencity = 1.f;
	bOuterRender = false;
	OuterDistortionIntencity =  DistortionIntencity = 10000000.f;
};

void FireCircle::PlayStart(const std::optional<Vector3>& Location,
	const Vector3& Rotation,
	const float RollRotateSpeed)
{
	if (Location)
	{
		GetComponent<Transform>().lock()->SetPosition(Location.value());
	}

	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		SpTransform->SetRotation(Rotation);
	}

	this->RollRotationSpeed = RollRotateSpeed;
	_RenderProperty.bRender = true;
	T = 0.0f;
	SpriteCurUpdateTime = SpriteUpdateCycle;
	SpriteProgressTime = SpriteCurUpdateTime / SpriteUpdateCycle;

	SpritePrevRowIdx = SpriteRowIdx = 0.f;
	SpritePrevColIdx  = 	SpriteColIdx = 0.f;
};

void FireCircle::PlayEnd()
{
	_RenderProperty.bRender = false;
};

void FireCircle::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("matWorld", &_RenderUpdateInfo.World);


	_Info.Fx->SetFloat("ColorIntencity", ColorIntencity);
	_Info.Fx->SetFloat("EmissiveIntencity", EmissiveIntencity);
	_Info.Fx->SetFloat("DistortionIntencity", DistortionIntencity);
	_Info.Fx->SetFloat("SpriteProgressTime", SpriteProgressTime);

	_Info.Fx->SetBool("bNoise", bNoise);

	if (bNoise)
	{
		_Info.Fx->SetFloatArray("NoiseScale", NoiseScale, 3u);
		const Vector3 Speed = NoiseScrollSpeed * T;
		_Info.Fx->SetFloatArray("NoiseScrollSpeed", Speed, 3u);

		_Info.Fx->SetFloatArray("NoiseDistortion0", NoiseDistortion0, 2u);
		_Info.Fx->SetFloatArray("NoiseDistortion1", NoiseDistortion1, 2u);
		_Info.Fx->SetFloatArray("NoiseDistortion2", NoiseDistortion2, 2u);
		_Info.Fx->SetTexture("NoiseMap", NoiseMap->GetTexture());

	}
	else
	{
		_Info.Fx->SetFloatArray("NoiseScale", nullptr, 3u);
		const Vector3 Speed = NoiseScrollSpeed * T;
		_Info.Fx->SetFloatArray("NoiseScrollSpeed", nullptr, 3u);

		_Info.Fx->SetFloatArray("NoiseDistortion0", nullptr, 2u);
		_Info.Fx->SetFloatArray("NoiseDistortion1", nullptr, 2u);
		_Info.Fx->SetFloatArray("NoiseDistortion2", nullptr, 2u);

		_Info.Fx->SetTexture("NoiseMap", nullptr);
	}

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

	if(bOuterRender)
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
	}
};

void FireCircle::SpriteUpdate(const float DeltaTime)
{
	SpriteCurUpdateTime -= DeltaTime;

	if (SpriteCurUpdateTime < 0.0f)
	{
		SpritePrevColIdx = SpriteColIdx;
		SpriteColIdx += 1.f;

		if (SpriteColIdx >= SpriteCol)
		{
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


	{
		const uint32 Numsubset = Outer->GetNumSubset();
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
	}
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
	if (auto spTransform = GetComponent<ENGINE::Transform>().lock();
		spTransform)
	{
		spTransform->Rotate(Vector3{ 0.f,0.f, RollRotationSpeed * _fDeltaTime });
	}

	SpriteUpdate(_fDeltaTime);
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
				Vector3 Point{ 0.f,0.f ,0.f };
				if (auto SpTransform = m_pTransform.lock();
					SpTransform)
				{
					Point = SpTransform->GetPosition();
				};

				PlayStart(Point, EditPlayStartRotation, EditPlayRollRotateSpeed);
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

			ImGui::Checkbox("bNoise", &bNoise);

			if (bNoise)
			{
				ImGui::SliderFloat3("NoiseScrollSpeed", NoiseScrollSpeed, FLT_MIN, 10.f, "%9.6f");
				ImGui::InputFloat3("In NoiseScrollSpeed", NoiseScrollSpeed, "%9.6f");

				ImGui::SliderFloat3("NoiseScale", NoiseScale, FLT_MIN, 10.f, "%9.6f");
				ImGui::InputFloat3("In NoiseScale", NoiseScale, "%9.6f");
			}

			ImGui::SliderFloat3("EditPlayStartRotation", EditPlayStartRotation, FLT_MIN, 10000.f, "%9.6f");
			ImGui::InputFloat3("In EditPlayStartRotation", EditPlayStartRotation, "%9.6f");

			ImGui::SliderFloat("EditPlayRollRotateSpeed", &EditPlayRollRotateSpeed, FLT_MIN, 10000.f, "%9.6f");
			ImGui::InputFloat("In EditPlayRollRotateSpeed", &EditPlayRollRotateSpeed,0.f,0.f ,"%9.6f");

			ImGui::SliderFloat("DistortionIntencity", &DistortionIntencity, FLT_MIN, 10000.f, "%9.6f");
			ImGui::InputFloat("In DistortionIntencity", &DistortionIntencity, 0.f, 0.f, "%9.6f");

			ImGui::SliderFloat("OuterDistortionIntencity", &OuterDistortionIntencity, FLT_MIN, 10000.f, "%9.6f");
			ImGui::InputFloat("In OuterDistortionIntencity", &OuterDistortionIntencity, 0.f, 0.f, "%9.6f");

			;

			ImGui::SliderFloat("EmissiveIntencity", &EmissiveIntencity, FLT_MIN, 10000.f, "%9.6f");
			ImGui::InputFloat("In EmissiveIntencity", &EmissiveIntencity, 0.f,0.f,"%9.6f");

			ImGui::SliderFloat("ColorIntencity", &ColorIntencity, FLT_MIN, 10000.f, "%9.6f");
			ImGui::InputFloat("In ColorIntencity", &ColorIntencity, 0.f, 0.f, "%9.6f");

			ImGui::SliderFloat("SpriteUpdateCycle", &SpriteUpdateCycle, FLT_MIN, 10.f, "%9.6f");
			ImGui::InputFloat("In SpriteUpdateCycle", &SpriteUpdateCycle, 0.f, 0.f, "%9.6f");
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

