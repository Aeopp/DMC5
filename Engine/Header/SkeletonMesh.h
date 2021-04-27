#ifndef _SKELETONMESH_H_
#define _SKELETONMESH_H_
#include "StaticMesh.h"
#include "AnimationBlendInfo.h"
#include "AnimationTrack.h"
#include "Node.h"
#include "AnimationInformation.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <map>
#include <map>
#include <functional>
#include <set>
#include "AnimNotify.h"
#include <optional>


class aiNode;

BEGIN(ENGINE)
class ENGINE_DLL SkeletonMesh final : public StaticMesh
{
private:
	explicit SkeletonMesh(LPDIRECT3DDEVICE9 const _pDevice);
	explicit SkeletonMesh(const SkeletonMesh& _rOther);
	virtual ~SkeletonMesh() = default;
	// Mesh��(��) ���� ��ӵ�
	virtual void Free() override;
public:
	static SkeletonMesh* Create(LPDIRECT3DDEVICE9 const _pDevice,
								const std::filesystem::path _Path,
								const std::any & InitParams);
	// Mesh��(��) ���� ��ӵ�
	virtual Resource* Clone() override;
	virtual void      Editor()override;
	virtual std::string GetName() override;
	void BindVTF(ID3DXEffect * Fx)&;
public:
	void    UpdateToRootMatricies();
	bool    IsAnimationEnd();
	void    EnableToRootMatricies();
	void    DisableToRootMatricies();
	// Prev VTF �ѱ� . (��Ǻ��� ���� ��Ű�� ������ �ʿ��� ���
	void    EnablePrevVTF()&;
	// Prev VTF ���� . (��Ÿ�ӿ� ���� ������ ���� �Ѱ� ���� ���� ��������)
	// _SkeletonMesh->DisablePrevVTF();
	void    DisablePrevVTF()&;
	std::tuple<Vector3, Quaternion, Vector3>   
		Update(const float DeltaTime)&;
	void    TPose();
	void    BoneDebugRender(const Matrix & OwnerTransformWorld,ID3DXEffect* const Fx)&;
	void    VTFUpdate()&;
	Node* GetRootNode()&;
	Node* GetNode(const std::string & NodeName)&;
	//      �� ��Ű�� ��Ʈ�������� ToRoot ��Ʈ������ ��� 
	//      (���� ��Ű�� ������Ʈ�� ���� �ʴ´ٸ� ��ȯ���� ������ ��Ű�� �������� ����)
	//      �ش� ��ȯ���� ���� ���� �ʴ´ٸ� ��ȿ�� ��尡 ���� ��.
	//      �ش� ��ȯ���� �ش� ���(��)�� �ִϸ��̼� �� ���
	//      ��ȯ�� * World �ϸ� �ش� ������Ʈ�� �ش� ���� ����� ����.
	std::optional<Matrix> GetNodeToRoot
					(const std::string & NodeName)&;
	// ��尡 �������� �ʴ´ٸ� �� ������ . ToRoot ��Ʈ������ ��� ������Ʈ �Ǳ� ����Ѵٸ�
	// ���̷��� �޽� ������Ʈ �ݵ�� ȣ�� 
	// EnableToRootMatricies ������� Ȯ�� (����ȭ�� ���� �⺻ �ɼ��� �ƴ� ) 
	Matrix* GetToRootMatrixPtr(const std::string & NodeName)&;

	void   PlayAnimation(
		const std::string & InitAnimName,
		const bool  bLoop,
		const AnimNotify & _Notify = {} ,
		const float _CurrentAccelerationFactor = 1.0f,
		const float _CurrentTransitionTimeFactor = 1.0f,
		bool _Overlap = false);
	void   PlayAnimation(
		const uint32 AnimationIndex,
		const bool  bLoop,
		const AnimNotify & _Notify = {} ,
		const float _CurrentAccelerationFactor = 1.0f,
		const float _CurrentTransitionTimeFactor = 1.0f);

	void    ContinueAnimation()&;
	void    StopAnimation();
	void	AnimationEnd()&;
	// 0 ~ 1 ����ȭ 
	float   PlayingTime();
	// 0 ~ 1 ����ȭ ���� ���� �ð� ( ���� �ִϸ��̼� ���� )  
	float   PlayingOriginTime();
	float   PlayingAccTime();
	//  ����ȭ�� �ð����� �Ѱ��ּ��� ������ ����� Clamp
	void    SetPlayingTime(float NewTime);
	std::optional<AnimationInformation> GetAnimInfo(const std::string & AnimName) const&;

	void SetDeltaTimeFactor(const float DeltaTimeFactor);
private:
	void	AnimationEditor()&;
	void	NodeEditor();
	std::tuple<Vector3, Quaternion, Vector3>    AnimationUpdateImplementation()&;
	void AnimationSave(const std::filesystem::path & FullPath)&;
private:
	virtual HRESULT LoadMeshImplementation(
		const aiScene * AiScene,
		const std::filesystem::path _Path,
		const std::any & InitParams)override;

	Node* MakeHierarchy(Node * NodeParent, const aiNode* const AiNode ,
		const std::unordered_map<std::string,
		std::pair<uint32, Matrix>>&BoneTableParserInfo);
	// Node* MakeHierarchyForclones(Node* const Parent,const Node* const SpProtoNode);
	void InitTextureForVertexTextureFetch()&;
	void AnimationNotify()&;

