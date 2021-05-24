#pragma once
#ifndef Nero_LWing_h__
#define Nero_LWing_h__

#include "GameObject.h"
#include "RenderInterface.h"
#include "DissolveInfo.h"
class Nero;
class Nero_LWing :	public GameObject,
	public ENGINE::RenderInterface
{
private:
	typedef struct tagClothBone
	{
		string			sName;
		D3DXMATRIX		matToRoot;
		tagClothBone* pParent;
		Node* pNode;
		UINT			nParticleIndex;
	}CLOTHBONE, * LPCLOTHBONE;

	vector<LPCLOTHBONE> m_vecClothBone;

	PxCloth* m_pCloth;

	Matrix* m_pChestRoot;
private:
	explicit Nero_LWing();
	virtual ~Nero_LWing() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
public:
	static Nero_LWing* Create();
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT Update(const float _fDeltaTime) override;
	virtual UINT LateUpdate(const float _fDeltaTime) override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;

	// 렌더링 함수...
	void RenderInit();
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);

	// RenderInterface을(를) 통해 상속됨
	virtual void RenderReady() override;
	virtual void Editor()override;
	virtual std::string GetName() override;
public:
	bool	Set_RenderState(bool _DrawOrNot) { m_bIsRender = _DrawOrNot; }
private:
	std::shared_ptr<ENGINE::SkeletonMesh>	m_pMesh;
	std::weak_ptr<Nero>						m_pNero;

	Matrix*		m_pParentBoneMat;
	bool		m_bIsRender;

	std::shared_ptr<ENGINE::Texture>		m_NRMRTex{};
	std::shared_ptr<ENGINE::Texture>		m_ATOSTex{};
	std::shared_ptr<ENGINE::Texture>		m_GradationTex{};
	std::shared_ptr<ENGINE::SkeletonMesh>	m_AuraMesh{};
	std::shared_ptr<ENGINE::Texture>		m_ClothAuraALBTex{};

	float	m_fAccTime = 0.f;
	Matrix	m_WingCloth01Matrix = Matrix();	// Aura의 ParentBoneMat
	Matrix	m_AuraOffsetMatrix = Matrix();
	Matrix	m_AuraWorldMatrix = Matrix();

	//Vector3 m_AuraRot = Vector3(0.f, 0.f, 0.f);
	DissolveInfo m_DissolveInfo{};
};



#endif // Nero_LWing_h__