#include "stdafx.h"
#include "..\Header\BtlPanel.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"

void BtlPanel::Free()
{
	SafeDeleteArray(_UIDescs);
}

std::string BtlPanel::GetName()
{
	return "BtlPanel";
}

BtlPanel* BtlPanel::Create()
{
	return new BtlPanel{};
}


void BtlPanel::RenderUI(const DrawInfo& _ImplInfo)
{
	UI_DESC_ID CurID = DESC_END;
	Matrix ScreenMat;

	_ImplInfo.Fx->SetMatrix("Perspective", &_PerspectiveProjMatrix);
	_ImplInfo.Fx->SetTexture("NoiseMap", _NoiseTex->GetTexture());
	_ImplInfo.Fx->SetFloatArray("LightDirection", _LightDir, 3u);

	//
	CurID = EX_GAUGE_BACK;
	if (_UIDescs[CurID].Using)
	{
		auto WeakSubset = _PlaneMesh->GetSubset(0u);
		if (auto SharedSubset = WeakSubset.lock();
			SharedSubset)
		{
			_ImplInfo.Fx->SetTexture("ALB0Map", _ExBackALBMTex->GetTexture());
			_ImplInfo.Fx->SetTexture("ATOS0Map", _ExBackATOSTex->GetTexture());
			_ImplInfo.Fx->SetTexture("NRMR0Map", _ExBackNRMRTex->GetTexture());

			Create_ScreenMat(CurID, ScreenMat);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

			_ImplInfo.Fx->BeginPass(0);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();
		}
	}

	//
	CurID = HP_GLASS;
	if (_UIDescs[CurID].Using)
	{
		auto WeakSubset = _HPGlassMesh->GetSubset(0u);
		if (auto SharedSubset = WeakSubset.lock();
			SharedSubset)
		{
			_ImplInfo.Fx->SetTexture("ATOS0Map", _HPGlassATOSTex->GetTexture());
			_ImplInfo.Fx->SetTexture("NRMR0Map", _HPGlassNRMRTex->GetTexture());
			_ImplInfo.Fx->SetTexture("ALB0Map", _GlassTex->GetTexture());
			_ImplInfo.Fx->SetTexture("ALB1Map", _BloodTex->GetTexture());
			_ImplInfo.Fx->SetFloat("_HPGlassDirt", _HPGlassDirt);

			Create_ScreenMat(CurID, ScreenMat);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

			_ImplInfo.Fx->BeginPass(7);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();
		}
	}

	auto WeakSubset = _PlaneMesh->GetSubset(0u);
	if (auto SharedSubset = WeakSubset.lock();
		SharedSubset)
	{
		//
		CurID = REDORB;
		if (_UIDescs[CurID].Using)
		{
			_ImplInfo.Fx->SetTexture("ALB0Map", _RedOrbALBMTex->GetTexture());
			_ImplInfo.Fx->SetTexture("ATOS0Map", _RedOrbATOSTex->GetTexture());
			_ImplInfo.Fx->SetTexture("NRMR0Map", _RedOrbNRMRTex->GetTexture());

			Create_ScreenMat(CurID, ScreenMat);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

			_ImplInfo.Fx->BeginPass(0);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();
		}

		//
		CurID = TARGET_CURSOR;
		if (_UIDescs[CurID].Using)
		{
			_ImplInfo.Fx->SetTexture("ATOS0Map", _TargetCursorTex->GetTexture());
			_ImplInfo.Fx->SetFloat("_AccumulationTexV", _TotalAccumulateTime * 0.3f);

			for (int i = 0; i < 3; ++i)
			{
				Create_ScreenMat(CurID, ScreenMat, i);
				_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

				_ImplInfo.Fx->BeginPass(1);
				SharedSubset->Render(_ImplInfo.Fx);
				_ImplInfo.Fx->EndPass();
			}
		}

		//
		CurID = BOSS_GUAGE;
		if (_UIDescs[CurID].Using)
		{
			_ImplInfo.Fx->SetTexture("ATOS0Map", _BossGaugeATOSTex->GetTexture());
			_ImplInfo.Fx->SetTexture("NRMR0Map", _BossGaugeNRMRTex->GetTexture());

			_ImplInfo.Fx->SetFloat("_HP_Degree", _TargetHP_Degree);
			_ImplInfo.Fx->SetFloat("_BossGaugeCurXPosOrtho", _BossGauge_CurXPosOrtho);

			Create_ScreenMat(CurID, ScreenMat);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

			_ImplInfo.Fx->BeginPass(3);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();

			_ImplInfo.Fx->BeginPass(4);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();

			_ImplInfo.Fx->BeginPass(5);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();

			_ImplInfo.Fx->BeginPass(6);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();
		}

		//
		CurID = HP_GAUGE;
		if (_UIDescs[CurID].Using)
		{
			_ImplInfo.Fx->SetTexture("ALB0Map", _HPGaugeBaseALBMTex->GetTexture());
			_ImplInfo.Fx->SetTexture("ATOS0Map", _HPGaugeBaseATOSTex->GetTexture());
			_ImplInfo.Fx->SetTexture("NRMR0Map", _HPGaugeBaseNRMRTex->GetTexture());

			for (int i = 0; i < _HPGaugeCount; ++i)
			{
				Create_ScreenMat(CurID, ScreenMat, i);
				_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

				_ImplInfo.Fx->BeginPass(0);
				SharedSubset->Render(_ImplInfo.Fx);
				_ImplInfo.Fx->EndPass();
			}

			_ImplInfo.Fx->SetTexture("ALB0Map", _HPGaugeALBMTex->GetTexture());
			_ImplInfo.Fx->SetTexture("ATOS0Map", _HPGaugeATOSTex->GetTexture());
			_ImplInfo.Fx->SetTexture("NRMR0Map", _HPGaugeNRMRTex->GetTexture());
			_ImplInfo.Fx->SetFloat("_HPGaugeCurXPosOrtho", _HPGauge_CurXPosOrtho);

			for (int i = 0; i < _HPGaugeCount; ++i)
			{
				Create_ScreenMat(CurID, ScreenMat, i);
				_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

				_ImplInfo.Fx->BeginPass(9);
				SharedSubset->Render(_ImplInfo.Fx);
				_ImplInfo.Fx->EndPass();
			}
		}

		//
		CurID = TDT_GAUGE;
		if (_UIDescs[CurID].Using)
		{
			_ImplInfo.Fx->SetTexture("ATOS0Map", _TDTGaugeATOSTex->GetTexture());
			_ImplInfo.Fx->SetTexture("NRMR0Map", _TDTGaugeNRMRTex->GetTexture());
			_ImplInfo.Fx->SetFloat("_AccumulationTexU", _TotalAccumulateTime * 0.3f);
			_ImplInfo.Fx->SetFloat("_AccumulationTexV", _TotalAccumulateTime * 0.1f);
			_ImplInfo.Fx->SetFloat("_TDTGaugeCurXPosOrtho", _TDTGauge_CurXPosOrtho);
			
			Create_ScreenMat(CurID, ScreenMat);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

			_ImplInfo.Fx->BeginPass(10);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();

			_ImplInfo.Fx->BeginPass(11);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();
		}

		//
		CurID = KEYBOARD;
		if (_UIDescs[CurID].Using)
		{
			// 키보드 base
			_ImplInfo.Fx->SetTexture("ALB0Map", _KeyBoardTex->GetTexture());
			_ImplInfo.Fx->SetFloatArray("_MinTexUV", Vector2(0.f, 0.f), 2u);
			_ImplInfo.Fx->SetFloatArray("_MaxTexUV", Vector2(1.f, 0.3f), 2u);

			Create_ScreenMat(CurID, ScreenMat);	
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

			_ImplInfo.Fx->BeginPass(12);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();

			// 마우스 base
			_ImplInfo.Fx->SetFloatArray("_MinTexUV", Vector2(0.f, 0.594f), 2u);
			_ImplInfo.Fx->SetFloatArray("_MaxTexUV", Vector2(0.136f, 0.78f), 2u);

			Create_ScreenMat(CurID, ScreenMat, 999);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

			_ImplInfo.Fx->BeginPass(12);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();

			// 키입력
			_ImplInfo.Fx->SetFloatArray("_MinTexUV", Vector2(0.316f, 0.596f), 2u);
			_ImplInfo.Fx->SetFloatArray("_MaxTexUV", Vector2(0.359f, 0.639f), 2u);

			for (int i = 0; i <= KEY_INPUT_ID::SPACE; ++i)
			{
				if (!_KeyboardInput[i])
					continue;

				Create_ScreenMat(CurID, ScreenMat, i + 1);
				_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

				_ImplInfo.Fx->BeginPass(12);
				SharedSubset->Render(_ImplInfo.Fx);
				_ImplInfo.Fx->EndPass();
			}

			// 마우스
			if (_KeyboardInput[KEY_INPUT_ID::LBUTTON])
			{
				_ImplInfo.Fx->SetFloatArray("_MinTexUV", Vector2(0.15f, 0.596f), 2u);
				_ImplInfo.Fx->SetFloatArray("_MaxTexUV", Vector2(0.205f, 0.666f), 2u);

				Create_ScreenMat(CurID, ScreenMat, KEY_INPUT_ID::LBUTTON + 1);
				_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

				_ImplInfo.Fx->BeginPass(12);
				SharedSubset->Render(_ImplInfo.Fx);
				_ImplInfo.Fx->EndPass();
			}
			if (_KeyboardInput[KEY_INPUT_ID::RBUTTON])
			{
				_ImplInfo.Fx->SetFloatArray("_MinTexUV", Vector2(0.205f, 0.596f), 2u);
				_ImplInfo.Fx->SetFloatArray("_MaxTexUV", Vector2(0.15f, 0.666f), 2u);

				Create_ScreenMat(CurID, ScreenMat, KEY_INPUT_ID::RBUTTON + 1);
				_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

				_ImplInfo.Fx->BeginPass(12);
				SharedSubset->Render(_ImplInfo.Fx);
				_ImplInfo.Fx->EndPass();
			}
			if (_KeyboardInput[KEY_INPUT_ID::MBUTTON])
			{
				_ImplInfo.Fx->SetFloatArray("_MinTexUV", Vector2(0.222f, 0.596f), 2u);
				_ImplInfo.Fx->SetFloatArray("_MaxTexUV", Vector2(0.240f, 0.646f), 2u);

				Create_ScreenMat(CurID, ScreenMat, KEY_INPUT_ID::MBUTTON + 1);
				_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

				_ImplInfo.Fx->BeginPass(12);
				SharedSubset->Render(_ImplInfo.Fx);
				_ImplInfo.Fx->EndPass();
			}

			// 키보드 글자
			_ImplInfo.Fx->SetFloatArray("_MinTexUV", Vector2(0.f, 0.296f), 2u);
			_ImplInfo.Fx->SetFloatArray("_MaxTexUV", Vector2(1.f, 0.592f), 2u);

			Create_ScreenMat(CurID, ScreenMat);	
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

			_ImplInfo.Fx->BeginPass(12);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();
		}

		//
		CurID = RANK_BACK;
		if (_UIDescs[CurID].Using && 0.00001f >_RankDissolveAmount)
		{
			_ImplInfo.Fx->SetTexture("ATOS0Map", _RingTex->GetTexture());

			for (int i = 0; i < 3; ++i)
			{
				Create_ScreenMat(CurID, ScreenMat, i);
				_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

				_ImplInfo.Fx->BeginPass(13);
				SharedSubset->Render(_ImplInfo.Fx);
				_ImplInfo.Fx->EndPass();
			}
		}

		//
		CurID = RANK_LETTER;
		if (_UIDescs[CurID].Using)
		{
			_ImplInfo.Fx->SetTexture("ALB0Map", _RankLetterGlintTex->GetTexture());
			_ImplInfo.Fx->SetFloatArray("_MinTexUV", Vector2(_RankLetter_GlintFrame.x, _RankLetter_GlintFrame.y), 2u);
			_ImplInfo.Fx->SetFloatArray("_MaxTexUV", Vector2(_RankLetter_GlintFrame.z, _RankLetter_GlintFrame.w), 2u);

			Create_ScreenMat(CurID, ScreenMat, 7);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

			_ImplInfo.Fx->BeginPass(12);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();

			_ImplInfo.Fx->SetTexture("ALB0Map", _RankLetterTex->GetTexture());
			_ImplInfo.Fx->SetFloat("_SliceAmount", _RankDissolveAmount);

			Create_ScreenMat(CurID, ScreenMat, _CurRank);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

			_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);	// Create_ScreenMat() 에서 셋팅
			_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);

			_ImplInfo.Fx->BeginPass(16);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();
		}
	}

	//
	CurID = TARGET_HP;
	if (_UIDescs[CurID].Using)
	{
		auto WeakSubset = _Pipe0Mesh->GetSubset(0u);
		if (auto SharedSubset = WeakSubset.lock();
			SharedSubset)
		{
			_ImplInfo.Fx->SetTexture("ATOS0Map", _EnemyHPTex->GetTexture());
			_ImplInfo.Fx->SetFloat("_TotalAccumulateTime", _TotalAccumulateTime);
			
			_ImplInfo.Fx->SetFloat("_HP_Degree", _TargetHP_Degree);
			_ImplInfo.Fx->SetFloatArray("_HP_StartPt", _TargetHP_StartPtOrtho, 2u);
			_ImplInfo.Fx->SetFloatArray("_HP_Normal0", _TargetHP_Normal0, 2u);
			_ImplInfo.Fx->SetFloatArray("_HP_Normal1", _TargetHP_Normal1, 2u);

			Create_ScreenMat(CurID, ScreenMat);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

			_ImplInfo.Fx->BeginPass(2);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();
		}
	}

	//
	CurID = EX_GAUGE;
	if (_UIDescs[CurID].Using)
	{
		_ImplInfo.Fx->SetTexture("ALB0Map", _ExALBM1Tex->GetTexture());
		_ImplInfo.Fx->SetTexture("NRMR0Map", _ExNRMR1Tex->GetTexture());

		auto WeakSubset0 = _Ex0Mesh->GetSubset(1u);
		auto WeakSubset1 = _Ex1Mesh->GetSubset(1u);
		auto WeakSubset2 = _Ex2Mesh->GetSubset(1u);

		if (auto SharedSubset = WeakSubset2.lock();
			SharedSubset)
		{
			Create_ScreenMat(CurID, ScreenMat, 2);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
			_ImplInfo.Fx->BeginPass(8);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();
		}
		if (auto SharedSubset = WeakSubset1.lock();
			SharedSubset)
		{
			Create_ScreenMat(CurID, ScreenMat, 1);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
			_ImplInfo.Fx->BeginPass(8);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();
		}
		if (auto SharedSubset = WeakSubset0.lock();
			SharedSubset)
		{
			Create_ScreenMat(CurID, ScreenMat, 0);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
			_ImplInfo.Fx->BeginPass(8);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();
		}

		_ImplInfo.Fx->SetTexture("ALB0Map", _ExALBM0Tex->GetTexture());
		_ImplInfo.Fx->SetTexture("NRMR0Map", _ExNRMR0Tex->GetTexture());

		WeakSubset0 = _Ex0Mesh->GetSubset(0u);
		WeakSubset1 = _Ex1Mesh->GetSubset(0u);
		WeakSubset2 = _Ex2Mesh->GetSubset(0u);

		if (auto SharedSubset = WeakSubset2.lock();
			SharedSubset)
		{
			Create_ScreenMat(CurID, ScreenMat, 2);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
			_ImplInfo.Fx->BeginPass(8);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();
		}
		if (auto SharedSubset = WeakSubset1.lock();
			SharedSubset)
		{
			Create_ScreenMat(CurID, ScreenMat, 1);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
			_ImplInfo.Fx->BeginPass(8);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();
		}
		if (auto SharedSubset = WeakSubset0.lock();
			SharedSubset)
		{
			Create_ScreenMat(CurID, ScreenMat, 0);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
			_ImplInfo.Fx->BeginPass(8);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();
		}
	}

	//
	CurID = RANK;
	if (_UIDescs[CurID].Using)
	{
		std::weak_ptr<ENGINE::StaticMesh> RankMesh;
		std::weak_ptr<ENGINE::Texture> RankTex;
		switch (_CurRank)
		{
		case 0:
			RankMesh = _RankDMesh;
			RankTex = _RankDCAlbTex;
			break;
		case 1:
			RankMesh = _RankCMesh;
			RankTex = _RankDCAlbTex;
			break;
		case 2:
			RankMesh = _RankBMesh;
			RankTex = _RankBAAlbTex;
			break;
		case 3:
			RankMesh = _RankAMesh;
			RankTex = _RankBAAlbTex;
			break;
		case 4:
			RankMesh = _RankSMesh;
			RankTex = _RankSAlbTex;
			break;
		case 5:
			RankMesh = _RankSSMesh;
			RankTex = _RankSAlbTex;
			break;
		case 6:
			RankMesh = _RankSSSMesh;
			RankTex = _RankSAlbTex;
			break;
		}

		if (!RankMesh.expired())
		{
			_ImplInfo.Fx->SetFloat("_SliceAmount", _RankDissolveAmount);

			auto WeakSubset0 = RankMesh.lock()->GetSubset(0u);
			if (auto SharedSubset = WeakSubset0.lock();
				SharedSubset)
			{
				_ImplInfo.Fx->SetTexture("ALB0Map", _HPGaugeBaseALBMTex->GetTexture());
				_ImplInfo.Fx->SetTexture("NRMR0Map", _NullNormalRoughnessTex->GetTexture());

				Create_ScreenMat(CurID, ScreenMat, _CurRank);
				_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

				_ImplInfo.Fx->BeginPass(14);
				SharedSubset->Render(_ImplInfo.Fx);
				_ImplInfo.Fx->EndPass();
			}

			auto WeakSubset1 = RankMesh.lock()->GetSubset(1u);
			if (auto SharedSubset = WeakSubset1.lock();
				SharedSubset)
			{
				_ImplInfo.Fx->SetTexture("ALB0Map", _RankGradationTex->GetTexture());
				_ImplInfo.Fx->SetTexture("ALB1Map", RankTex.lock()->GetTexture());
				_ImplInfo.Fx->SetTexture("NRMR0Map", _RankNormalTex->GetTexture());
				_ImplInfo.Fx->SetFloat("_RankGaugeCurYPosOrtho", _RankGauge_CurYPosOrtho);
				_ImplInfo.Fx->SetFloat("_AccumulationTexU", _TotalAccumulateTime * 0.1f);

				Create_ScreenMat(CurID, ScreenMat, _CurRank);
				_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

				_ImplInfo.Fx->BeginPass(15);
				SharedSubset->Render(_ImplInfo.Fx);
				_ImplInfo.Fx->EndPass();
			}
		}
	}
};

