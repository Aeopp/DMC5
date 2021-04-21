#ifndef __EFF_LIQUID_H__
#define __EFF_LIQUID_H__
#include "Effect.h"
#include <vector>

class Liquid final : public Effect
{
public:
	enum VARIATION : uint32
	{
		BLOOD_0 = 0u,
		BLOOD_1,
		BLOOD_2,
		BLOOD_3,
		BLOOD_4,
		BLOOD_5,
		BLOOD_6,
		BLOOD_7,
		BLOOD_8,
		BLOOD_9,
		BLOOD_10,
		MAX_VARIATION_IDX
	};
	void SetVariationIdx(Liquid::VARIATION Idx);

private:
	std::vector<std::shared_ptr<ENGINE::StaticMesh>> _LiquidMeshVec;
	std::shared_ptr<ENGINE::Texture> _BloodALB0Tex{};
	std::shared_ptr<ENGINE::Texture> _BloodNRMR0Tex{};
	std::shared_ptr<ENGINE::Texture> _MeatALB0Tex{};
	std::shared_ptr<ENGINE::Texture> _MeatNRMR0Tex{};

	uint32 _SubsetIdx = 0u;	// �ִϸ��̼� ������ ���� ����
	uint32 _SubsetMaxIdx = 0u;
	Vector3 _ExtraColor = Vector3(0.f, 0.f, 0.f);	// ���� albedo�� ������ ��

	Liquid::VARIATION _VariationIdx = MAX_VARIATION_IDX;	// �� �������̼�

private:
	explicit Liquid() = default;
	virtual ~Liquid() = default;
	// Effect��(��) ���� ��ӵ�
	virtual void Free() override;
	virtual std::string GetName() override;
	virtual void Reset() override;
	virtual void Imgui_Modify() override;
private:
	void RenderInit();
	void RenderGBuffer(const DrawInfo& _Info);
public:
	static Liquid* Create();
public:
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT    Update(const float _fDeltaTime) override;
	virtual UINT    LateUpdate(const float _fDeltaTime) override;
	virtual void    Editor()override;
	virtual void	OnEnable() override;
	virtual void    OnDisable() override;
};
#endif // !__EFF_LIQUID_H__