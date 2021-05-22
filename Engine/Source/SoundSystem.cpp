#include "..\Header\SoundSystem.h"
#include <random>
#include <string>
#include "FMath.hpp"

USING(ENGINE)
IMPLEMENT_SINGLETON(SoundSystem)

SoundSystem::SoundSystem()
{

};

void SoundSystem::Free()
{

};

void SoundSystem::PlayFromLocation(
	const std::string& SoundKey,
	float Volume,
	const bool bBeginIfPlaying,
	const std::optional<Vector3>& TargetLocation,
	const bool bLoop)
{
	if (TargetLocation.has_value())
	{
		if (auto SpTargetTransform = TargetTransform.lock();
			SpTargetTransform)
		{
			const float Distance = FMath::Length(TargetLocation.value()-SpTargetTransform->GetPosition());
			Play(SoundKey, Volume, bBeginIfPlaying, Distance, bLoop);
		}
		else
		{
			PRINT_LOG(L"Warning!", L"거리를 비교할 트랜스폼을 설정 안한거 같음.");
		}
	}
	else
	{
		Play(SoundKey, Volume, bBeginIfPlaying, std::nullopt, bLoop);
	}
};

void SoundSystem::Play(
	const std::string& SoundKey,
	float Volume,
	const bool bBeginIfPlaying,
	const std::optional<float>& Distance,
	const bool bLoop)
{
	FMOD_RESULT Result;
	bool bPlay = false;

	if (Distance)
	{
		Volume *= _DistanceDecrease.GetFactor(Distance.value());
	}

	if (auto iter = Sounds.find(SoundKey); iter != std::end(Sounds))
	{
		auto& [Sound, Channel] = iter->second;
		{
			Channel->isPlaying(&bPlay);

			if (bBeginIfPlaying || !bPlay)
			{
				Result = FmodSystem->playSound(Sound.get(), nullptr, false, &Channel);
				if (bLoop)
				{
					LoopInfo _InsertLoopInfo{};
					_InsertLoopInfo.Volume = Volume;
					LoopSoundMap[SoundKey] = _InsertLoopInfo;
				}
			}

			Channel->setVolume(Volume);
		}
	}
};

void SoundSystem::VolumeChange(const std::string& SoundKey, 
							   float Volume,
							   const std::optional<float>& Distance)
{
	FMOD_RESULT Result;
	bool bPlay = false;

	if (Distance)
	{
		Volume *= _DistanceDecrease.GetFactor(Distance.value());
	}

	if (auto iter = Sounds.find(SoundKey); iter != std::end(Sounds))
	{
		auto& [Sound, Channel] = iter->second;
		{
			Channel->isPlaying(&bPlay);

			if (bPlay)
			{
				Channel->setVolume(Volume);

				if (auto LoopSoundIter = LoopSoundMap.find(SoundKey);
					LoopSoundIter != LoopSoundMap.end())
				{
					LoopSoundIter->second.Volume = Volume;
				}
			}
		}
	}
};

void SoundSystem::Stop(const std::string& SoundKey)&
{
	if (auto iter = Sounds.find(SoundKey); iter != std::end(Sounds))
	{
		auto& Channel = iter->second.second;
		Channel->stop();
		LoopSoundMap.erase(SoundKey);
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
	const bool bBeginIfPlaying ,
	const std::optional<float>& Distance,
	const bool bLoop)&
{
	static std::random_device Device;
	static std::mt19937 Mt19937(Device());
	std::uniform_int_distribution<uint32> Distribution(Range.first, Range.second);

	Play(SoundKey + "_" + std::to_string(Distribution(Mt19937)), 
		Volume, bBeginIfPlaying ,
		Distance,bLoop);

}


void SoundSystem::SetDisanceDecrease(const float DistanceMin, const float DistanceMax ,
									std::weak_ptr<Transform> TargetTransform)
{
	_DistanceDecrease.DistanceMin = DistanceMin;
	_DistanceDecrease.DistanceMax = DistanceMax;
	this->TargetTransform = TargetTransform;
};

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

	// 무한 재생 루프
	for (auto&  [LoopSoundKey,LoopDesc]: LoopSoundMap)
	{
		auto iter = Sounds.find(LoopSoundKey);
		if (iter != std::end(Sounds))
		{
			auto& [Sound, Channel] = iter->second;

			bool bPlayed = false;
			Channel->isPlaying(&bPlayed);
			if (!bPlayed)
			{
				FmodSystem->playSound(Sound.get(), nullptr, false, &Channel);
				Channel->setVolume(LoopDesc.Volume);
			}
		};
	};

	return S_OK;
}

void SoundSystem::Editor()
{
	ImGui::Begin("Sound");

	static float EditVolume = 1.f;
	static bool bEditBeginIfPlaying = false;
	static float EditDistance = 0.0f;
	static bool bEditLoop = false;

	ImGui::SliderFloat("EditVolume", &EditVolume, 0.0f, 10.f);
	ImGui::SliderFloat("EditDistance", &EditDistance, 0.0f, 100.f);
	ImGui::Checkbox("bEditBeginIfPlaying", &bEditBeginIfPlaying);
	ImGui::Checkbox("bEditLoop", &bEditLoop);

	ImGui::SliderFloat("Distance Min", &_DistanceDecrease.DistanceMin, 0.0f, 20.f);
	ImGui::SliderFloat("Distance Max", &_DistanceDecrease.DistanceMax, 0.0f, 20.f);

	for (auto& [_SoundKey,_Channel]: Sounds)
	{
		if (ImGui::TreeNode(_SoundKey.c_str()))
		{
			if (ImGui::SmallButton("Play"))
			{
				std::optional<float> OEditDistance;
				if (FMath::AlmostEqual<float>(0.0f, EditDistance))
					OEditDistance = std::nullopt;
				else
					OEditDistance = EditDistance;

				Play(_SoundKey, EditVolume, bEditBeginIfPlaying, EditDistance, bEditLoop);
			};

			if (ImGui::SmallButton("Stop"))
			{
				Stop(_SoundKey);
			};

			ImGui::TreePop();
		}
	}

	ImGui::End();
};

float SoundSystem::DistanceDecrease::GetFactor(const float Distance) const
{
	const float ClampDistance = FMath::Clamp(Distance, DistanceMin, DistanceMax);

	return  (1.0f - (ClampDistance - DistanceMin) / (DistanceMax - DistanceMin));
}
