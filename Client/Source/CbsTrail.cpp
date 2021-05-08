#include "stdafx.h"

#include "..\Header\CbsTrail.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "GraphicSystem.h"
#include "Cbs_Short.h"
#include "ParticleSystem.h"

CbsTrail::CbsTrail()
{
	VtxBuffers.fill(nullptr);
	IdxBuffers.fill(nullptr);
}

void CbsTrail::Free()
{
	GameObject::Free();

	for (auto& _VtxBuf : VtxBuffers)
	{
		if (_VtxBuf)
		{
			_VtxBuf->Release();
		}
	}
	for (auto& _IdxBuf : IdxBuffers)
	{
		if (_IdxBuf)
		{
			_IdxBuf->Release();
		}
	}

	if (VtxDecl)
	{
		VtxDecl->Release();
	}
};

std::string CbsTrail::GetName()
{
	return "CbsTrail";
};

CbsTrail* CbsTrail::Create()
{
	return new CbsTrail{};
};

void CbsTrail::RenderReady()
{
	// �̹� ���ؽ� ��ü�� ���� ��ġ�� . 
	_RenderUpdateInfo.World = FMath::Identity();
};

void CbsTrail::RenderInit()
{
	m_nTag = Eff_CbsTrail;
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

	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{
		{"CbsTrail",[this](const DrawInfo& _Info)
			{
				this->RenderTrail(_Info);
			}
		}
	};

	RenderInterface::Initialize(_InitRenderProp);

	const int32 TriCnt = 20;

	_Desc.VtxSize = sizeof(Vertex::TrailVertex);
	_Desc.VtxCnt = TriCnt+2;
	// �ݵ�� ¦���� ��Ī . 
	_Desc.TriCnt = TriCnt;
	_Desc.DrawTriCnt = TriCnt;
	_Desc.IdxSize = sizeof(Vertex::Index32);
	_Desc.IdxFmt = D3DFMT_INDEX32;

	_Desc.UpdateCycle = 0.0f;
	_Desc.NewVtxCnt = 0;
	UV0Multiply = 1.f;

	Device = g_pDevice;
	VtxDecl = Vertex::TrailVertex::GetVertexDecl(Device);

	for (uint32 i = 0; i < TrailCnt; ++i)
	{
		Device->CreateVertexBuffer
		(_Desc.VtxSize * _Desc.VtxCnt, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
			NULL,
			D3DPOOL_DEFAULT,
			&VtxBuffers[i], nullptr);

		Device->CreateIndexBuffer
		(_Desc.IdxSize * _Desc.TriCnt
			, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, _Desc.IdxFmt,
			D3DPOOL_DEFAULT, &IdxBuffers[i], nullptr);
	}


	TrailMap = Resources::Load<Texture>(
		"..\\..\\Resource\\Texture\\Effect\\mesh_03_cs_trailmap_53_002_msk1.tga");
	FireSpriteMap = Resources::Load<Texture >(
		  "..\\..\\Resource\\Texture\\Effect\\Sprite\\Fire\\tex_capcom_fire_explosive_0014_alpg.tex_noesispreviewdata.tga");
	ExplosionTrailMap =Resources::Load<Texture >(
		"..\\..\\Resource\\Texture\\Effect\\fire.tga");

	EmissiveMskMap = Resources::Load<Texture>(
		"..\\..\\Resource\\Texture\\Effect\\emissive_msk.tga");
	//NoiseMap = Resources::Load<Texture>(
	//	"..\\..\\Resource\\Texture\\Effect\\noiseInput_ATOS.tga");
	//	
	SpriteCol = 8;
	SpriteRow = 8;

	Scale = { 1.f,2.f,3.f };
	ScrollSpeed = { 1.f,2.f,3.f };

	EmissiveIntencity = 0.0f;
};

