#include "stdafx.h"
#include "..\Header\Trail.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "GraphicSystem.h"
#include "RedQueen.h"

Trail::Trail()
{

}

void Trail::Free()
{
	GameObject::Free();

	if (VtxBuffer)
	{
		VtxBuffer->Release();
	}
	if (IdxBuffer)
	{
		IdxBuffer->Release();
	}
	if (VtxDecl)
	{
		VtxDecl->Release();
	}
};

std::string Trail::GetName()
{
	return "Trail";
};

Trail* Trail::Create()
{
	return new Trail{};
};

void Trail::RenderReady()
{
	// �̹� ���ؽ� ��ü�� ���� ��ġ�� . 
	_RenderUpdateInfo.World = FMath::Identity();
};

void Trail::RenderInit()
{
	m_nTag = Eff_Trail;
	// ������ �����ؾ��ϴ� ������Ʈ��� (�������� ��� ����) �˸�.
	// ���� �������̽� ��ӹ��� �ʾҴٸ� Ű��������.
	SetRenderEnable(true);

	// ���� ���� �ʱ�ȭ ...
	ENGINE::RenderProperty _InitRenderProp;
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
	// ���� �Ӽ� ��ü �ʱ�ȭ 
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
	_InitRenderProp.bRender = false;

	_InitRenderProp.RenderOrders[RenderProperty::Order::Debug]
		=
	{
		{"Debug" ,[this](const DrawInfo& _Info)
			{
				RenderDebug(_Info);
			}
	} };

	_InitRenderProp.RenderOrders[RenderProperty::Order::Collider]
		=
	{
		{"Collider" ,[this](const DrawInfo& _Info)
			{
				DrawCollider(_Info);
			}
		}
	};

	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{
		{"Trail",[this](const DrawInfo& _Info)
			{
				this->RenderTrail(_Info);
			}
		}
	};
	RenderInterface::Initialize(_InitRenderProp);

	const int32 TriCnt = 16;

	_Desc.VtxSize = sizeof(Vertex::TrailVertex);
	_Desc.VtxCnt = TriCnt+2;
	// �ݵ�� ¦���� ��Ī . 
	_Desc.TriCnt = TriCnt;
	_Desc.DrawTriCnt = TriCnt;
	_Desc.IdxSize = sizeof(Vertex::Index32);
	_Desc.IdxFmt = D3DFMT_INDEX32;

	_Desc.UpdateCycle = 0.0f;
	_Desc.NewVtxCnt = 0;

	Device = g_pDevice;

	VtxDecl = Vertex::TrailVertex::GetVertexDecl(Device);
	Device->CreateVertexBuffer
	(_Desc.VtxSize * _Desc.VtxCnt, 0, 
		NULL, 
		D3DPOOL_MANAGED, 
		&VtxBuffer, nullptr);

	Device->CreateIndexBuffer
	(_Desc.IdxSize* _Desc.TriCnt
		, 0, _Desc.IdxFmt, 
		D3DPOOL_MANAGED, &IdxBuffer, nullptr);

	TrailMap = Resources::Load<Texture>(
		"..\\..\\Resource\\Texture\\Effect\\mesh_03_cs_trailmap_53_002_msk1.tga");

	FireSpriteMap = Resources::Load<Texture >(
		  "..\\..\\Resource\\Texture\\Effect\\Sprite\\Fire\\tex_capcom_fire_explosive_0014_alpg.tex_noesispreviewdata.tga");

	SpriteCol = 8;
	SpriteRow = 8;
};

void Trail::PlayStart(const Mode _Mode,
	const std::optional<Vector3>& Location)
{
	if (Location)
	{
		GetComponent<Transform>().lock()->SetPosition(Location.value());
	}

	Vertex::TrailVertex* VtxPtr{nullptr};
	VtxBuffer->Lock(0, 0, (void**)&VtxPtr, NULL);
	ZeroMemory(VtxPtr, sizeof(Vertex::TrailVertex) * _Desc.VtxCnt);
	VtxBuffer->Unlock();

	CurMode = _Mode;
	_RenderProperty.bRender = true;
	T = 0.0f;
	_Desc.NewVtxCnt = 0;
	_Desc.UpdateCycle = _Desc.CurVtxUpdateCycle;
	SpriteCurUpdateCycle = SpriteUpdateCycle;
	SpriteRowIdx = 0;
	SpriteColIdx = 0;
};

