#include "..\Header\ParticleSystem.h"
#include "Renderer.h"
#include "Subset.h"

USING(ENGINE)
IMPLEMENT_SINGLETON(ParticleSystem)

ParticleSystem::ParticleSystem()
{

};

void ParticleSystem::Free()
{

};

HRESULT ParticleSystem::ReadyParticleSystem()
{

	return S_OK;
};

std::vector<ParticleSystem::ParticleInstance>* 
ParticleSystem::PreGenerated
(const std::string& Identifier /*등록한 이름으로 제어하세요*/,
	const Particle& _Particle ,
	const uint64 PoolSize )
{
	auto& _RefParticle = _Particles[Identifier] = (_Particle);
	auto& _RefParticleInstances = _RefParticle.RefInstances();
	_RefParticleInstances.resize(PoolSize);
	return &_RefParticleInstances;
};

ParticleSystem::Particle* ParticleSystem::PtrParticle(const std::string& Identifier)
{
	auto iter = _Particles.find(Identifier);
	if (iter != std::end(_Particles))
	{
		return (&iter->second);
	};

	return nullptr;
};

HRESULT ParticleSystem::UpdateParticleSystem(const float Delta)
{
	for (auto& [Key , _TargetParticle ] : _Particles)
	{
		// _TargetParticle.SetPlayTime(_TargetParticle.GetPlayTime() - Delta);
		auto& _RefTargetParticleInstances = _TargetParticle.RefInstances();

		if (_TargetParticle.bPlayable())
		{
			// const float LerpTimeNormalize = _TargetParticle.bLerpTimeNormalized ? _TargetParticle.GetDuration() : 1.0f;

			bool bAnyOne = false;

			for (auto& _ParticleInstance : _RefTargetParticleInstances)
			{
				bAnyOne  |= _ParticleInstance.Update(Delta);
			}

			if (false == bAnyOne)
			{
				_TargetParticle.Sleep();
			}
		}
	/*	else
		{
			auto& _RefTargetParticleInstances = _TargetParticle.RefInstances();

			for (auto& _ParticleInstance : _RefTargetParticleInstances)
			{
				_ParticleInstance.Reset();
			}
		}*/
	}

	return S_OK;
}


HRESULT ParticleSystem::Render(class Renderer* const _Renderer)
{
	for (auto& [Key, _TargetParticle] : _Particles)
	{
		if (_TargetParticle.bPlayable())
		{
			if (auto iter = _Renderer->Shaders.find(_TargetParticle._ShaderKey);
				iter != std::end(_Renderer->Shaders))
			{
				auto& CurShader = iter->second;
				auto* const CurFx = CurShader->GetEffect();

				CurFx->SetMatrix("ViewProjection", &_Renderer->_RenderInfo.ViewProjection);
				CurFx->SetMatrix("InverseProjection", &_Renderer->_RenderInfo.ProjectionInverse);
				CurFx->SetTexture("DepthMap", _Renderer->RenderTargets["Depth"]->GetTexture());
				CurFx->SetFloat("SoftParticleDepthScale", _Renderer->SoftParticleDepthScale);
				CurFx->SetFloat("exposure_corr", 1.f / (_Renderer->exposure * 0.002f));
				
				Vector3  LightDirection = { 0,-1,0 };
				if (_Renderer->CurDirLight)
				{
					LightDirection = _Renderer->CurDirLight->GetDirection();
				}

				LightDirection = FMath::Normalize(LightDirection);

				CurFx->SetFloatArray("LightDirection", LightDirection, 3u);
				CurFx->SetFloatArray("EyePos", _Renderer->_RenderInfo.Eye, 3u);

				_TargetParticle.SharedResourceBind(_TargetParticle, CurFx);

				UINT Passes{ 0u };
				CurFx->Begin(&Passes, NULL);
				{
					for (int32 i = 0; i < Passes; ++i)
					{
						CurFx->BeginPass(i);

						auto& _RefTargetParticleInstances = _TargetParticle.RefInstances();

						for (auto& _ParticleInstance : _RefTargetParticleInstances)
						{
							if (_ParticleInstance.GetLifeTime().has_value())
							{
								if (auto _Subset = _TargetParticle._Mesh->GetSubset(0).lock();
									_Subset)
								{
									const Matrix matWorld = _ParticleInstance.CalcWorld();
									CurFx->SetFloat("LifeTimeAlphaFactor", _ParticleInstance.GetLifeTimeAlphaFactor());
									CurFx->SetMatrix("matWorld", &matWorld);

									if (const auto& bTargetSprite = _ParticleInstance.GetSpriteDesc();
										bTargetSprite)
									{
										const auto [XStart,XEnd,YStart,YEnd]=bTargetSprite->GetUVRatio();
										CurFx->SetFloat("SpriteXStart", XStart);
										CurFx->SetFloat("SpriteXEnd", XEnd);
										CurFx->SetFloat("SpriteYStart", YStart);
										CurFx->SetFloat("SpriteYEnd", YEnd);
									}

									_TargetParticle.InstanceBind(_ParticleInstance.GetInstanceValue(), CurFx);
									_Subset->Render(CurFx);
								}
							}
						}

						CurFx->EndPass();
					}
				}
				CurFx->End();
			}
		}
	}

	return S_OK;
}

void ParticleSystem::Editor()
{
	ImGui::Begin("ParticleSystem");

	ImGui::End();
};

std::vector<ParticleSystem::ParticleInstance*> ParticleSystem::PlayParticle(
	const std::string& Identifier ,
	const bool bLifeTimeAlphaFactor)
{
	std::vector<ParticleSystem::ParticleInstance*> _Particles{};

	if (auto* const _PtrParticle = PtrParticle(Identifier);
		_PtrParticle)
	{
		auto& _PtrParticleRefInstance = _PtrParticle->RefInstances();

		_Particles.reserve(_PtrParticleRefInstance.size());

		for (auto& _TargetInstance : _PtrParticleRefInstance)
		{
			if (_TargetInstance.GetLifeTime().has_value() == false)
			{
				_Particles.push_back(&_TargetInstance);
			}
		}

		if (_Particles.empty() == false)
		{
			_PtrParticle->Play();
			this->bLifeTimeAlphaFactor = bLifeTimeAlphaFactor;
		}
	}

	return _Particles;
};