void BtlPanel::RenderReady()
{
	_RenderProperty.bRender = true;
}

HRESULT BtlPanel::Ready()
{
	SetRenderEnable(true);

	m_nTag = GAMEOBJECTTAG::UI_BtlPanel;

	ENGINE::RenderProperty _InitRenderProp;
	_InitRenderProp.bRender = true;
	_InitRenderProp.RenderOrders[RenderProperty::Order::UI] =
	{
		{"BtlPanel",
		[this](const DrawInfo& _Info)
			{
				this->RenderUI(_Info);
			}
		},
	};
	RenderInterface::Initialize(_InitRenderProp);

	_PlaneMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx");
	_Pipe0Mesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\pipe00.fbx");

	_NoiseTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\noiseInput_ATOS.tga");

	_RedOrbALBMTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\red_orb_albm.tga");
	_RedOrbATOSTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\red_orb_atos.tga");
	_RedOrbNRMRTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\red_orb_nrmr.tga");

	_TargetCursorTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\Cursor_MET.tga");
	_EnemyHPTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\Enemy_HP_Target_01_IM.tga");

	_BossGaugeATOSTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\BossGauge_ATOS.tga");
	_BossGaugeNRMRTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\BossGauge_NRMR.tga");
	
	_HPGlassMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\UI\\hud_hp_glass.fbx");
	_HPGlassATOSTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\HUD_Nero_HP_Glass_ATOS.tga");
	_HPGlassNRMRTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\HUD_Nero_HP_Glass_NRMR.tga");
	_GlassTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\HP_IL_A_ALB.tga");
	_BloodTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\BloodStoneCH16.png");

	_Ex0Mesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\UI\\hud_ex_01.fbx");
	_Ex1Mesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\UI\\hud_ex_02.fbx");
	_Ex2Mesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\UI\\hud_ex_03.fbx");
	_ExBackALBMTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\EXBack_ALBM.tga");
	_ExBackATOSTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\EXBack_ATOS.tga");
	_ExBackNRMRTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\EXBack_NRMR.tga");
	_ExALBM0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\Ex_ALBM.tga");
	_ExNRMR0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\Ex_NRMR.tga");
	_ExALBM1Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\wp00_001_ALBM.tga");
	_ExNRMR1Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\wp00_001_NRMR.tga");

	_HPGaugeBaseALBMTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\HP_GaugeBase_ALBM.tga");
	_HPGaugeBaseATOSTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\HP_GaugeBase_ATOS.tga");
	_HPGaugeBaseNRMRTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\HP_GaugeBase_NRMR.tga");
	_HPGaugeALBMTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\HP_Gauge_ALBM.tga");
	_HPGaugeATOSTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\HP_Gauge_ATOS.tga");
	_HPGaugeNRMRTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\HP_Gauge_NRMR.tga");

	_TDTGaugeATOSTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\TDT_GaugeOut_ATOS.tga");
	_TDTGaugeNRMRTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\TDT_GaugeOut_NRMR.tga");

	_KeyBoardTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\ui8013_01_iam.tga");
	
	_RingTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Light\\tex_capcom_light_ring_0007_alpg.tga");

	_RankDMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\UI\\srt3_d.fbx");
	_RankCMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\UI\\srt3_c.fbx");
	_RankBMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\UI\\srt3_b.fbx");
	_RankAMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\UI\\srt3_a.fbx");
	_RankSMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\UI\\srt3_s.fbx");
	_RankSSMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\UI\\srt3_ss.fbx");
	_RankSSSMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\UI\\srt3_sss.fbx");
	_NullNormalRoughnessTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\NullNormalRoughness.tga");
	_RankGradationTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\SR_Gradation_BM.tga");
	_RankNormalTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\SR_w_NRM.tga");
	_RankLetterTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\ui1014_iam.tga");
	_RankLetterGlintTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\ui1012_iam.tga");
	_RankDCAlbTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\RankDC.png");
	_RankBAAlbTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\RankBA.png");
	_RankSAlbTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\RankS.png");

	D3DXMatrixPerspectiveFovLH(&_PerspectiveProjMatrix, D3DXToRadian(2.5f), (float)g_nWndCX / g_nWndCY, 0.1f, 1.f);
		 
	Init_UIDescs();

	return S_OK;
}

