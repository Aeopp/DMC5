#ifndef __M01_HOTEL_S02_H__
#define __M01_HOTEL_S02_H__
#include "Scene.h"

class Hotel_S02 : public Scene
{
private:
	std::weak_ptr<class Nero> _Player{};
	std::vector<std::weak_ptr<class MakaiButterfly>> _MakaiButterflyVec{};
	bool _LateInit = false;
private:
	explicit Hotel_S02();
	virtual ~Hotel_S02() = default;
	// Scene��(��) ���� ��ӵ�
	virtual void Free() override;
public:
	static Hotel_S02* Create();
public:
	// Scene��(��) ���� ��ӵ�
	virtual HRESULT LoadScene()							override;
	virtual HRESULT Awake()								override;
	virtual HRESULT Start()								override;
	virtual HRESULT Update(const float _fDeltaTime)		override;
	virtual HRESULT LateUpdate(const float _fDeltaTime) override;
private:
	void LoadObjects(const std::filesystem::path& path);
	void RenderDataSetUp();
	void TriggerSetUp();
	void LateInit();
};
#endif // !__M01_HOTEL_S02_H__