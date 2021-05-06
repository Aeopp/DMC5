#ifndef __PARTICLESYSTEM_H__
#define __PARTICLESYSTEM_H__
#include "Object.h"
#include <chrono>
#include <optional>
#include "FMath.hpp"
#include <any>

BEGIN(ENGINE)
class ENGINE_DLL ParticleSystem final : public Object
{
	DECLARE_SINGLETON(ParticleSystem)
public:
	struct ENGINE_DLL ParticleInstance
	{
	public:
		void PlayDescBind(const std::array<Vector3, 4u>& CurveControlPoints,
						const std::array<Vector3, 4u>& CurveControlRotationPoints ,
						const Vector3& Scale,
							const float LifeTime,
							const float T ,
							const std::any& InstanceVariable)
		{
			CurrentLocation= CurveControlPoints[0];
			CurRotation = CurveControlRotationPoints[0];
			this->CurveControlPoints = CurveControlPoints;
			this->CurveControlRotationPoints = CurveControlRotationPoints;
			this->Scale = Scale;
			this->LifeTime = LifeTime;
			this->T = T;
			this->InstanceVariable = InstanceVariable;
		};
		
		Matrix CalcWorld() { return FMath::WorldMatrix(Scale, CurRotation, CurrentLocation); };

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
			}
		}
	private:

		std::any InstanceVariable{};



		// LifeTime �����ϴ� ��� ��� �ٷ� ���� .
		// LifeTime 0 ���� �۾����� ��� �ٷ� ��� �����ϰ� Ǯ�� ���ư�.
		std::optional<float> LifeTime{ std::nullopt };

		// Reserve �Ҷ� ���� ä���ֱ� . 
		Vector3 Scale{ 1.f,1.f,1.f };
		// Ŀ�� ���� 
		std::array<Vector3, 4u> CurveControlPoints{};
		std::array<Vector3, 4u> CurveControlRotationPoints{};




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
	void PreGenerated
	(const std::string& Identifier /*����� �̸����� �����ϼ���*/,
				 const Particle& _Particle ,
				const uint64 PoolSize);
	// Reserve �� ��ƼŬ�� ���� Reset ���¿� �� ��ƼŬ��.
	std::vector<ParticleInstance*>PlayableParticles(const std::string& Identifier,
													const float PlayTime);
	Particle* PtrParticle(const std::string& Identifier);
private:
	 // ��ƼŬ �ĺ� �̸��� ���� . 
	std::map<std::string, Particle> _Particles{};
};
END
#endif // !__PARTICLESYSTEM_H__
