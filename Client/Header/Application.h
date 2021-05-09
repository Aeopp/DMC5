#ifndef __APPLICATION_H__
#define __APPLICATION_H__
class Application final
{
public:
	Application();
	~Application() = default;
public:
	HRESULT ReadyApplication(const bool bWindowed,
						const bool bMultiSample);
	HRESULT UpdateApplication(const float Delta);
	HRESULT ReleaseApplication();

private:
	void StaticResourceLoad();

private:
	void ParticlePoolLoad();
	void IceParticlePoolLoad();
	void IceCbsMidParticlePoolLoad();
	void IceAgeParticlePoolLoad();
	void FireParticlePoolLoad();
	void ElectricParticlePoolLoad();

};
#endif // !__APPLICATION_H__