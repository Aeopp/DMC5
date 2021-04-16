#ifndef __TEMPMAP__H_
#define __TEMPMAP__H_
#include "GameObject.h"
#include "RenderInterface.h"

class TempMap : public ENGINE::GameObject ,
				   public ENGINE::RenderInterface				
{
private:
	std::shared_ptr<ENGINE::StaticMesh> _StaticMesh{};
private:
	explicit TempMap() = default;
	virtual ~TempMap() = default;
	// GameObject��(��) ���� ��ӵ�
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static TempMap* Create();
public:
	virtual void    RenderReady()                          override;
public:
	void RenderInit();
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT    Update(const float _fDeltaTime) override;
	virtual UINT    LateUpdate(const float _fDeltaTime) override;
	virtual void    Editor()override;
	virtual void	OnEnable() override;
	virtual void    OnDisable() override;
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderGBuffer(const DrawInfo& _Info);
	void RenderShadow(const DrawInfo& _Info);
};
#endif //__TEMPMAP__H_