HRESULT BtlPanel::Awake()
{
	return S_OK;
}

HRESULT BtlPanel::Start()
{
	return S_OK;
}

UINT BtlPanel::Update(const float _fDeltaTime)
{
	_DeltaTime = _fDeltaTime;
	_TotalAccumulateTime += _fDeltaTime;

	//
	Update_TargetInfo();
	Update_Rank(_fDeltaTime);
	Update_GaugeOrthoPos();
	Check_KeyInput();

	//
	Imgui_ModifyUI(RANK_LETTER);

	//POINT pt{};
	//GetCursorPos(&pt);
	//ScreenToClient(g_hWnd, &pt);
	//Vector2 TargetPos = Vector2(static_cast<float>(pt.x), static_cast<float>(pt.y));
	//
	//std::cout << ScreenPosToOrtho(0.f, TargetPos.y).y << std::endl;

	return 0;
}          

UINT BtlPanel::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void BtlPanel::Editor()
{
	GameObject::Editor();
}

void BtlPanel::OnEnable()
{

}

void BtlPanel::OnDisable()
{

}

void BtlPanel::SetTargetActive(bool IsActive)
{
	_UIDescs[TARGET_CURSOR].Using = IsActive;
	_UIDescs[TARGET_HP].Using = IsActive;
}

