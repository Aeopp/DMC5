#ifndef __SOUNDSYSTEM_H_
#define __SOUNDSYSTEM_H_
#include "fmod.hpp"
#include "Object.h"
#include <chrono>
#include <unordered_map>
#include <functional>
#include <filesystem>

/*
1. 폴더의 모든 디렉터리를 순회해 로딩 (중첩된 디렉터리까지 다 뒤져서 로딩)
2. 확장자 상관 없음.
3. 사운드 키는 확장자를 제외한 순수한 이름
4. 서로 다른 폴더인데 파일 이름이 같은 경우 마지막의 로딩된 파일만 존재하므로 이름이 겹쳐선 안됨 .
*/

BEGIN(ENGINE)
class ENGINE_DLL SoundSystem final : public Object
{
	DECLARE_SINGLETON(SoundSystem)
private:
	explicit SoundSystem();
	virtual ~SoundSystem() = default;
	// Object을(를) 통해 상속됨
	virtual void Free() override;
public:
	static inline auto FmodDeleter = [](auto  Target)
	{
		Target->release();
	};
	using SoundType = std::pair<
		std::unique_ptr<FMOD::Sound, decltype(FmodDeleter)>, FMOD::Channel*>;
	std::unique_ptr<FMOD::System, decltype(FmodDeleter)> FmodSystem{};

	void Play(
		// 사운드 키 (확장자 필요없음 ) 
		const std::string & SoundKey,
		// 볼륨 0~1
		const float Volume,
		/*플레이 요청 사운드가 현재 재생중일 경우 다시 시작할지 여부*/
		const bool bBeginIfPlaying,
		// 배경 음악 여부 . 
		const bool IsBgm = false)&;
	// 멈추기 . 
	void Stop(const std::string & SoundKey)&;
	void Load(const std::string & FullPath, std::string Key)&;
	// 현재 재생중인가 ? 
	bool IsPlay(const std::string & SoundKey)const&;
	// 랜덤 사운드 . 네이밍 규칙은 ex [ Rs_1 ,Rs_2 , Rs_3 .... Rs_n ] 이다 .
	/*   호출 방법 ex "Rs",{1u,3u} .*/
	void RandSoundKeyPlay(
		const std::string & SoundKey,
		const std::pair<uint32, uint32> Range,
		const float Volume,
		const bool bBeginIfPlaying)&;
	std::string GetCurrentBgmKey();
private:
	std::unordered_map<std::string, SoundType> Sounds;
	std::string CurrentBgmKey{};
public:
	HRESULT ReadySoundSystem(const std::filesystem::path & SoundDirectoryPath);
	HRESULT UpdateSoundSystem(const float Delta);
	void Editor();
};
END
#endif // 
