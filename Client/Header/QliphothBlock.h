#ifndef __EFF_QLIPHOTH_BLOCK_H__
#define __EFF_QLIPHOTH_BLOCK_H__
#include "Effect.h"

class QliphothBlock final : public Effect
{
private:
	std::shared_ptr<ENGINE::StaticMesh> _Pipe0Mesh{};
	
	std::shared_ptr<ENGINE::Texture> _QliphothALB0Tex{};
	std::shared_ptr<ENGINE::Texture> _QliphothNRMR0Tex{};	// perfection
	std::shared_ptr<ENGINE::Texture> _QliphothNRMR1Tex{};	// liquid

	std::shared_ptr<ENGINE::Texture> _NoiseTex{};

	float _SliceAmount = 0.f;

private:
	explicit QliphothBlock() = default;
	virtual ~QliphothBlock() = default;
	// Effect��(��) ���� ��ӵ�
	virtual void Free() override;
	virtual std::string GetName() override;
	virtual void Imgui_Modify() override;
private:
	void RenderInit();
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
public:
	static QliphothBlock* Create();
public:
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT    Update(const float _fDeltaTime) override;
	virtual UINT    LateUpdate(const float _fDeltaTime) override;
	virtual void    Editor()override;
	virtual void	OnEnable() override;
	virtual void    OnDisable() override;
public:
	virtual void Reset() override;
};
#endif // !__EFF_QLIPHOTH_BLOCK_H__