void BtlPanel::SetKeyInputActive(bool IsActive)
{
	_UIDescs[KEYBOARD].Using = IsActive;
}

void BtlPanel::AddRankScore(float Score)
{
	if (_PreRank < static_cast<int>((_RankScore + Score) / 100.f))
	{
		_RankScore = (_PreRank * 100.f + 150.f);
		return;
	}

	_RankScore += Score;
}

void BtlPanel::Init_UIDescs()
{
	if (!_UIDescs)
		_UIDescs = new UI_DESC[DESC_END];

	// Using, Pos, Scale
	_UIDescs[REDORB] = { true, Vector3(1090.f, 50.f, 0.5f), Vector3(0.55f, 0.55f, 1.f) };
	_UIDescs[TARGET_CURSOR] = { false, Vector3(640.f, 360.f, 0.02f), Vector3(0.3f, 0.3f, 1.f) };
	_UIDescs[TARGET_HP] = { false, Vector3(640.f, 360.f, 0.5f), Vector3(0.46f, 0.46f, 1.f) };
	_UIDescs[BOSS_GUAGE] = { true, Vector3(640.f, 670.f, 0.5f), Vector3(4.7f, 5.f, 1.f) };
	_UIDescs[HP_GLASS] = { true, Vector3(240.f, 155.f, 0.4f), Vector3(0.5f, 0.5f, 1.f) };
	_UIDescs[EX_GAUGE_BACK] = { true, Vector3(95.f, 95.f, 0.5f), Vector3(2.f, 2.f, 1.f) };
	_UIDescs[EX_GAUGE] = { true, Vector3(-7.55f, 3.15f, 15.f), Vector3(0.01f, 0.01f, 0.01f) };
	_UIDescs[HP_GAUGE] = { true, Vector3(210.f, 50.f, 0.02f), Vector3(0.5f, 0.5f, 1.f) };
	_UIDescs[TDT_GAUGE] = { true, Vector3(305.f, 75.f, 0.5f), Vector3(3.5f, 3.5f, 1.f) };
	_UIDescs[KEYBOARD] = { true, Vector3(270.f, 570.f, 0.02f), Vector3(5.f, 1.5f, 1.f) };
	_UIDescs[RANK_BACK] = { false, Vector3(1120.f, 270.f, 0.02f), Vector3(_RankBackMaxScale, _RankBackMaxScale, 1.f) };
	_UIDescs[RANK] = { false, Vector3(6.5f, 1.3f, 15.f), Vector3(0.08f, 0.08f, 0.08f) };
	_UIDescs[RANK_LETTER] = { false, Vector3(1120.f, 330.f, 0.02f), Vector3(1.5f, 1.5f, 1.f) };
}