void Trail::PlayEnd()
{
	_RenderProperty.bRender = false;
};

void Trail::RenderTrail(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("matWorld", &_RenderUpdateInfo.World);
	_Info.Fx->SetTexture("TrailMap", TrailMap->GetTexture());

	if (CurMode ==Mode::Explosion)
	{
		_Info.Fx->SetFloat("DistortionIntencity", DistortionIntencity);
		_Info.Fx->SetTexture("SpriteMap", FireSpriteMap->GetTexture());
	}
	else if (CurMode == Mode::Non)
	{
		_Info.Fx->SetFloat("DistortionIntencity", NonDistortionIntencity);
		_Info.Fx->SetTexture("SpriteMap", nullptr);
	}

	_Info.Fx->SetVector("_Color", &_Color);
	_Info.Fx->SetFloat("ColorIntencity", ColorIntencity);

	_Info.Fx->SetFloat("SpriteXStart", SpriteColIdx / SpriteCol);
	_Info.Fx->SetFloat("SpriteXEnd",  ( SpriteColIdx + 1)/ SpriteCol);
	_Info.Fx->SetFloat("SpriteYStart",SpriteRowIdx/ SpriteRow );
	_Info.Fx->SetFloat("SpriteYEnd", (SpriteRowIdx + 1) / SpriteRow);
	
	Device->SetStreamSource(0, VtxBuffer, 0, _Desc.VtxSize);
	Device->SetVertexDeclaration(VtxDecl);
	Device->SetIndices(IdxBuffer);
	_Info.Fx->CommitChanges();
	Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, _Desc.VtxCnt, 0, _Desc.DrawTriCnt);
}
void Trail::SpriteUpdate(const float DeltaTime)
{
	SpriteCurUpdateCycle -= DeltaTime;
	if (SpriteCurUpdateCycle < 0.0f)
	{
		++SpriteColIdx;
		if (SpriteColIdx >= SpriteCol)
		{
			SpriteColIdx = 0;
			++SpriteRowIdx;
			if (SpriteRowIdx >= SpriteRow)
			{
				SpriteRowIdx = 0;
			}
		}
		SpriteCurUpdateCycle += 0.05f;
	}
}
void Trail::BufferUpdate(const float DeltaTime)
{
	_Desc.CurVtxUpdateCycle -= DeltaTime;

	if (_Desc.CurVtxUpdateCycle < 0.0f)
	{
		_Desc.CurVtxUpdateCycle += _Desc.UpdateCycle;

		IndexBufUpdate();
		VertexBufUpdate();
	}
}
void Trail::VtxSplineInterpolation(Vertex::TrailVertex* const VtxPtr)
{
	// � ���� .....
	for (int32 i = 0; i < _Desc.NewVtxCnt; i += 2)
	{
		{
			Vector3 VtxPt{};

			const int32 p0 = std::clamp<int32>(i - 2, 0, _Desc.NewVtxCnt - 1);
			const int32 p1 = std::clamp<int32>(i, 0, _Desc.NewVtxCnt - 1);
			const int32 p2 = std::clamp<int32>(i + 2, 0, _Desc.NewVtxCnt - 1);
			const int32 p3 = std::clamp<int32>(i + 4, 0, _Desc.NewVtxCnt - 1);

			D3DXVec3CatmullRom(
				&VtxPt,
				&VtxPtr[p0].Location,
				&VtxPtr[p1].Location,
				&VtxPtr[p2].Location,
				&VtxPtr[p3].Location,
				CurveT);

			VtxPtr[i].Location = VtxPt;
		}

		{
			Vector3 VtxPt{};

			const int32 p0 = std::clamp<int32>((i + 1) - 2, 0, _Desc.NewVtxCnt - 1);
			const int32 p1 = std::clamp<int32>((i + 1), 0, _Desc.NewVtxCnt - 1);
			const int32 p2 = std::clamp<int32>((i + 1) + 2, 0, _Desc.NewVtxCnt - 1);
			const int32 p3 = std::clamp<int32>((i + 1) + 4, 0, _Desc.NewVtxCnt - 1);

			D3DXVec3CatmullRom(
				&VtxPt,
				&VtxPtr[p0].Location,
				&VtxPtr[p1].Location,
				&VtxPtr[p2].Location,
				&VtxPtr[p3].Location,
				CurveT);

			VtxPtr[i + 1].Location = VtxPt;
		}
	}
};

