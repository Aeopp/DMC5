#ifndef __M01_HOTEL_S01_H__
#define __M01_HOTEL_S01_H__
#include "Scene.h"

class Hotel_S01 : public Scene
{
private:
	std::weak_ptr<class Nero> _Player{};
private:
	explicit Hotel_S01();
	virtual ~Hotel_S01() = default;
	// Scene��(��) ���� ��ӵ�
	virtual void Free() override;
public:
	static Hotel_S01* Create();
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
};
#endif // !__M01_HOTEL_S01_H__