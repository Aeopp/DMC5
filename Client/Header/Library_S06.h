#ifndef __M02_LIBRARY_S06_H__
#define __M02_LIBRARY_S06_H__
#include "Scene.h"

class Em5300;
class Trigger;

class Library_S06 : public Scene
{
private:
	std::weak_ptr<class Nero> _Player{};
	std::weak_ptr<class BtlPanel> _BtlPanel{};
	bool _LateInit = false;
private:
	explicit Library_S06();
	virtual ~Library_S06() = default;
	// Scene��(��) ���� ��ӵ�
	virtual void Free() override;
public:
	static Library_S06* Create();
public:
	// Scene��(��) ���� ��ӵ�
	virtual HRESULT LoadScene()							override;
	virtual HRESULT Awake()								override;
	virtual HRESULT Start()								override;
	virtual HRESULT Update(const float _fDeltaTime)		override;
	virtual HRESULT LateUpdate(const float _fDeltaTime) override;
private:
	void LoadObjects(const std::filesystem::path& path);
	void BgmPlay();
	void RenderDataSetUp(const bool bTest);
	void TriggerSetUp();
	std::weak_ptr<Trigger> TriggerUlte();
	void LateInit();
private:
	std::weak_ptr<Em5300> m_pBoss{};
};
#endif // !__M02_LIBRARY_S06_H__