void Trail::VertexBufUpdate()
{
	Vertex::TrailVertex* VtxPtr{};
	VtxBuffer->Lock(0, 0, (void**)&VtxPtr, 0);
	// �ִ� ���ؽ� ī��Ʈ�� �ʰ��� ��� .

	if (_Desc.NewVtxCnt > _Desc.VtxCnt)
	{
		static constexpr uint32 RemoveCount = 2;
		_Desc.NewVtxCnt -= RemoveCount;

		for (uint32 i = 0; i < _Desc.NewVtxCnt; i += 2)
		{
			VtxPtr[i].Location = VtxPtr[RemoveCount + i].Location;
			VtxPtr[i + 1].Location = VtxPtr[RemoveCount + i + 1].Location;
		}
	}

	if (auto _GameObject = FindGameObjectWithTag(TAG_RedQueen).lock();
		_GameObject)
	{
		if (auto RQ = std::dynamic_pointer_cast<RedQueen>(_GameObject);
			RQ)
		{
			const auto SwordWorld = RQ->GetComponent<Transform>().lock()->GetWorldMatrix();

			auto Low = RQ->Get_BoneMatrixPtr("_000");
			const Vector3 LowPos = FMath::Mul(LowOffset, *Low * SwordWorld);

			auto High = RQ->Get_BoneMatrixPtr("_001");
			const Vector3 HighPos = FMath::Mul(HighOffset, *High * SwordWorld);

			VtxPtr[_Desc.NewVtxCnt + 1].Location = HighPos;
			VtxPtr[_Desc.NewVtxCnt].Location = LowPos;
		}
	};

	VtxUVCalc(VtxPtr);

	_Desc.NewVtxCnt += 2;

	VtxSplineInterpolation(VtxPtr);

	if (bEdit)
	{
		for (size_t i = 0; i < _VtxLog.size(); ++i)
		{
			_VtxLog[i]   = VtxPtr[i];
		}
	}
	VtxBuffer->Unlock();
}
void Trail::IndexBufUpdate()
{
	/*
		1 3 5 7 9 11.......
		0 2 4 6 8 10 .....

		1�� �ﰢ��
		1 ��  3
		�� ��
		0

		2�� �ﰢ��
		   3
		 �� ��
		0 �� 2
		*/

	Vertex::Index32* IdxPtr{ nullptr };
	IdxBuffer->Lock(0, 0, (void**)&IdxPtr, 0);

	for (uint32 i = 0; i < _Desc.TriCnt; i += 2)
	{
		IdxPtr[i]._0 = i;
		IdxPtr[i]._1 = i + 1;
		IdxPtr[i]._2 = i + 3;

		IdxPtr[i + 1]._0 = i;
		IdxPtr[i + 1]._1 = i + 3;
		IdxPtr[i + 1]._2 = i + 2;
	}

	if (bEdit)
	{
		_IdxLog.resize(_Desc.TriCnt);
		for (size_t i = 0; i < _IdxLog.size(); ++i)
		{
			_IdxLog[i] = IdxPtr[i];
		}
	}

	IdxBuffer->Unlock();
};

