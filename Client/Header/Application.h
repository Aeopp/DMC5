#ifndef __APPLICATION_H__
#define __APPLICATION_H__
class Application final
{
public:
	Application() = default;
	~Application() = default;

public:
	HRESULT ReadyApplication(const bool bWindowed, const bool bMultiSample ,
							const std::filesystem::path& SoundDirectoryPath ,
							const bool bImguiInit);
	HRESULT UpdateApplication(const float Delta);
	HRESULT ReleaseApplication();

};
#endif // !__APPLICATION_H__