	static inline std::string NormallyRootMotionTransitionName =
		"NULL_TRANSITION";
	static inline std::string NormallyRootMotionScaleName =
		"NULL_SCALING";
	static inline std::string NormallyRootMotionRotationName =
		"NULL_ROTATION";
private:
	std::shared_ptr<std::set<std::filesystem::path>>
		LoadAnimationFromDirectoryPathSet{};
	std::shared_ptr<std::set<std::filesystem::path>>
		LoadAnimationPathSet{};
	std::shared_ptr<std::set<std::filesystem::path>>
		AnimationDataLoadFromJsonTablePathSet{};

	std::shared_ptr<std::string> RootMotionScaleName = NormallyRootMotionScaleName;
	std::shared_ptr<std::string> RootMotionRotationName = NormallyRootMotionRotationName;
	std::shared_ptr<std::string> RootMotionTransitionName = NormallyRootMotionTransitionName;

	Vector3 CalcRootMotionDeltaPos(std::optional<float> bTimeBeyondAnimation,
									const std::string & _TargetAnimName,
									const float AnimDuraion,
									const float AnimPrevFrameMotionTime,
									const float AnimMotionTime)&;

	Vector3 CalcRootMotionDeltaScale(std::optional<float> bTimeBeyondAnimation,
		const std::string & _TargetAnimName,
		const float AnimDuraion,
		const float AnimPrevFrameMotionTime,
		const float AnimMotionTime)&;

	// ���� ȸ���� �׽�Ʈ ���� �ʾ���.
	Quaternion CalcRootMotionDeltaQuat(std::optional<float> bTimeBeyondAnimation,
		const std::string & _TargetAnimName,
		const float AnimDuraion,
		const float AnimPrevFrameMotionTime,
		const float AnimMotionTime)&;

public:
	void    AnimationDataLoadFromJsonTable(const std::filesystem::path& FullPath)&;

	// fbx ���Ϸκ��� �ִϸ��̼Ǹ� �ε� . �ִϸ��̼� �̸��� fbx ���� �̸����� Ȯ���ڸ� ���� �� ��
	void    LoadAnimation(const std::filesystem::path & FilePath)&;
	// ���� �Լ��� ���� ���� . ����� �Ȱ��� . 
	void    LoadAnimationFromDirectory(const std::filesystem::path & Directory)&;

	// ��� ������ Ŭ�е鳢�� �����ϹǷ� �ϳ��� Ŭ���� ������ ������ ��� Ŭ���� �۵�.
	void    EnableScaleRootMotion(const std::string & ScalingRootName = "");
	void    EnableRotationRootMotion(const std::string & RotationRootName = "");
	void    EnableSetQuatOffset(const Vector3& Euler)&;
	void    EnableTransitionRootMotion(const std::string & TransitionRootName = "");
	void    DisableScaleRootMotion();
	void    DisableRotationRootMotion();
	void    DisableTransitionRootMotion();

	float DeltaTimeFactor = 1.f;
	float RootMotionDeltaFactor = 1.f;
	bool  bRootMotionScale = false;
	bool  bRootMotionRotation = false;
	bool  bRootMotionTransition = false;

	std::string PrevAnimName{};
	std::string AnimName{};

	float  CurrentAccelerationFactor = 1.0f;
	float  CurrentTransitionTimeFactor = 1.0f;
	float  CurrentAnimMotionTime{ 0.0f };
	float  CurAccMotionTime{ 0.0f };
	float  CurrentAnimPrevFrameMotionTime{ 0.0f };

	float  PrevAnimMotionTime     { 0.0f };
	float  PrevAnimPrevFrameMotionTime{ 0.0f };
	float  PrevAccelerationFactor{ 1.0f };

	float  TransitionRemainTime = -1.0;
	float  TransitionDuration = 0.0;
	bool   bLoop = false;
	bool   bAnimationEnd = true;
	bool   bAnimStop = false;
	bool   bAnimSaveButton = false;

	Vector3    RootMotionLastCalcDeltaScale = { 0,0,0 };
	Quaternion RootMotionLastCalcDeltaQuat = { 0,0,0,1 };
	Vector3    RootMotionLastCalcDeltaPos = { 0,0,0 };

	AnimNotify           CurAnimNotify{};
	AnimationInformation CurPlayAnimInfo{};
	AnimationInformation PrevPlayAnimInfo{};
	IDirect3DTexture9*   BoneAnimMatrixInfo{ nullptr };
	IDirect3DTexture9*   PrevBoneAnimMatrixInfo{ nullptr };
	int32				VTFPitch{ 0 };
	std::vector<Matrix> BoneSkinningMatries{};
	std::vector<Matrix> PrevBoneSkinningMatries{};
	bool bHasAnimation = false;

	std::string RootNodeName{};
	std::shared_ptr<std::map<uint32, std::string>>				AnimIndexNameMap{};
	std::shared_ptr<std::map<std::string,AnimationInformation>> AnimInfoTable{};
	std::shared_ptr<std::unordered_map<std::string,std::shared_ptr<Node>>> Nodes{};
	//��� �̸��� ToRoot ��Ʈ���� ���� ... 
	std::optional<std::unordered_map<std::string, Matrix>> ToRoots{};

	std::shared_ptr<Vector3> EulerOffset{};
	std::shared_ptr<Quaternion> tOffset{};
};
END
#endif // !_SKELETONMESH_H_
