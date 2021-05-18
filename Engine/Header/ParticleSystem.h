#ifndef __PARTICLESYSTEM_H__
#define __PARTICLESYSTEM_H__
#include "Object.h"
#include <chrono>
#include <optional>
#include "FMath.hpp"
#include <any>
#include <tuple>
#include "Renderer.h"

BEGIN(ENGINE)
class ENGINE_DLL ParticleSystem final : public Object
{
	DECLARE_SINGLETON(ParticleSystem)
public:
	struct ENGINE_DLL ParticleInstance
	{
	public:
		struct SpriteDesc
		{
		public:
			static SpriteDesc Make(
						const uint32 SpriteColCnt,
						const uint32 SpriteRowCnt,
						const uint32 SpriteCurCol,
						const uint32 SpriteCurRow,
						const float Interval ,
						const bool bColLoop,
						const bool bRowLoop)
			{
				SpriteDesc _SpriteDesc{};
				_SpriteDesc.SpriteColCnt = static_cast<float>(SpriteColCnt);
				_SpriteDesc.SpriteRowCnt =  static_cast<float>(SpriteRowCnt);
				_SpriteDesc.SpriteCurCol =  static_cast<float>(SpriteCurCol);
				_SpriteDesc.SpriteCurRow =  static_cast<float>(SpriteCurRow);
				_SpriteDesc.CurInterval = _SpriteDesc.Interval = Interval;
				_SpriteDesc.bColLoop = bColLoop;
				_SpriteDesc.bRowLoop = bRowLoop;

				return _SpriteDesc;
			};


			

			void Update(const float Delta)
			{
				CurInterval -= Delta;

				if (CurInterval < 0.0f)
				{
					CurInterval += Interval;

					SpriteCurCol += 1.f;

					if (SpriteCurCol >= SpriteColCnt)
					{
						if (bColLoop)
						{
							SpriteCurCol = 0.0f;
						}
						else
						{
							SpriteCurCol = SpriteColCnt - 1.f;
						}

						if (bRowLoop)
						{
							SpriteCurRow += 1.f;
						}

						if (SpriteCurRow >= SpriteRowCnt)
						{
							SpriteCurRow = 0.0f;
						}
					}
				}

				return;
			}

			// x(0) ~ x(1)'  y(0) ~ y'(1)
			std::tuple<float, float, float, float> GetUVRatio()const&
			{
				return { SpriteCurCol / SpriteColCnt, (SpriteCurCol + 1.f) / SpriteColCnt, SpriteCurRow / SpriteRowCnt, (SpriteCurRow + 1.f) / SpriteRowCnt };
			}
		private:
			float SpriteRowCnt;
			float SpriteColCnt;

			float SpriteCurRow;
			float SpriteCurCol;

			float Interval;
			float CurInterval;

			// Sprite ���� �ٴٶ����� (CurCol == ColCnt) ���� ���� �ƴϸ� ��������
			bool bColLoop = true;
			bool bRowLoop = true;
		};

		void PlayDescBind(const Matrix& World)
		{
			LifeTime = StartLifeTime;
			CurWorld = World;
		};

		void PreSetup(const std::array<Vector3, 4u>& CurveControlPoints,
			const std::array<Vector3, 4u >& CurveControlRotationPoints,
			const Vector3& Scale,
			const float PreLifeTime,
			const float StartT,
			const std::any& InstanceVariable,
			const std::optional<SpriteDesc>& _SpriteDesc)
		{
			this->CurveControlPoints         = CurveControlPoints;
			this->CurveControlRotationPoints = CurveControlRotationPoints;
			this->Scale						 = Scale;
			this->StartT                     = StartT;
			this->StartLifeTime              = PreLifeTime;
			this->InstanceVariable           = InstanceVariable;
			this->_SpriteDesc                = _SpriteDesc;
		}

		Matrix CalcWorld(
			const std::optional<Matrix>& bBillboard) { 
			Matrix World;

			if(bBillboard.has_value()==false)
				World = FMath::WorldMatrix(Scale, CurRotation, CurrentLocation);
			else
			{
				World = FMath::Scale(Scale) * bBillboard.value() * FMath::Translation(CurrentLocation);
			}

			return World;
		};

		const std::optional<SpriteDesc>& GetSpriteDesc(){ return _SpriteDesc; };
		const Vector3& GetCurLocation() { return CurrentLocation; };
		const Vector3& GetCurRotation() { return CurRotation; };
		const std::any& GetInstanceValue() {return InstanceVariable;};
		float GetT() { return T; };
		const std::optional<float>& GetLifeTime(){return LifeTime;}
		const float GetLifeTimeAlphaFactor() const { return LifeTime.value() / StartLifeTime; };

