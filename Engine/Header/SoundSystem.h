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

	/*struct SoundTransform
	{
		Vector3 Position{};
		Vector3 GetPosition() { return Position; };
	};
	using SoundTransformType = SoundTransform;*/
public:
	// 거리가 Min 보다 작거나 같으면 볼륨 그대로 Min ~ Max 사이에 있다면 선형으로 감소 . 
	// Max 보다 크다면 0 
	struct DistanceDecrease
	{
		float DistanceMin{ 0.f };
		float DistanceMax{ 1.f };
		float GetFactor(const float Distance)const;
	};

	struct SoundInfo
	{
		enum Option : int32
		{
			None=0u,
			Static,
			Dynamic,

			Max,
		};
		std::optional<int32> LoopInfo{std::nullopt};
		Option _Option = Option::None;
		std::optional<Vector3> Location{ std::nullopt };
		std::weak_ptr<Transform> _Transform{};
		float OriginVolume = 1.f;
	};

	static inline auto FmodDeleter = [](auto  Target)
	{
		Target->release();
	};
	using SoundType = std::pair<
		std::unique_ptr<FMOD::Sound,decltype(FmodDeleter)>, FMOD::Channel*>;
	std::unique_ptr<FMOD::System,decltype(FmodDeleter)> FmodSystem{};

	// 거리기반으로 재생 ( 재생 시킨 이후에 거리를 다시 측정해서 볼륨을 조절해주지 않음)
	void Play(
		// 사운드 키 (확장자 필요없음 ) 
		const std::string & SoundKey,
		// 볼륨 0~1 
		float Volume,
		/* 플레이 요청 사운드가 현재 재생중일 경우 처음부터 다시 시작할지 여부*/
		const bool bBeginIfPlaying,
		// 거리를 넘겨주지 않으면 요청한 볼륨 그대로 출력 ! 
		// 거리를 넘겨주면 거리비례 감소 적용해서 내부적으로 볼륨 줄여서 재생 .
		const std::optional<float>& Distance=std::nullopt,
		// 재생 끝나면 자동으로 다시 재생 할까요 ? (0 이면 무조건 끝까지 재생하고 리플레이 0 을 초과하면 해당 값보다 플레이시간이 추월했을경우 리플레이  ) 
		const std::optional<int32>& LoopEnd = std::nullopt);

	// 거리기반으로 재생 ( 재생 시킨 이후에 거리를 다시 측정해서 볼륨을 조절해줌 ! 대상이 움직이지 않을때 사용.)
	void PlayFromLocation(
		const std::string& SoundKey,
		const float Volume,
		const bool bBeginIfPlaying,
		const std::optional<Vector3>& TargetLocation = std::nullopt,
		const std::optional<int32>&  LoopEnd = std::nullopt);

	// 거리기반으로 재생 ( 재생 시킨 이후에 거리를 다시 측정해서 볼륨을 조절해줌 ! 대상이 움직일때 사용.)
	void PlayFromLocation(
		const std::string& SoundKey,
		const float Volume,
		const bool bBeginIfPlaying,
		const std::weak_ptr<Transform>& TargetTransform = {},
		const std::optional<int32>& LoopEnd = std::nullopt);

	// 해당 사운드 키가 재생 중이라면 볼륨을 바꿔줘요 .
	void VolumeChange(const std::string& SoundKey,
					  float Volume,
				      const std::optional<float>& Distance = std::nullopt);

	// 사운드 재생을 끝내버림 .
	void Stop(const std::string & SoundKey)&;
	void Load(const std::string & FullPath, std::string Key)&;
	// 현재 재생중인가요 ?
	bool IsPlay(const std::string & SoundKey)const&;
	// 재생 중이지 않을 경우 음수 재생 중일 경우 현재 포지션 리턴 .
	int32 CurrentPosition(const std::string& SoundKey)const&;
	// 랜덤 사운드 . 네이밍 규칙은 ex [ Rs_1 ,Rs_2 , Rs_3 .... Rs_n ] 이다 .
	/*   호출 방법 ex "Rs",{1u,3u} .*/
	void RandSoundKeyPlay(
		const std::string & SoundKey,
		const std::pair<uint32, uint32> Range,
		const float Volume,
		const bool bBeginIfPlaying ,
		const std::optional<float>& Distance = std::nullopt,
		const std::optional<int32>& LoopEnd = std::nullopt)&;

	// 씬마다 해야 할듯.
	void SetDisanceDecrease(const  float  DistanceMin
							,const float DistanceMax,
							std::weak_ptr<Transform> ListenerTransform);
	void ClearSound();
private:
	std::unordered_map<std::string, SoundType> Sounds{};
	std::unordered_map<std::string, SoundInfo> CurSoundInfoMap{};

	DistanceDecrease _DistanceDecrease{};
	std::weak_ptr<Transform> ListenerTransform{};
public:
	// 거리 비례 감소 활성화 여부 .
	HRESULT ReadySoundSystem(const std::filesystem::path & SoundDirectoryPath);
	HRESULT UpdateSoundSystem(const float Delta);
	void Editor();
};
END
#endif // 