void CbsTrail::PlayStart(const Mode _Mode,
	const std::optional<Vector3>& Location)
{
	if (Location)
	{
		GetComponent<Transform>().lock()->SetPosition(Location.value());
	}

	for (uint32 i=0;i<TrailCnt;++i)
	{
		auto& _VtxBuffer = VtxBuffers[i];

		Vertex::TrailVertex* VtxPtr{ nullptr };
		_VtxBuffer->Lock(0, 0, (void**)&VtxPtr, NULL);

		if (auto _GameObject = FindGameObjectWithTag(Tag_Cbs_Short).lock();
			_GameObject)
		{
			if (auto _CbsShort = std::dynamic_pointer_cast<Cbs_Short>(_GameObject);
				_CbsShort)
			{
				const auto CbsWorld = _CbsShort->GetComponent<Transform>().lock()->GetWorldMatrix();

				auto Low = _CbsShort->Get_BoneMatrixPtr(BoneNames[i]);
				
				LatelyOffsets[i].first = FMath::Mul(Offset[CurMode].first, *Low * CbsWorld);
				LatelyOffsets[i].second = FMath::Mul(Offset[CurMode].second, *Low * CbsWorld);

				for (int32 i = 0; i < _Desc.VtxCnt; ++i)
				{
					VtxPtr[i + 1].Location = LatelyOffsets[i].second;
					VtxPtr[i].Location = LatelyOffsets[i].first;
				}
			}
		};

		// ZeroMemory(VtxPtr, sizeof(Vertex::TrailVertex) * _Desc.VtxCnt);
		_VtxBuffer->Unlock();
	}

	for (auto& _IdxBuffer : IdxBuffers)
	{
		void* IdxPtr{ nullptr };
		_IdxBuffer->Lock(0, 0, (void**)&IdxPtr, NULL);
		ZeroMemory(IdxPtr, _Desc.IdxSize * _Desc.TriCnt);
		_IdxBuffer->Unlock();
	}

	CurMode = static_cast<uint32>(_Mode);
	_RenderProperty.bRender = true;
	T = 0.0f;
	_Desc.NewVtxCnt = 0;
	_Desc.UpdateCycle = _Desc.CurVtxUpdateCycle;
	SpriteCurUpdateCycle = SpriteUpdateCycle;
	SpriteRowIdx = 0;
	SpriteColIdx = 0;
};

void CbsTrail::PlayEnd()
{
	_RenderProperty.bRender = false;
};

void CbsTrail::RenderTrail(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("matWorld", &_RenderUpdateInfo.World);

	if (CurMode == Mode::Ice)
	{
		_Info.Fx->SetFloat("DistortionIntencity", DistortionIntencity);
		_Info.Fx->SetTexture("TrailMap", ExplosionTrailMap->GetTexture());
		_Info.Fx->SetTexture("SpriteMap", FireSpriteMap->GetTexture());
		/*_Info.Fx->SetTexture("NoiseMap", NoiseMap->GetTexture());
		_Info.Fx->SetFloatArray("ScrollSpeed", ScrollSpeed, 3);
		_Info.Fx->SetFloatArray("Scale", Scale, 3);*/
		
	}
	else if (CurMode == Mode::Non)
	{
		_Info.Fx->SetFloat("DistortionIntencity", NonDistortionIntencity);
		_Info.Fx->SetTexture("TrailMap", TrailMap->GetTexture());
		_Info.Fx->SetTexture("SpriteMap", nullptr);
	}


	_Info.Fx->SetTexture("EmissiveMskMap", EmissiveMskMap->GetTexture());
	_Info.Fx->SetFloat("EmissiveIntencity", EmissiveIntencity);

	_Info.Fx->SetVector("_Color", &_Color);
	_Info.Fx->SetFloat("ColorIntencity", ColorIntencity);

	_Info.Fx->SetFloat("SpriteXStart", SpriteColIdx / SpriteCol);
	_Info.Fx->SetFloat("SpriteXEnd",  ( SpriteColIdx + 1)/ SpriteCol);
	_Info.Fx->SetFloat("SpriteYStart",SpriteRowIdx/ SpriteRow );
	_Info.Fx->SetFloat("SpriteYEnd", (SpriteRowIdx + 1) / SpriteRow);

	for (int32 i = 0; i < TrailCnt; ++i)
	{
		Device->SetStreamSource(0, VtxBuffers[i], 0, _Desc.VtxSize);
		Device->SetVertexDeclaration(VtxDecl);
		Device->SetIndices(IdxBuffers[i]);
		_Info.Fx->CommitChanges();
		Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, _Desc.VtxCnt, 0, _Desc.NewVtxCnt);
	}
	
}
void CbsTrail::SpriteUpdate(const float DeltaTime)
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
		SpriteCurUpdateCycle += SpriteUpdateCycle;
	}
}
void CbsTrail::BufferUpdate(const float DeltaTime)
{
	_Desc.CurVtxUpdateCycle -= DeltaTime;

	if (_Desc.CurVtxUpdateCycle < 0.0f)
	{
		_Desc.CurVtxUpdateCycle += _Desc.UpdateCycle;

		IndexBufUpdate();
		VertexBufUpdate();
	}
}
void CbsTrail::ParticleUpdate(const float DeltaTime)
{
	CurParticleCycle-= DeltaTime;

	if (CurParticleCycle)
	{
		CurParticleCycle += ParticleCycle;

		if (auto SpTransform = m_pTransform.lock();
			SpTransform)
		{
			auto _PlayableParticle = 
				ParticleSystem::GetInstance()->PlayParticle("IceCbsMid",true);
			// ���� �Ѹ���
			const uint32 JumpOffset = 350u;

			for (int32 i = 0; i < _PlayableParticle.size();
				i += JumpOffset)
			{
				auto& _PlayInstance = _PlayableParticle[i];

				const uint32 BoneIdx = FMath::Random(0u, BoneCnt - 1); 
				
				const Vector3 WorldLocation = 
					FMath::Lerp(LatelyOffsets[BoneIdx].first,
					LatelyOffsets[BoneIdx].second,
					FMath::Random(0.5f,1.f) );

				const Vector3 Scale = SpTransform->GetScale();
				const Vector3 Rotation = Vector3{ 0.f,0.f,0.f };

				_PlayInstance->PlayDescBind(
					FMath::WorldMatrix(Scale, Rotation, WorldLocation));
			}		
		}
	}
}
void CbsTrail::VtxSplineInterpolation(Vertex::TrailVertex* const VtxPtr)
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

