#ifndef NoCameraColl_h__
#define NoCameraColl_h__

#include "GameObject.h"
#include "RenderInterface.h"
class NoCameraColl : public GameObject,
	public ENGINE::RenderInterface
{
private:
	explicit NoCameraColl();
	virtual ~NoCameraColl() = default;
	// GameObject��(��) ���� ��ӵ�

	virtual void Free() override;
public:
	static NoCameraColl* Create();
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT Update(const float _fDeltaTime) override;
	virtual UINT LateUpdate(const float _fDeltaTime) override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	virtual void Editor()override;
	virtual std::string GetName() override;
public:
	virtual void	OnTriggerEnter(std::weak_ptr<GameObject> _pOther);
	virtual void	OnTriggerExit(std::weak_ptr<GameObject> _pOther);

	// RenderInterface��(��) ���� ��ӵ�
	virtual void RenderReady() override;
	void RenderInit();
private:
	std::weak_ptr<class MainCamera> m_pMainCamera;
	std::weak_ptr<BoxCollider>	m_pCollider;
};



#endif // NoCameraColl_h__