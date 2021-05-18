#ifndef __SOUNDSYSTEM_H_
#define __SOUNDSYSTEM_H_
#include "fmod.hpp"
#include "Object.h"
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
		const float Volume,
		/*�÷��� ��û ���尡 ���� ������� ��� �ٽ� �������� ����*/
		const bool bBeginIfPlaying,
		// ��� ���� ���� . 
		const bool IsBgm = false)&;
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