void CbsTrail::VertexBufUpdate()
{
	for (int32 i = 0; i < TrailCnt; ++i)
	{
		auto& VtxBuffer = VtxBuffers[i];

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

		if (auto _GameObject = FindGameObjectWithTag(Tag_Cbs_Short).lock();
			_GameObject)
		{
			if (auto _CbsShort = std::dynamic_pointer_cast<Cbs_Short>(_GameObject);
				_CbsShort)
			{
				const auto SwordWorld = _CbsShort->GetComponent<Transform>().lock()->GetWorldMatrix();

				auto Low = _CbsShort->Get_BoneMatrixPtr(BoneNames[i]);
				
				LatelyOffsets[i].first = FMath::Mul(Offset[CurMode].first, *Low * SwordWorld);

				auto High = Low;
				LatelyOffsets[i].second = FMath::Mul(Offset[CurMode].second, *High * SwordWorld);

				VtxPtr[_Desc.NewVtxCnt + 1].Location = LatelyOffsets[i].second;
				VtxPtr[_Desc.NewVtxCnt].Location = LatelyOffsets[i].first;
			}
		};

		_Desc.NewVtxCnt += 2;
		VtxUVCalc(VtxPtr);
		VtxSplineInterpolation(VtxPtr);

		if (bEdit)
		{
			for (int32 i = 0; i < TrailCnt; ++i)
			{
				auto& _CurVtxLog = _VtxLog[i];
				_CurVtxLog.resize(_Desc.NewVtxCnt);
				for (size_t j = 0; j < _CurVtxLog.size(); ++j)
				{
					_CurVtxLog[j] = VtxPtr[j];
				}
			}
		}

		VtxBuffer->Unlock();
	};

};

void CbsTrail::IndexBufUpdate()
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
	for (uint32 i = 0; i < TrailCnt; ++i)
	{
		Vertex::Index32* IdxPtr{ nullptr };
		auto& IdxBuffer = IdxBuffers[i];
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
			auto& _CurIdxLog = _IdxLog[i];
			_CurIdxLog.resize(_Desc.TriCnt);
			for (size_t j = 0; j < _CurIdxLog.size(); ++j)
			{
				_CurIdxLog[j] = IdxPtr[j];
			}
		}

		IdxBuffer->Unlock();
	}
};

void CbsTrail::VtxUVCalc(Vertex::TrailVertex* const VtxPtr)
{
	for (uint32 i = 0; i < _Desc.NewVtxCnt; i += 2)
	{
		VtxPtr[i + 1].UV0 = { ((float)i / ((float)_Desc.NewVtxCnt - 2)) ,0.f };
		VtxPtr[i].UV0 = { ((float)i / ((float)_Desc.NewVtxCnt - 2)),1.f };

		VtxPtr[i + 1].UV1 = { (float)i / ((float)_Desc.NewVtxCnt - 2),0.f };
		VtxPtr[i].UV1 = { (float)i / ((float)_Desc.NewVtxCnt - 2) ,1.f };
	}
};