void BtlPanel::Create_ScreenMat(UI_DESC_ID _ID, Matrix& _Out, int _Opt/*= 0*/)
{
	Matrix RotMat;

	D3DXMatrixIdentity(&_Out);

	switch (_ID)
	{
	case TARGET_CURSOR:
		if (1 == _Opt)
		{
			_Out._11 = _UIDescs[_ID].Scale.x;
			_Out._22 = _UIDescs[_ID].Scale.y;
			_Out._33 = _UIDescs[_ID].Scale.z;
			D3DXMatrixRotationZ(&RotMat, D3DXToRadian(120.f));
			_Out *= RotMat;
			_Out._41 = (_UIDescs[_ID].Pos.x - 35.f) - (g_nWndCX >> 1);
			_Out._42 = -((_UIDescs[_ID].Pos.y + 20.f) - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z; 

		}
		else if (2 == _Opt)
		{
			_Out._11 = _UIDescs[_ID].Scale.x;
			_Out._22 = _UIDescs[_ID].Scale.y;
			_Out._33 = _UIDescs[_ID].Scale.z;
			D3DXMatrixRotationZ(&RotMat, D3DXToRadian(-120.f));
			_Out *= RotMat;
			_Out._41 = (_UIDescs[_ID].Pos.x + 35.f) - (g_nWndCX >> 1);
			_Out._42 = -((_UIDescs[_ID].Pos.y + 20.f) - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
		}
		else
		{
			_Out._11 = _UIDescs[_ID].Scale.x;
			_Out._22 = _UIDescs[_ID].Scale.y;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = _UIDescs[_ID].Pos.x - (g_nWndCX >> 1);
			_Out._42 = -((_UIDescs[_ID].Pos.y - 40.f) - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
		}
		break;

	case TARGET_HP:
		_Out._11 = _UIDescs[_ID].Scale.x;
		_Out._22 = _UIDescs[_ID].Scale.y;
		_Out._33 = _UIDescs[_ID].Scale.z;
		_Out._41 = _UIDescs[_ID].Pos.x - (g_nWndCX >> 1);
		_Out._42 = -(_UIDescs[_ID].Pos.y - (g_nWndCY >> 1));
		_Out._43 = _UIDescs[_ID].Pos.z;
		break;

	case HP_GLASS: 
		_Out._11 = _UIDescs[_ID].Scale.x;
		_Out._22 = _UIDescs[_ID].Scale.z * 0.00001f; // y z 축 잘못뽑음 ㅠㅠ
		_Out._33 = _UIDescs[_ID].Scale.y;
		D3DXMatrixRotationX(&RotMat, D3DXToRadian(-90.f));
		_Out *= RotMat;
		_Out._41 = _UIDescs[_ID].Pos.x - (g_nWndCX >> 1);
		_Out._42 = -(_UIDescs[_ID].Pos.y - (g_nWndCY >> 1));
		_Out._43 = _UIDescs[_ID].Pos.z;
		break;

	case EX_GAUGE_BACK:
		_Out._11 = _UIDescs[_ID].Scale.x;
		_Out._22 = _UIDescs[_ID].Scale.y;
		_Out._33 = _UIDescs[_ID].Scale.z;
		D3DXMatrixRotationZ(&RotMat, D3DXToRadian(-50.f));
		_Out *= RotMat;
		_Out._41 = _UIDescs[_ID].Pos.x - (g_nWndCX >> 1);
		_Out._42 = -(_UIDescs[_ID].Pos.y - (g_nWndCY >> 1));
		_Out._43 = _UIDescs[_ID].Pos.z;
		break;

	case EX_GAUGE:
		if (1 == _Opt)
		{
			_Out._11 = _UIDescs[_ID].Scale.x;
			_Out._22 = _UIDescs[_ID].Scale.y;
			_Out._33 = _UIDescs[_ID].Scale.z;
			D3DXMatrixRotationX(&RotMat, D3DXToRadian(35.f));
			_Out *= RotMat;
			D3DXMatrixRotationY(&RotMat, D3DXToRadian(-95.f));
			_Out *= RotMat;
			D3DXMatrixRotationZ(&RotMat, D3DXToRadian(-10.f));
			_Out *= RotMat;
			_Out._41 = -8.2f; //_UIDescs[_ID].Pos.x;
			_Out._42 = 3.65f; //_UIDescs[_ID].Pos.y;
			_Out._43 = 15.5f; //_UIDescs[_ID].Pos.z;
		}
		else if (2 == _Opt)
		{
			_Out._11 = _UIDescs[_ID].Scale.x;
			_Out._22 = _UIDescs[_ID].Scale.y;
			_Out._33 = _UIDescs[_ID].Scale.z;
			D3DXMatrixRotationX(&RotMat, D3DXToRadian(21.f));
			_Out *= RotMat;
			D3DXMatrixRotationY(&RotMat, D3DXToRadian(-89.f));
			_Out *= RotMat;
			D3DXMatrixRotationZ(&RotMat, D3DXToRadian(15.5f));
			_Out *= RotMat;
			_Out._41 = -9.07f; //_UIDescs[_ID].Pos.x;
			_Out._42 = 4.14f; //_UIDescs[_ID].Pos.y;
			_Out._43 = 16.f; //_UIDescs[_ID].Pos.z;
		}
		else // 0
		{
			_Out._11 = _UIDescs[_ID].Scale.x;
			_Out._22 = _UIDescs[_ID].Scale.y;
			_Out._33 = _UIDescs[_ID].Scale.z;
			D3DXMatrixRotationX(&RotMat, D3DXToRadian(180.f));
			_Out *= RotMat;
			D3DXMatrixRotationY(&RotMat, D3DXToRadian(-100.f));
			_Out *= RotMat;
			D3DXMatrixRotationZ(&RotMat, D3DXToRadian(-80.f));
			_Out *= RotMat;
			_Out._41 = -7.55f; //_UIDescs[_ID].Pos.x;
			_Out._42 = 3.15f; //_UIDescs[_ID].Pos.y;
			_Out._43 = 15.f; //_UIDescs[_ID].Pos.z;
		}
		break;

	case HP_GAUGE:
		_Out._11 = _UIDescs[_ID].Scale.x;
		_Out._22 = _UIDescs[_ID].Scale.y;
		_Out._33 = _UIDescs[_ID].Scale.z;
		_Out._41 = (_UIDescs[_ID].Pos.x + _Opt * _HPGaugeWidth) - (g_nWndCX >> 1);
		_Out._42 = -(_UIDescs[_ID].Pos.y - (g_nWndCY >> 1));
		_Out._43 = _UIDescs[_ID].Pos.z;
		break;

	case KEYBOARD:
		switch (_Opt)
		{
		case 999:	// 마우스 base
			_Out._11 = 0.89f;
			_Out._22 = 1.;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 470.f - (g_nWndCX >> 1);
			_Out._42 = -(545.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			break;
		case 1: // Q
			_Out._11 = 0.21f; //_UIDescs[_ID].Scale.x;
			_Out._22 = 0.21f; //_UIDescs[_ID].Scale.y;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 65.f - (g_nWndCX >> 1);
			_Out._42 = -(562.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			break;
		case 2: // W
			_Out._11 = 0.21f; //_UIDescs[_ID].Scale.x;
			_Out._22 = 0.21f; //_UIDescs[_ID].Scale.y;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 86.f - (g_nWndCX >> 1);
			_Out._42 = -(562.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			break;
		case 3: // E
			_Out._11 = 0.21f; //_UIDescs[_ID].Scale.x;
			_Out._22 = 0.21f; //_UIDescs[_ID].Scale.y;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 107.f - (g_nWndCX >> 1);
			_Out._42 = -(562.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			break;
		case 4: // R
			_Out._11 = 0.21f; //_UIDescs[_ID].Scale.x;
			_Out._22 = 0.21f; //_UIDescs[_ID].Scale.y;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 129.f - (g_nWndCX >> 1);
			_Out._42 = -(562.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			break;
		case 5: // A
			_Out._11 = 0.21f; //_UIDescs[_ID].Scale.x;
			_Out._22 = 0.21f; //_UIDescs[_ID].Scale.y;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 76.f - (g_nWndCX >> 1);
			_Out._42 = -(585.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			break;
		case 6: // S
			_Out._11 = 0.21f; //_UIDescs[_ID].Scale.x;
			_Out._22 = 0.21f; //_UIDescs[_ID].Scale.y;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 98.f - (g_nWndCX >> 1);
			_Out._42 = -(585.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			break;
		case 7: // D
			_Out._11 = 0.21f; //_UIDescs[_ID].Scale.x;
			_Out._22 = 0.21f; //_UIDescs[_ID].Scale.y;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 119.f - (g_nWndCX >> 1);
			_Out._42 = -(585.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			break;
		case 8: // A
			_Out._11 = 0.21f; //_UIDescs[_ID].Scale.x;
			_Out._22 = 0.21f; //_UIDescs[_ID].Scale.y;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 141.f - (g_nWndCX >> 1);
			_Out._42 = -(585.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			break;
		case 9: // Z
			_Out._11 = 0.21f; //_UIDescs[_ID].Scale.x;
			_Out._22 = 0.21f; //_UIDescs[_ID].Scale.y;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 86.f - (g_nWndCX >> 1);
			_Out._42 = -(608.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			break;
		case 10: // X
			_Out._11 = 0.21f; //_UIDescs[_ID].Scale.x;
			_Out._22 = 0.21f; //_UIDescs[_ID].Scale.y;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 108.f - (g_nWndCX >> 1);
			_Out._42 = -(608.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			break;
		case 11: // C
			_Out._11 = 0.21f; //_UIDescs[_ID].Scale.x;
			_Out._22 = 0.21f; //_UIDescs[_ID].Scale.y;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 129.f - (g_nWndCX >> 1);
			_Out._42 = -(608.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			break;
		case 12: // W
			_Out._11 = 0.21f; //_UIDescs[_ID].Scale.x;
			_Out._22 = 0.21f; //_UIDescs[_ID].Scale.y;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 150.f - (g_nWndCX >> 1);
			_Out._42 = -(608.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			break;
		case 13: // SHIFT
			_Out._11 = 0.56f;
			_Out._22 = 0.21f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 47.f - (g_nWndCX >> 1);
			_Out._42 = -(607.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			break;
		case 14: // CTRL
			_Out._11 = 0.34f;
			_Out._22 = 0.21f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 38.f - (g_nWndCX >> 1);
			_Out._42 = -(629.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			break;
		case 15: // SPACE
			_Out._11 = 1.01f;
			_Out._22 = 0.21f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 181.f - (g_nWndCX >> 1);
			_Out._42 = -(629.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			break;
		case 16: // LBTN
			_Out._11 = 0.45f;
			_Out._22 = 0.45f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 453.f - (g_nWndCX >> 1);
			_Out._42 = -(517.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			break;
		case 17: // MBTN
			_Out._11 = 0.12f;
			_Out._22 = 0.26f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 474.f - (g_nWndCX >> 1);
			_Out._42 = -(514.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			break;
		case 18: // RBTN
			_Out._11 = 0.45f;
			_Out._22 = 0.45f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 494.f - (g_nWndCX >> 1);
			_Out._42 = -(517.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			break;
		default: case 0:
			_Out._11 = 5.f; //_UIDescs[_ID].Scale.x;
			_Out._22 = 1.5f;// _UIDescs[_ID].Scale.y;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 270.f/*_UIDescs[_ID].Pos.x*/ - (g_nWndCX >> 1);
			_Out._42 = -(570.f/*_UIDescs[_ID].Pos.y*/ - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			break;
		}

		_Out._41 += _InputUIOffset.x;
		_Out._42 += _InputUIOffset.y;

		break;

	case RANK_BACK:
		switch (_Opt)
		{
		case 1:
			_Out._11 = _UIDescs[_ID].Scale.x * 0.666f;
			_Out._22 = _UIDescs[_ID].Scale.y * 0.666f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = _UIDescs[_ID].Pos.x - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			break;
		case 2:
			_Out._11 = _UIDescs[_ID].Scale.x * 0.333f;
			_Out._22 = _UIDescs[_ID].Scale.y * 0.333f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = _UIDescs[_ID].Pos.x - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			break;
		//case 3:
		//	_Out._11 = _UIDescs[_ID].Scale.x * 0.4f;
		//	_Out._22 = _UIDescs[_ID].Scale.y * 0.4f;
		//	_Out._33 = _UIDescs[_ID].Scale.z;
		//	_Out._41 = _UIDescs[_ID].Pos.x - (g_nWndCX >> 1);
		//	_Out._42 = -(_UIDescs[_ID].Pos.y - (g_nWndCY >> 1));
		//	_Out._43 = _UIDescs[_ID].Pos.z;
		//	break;
		//case 4:
		//	_Out._11 = _UIDescs[_ID].Scale.x * 0.2f;
		//	_Out._22 = _UIDescs[_ID].Scale.y * 0.2f;
		//	_Out._33 = _UIDescs[_ID].Scale.z;
		//	_Out._41 = _UIDescs[_ID].Pos.x - (g_nWndCX >> 1);
		//	_Out._42 = -(_UIDescs[_ID].Pos.y - (g_nWndCY >> 1));
		//	_Out._43 = _UIDescs[_ID].Pos.z;
		//	break;
		default: case 0:
			goto DEFAULT;
			break;
		}
		break;

	case RANK:
		if (1 == _Opt) // C
		{
			_Out._11 = _UIDescs[_ID].Scale.x;
			_Out._22 = _UIDescs[_ID].Scale.z; // y z 축 잘못뽑음 ㅠㅠ
			_Out._33 = _UIDescs[_ID].Scale.y;
			D3DXMatrixRotationX(&RotMat, D3DXToRadian(-90.f));
			_Out *= RotMat;
			D3DXMatrixRotationY(&RotMat, D3DXToRadian(_RankCurRotY));
			_Out *= RotMat;
			_Out._41 = _UIDescs[_ID].Pos.x;
			_Out._42 = _UIDescs[_ID].Pos.y + 0.22f;
			_Out._43 = _UIDescs[_ID].Pos.z;
		}
		else if (3 == _Opt) // A
		{
			_Out._11 = _UIDescs[_ID].Scale.x;
			_Out._22 = _UIDescs[_ID].Scale.z; // y z 축 잘못뽑음 ㅠㅠ
			_Out._33 = _UIDescs[_ID].Scale.y;
			D3DXMatrixRotationX(&RotMat, D3DXToRadian(-90.f));
			_Out *= RotMat;
			D3DXMatrixRotationY(&RotMat, D3DXToRadian(_RankCurRotY));
			_Out *= RotMat;
			_Out._41 = _UIDescs[_ID].Pos.x;
			_Out._42 = _UIDescs[_ID].Pos.y - 0.1f;
			_Out._43 = _UIDescs[_ID].Pos.z;
		}
		else
		{
			_Out._11 = _UIDescs[_ID].Scale.x;
			_Out._22 = _UIDescs[_ID].Scale.z; // y z 축 잘못뽑음 ㅠㅠ
			_Out._33 = _UIDescs[_ID].Scale.y;
			D3DXMatrixRotationX(&RotMat, D3DXToRadian(-90.f));
			_Out *= RotMat;
			D3DXMatrixRotationY(&RotMat, D3DXToRadian(_RankCurRotY));
			_Out *= RotMat;
			_Out._41 = _UIDescs[_ID].Pos.x;
			_Out._42 = _UIDescs[_ID].Pos.y;
			_Out._43 = _UIDescs[_ID].Pos.z;
		}

		break;

	case RANK_LETTER:
		switch (_Opt)
		{
		case 0:	// D
			_Out._11 = 1.2f;
			_Out._22 = 0.6f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = _UIDescs[_ID].Pos.x - (g_nWndCX >> 1);
			_Out._42 = -(415.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.f);
			_MaxTexUV = Vector2(0.23f, 0.115f);
			break;
		case 1:	// C
			_Out._11 = 1.2f;
			_Out._22 = 0.6f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = _UIDescs[_ID].Pos.x - (g_nWndCX >> 1);
			_Out._42 = -(420.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.115f);
			_MaxTexUV = Vector2(0.23f, 0.23f);
			break;
		case 2: // B
			_Out._11 = 1.4f;
			_Out._22 = 0.6f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = _UIDescs[_ID].Pos.x - (g_nWndCX >> 1);
			_Out._42 = -(415.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.23f, 0.f);
			_MaxTexUV = Vector2(0.5f, 0.115f);
			break;
		case 3: // A
			_Out._11 = 2.23f;
			_Out._22 = 0.6f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = _UIDescs[_ID].Pos.x - (g_nWndCX >> 1);
			_Out._42 = -(415.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.23f, 0.115f);
			_MaxTexUV = Vector2(0.67f, 0.23f);
			break;
		case 4: // S
			_Out._11 = 1.5f;
			_Out._22 = 0.9f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = _UIDescs[_ID].Pos.x - (g_nWndCX >> 1);
			_Out._42 = -(410.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.25f);
			_MaxTexUV = Vector2(0.3f, 0.4f);
			break;
		case 5: // SS
			_Out._11 = 1.8f;
			_Out._22 = 1.2f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = _UIDescs[_ID].Pos.x - (g_nWndCX >> 1);
			_Out._42 = -(420.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.4f);
			_MaxTexUV = Vector2(0.3f, 0.62f);
			break;
		case 6: // SSS
			_Out._11 = 2.2f;
			_Out._22 = 1.22f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = _UIDescs[_ID].Pos.x - (g_nWndCX >> 1);
			_Out._42 = -(430.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.3f, 0.25f);
			_MaxTexUV = Vector2(0.75f, 0.5f);
			break;
		case 7: // Glint
			_Out._11 = 3.5f;
			_Out._22 = 3.5f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = _UIDescs[_ID].Pos.x - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y - (g_nWndCY >> 1)) - 100.f;
			_Out._43 = _UIDescs[_ID].Pos.z;
			break;
		}
		break;

	default: DEFAULT:
		_Out._11 = _UIDescs[_ID].Scale.x;
		_Out._22 = _UIDescs[_ID].Scale.y;
		_Out._33 = _UIDescs[_ID].Scale.z;
		_Out._41 = _UIDescs[_ID].Pos.x - (g_nWndCX >> 1);
		_Out._42 = -(_UIDescs[_ID].Pos.y - (g_nWndCY >> 1));
		_Out._43 = _UIDescs[_ID].Pos.z;
		break;
	}
}

void BtlPanel::Update_TargetInfo()
{
	Matrix RotMat;
	Vector2 TargetPos = WorldPosToScreenPos(_TargetPos);
	//std::cout << TargetPos.x << " " << TargetPos.y << std::endl;
	
	///////////////////////
	// 임시로 마우스 위치를 타겟 위치로 지정
	///POINT pt{};
	///GetCursorPos(&pt);
	///ScreenToClient(g_hWnd, &pt);
	///TargetPos = Vector2(static_cast<float>(pt.x), static_cast<float>(pt.y));
	///////////////////////

	_UIDescs[TARGET_CURSOR].Pos.x = _UIDescs[TARGET_HP].Pos.x = TargetPos.x;
	_UIDescs[TARGET_CURSOR].Pos.y = _UIDescs[TARGET_HP].Pos.y = TargetPos.y;

	_TargetHP_StartPtOrtho = ScreenPosToOrtho(TargetPos.x, TargetPos.y);
	Vector2 Offset = Vector2(0.f, -100.f);	// EndPt offset (처음은 12시방향. 방향이 중요한거라 값의 크기는 관계 없는듯)

	Vector2 EndPtOrtho = ScreenPosToOrtho(TargetPos.x + Offset.x, TargetPos.y + Offset.y);
	Vector2 Dir = EndPtOrtho - _TargetHP_StartPtOrtho;
	_TargetHP_Normal0 = Vector2(-Dir.y, Dir.x);
	D3DXVec2Normalize(&_TargetHP_Normal0, &_TargetHP_Normal0);

	// offset 회전
	D3DXMatrixRotationZ(&RotMat, D3DXToRadian(_TargetHP_Degree));
	D3DXVec2TransformCoord(&Offset, &Offset, &RotMat);

	EndPtOrtho = ScreenPosToOrtho(TargetPos.x + Offset.x, TargetPos.y + Offset.y);
	Dir = EndPtOrtho - _TargetHP_StartPtOrtho;
	_TargetHP_Normal1 = Vector2(-Dir.y, Dir.x);
	D3DXVec2Normalize(&_TargetHP_Normal1, &_TargetHP_Normal1);
}

void BtlPanel::Update_Rank(const float _fDeltaTime)
{
	_CurRank = static_cast<int>(_RankScore / 100.f);
	if (0.f >= _RankScore)
	{
		_CurRank = -1;
		_RankScore = 0.f;
	}
	else if (699.999f < _RankScore)
	{
		_CurRank = 6;
		_RankScore = 699.999f;
	}

	if (_PreRank != _CurRank)
	{
		if (-1 == _CurRank)
		{
			_UIDescs[RANK_BACK].Using = false;
			_UIDescs[RANK].Using = false;
			_UIDescs[RANK_LETTER].Using = false;
		}
		else
		{
			_UIDescs[RANK_BACK].Using = true;
			_UIDescs[RANK].Using = true;
			_UIDescs[RANK_LETTER].Using = true;
			_UIDescs[RANK_BACK].Using = true;
			_UIDescs[RANK].Using = true;
			_UIDescs[RANK_LETTER].Using = true;
		}

		_RankCurRotY = 45.f;

		_UIDescs[RANK].Pos.z = 12.f;
		_RankGauge_GoalZPos = 15.f;

		_UIDescs[RANK_LETTER].Pos.x = 2000.f;

		switch (_CurRank)
		{
		case -1:
			_RankLetter_GoalXPos = 2000.f;
			break;
		case 0:
			_RankLetter_GoalXPos = 1120.f;
			break;
		case 1:
			_RankLetter_GoalXPos = 1115.f;
			break;
		case 2: case 3: case 4:
			_RankLetter_GoalXPos = 1125.f;
			break;
		case 5:
			_RankLetter_GoalXPos = 1135.f;
			break;
		case 6:
			_RankLetter_GoalXPos = 1115.f;
			break;
		}

		_RankLetter_GlintAccumulateTime = 0.f;

		_RankDissolveAmount = 0.f;

		_PreRank = _CurRank;
	}

	if (0 <= _PreRank)
	{
		float newRankBackScale = _RankBackMaxScale * cosf(_TotalAccumulateTime * 8.f);
		_UIDescs[RANK_BACK].Scale.x = newRankBackScale;
		_UIDescs[RANK_BACK].Scale.y = newRankBackScale;

		_RankCurRotY = 45.f + _RankMaxRotY * sinf(_TotalAccumulateTime);

		float RankGaugeStartYPos = 0.816f;
		float RankGaugeEndYPos = 7.224f;
		_RankGauge_CurYPosOrtho = RankGaugeStartYPos + (static_cast<int>(_RankScore) % 100) / 100.f * (RankGaugeEndYPos - RankGaugeStartYPos);

		if (_UIDescs[RANK].Pos.z < _RankGauge_GoalZPos)
		{
			_UIDescs[RANK].Pos.z += _fDeltaTime * 20.f;
			if (_UIDescs[RANK].Pos.z > _RankGauge_GoalZPos)
				_UIDescs[RANK].Pos.z = _RankGauge_GoalZPos;
		}

		if (_UIDescs[RANK_LETTER].Pos.x > _RankLetter_GoalXPos)
		{
			_UIDescs[RANK_LETTER].Pos.x -= _fDeltaTime * 1500.f;
			if (_UIDescs[RANK_LETTER].Pos.x < _RankLetter_GoalXPos)
				_UIDescs[RANK_LETTER].Pos.x = _RankLetter_GoalXPos;
		}

		if (g_nWndCX >_UIDescs[RANK_LETTER].Pos.x)
			_RankLetter_GlintAccumulateTime += _fDeltaTime * 20.f;

		float cx = 4.f;	// 가로 갯수
		float cy = 4.f; // 세로 갯수
		if (cx * cy > _RankLetter_GlintAccumulateTime)
		{
			int Frame = static_cast<int>(_RankLetter_GlintAccumulateTime);
			int w = Frame % static_cast<int>(cx);
			int h = Frame / static_cast<int>(cx);
			_RankLetter_GlintFrame.x = 1.f / cx * static_cast<float>(w);
			_RankLetter_GlintFrame.y = 1.f / cy * static_cast<float>(h);
			_RankLetter_GlintFrame.z = _RankLetter_GlintFrame.x + 1.f / cx;
			_RankLetter_GlintFrame.w = _RankLetter_GlintFrame.y + 1.f / cy;
		}

		//
		_RankScore -= _fDeltaTime * 10.f;

		float Dissolve = _RankScore - _PreRank * 100.f;
		if (25.f < Dissolve)
			_RankDissolveAmount = 0.f;
		else
			_RankDissolveAmount = 1.f - Dissolve * 0.04f;
	}
}

void BtlPanel::Update_GaugeOrthoPos()
{
	float BossGaugeOrthoOffsetToCenter = 0.344f; // 직접 수작업으로 찾아야 하나 ㅠㅠ
	// + 적 체력 받아와서 degree 같은 애들 갱신하자
	// 일단 임시. 보스게이지가 한가운데 있어서 밑 로직 가능
	_BossGauge_CurXPosOrtho = -BossGaugeOrthoOffsetToCenter + ((360.f - _TargetHP_Degree) / 360.f * 2.f * BossGaugeOrthoOffsetToCenter);

	//
	float HPGaugeOrthoWidth = 0.078125f;
	float HPGaugeOrthoStartX = ScreenPosToOrtho(_UIDescs[HP_GAUGE].Pos.x, 0.f).x - HPGaugeOrthoWidth * 0.5f;
	_HPGauge_CurXPosOrtho = HPGaugeOrthoStartX + (360.f - _TargetHP_Degree) / 360.f * HPGaugeOrthoWidth * static_cast<float>(_HPGaugeCount);

	//
	float TDTGaugeOrthoCenterX = -0.523437f;
	float TDTGagueOrthoOffsetToCenter = 0.18125f;
	_TDTGauge_CurXPosOrtho = (TDTGaugeOrthoCenterX - TDTGagueOrthoOffsetToCenter) + (360.f - _TargetHP_Degree) / 360.f * 2.f * TDTGagueOrthoOffsetToCenter;
}

Vector2 BtlPanel::WorldPosToScreenPos(const Vector3& WorldPos)
{
	const ENGINE::RenderInformation& info = Renderer::GetInstance()->_RenderInfo;
	
	Vector4 Pos = Vector4(WorldPos.x, WorldPos.y, WorldPos.z, 1.f);
	D3DXVec4Transform(&Pos, &Pos, &info.View);
	D3DXVec4Transform(&Pos, &Pos, &info.Projection);

	float x = (Pos.x / Pos.w + 1.f) * (g_nWndCX >> 1);
	float y = (-Pos.y / Pos.w + 1.f) * (g_nWndCY >> 1);

	return Vector2(x, y);
}

Vector2 BtlPanel::ScreenPosToOrtho(float _ScreenPosX, float _ScreenPosY)
{
	Vector2 Ret = Vector2(_ScreenPosX - (g_nWndCX >> 1), -(_ScreenPosY - (g_nWndCY >> 1)));	
	D3DXVec2TransformCoord(&Ret, &Ret, &Renderer::GetInstance()->_RenderInfo.Ortho);
	return Ret;
}

void BtlPanel::Check_KeyInput()
{
	////////////////////////////
	// 임시
	//if (Input::GetKey(DIK_LEFTARROW))
	//{
	//	_TargetHP_Degree += 150.f * _fDeltaTime;
	//	if (360.f < _TargetHP_Degree)
	//		_TargetHP_Degree = 360.f;
	//}
	//if (Input::GetKey(DIK_RIGHTARROW))
	//{
	//	_TargetHP_Degree -= 150.f * _fDeltaTime;
	//	if (0.f > _TargetHP_Degree)
	//		_TargetHP_Degree = 0.f;
	//}
	if (Input::GetKeyDown(DIK_F1))
	{
		static bool bActive = _UIDescs[TARGET_CURSOR].Using;
		bActive = !bActive;
		SetTargetActive(bActive);
	}
	if (Input::GetKeyDown(DIK_F2))
	{
		static bool bActive = _UIDescs[KEYBOARD].Using;
		bActive = !bActive;
		SetKeyInputActive(bActive);
	}
	if (Input::GetKeyDown(DIK_F3))
	{
		AddRankScore(10.f);
	}
	////////////////////////////

	if (!_UIDescs[KEYBOARD].Using)
		return;

	ZeroMemory(_KeyboardInput, KEY_INPUT_END);

	if (Input::GetKey(DIK_Q))
		_KeyboardInput[KEY_INPUT_ID::Q] = true;
	if (Input::GetKey(DIK_W))
		_KeyboardInput[KEY_INPUT_ID::W] = true;
	if (Input::GetKey(DIK_E))
		_KeyboardInput[KEY_INPUT_ID::E] = true;
	if (Input::GetKey(DIK_R))
		_KeyboardInput[KEY_INPUT_ID::R] = true;
	if (Input::GetKey(DIK_A))
		_KeyboardInput[KEY_INPUT_ID::A] = true;
	if (Input::GetKey(DIK_S))
		_KeyboardInput[KEY_INPUT_ID::S] = true;
	if (Input::GetKey(DIK_D))
		_KeyboardInput[KEY_INPUT_ID::D] = true;
	if (Input::GetKey(DIK_F))
		_KeyboardInput[KEY_INPUT_ID::F] = true;
	if (Input::GetKey(DIK_Z))
		_KeyboardInput[KEY_INPUT_ID::Z] = true;
	if (Input::GetKey(DIK_X))
		_KeyboardInput[KEY_INPUT_ID::X] = true;
	if (Input::GetKey(DIK_C))
		_KeyboardInput[KEY_INPUT_ID::C] = true;
	if (Input::GetKey(DIK_V))
		_KeyboardInput[KEY_INPUT_ID::V] = true;
	if (Input::GetKey(DIK_LSHIFT))
		_KeyboardInput[KEY_INPUT_ID::SHIFT] = true;
	if (Input::GetKey(DIK_LCONTROL))
		_KeyboardInput[KEY_INPUT_ID::CTRL] = true;
	if (Input::GetKey(DIK_SPACE))
		_KeyboardInput[KEY_INPUT_ID::SPACE] = true;
	if (Input::GetMouse(DIM_L))
		_KeyboardInput[KEY_INPUT_ID::LBUTTON] = true;
	if (Input::GetMouse(DIM_M))
		_KeyboardInput[KEY_INPUT_ID::MBUTTON] = true;
	if (Input::GetMouse(DIM_R))
		_KeyboardInput[KEY_INPUT_ID::RBUTTON] = true;
}

void BtlPanel::Imgui_ModifyUI(UI_DESC_ID _ID)
{
	ImGui::Text("----- BtlPanel : %d -----", _ID);

	Vector3 Position = _UIDescs[_ID].Pos;
	ImGui::InputFloat3("Position", Position);
	_UIDescs[_ID].Pos = Position;

	Vector3 Scale = _UIDescs[_ID].Scale;
	ImGui::InputFloat3("Scale", Scale);
	_UIDescs[_ID].Scale = Scale;

	Vector3 Rot = _Rot;
	ImGui::InputFloat3("Rot", Rot);
	_Rot = Rot;

	Vector2 MinTexUV = _MinTexUV;
	ImGui::InputFloat2("MinTexUV", MinTexUV);
	_MinTexUV = MinTexUV;

	Vector2 MaxTexUV = _MaxTexUV;
	ImGui::InputFloat2("MaxTexUV", MaxTexUV);
	_MaxTexUV = MaxTexUV;
	
	ImGui::Text("Global Option");

	Vector3 Vector = _LightDir;
	ImGui::SliderFloat3("LightVec", Vector, -1.f, 1.f);
	_LightDir = Vector;

	float HPGlassDirt = _HPGlassDirt;
	ImGui::SliderFloat("GlassDirt", &HPGlassDirt, 0.f, 1.f);
	_HPGlassDirt = HPGlassDirt;

	int HPGaugeCount = _HPGaugeCount;
	ImGui::SliderInt("HPGaugeCount", &HPGaugeCount, 1, 10);
	_HPGaugeCount = HPGaugeCount;

	Vector3 TargetPos = _TargetPos;
	ImGui::SliderFloat3("TargetPos", TargetPos, -10.f, 10.f);
	_TargetPos = TargetPos;

	float RankScore = _RankScore;
	ImGui::SliderFloat("RankScore", &RankScore, 0.f, 699.999f);
	_RankScore = RankScore;

	Vector2 InputUIOffset = _InputUIOffset;
	ImGui::SliderFloat2("InputUIOffset", InputUIOffset, 0, 1000);
	_InputUIOffset = InputUIOffset;

	ImGui::Text("----- BtlPanel End -----");
}