void Trail::VtxUVCalc(Vertex::TrailVertex* const VtxPtr)
{
	for (uint32 i = 0; i < _Desc.NewVtxCnt; i += 2)
	{
		VtxPtr[i + 1].UV0 = { ((float)i / ((float)_Desc.NewVtxCnt - 2)) * UV0Multiply,0.f };
		VtxPtr[i].UV0 = { ((float)i / ((float)_Desc.NewVtxCnt - 2)) * UV0Multiply ,1.f };

		VtxPtr[i + 1].UV1 = { (float)i / ((float)_Desc.NewVtxCnt - 2),0.f };
		VtxPtr[i].UV1 = { (float)i / ((float)_Desc.NewVtxCnt - 2) ,1.f };
	}
};

void Trail::RenderDebug(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("World", &_RenderUpdateInfo.World);
	Device->SetStreamSource(0, VtxBuffer, 0, _Desc.VtxSize);
	Device->SetVertexDeclaration(VtxDecl);
	Device->SetIndices(IdxBuffer);
	_Info.Fx->CommitChanges();
	Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, _Desc.VtxCnt, 0, _Desc.DrawTriCnt);
};


HRESULT Trail::Ready()
{
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetPosition(Vector3{ 0.f,0.f,0.f });
	InitTransform.lock()->SetScale(Vector3{ 1.f,1.f,1.f });
	InitTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	return S_OK;
};

HRESULT Trail::Awake()
{
	GameObject::Awake();
	m_pTransform.lock()->SetPosition(Vector3{ 0.f,0.f,0.f });
	m_pTransform.lock()->SetScale(Vector3{ 1.f,1.f,1.f });
	m_pTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	return S_OK;
}

HRESULT Trail::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT Trail::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	if (_RenderProperty.bRender == false) return 0;


	SpriteUpdate(_fDeltaTime);
	BufferUpdate(_fDeltaTime);

	return 0;
}

UINT Trail::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
}

void Trail::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		const std::string ChildName = GetName() + "_Play";
		ImGui::BeginChild(ChildName.c_str());
		{
			static int32 _Mode;
			ImGui::InputInt("Mode", &_Mode);
			if (ImGui::SmallButton("Play"))
			{
				PlayStart(static_cast<Trail::Mode>(_Mode));
			}
			if (ImGui::SmallButton("PlayEnd"))
			{
				PlayEnd();
			}

			ImGui::Text("Cur Col Idx : %d", (int32)SpriteColIdx);
			ImGui::Text("Cur Row Idx : %d", (int32)SpriteRowIdx);

			ImGui::SliderFloat3("LowOffset", LowOffset, -300.f, 300.f, "%9.6f");
			ImGui::SliderFloat3("HighOffset", HighOffset, -300.f, 300.f, "%9.6f");
			ImGui::SliderFloat("UpdateCycle", &_Desc.UpdateCycle, FLT_MIN, 10.f, "%9.6f");
			ImGui::SliderFloat("SpriteUpdateCycle", &SpriteUpdateCycle, FLT_MIN, 10.f, "%9.6f");
			ImGui::SliderInt("DrawTriCnt", &_Desc.DrawTriCnt, 0, _Desc.TriCnt);
			ImGui::SliderFloat("DistortionIntencity", &DistortionIntencity, FLT_MIN, 1.f, "%9.6f");
			ImGui::InputFloat("In DistortionIntencity", &DistortionIntencity, FLT_MIN, 1.f, "%9.6f");
			ImGui::SliderFloat("NonDistortionIntencity", &NonDistortionIntencity, FLT_MIN, 1.f, "%9.6f");
			ImGui::InputFloat("In NonDistortionIntencity", &NonDistortionIntencity, FLT_MIN, 1.f, "%9.6f");


			ImGui::InputFloat("ColoIntencity", &ColorIntencity, FLT_MIN,1.f,"%9.6f");
			ImGui::ColorEdit4("Color", _Color);
			ImGui::SliderFloat("UV0Multiply", &UV0Multiply,0.f,10.f,"%1.6f");
			ImGui::SliderFloat("CurveT", &CurveT, 0.f, 1.f);
		}
		ImGui::EndChild();
	}
}


void Trail::OnEnable()
{
	GameObject::OnEnable();
}

void Trail::OnDisable()
{
	GameObject::OnDisable();
};



