#ifndef __PRELOADER_H__
#define __PRELOADER_H__

class PreLoader final
{
public:
	PreLoader() = default;
	~PreLoader() = default;

public:
	static void PreLoadResources();

private:
	static void IceCbsMidParticlePoolLoad();
	static void CbsMidParticlePoolLoad();

	static void IceAgeParticlePoolLoad();
	static void FireParticlePoolLoad();
	static void ElectricEffectParticlePoolLoad(
		const std::filesystem::path& _TexPath,
		const uint32 SpriteColCnt,
		const uint32 SpriteRowCnt,
		const std::string& ParticleName);
	static void ElectricParticlePoolLoad();
	static void ElectricVoertexParticleLoad();
	static void ThunderBoltParticlePoolLoad();
	static void ThunderBoltEndParticlePoolLoad();
	static void ThunderBoltSecondParticlePoolLoad();
	static void ThunderBoltSecondEndParticlePoolLoad();
	static void ElectricBranchParticlePoolLoad();
	static void ElectricBranchEndParticlePoolLoad();

	static void AirHikeParticleEndPoolLoad();
	static void AirHikeParticlePoolLoad();

};
#endif // !__PRELOADER_H__