void CbsTrail::RenderDebug(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("World", &_RenderUpdateInfo.World);

	for (int32 i = 0; i < TrailCnt; ++i)
	{
		auto& VtxBuffer = VtxBuffers[i];
		auto& IdxBuffer = IdxBuffers[i];

		Device->SetStreamSource(0, VtxBuffer, 0, _Desc.VtxSize);
		Device->SetVertexDeclaration(VtxDecl);
		Device->SetIndices(IdxBuffer);
		_Info.Fx->CommitChanges();
		Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, _Desc.VtxCnt, 0, _Desc.DrawTriCnt);
	}
};


HRESULT CbsTrail::Ready()
{
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetPosition(Vector3{ 0.f,0.f,0.f });
	InitTransform.lock()->SetScale(Vector3{ 1.f,1.f,1.f });
	InitTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	return S_OK;
};

HRESULT CbsTrail::Awake()
{
	GameObject::Awake();
	m_pTransform.lock()->SetPosition(Vector3{ 0.f,0.f,0.f });
	m_pTransform.lock()->SetScale(Vector3{ 1.f,1.f,1.f });
	m_pTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	return S_OK;
}

HRESULT CbsTrail::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT CbsTrail::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	if (_RenderProperty.bRender == false) return 0;

	SpriteUpdate(_fDeltaTime);
	BufferUpdate(_fDeltaTime);
	ParticleUpdate(_fDeltaTime);

	return 0;
}

UINT CbsTrail::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
}

void CbsTrail::Editor()
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
				PlayStart(static_cast<CbsTrail::Mode>(_Mode));
			}
			if (ImGui::SmallButton("PlayEnd"))
			{
				PlayEnd();
			}
			ImGui::Text("Cur Col Idx : %d", (int32)SpriteColIdx);
			ImGui::Text("Cur Row Idx : %d", (int32)SpriteRowIdx);

			for (auto& _Vtx : _VtxLog)
			{
				for (auto& _CurVtx : _Vtx)
				{
					ImGui::Text("Location x %9.6f y %9.6f z %9.6f", 
							_CurVtx.Location.x, _CurVtx.Location.y ,_CurVtx.Location.z);
					ImGui::Text(" UV0 %9.6f ,%9.6f", _CurVtx.UV0.x, _CurVtx.UV0.y);
					ImGui::Text(" UV1 %9.6f ,%9.6f", _CurVtx.UV1.x, _CurVtx.UV1.y);
				}
			}
			
			
			ImGui::SliderFloat3("LowOffset",Offset[CurMode].first, -300.f, 300.f, "%9.6f");
			ImGui::SliderFloat3("HighOffset", Offset[CurMode].second, -300.f, 300.f, "%9.6f");
			ImGui::SliderFloat("UpdateCycle", &_Desc.UpdateCycle, FLT_MIN, 10.f, "%9.6f");
			ImGui::SliderFloat("SpriteUpdateCycle", &SpriteUpdateCycle, FLT_MIN, 10.f, "%9.6f");
			ImGui::SliderInt("DrawTriCnt", &_Desc.DrawTriCnt, 0, _Desc.TriCnt);

			ImGui::SliderFloat("DistortionIntencity", &DistortionIntencity, FLT_MIN, 1.f, "%9.6f");
			ImGui::InputFloat("In DistortionIntencity", &DistortionIntencity, FLT_MIN, 1.f, "%9.6f");

			ImGui::SliderFloat("NonDistortionIntencity", &NonDistortionIntencity, FLT_MIN, 1.f, "%9.6f");
			ImGui::InputFloat("In NonDistortionIntencity", &NonDistortionIntencity, FLT_MIN, 1.f, "%9.6f");

			ImGui::SliderFloat3("Noise Scale", Scale, FLT_MIN, 100.f, "%9.6f");
			ImGui::SliderFloat3("Noise ScrollSpeed", ScrollSpeed, FLT_MIN, 100.f, "%9.6f");


			ImGui::InputFloat("ColoIntencity", &ColorIntencity, FLT_MIN,1.f,"%9.6f");
			ImGui::InputFloat("EmissiveIntencity", &EmissiveIntencity, FLT_MIN, 1.f, "%9.6f");
			
			ImGui::ColorEdit4("Color", _Color);
			ImGui::SliderFloat("UV0Multiply", &UV0Multiply,0.f,10.f,"%1.6f");
			ImGui::SliderFloat("CurveT", &CurveT, 0.f, 1.f);
		}
		ImGui::EndChild();
	}
}


void CbsTrail::OnEnable()
{
	GameObject::OnEnable();
}

void CbsTrail::OnDisable()
{
	GameObject::OnDisable();
};



