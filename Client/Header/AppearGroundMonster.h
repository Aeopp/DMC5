#ifndef __EFF_APPEAR_GROUND_MONSTER_H__
#define __EFF_APPEAR_GROUND_MONSTER_H__
#include "Effect.h"

class AppearGroundMonster final : public Effect
{
private:
	std::shared_ptr<ENGINE::StaticMesh> _PlaneMesh{};
	
	struct BloodMesh
	{
		std::shared_ptr<ENGINE::StaticMesh> Mesh;
		float SubsetIdx = 0.f;	// uint ����ȯ�ؼ� frame���� ���
		float MaxSubsetIdx = 0.f;
		Matrix ChildWorldMatrix;
	};
	enum { MESH_CNT = 6 };
	std::vector<BloodMesh> _BloodMeshVec;	// <mesh, maxSubsetIdx>

	std::shared_ptr<ENGINE::Texture> _BloodALB0Tex{};
	std::shared_ptr<ENGINE::Texture> _BloodNRMR0Tex{};

	std::shared_ptr<ENGINE::Texture> _DecalBloodNRMR0Tex{};
	std::shared_ptr<ENGINE::Texture> _DecalBloodMsk0Tex{};
	Matrix _DecalBloodChildWorldMatrix = Matrix();

	std::shared_ptr<ENGINE::Texture> _NoiseTex{};

	Vector3 _ExtraColor = Vector3(0.f, 0.f, 0.f);	// ���� albedo�� ������ ��
	float _SliceAmount = 1.f;

private:
	explicit AppearGroundMonster() = default;
	virtual ~AppearGroundMonster() = default;
	// Effect��(��) ���� ��ӵ�
	virtual void Free() override;
	virtual std::string GetName() override;
	virtual void Imgui_Modify() override;
private:
	void RenderInit();
	void RenderGBuffer(const DrawInfo& _Info);
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
public:
	static AppearGroundMonster* Create();
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
#endif // !__EFF_APPEAR_GROUND_MONSTER_H__