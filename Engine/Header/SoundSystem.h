#ifndef __SOUNDSYSTEM_H_
#define __SOUNDSYSTEM_H_
#include "fmod.hpp"
#include "Object.h"
#include "Transform.h"
#include <optional>
#include <chrono>
#include <unordered_map>
#include <functional>
#include <filesystem>

/*
1. ������ ��� ���͸��� ��ȸ�� �ε� (��ø�� ���͸����� �� ������ �ε�)
2. Ȯ���� ��� ����.
3. ���� Ű�� Ȯ���ڸ� ������ ������ �̸�
4. ���� �ٸ� �����ε� ���� �̸��� ���� ��� �������� �ε��� ���ϸ� �����ϹǷ� �̸��� ���ļ� �ȵ� .
*/

BEGIN(ENGINE)
class ENGINE_DLL SoundSystem final : public Object
{
	DECLARE_SINGLETON(SoundSystem)
private:
	explicit SoundSystem();
	virtual ~SoundSystem() = default;
	// Object��(��) ���� ��ӵ�
	virtual void Free() override;
public:
	// �Ÿ��� Min ���� �۰ų� ������ ���� �״�� Min ~ Max ���̿� �ִٸ� �������� ���� . 
	// Max ���� ũ�ٸ� 0 
	struct DistanceDecrease
	{
		float DistanceMin{ 0.f };
		float DistanceMax{ 1.f };
		float GetFactor(const float Distance)const;
	};

	static inline auto FmodDeleter = [](auto  Target)
	{
		Target->release();
	};
	using SoundType = std::pair<
		std::unique_ptr<FMOD::Sound, decltype(FmodDeleter)>, FMOD::Channel*>;
	std::unique_ptr<FMOD::System, decltype(FmodDeleter)> FmodSystem{};

	void Play(
		// ���� Ű (Ȯ���� �ʿ���� ) 
		const std::string & SoundKey,
		// ���� 0~1 
		float Volume,
		/* �÷��� ��û ���尡 ���� ������� ��� ó������ �ٽ� �������� ����*/
		const bool bBeginIfPlaying,
		// �Ÿ��� �Ѱ����� ������ ��û�� ���� �״�� ��� ! 
		// �Ÿ��� �Ѱ��ָ� �Ÿ���� ���� �����ؼ� ���������� ���� �ٿ��� ��� .
		const std::optional<float>& Distance=std::nullopt,
		// ��� ������ �ڵ����� �ٽ� ��� �ұ�� ?
		const bool bLoop=false);

	void PlayFromLocation(
		const std::string& SoundKey,
		float Volume,
		const bool bBeginIfPlaying,
		const std::optional<Vector3>& TargetLocation = std::nullopt,
		const bool bLoop = false);


	// �ش� ���� Ű�� ��� ���̶�� ������ �ٲ���� .
	void VolumeChange(const std::string& SoundKey,
					  float Volume,
				      const std::optional<float>& Distance = std::nullopt);
	// ���߱� . 
	void Stop(const std::string & SoundKey)&;
	void Load(const std::string & FullPath, std::string Key)&;
	// ���� ������ΰ� ? 
	bool IsPlay(const std::string & SoundKey)const&;
	// ���� ���� . ���̹� ��Ģ�� ex [ Rs_1 ,Rs_2 , Rs_3 .... Rs_n ] �̴� .
	/*   ȣ�� ��� ex "Rs",{1u,3u} .*/
	void RandSoundKeyPlay(
		const std::string & SoundKey,
		const std::pair<uint32, uint32> Range,
		const float Volume,
		const bool bBeginIfPlaying ,
		const std::optional<float>& Distance = std::nullopt,
		const bool bLoop = false)&;

	// ������ �ؾ� �ҵ�.
	void SetDisanceDecrease(const  float  DistanceMin
							,const float DistanceMax,
							std::weak_ptr<Transform> _Transform);
private:
	std::unordered_map<std::string, SoundType> Sounds;
	DistanceDecrease _DistanceDecrease{};
	std::set<std::string> LoopSoundKeys{};
	std::weak_ptr<Transform> TargetTransform{};
public:
	// �Ÿ� ��� ���� Ȱ��ȭ ���� .
	HRESULT ReadySoundSystem(const std::filesystem::path & SoundDirectoryPath);
	HRESULT UpdateSoundSystem(const float Delta);
	void Editor();
};
END
#endif // 
