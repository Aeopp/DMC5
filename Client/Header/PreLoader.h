#ifndef __PRELOADER_H__
#define __PRELOADER_H__

class PreLoader final
{
public:
	PreLoader() = default;
	~PreLoader() = default;
public:
	static void PreLoadResources();
public:
	static void DissolveParticlePoolLoad(
		const std::filesystem::path& _Path,
		const Vector3& StartColor);
public:
	static const inline float SecretVisionDisappearParticleLifeEnd{ 1.33f };
private:
	static void IceCbsMidParticlePoolLoad();
	static void CbsMidParticlePoolLoad();
	static void IceAgeParticlePoolLoad();
	static void ChangeParticlePoolLoad();
	static void ArtemisMissileParticlePoolLoad();
	static void ReverberationParticlePoolLoad();

	static void FireParticlePoolLoad();
	static void FireVortexSpritePoolLoad(const uint32 SpriteIdx);
	static void FireSpritePoolLoad(const uint32 SpriteIdx);
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

	static void SecretVisionDisappearParticlePoolLoad();
	static void DissolveNhDoorParticlePoolLoad();

	static void SVMCParticleEndPoolLoad();

};
#endif // !__PRELOADER_H__