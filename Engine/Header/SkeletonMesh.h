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
	// Mesh을(를) 통해 상속됨
	virtual void Free() override;
public:
	static SkeletonMesh* Create(LPDIRECT3DDEVICE9 const _pDevice,
								const std::filesystem::path _Path,
								const std::any & InitParams);
	// Mesh을(를) 통해 상속됨
	virtual Resource* Clone() override;
	virtual void      Editor()override;
	virtual std::string GetName() override;
	void BindVTF(ID3DXEffect * Fx)&;
public:
	void UpdateToRootMatricies();
	bool    IsAnimationEnd();
	void    EnableToRootMatricies();
	void    DisableToRootMatricies();
	// Prev VTF 켜기 . (모션블러등 이전 스키닝 정보가 필요할 경우
	void    EnablePrevVTF()&;
	// Prev VTF 끄기 . (런타임에 끌수 있으며 자주 켜고 끄는 것은 좋지않음)
	// _SkeletonMesh->DisablePrevVTF();
	void    DisablePrevVTF()&;
	std::tuple<Vector3, Quaternion, Vector3>   
		Update(const float DeltaTime)&;
	void TPose();
	void    BoneDebugRender(const Matrix & OwnerTransformWorld,ID3DXEffect* const Fx)&;
	void    VTFUpdate()&;
	Node* GetRootNode()&;
	Node* GetNode(const std::string & NodeName)&;
	//      본 스키닝 매트릭스에서 ToRoot 매트릭스를 계산 
	//      (현재 스키닝 업데이트를 하지 않는다면 반환값은 마지막 스키닝 했을시의 정보)
	//      해당 반환값이 존재 하지 않는다면 유효한 노드가 없는 것.
	//      해당 반환값은 해당 노드(뼈)의 애니메이션 된 행렬
	//      반환값 * World 하면 해당 오브젝트의 해당 뼈의 행렬을 구함.
	std::optional<Matrix> GetNodeToRoot
					(const std::string & NodeName)&;
	// 노드가 존재하지 않는다면 널 포인터 . ToRoot 매트릭스가 계속 업데이트 되길 기대한다면
	// 스켈레톤 메쉬 업데이트 반드시 호출 
	// EnableToRootMatricies 켜줬는지 확인 (최적화로 인해 기본 옵션이 아님 ) 
	Matrix* GetToRootMatrixPtr(const std::string & NodeName)&;

	void   PlayAnimation(
		const std::string & InitAnimName,
		const bool  bLoop,
		const AnimNotify & _Notify = {} ,
		const float _CurrentAccelerationFactor = 1.0f,
		const float _CurrentTransitionTimeFactor = 1.0f);
	void   PlayAnimation(
		const uint32 AnimationIndex,
		const bool  bLoop,
		const AnimNotify & _Notify = {} ,
		const float _CurrentAccelerationFactor = 1.0f,
		const float _CurrentTransitionTimeFactor = 1.0f);

	void    ContinueAnimation()&;
	void    StopAnimation();
	void	AnimationEnd()&;
	// 0 ~ 1 정규화 
	float   PlayingTime();
	float   PlayingAccTime();
	//  정규화된 시간으로 넘겨주세요 범위를 벗어나면 Clamp
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
	std::string RootMotionScaleName = NormallyRootMotionScaleName;
	std::string RootMotionRotationName = NormallyRootMotionRotationName;
	std::string RootMotionTransitionName = NormallyRootMotionTransitionName;

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

	// 아직 회전은 테스트 하지 않았음.
	Quaternion CalcRootMotionDeltaQuat(std::optional<float> bTimeBeyondAnimation,
		const std::string & _TargetAnimName,
		const float AnimDuraion,
		const float AnimPrevFrameMotionTime,
		const float AnimMotionTime)&;

public:
	void    AnimationDataLoadFromJsonTable(const std::filesystem::path& FullPath)&;

	// fbx 파일로부터 애니메이션만 로딩 . 애니메이션 이름은 fbx 파일 이름에서 확장자를 제거 한 것
	void    LoadAnimation(const std::filesystem::path & FilePath)&;
	// 위의 함수의 폴더 버전 . 사양은 똑같음 . 
	void    LoadAnimationFromDirectory(const std::filesystem::path & Directory)&;

	// 노드 정보는 클론들끼리 공유하므로 하나의 클론이 설정한 값으로 모든 클론이 작동.
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
	IDirect3DTexture9* BoneAnimMatrixInfo{ nullptr };
	IDirect3DTexture9* PrevBoneAnimMatrixInfo{ nullptr };
	int32 VTFPitch{ 0 };
	std::vector<Matrix> BoneSkinningMatries{};
	std::vector<Matrix> PrevBoneSkinningMatries{};
	bool bHasAnimation = false;

	std::string RootNodeName{};
	std::shared_ptr<std::map<uint32, std::string>>				AnimIndexNameMap{};
	std::shared_ptr<std::map<std::string,AnimationInformation>> AnimInfoTable{};
	std::shared_ptr<std::unordered_map<std::string,std::shared_ptr<Node>>> Nodes{};
	//              노드 이름과 ToRoot 매트릭스 매핑 ... 
	std::optional<std::unordered_map<std::string, Matrix>> ToRoots{};

	Vector3    EulerOffset{0,0,0};
	Quaternion tOffset{ 0,0,0,1 };
};
END
#endif // !_SKELETONMESH_H_
