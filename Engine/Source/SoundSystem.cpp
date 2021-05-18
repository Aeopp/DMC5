#include "..\Header\SoundSystem.h"
#include <random>
#include <string>

USING(ENGINE)
IMPLEMENT_SINGLETON(SoundSystem)

SoundSystem::SoundSystem()
{

};

void SoundSystem::Free()
{

}
void SoundSystem::Play(
	const std::string& SoundKey,
	const float Volume, 
	const bool bBeginIfPlaying, 
	const bool IsBgm)&
{
	if (IsBgm)
		CurrentBgmKey = SoundKey;

	FMOD_RESULT Result;
	bool bPlay = false;

	if (auto iter = Sounds.find(SoundKey); iter != std::end(Sounds))
	{
		auto& [Sound, Channel] = iter->second;
		{
			Channel->isPlaying(&bPlay);

			if (  bBeginIfPlaying || !bPlay )
			{
				Result = FmodSystem->playSound(Sound.get(), nullptr, false, &Channel);
			}

			Channel->setVolume(Volume);
		}
	}
}
void SoundSystem::Stop(const std::string& SoundKey)&
{
	if (auto iter = Sounds.find(SoundKey); iter != std::end(Sounds))
	{
		auto& Channel = iter->second.second;
		Channel->stop();
	}
};

void SoundSystem::Load(const std::string& FullPath, std::string Key)&
{
	auto iter = Sounds.find(Key);

	if (iter == std::end(Sounds))
	{
		FMOD::Channel* Channel{ nullptr };
		FMOD::Sound* LoadSound{ nullptr };
		FMOD_RESULT HR = FmodSystem->createSound(FullPath.c_str(), FMOD_DEFAULT, 0, &LoadSound);
		if (HR != FMOD_OK)
		{
			PRINT_LOG(L"Log", L"Sound Load Failed!");
		};
		SoundType Sound =
			SoundType{ std::move(std::unique_ptr<FMOD::Sound,
				decltype(FmodDeleter)>(LoadSound,
			FmodDeleter)) ,Channel };

		Sounds.insert(iter, { (Key)  , std::move(Sound) });
	}
};

bool SoundSystem::IsPlay(const std::string& SoundKey) const&
{
	bool bPlay{ false };

	auto SoundIter = Sounds.find(SoundKey);

	if (std::end(Sounds) != SoundIter) {
		auto& [Key, SoundObject] = *SoundIter;
		auto& [Sound, Channel] = SoundObject;
		Channel->isPlaying(&bPlay);
	};

	return bPlay;
};

void SoundSystem::RandSoundKeyPlay(
	const std::string& SoundKey, 
	const std::pair<uint32, uint32> Range,
	const float Volume,
	const bool bBeginIfPlaying)&
{
	static std::random_device Device;
	static std::mt19937 Mt19937(Device());
	std::uniform_int_distribution<uint32> Distribution(Range.first, Range.second);
	Play(SoundKey + "_" + std::to_string(Distribution(Mt19937)), Volume, bBeginIfPlaying);
}

std::string SoundSystem::GetCurrentBgmKey()
{
	return CurrentBgmKey;
}

HRESULT SoundSystem::ReadySoundSystem(const std::filesystem::path& SoundDirectoryPath)
{
	FMOD_RESULT FModResult;
	FMOD::System* CreateFmodSystem;
	FModResult = FMOD::System_Create(&CreateFmodSystem);

	{
		if (FModResult != FMOD_OK)
			throw std::exception(__FUNCTION__);
		FModResult = CreateFmodSystem->init(512, FMOD_INIT_NORMAL, 0);
		if (FModResult != FMOD_OK)
			throw std::exception(__FUNCTION__);
	};

	FmodSystem = std::move(std::unique_ptr<FMOD::System, decltype(FmodDeleter)>(CreateFmodSystem, FmodDeleter));

	namespace fs = std::filesystem;

	for (const fs::directory_entry& entry :
		fs::recursive_directory_iterator(SoundDirectoryPath)) {
		if (entry.is_directory() == false)
		{
			Load(entry.path().string(), entry.path().stem().string());
		}
	}

	return S_OK;
};


HRESULT SoundSystem::UpdateSoundSystem(const float Delta)
{
	if (FmodSystem == nullptr)
	{
		PRINT_LOG(L"Log", L"Sound Update Failed!");
		return E_FAIL;
	}
		
	// Update 함수는 매프레임마다 반드시 호출해줘야함을 요구함
	FmodSystem->update();

	// BGM 무한 재생 루프
	auto iter = Sounds.find(CurrentBgmKey);
	if (iter != std::end(Sounds))
	{
		auto& [Sound, Channel] = iter->second;

		bool bPlayedBgm = false;
		Channel->isPlaying(&bPlayedBgm);
		if (!bPlayedBgm)
		{
			FmodSystem->playSound(Sound.get(), nullptr, false, &Channel);
		}
	};

	return S_OK;
}

void SoundSystem::Editor()
{
	ImGui::Begin("Sound");

	ImGui::End();
}
