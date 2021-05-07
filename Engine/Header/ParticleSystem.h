#ifndef __PARTICLESYSTEM_H__
#define __PARTICLESYSTEM_H__
#include "Object.h"
#include <chrono>
#include <optional>
#include "FMath.hpp"
#include <any>
#include <tuple>

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
						const float SpriteColCnt,
						const float SpriteRowCnt,
						const float SpriteCurCol,
						const float SpriteCurRow,
						const float Interval ,
						const bool bColLoop,
						const bool bRowLoop)
			{
				SpriteDesc _SpriteDesc{};
				_SpriteDesc.SpriteColCnt = SpriteColCnt;
				_SpriteDesc.SpriteRowCnt = SpriteRowCnt;
				_SpriteDesc.SpriteCurCol = SpriteCurCol;
				_SpriteDesc.SpriteCurRow = SpriteCurRow;
				_SpriteDesc.CurInterval = _SpriteDesc.Interval = Interval;
				_SpriteDesc.bColLoop = bColLoop;
				_SpriteDesc.bRowLoop = bRowLoop;
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
							SpriteCurCol  = SpriteColCnt - 1.f;
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

			// Sprite 끝에 다다랐을때 (CurCol == ColCnt) 스톱 할지 아니면 루프할지
			bool bColLoop=true;
			bool bRowLoop=true;
		};

		void PlayDescBind(const std::array<Vector3, 4u>& CurveControlPoints,
						const std::array<Vector3, 4u>& CurveControlRotationPoints ,
						const Vector3& Scale,
						const float LifeTime,
						const float T ,
						const std::any& InstanceVariable ,
						const std::optional<SpriteDesc>& _SpriteDesc)
		{
			CurrentLocation= CurveControlPoints[0];
			CurRotation = CurveControlRotationPoints[0];
			this->CurveControlPoints = CurveControlPoints;
			this->CurveControlRotationPoints = CurveControlRotationPoints;
			this->Scale = Scale;
			this->LifeTime = LifeTime;
			this->T = T;
			this->InstanceVariable = InstanceVariable;
			this->_SpriteDesc = _SpriteDesc;
		};
		
		Matrix CalcWorld() { return FMath::WorldMatrix(Scale, CurRotation, CurrentLocation); };

		const std::optional<SpriteDesc>& GetSpriteDesc(){ return _SpriteDesc; };
		const Vector3& GetCurLocation() { return CurrentLocation; };
		const Vector3& GetCurRotation() { return CurRotation; };
		 const std::any& GetInstanceValue() {return InstanceVariable;};
		float GetT() { return T; };
		const std::optional<float>& GetLifeTime(){return LifeTime;}
		void Reset()
		{
			LifeTime.reset();
			T = 0.0f;
		};

		void Update(const float Delta,
			const float LerpTimeNormalize)
		{
			if (LifeTime)
			{
				T += Delta;

				float& RefLifeTime = *LifeTime;
				RefLifeTime -= Delta;

				if (RefLifeTime < 0.0f)
				{
					Reset();
					return;
				}

				const float VelocityTime = T / LerpTimeNormalize;

				CurrentLocation = FMath::BezierCurve((CurveControlPoints)[0], (CurveControlPoints)[1], (CurveControlPoints)[2], (CurveControlPoints)[3], VelocityTime);
				CurRotation = FMath::BezierCurve((CurveControlRotationPoints)[0], (CurveControlRotationPoints)[1], (CurveControlRotationPoints)[2], (CurveControlRotationPoints)[3], VelocityTime);

				if (_SpriteDesc)
				{
					_SpriteDesc->Update(Delta);
				}
			}
		}
	private:
		std::optional<SpriteDesc> _SpriteDesc{};
		std::any InstanceVariable{};

		// LifeTime 세팅하는 경우 재생 바로 시작 .
		// LifeTime 0 보다 작아지는 경우 바로 재생 정지하고 풀로 돌아감.
		std::optional<float> LifeTime{ std::nullopt };

		// Reserve 할때 정보 채워넣기 . 
		Vector3 Scale{ 1.f,1.f,1.f };
		// 커브 세팅 
		std::array<Vector3, 4u> CurveControlPoints{};
		std::array<Vector3, 4u> CurveControlRotationPoints{};

		float T = 0.0f;
		Vector3 CurrentLocation{0.f,0.f,0.f};
		Vector3 CurRotation{ 0.0f,0.0f,0.0f };
	};

	struct ENGINE_DLL Particle
	{
		// 기하
		std::shared_ptr<class StaticMesh> _Mesh{};
		// 쉐이더 키 ( 렌더러가 관리 ) 
		std::string _ShaderKey{};
		// 개수 . 
		std::function<void(Particle&,ID3DXEffect*const)> SharedResourceBind{};
		std::function<void(const std::any& _InstanceValue,ID3DXEffect* const Fx)> InstanceBind{};
		// 켤 경우 파티클 인스턴스 운동 T는 반드시 0~1이고 안 켰고 PlayTime이 1을 초과하는 경우 T도 1을 초과함.
		bool bLerpTimeNormalized = false;

		float GetDuration()const
		{
			return Duration;
		}
		float GetPlayTime()const 
		{
			return PlayTime;
		}
		void SetPlayTime(const float PlayTime)
		{
			Duration =this->PlayTime = PlayTime;
		}
		std::vector<ParticleInstance>& RefInstances()
		{
			return _Instances;
		}
	private:
		std::vector<ParticleInstance> _Instances{};
		float PlayTime = 0.0f;
		float Duration = 0.0f;
	};
// 1번 기하
// 2번 메쉬 
private:
private:
	explicit ParticleSystem();
	virtual ~ParticleSystem() = default;
	// Object을(를) 통해 상속됨
	virtual void Free() override;
public:
	HRESULT ReadyParticleSystem();
	HRESULT UpdateParticleSystem(const float Delta);
	HRESULT Render(class Renderer*const _Renderer);
	void	Editor();
public:
	void PreGenerated
	(const std::string& Identifier /*등록한 이름으로 제어하세요*/,
				 const Particle& _Particle ,
				const uint64 PoolSize);
	// Reserve 한 파티클중 현재 Reset 상태에 들어간 파티클들.
	std::vector<ParticleInstance*>PlayableParticles(const std::string& Identifier,
													const float PlayTime);
	Particle* PtrParticle(const std::string& Identifier);
private:
	 // 파티클 식별 이름과 매핑 . 
	std::map<std::string, Particle> _Particles{};
};
END
#endif // !__PARTICLESYSTEM_H__
