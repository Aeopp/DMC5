#ifndef __APPLICATION_H__
#define __APPLICATION_H__
class Application final
{
public:
	Application() = default;
	~Application() = default;

public:
	HRESULT ReadyApplication(const bool bWindowed, const bool bMultiSample);
	HRESULT UpdateApplication(const float Delta);
	HRESULT ReleaseApplication();
private:
	void StaticResourceLoad();
private:
	void ParticlePoolLoad();
	void IceParticlePoolLoad();
	void IceCbsMidParticlePoolLoad();
	void CbsMidParticlePoolLoad();

	void IceAgeParticlePoolLoad();
	void FireParticlePoolLoad();
	void ElectricEffectParticlePoolLoad(const std::filesystem::path& _TexPath,
		const uint32 SpriteColCnt,
		const uint32 SpriteRowCnt,
		const std::string& ParticleName);
	void ElectricParticlePoolLoad();
	void ElectricVoertexParticleLoad();
	void ThunderBoltParticlePoolLoad();
	void ThunderBoltEndParticlePoolLoad();
	void ThunderBoltSecondParticlePoolLoad();
	void ThunderBoltSecondEndParticlePoolLoad();
	void ElectricBranchParticlePoolLoad();
	void ElectricBranchEndParticlePoolLoad();

	void AirHikeParticleEndPoolLoad();
	void AirHikeParticlePoolLoad();
};
#endif // !__APPLICATION_H__