		void Reset()
		{
			LifeTime.reset();
			T = StartT;
		};

		[[nodiscard]] bool Update(const float Delta)
		{
			if (LifeTime)
			{
				T += Delta;

				float& RefLifeTime = *LifeTime;
				RefLifeTime -= Delta;

				if (RefLifeTime < 0.0f)
				{
					Reset();
					return false;
				}

				const float VelocityTime = T / StartLifeTime;

				CurrentLocation =
					FMath::Mul(FMath::BezierCurve(
						(CurveControlPoints)[0],
						(CurveControlPoints)[1],
						(CurveControlPoints)[2],
						(CurveControlPoints)[3],
						VelocityTime), CurWorld);

				CurRotation =
					FMath::BezierCurve
					((CurveControlRotationPoints)[0],
					(CurveControlRotationPoints)[1],
					(CurveControlRotationPoints)[2],
					(CurveControlRotationPoints)[3],
					VelocityTime);

				if (_SpriteDesc)
				{
					_SpriteDesc->Update(Delta);
				}

				return true;
			}

			return false;
		}
	private:
		Matrix CurWorld = FMath::Identity();

		std::optional<SpriteDesc> _SpriteDesc{};
		std::any InstanceVariable{};

		// LifeTime �����ϴ� ��� ��� �ٷ� ���� .
		// LifeTime 0 ���� �۾����� ��� �ٷ� ��� �����ϰ� Ǯ�� ���ư�.
		std::optional<float> LifeTime{ std::nullopt };

		float StartLifeTime = 0.0f;
		// Reserve �Ҷ� ���� ä���ֱ� . 
		Vector3 Scale{ 1.f,1.f,1.f };
		// Ŀ�� ���� 
		std::array<Vector3, 4u> CurveControlPoints{};
		std::array<Vector3, 4u> CurveControlRotationPoints{};

		float StartT = 0.0f;
		float T = 0.0f;

		Vector3 CurrentLocation{0.f,0.f,0.f};
		Vector3 CurRotation{ 0.0f,0.0f,0.0f };
	};

	struct ENGINE_DLL Particle
	{
		// ����
		std::shared_ptr<class StaticMesh> _Mesh{};
		// ���̴� Ű ( �������� ���� ) 
		std::string _ShaderKey{};
		// ���� . 
		std::function<void(Particle&,ID3DXEffect*const)> SharedResourceBind{};
		std::function<void(const std::any& _InstanceValue,ID3DXEffect* const Fx)> InstanceBind{};
		// �� ��� ��ƼŬ �ν��Ͻ� � T�� �ݵ�� 0~1�̰� �� �װ� PlayTime�� 1�� �ʰ��ϴ� ��� T�� 1�� �ʰ���.
		bool bLerpTimeNormalized = false;
		bool bBillboard = false;

		std::vector<ParticleInstance>& RefInstances()
		{
			return _Instances;
		}

		bool bPlayable()
		{
			return !bSleep;
		}
		void Play()
		{
			bSleep = false;
		}
		void Sleep()
		{
			bSleep = true;
		}
	private:
		std::vector<ParticleInstance> _Instances{};
		bool bSleep = true;
	};
// 1�� ����
// 2�� �޽� 
private:
private:
	explicit ParticleSystem();
	virtual ~ParticleSystem() = default;
	// Object��(��) ���� ��ӵ�
	virtual void Free() override;
public:
	HRESULT ReadyParticleSystem();
	HRESULT UpdateParticleSystem(const float Delta);
	HRESULT Render(class Renderer*const _Renderer);
	void	Editor();
public:
	std::vector<ParticleSystem::ParticleInstance>* 
		PreGenerated
	(const std::string& Identifier /*����� �̸����� �����ϼ���*/,
				const Particle& _Particle ,
				const uint64 PoolSize ,
		const bool bBillboard);
	// Reserve �� ��ƼŬ�� ���� Reset ���¿� �� ��ƼŬ��.
	std::vector<ParticleInstance*>PlayParticle(
		const std::string& Identifier ,
		const uint32 RequestParticleCnt,
		const bool bLifeTimeAlphaFactor);
	Particle* PtrParticle(const std::string& Identifier);
private:
	bool bLifeTimeAlphaFactor = false;
	 // ��ƼŬ �ĺ� �̸��� ���� . 
	std::map<std::string, Particle> _Particles{};
};
END
#endif // !__PARTICLESYSTEM_H__
