#ifndef __M02_LIBRARY_S05_H__
#define __M02_LIBRARY_S05_H__
#include "Scene.h"

class Library_S05 : public Scene
{
private:
	std::weak_ptr<class Nero> _Player{};
	std::weak_ptr<class BtlPanel> _BtlPanel{};
	bool _LateInit = false;
private:
	explicit Library_S05();
	virtual ~Library_S05() = default;
	// Scene��(��) ���� ��ӵ�
	virtual void Free() override;
public:
	static Library_S05* Create();
public:
	// Scene��(��) ���� ��ӵ�
	virtual HRESULT LoadScene()							override;
	virtual HRESULT Awake()								override;
	virtual HRESULT Start()								override;
	virtual HRESULT Update(const float _fDeltaTime)		override;
	virtual HRESULT LateUpdate(const float _fDeltaTime) override;
private:
	void LoadObjects(const std::filesystem::path& path, const bool _bAni = false);
	void LoadCollObjects(const std::filesystem::path& path);
	void LoadBreakablebjects(const std::filesystem::path& path);
	void BgmPlay();
	void RenderDataSetUp(const bool bTest);
	void TriggerSetUp();
	void LateInit();
};
#endif // !__M02_LIBRARY_S05_H__