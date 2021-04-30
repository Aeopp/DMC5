#ifndef __TEST_OBJECT_H__
#define __TEST_OBJECT_H__
#include "GameObject.h"
#include "RenderInterface.h"

class TestObject : public ENGINE::GameObject ,
				   public ENGINE::RenderInterface				
{
private:
	typedef struct tagClothBone
	{
		D3DXVECTOR3		vPos;
		string			sName;
		tagClothBone*	pParent;
		D3DXMATRIX		matToRoot;	//월드
		D3DXMATRIX		matLocal;	//부모에 상대적인 행렬
		Node*			pNode;
	}CLOTHBONE, * LPCLOTHBONE;

	vector<LPCLOTHBONE> m_vecClothBone;
	vector<PxU32>		m_vecIndices;
	vector<PxClothParticle>		m_vecClothParticle;

	PxCloth* pCloth;
private:
	std::shared_ptr<ENGINE::SkeletonMesh> m_pSkeletonMesh;
private:
	explicit TestObject() = default;
	virtual ~TestObject() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static TestObject* Create();
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
#endif //