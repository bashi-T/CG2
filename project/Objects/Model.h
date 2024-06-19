#pragma once
#include"Commons/ModelCommon.h"
#include"Managers/SRVManager.h"
#include<map>
#include<span>

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

class Model
{
public:
	struct VertexData
	{
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
		Vector3 worldPosition;
	};
	struct Material {
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
		float shininess;
	};
	struct MaterialData
	{
		std::string textureFilePath;
		uint32_t textureIndex = 0;
	};
	struct Node
	{
		Matrix4x4 localMatrix;
		std::string name;
		std::vector<Node> children;
		QuaternionTransform qTransform;
	};
	struct VertexWeightData
	{
		float weight;
		uint32_t vertexIndex;
	};
	struct JointWeightData
	{
		Matrix4x4 inverseBindPoseMatrix;
		std::vector<VertexWeightData>vertexWeights;
	};
	struct ModelData
	{
		std::map<std::string, JointWeightData>skinClusterData;
		std::vector<VertexData> vertices;
		std::vector<uint32_t> indices;
		MaterialData material;
		Node rootNode;
	};
	struct Joint
	{
		QuaternionTransform transform;
		Matrix4x4 localMatrix;
		Matrix4x4 skeltonSpaceMatrix;
		Matrix4x4 worldMatrix;
		std::string name;
		std::vector<int32_t>children;
		int32_t index;
		std::optional<int32_t>parent;
	};
	struct Skelton
	{
		int32_t root;
		std::map<std::string, int32_t>jointMap;
		std::vector<Joint>joints;
	};
	template<typename tValue>
	struct Keyframe
	{
		float time;//キーフレームの値
		tValue value;//キーフレームの時刻
	};
	using KeyFrameVector3 = Keyframe<Vector3>;
	using KeyFrameQuaternion = Keyframe<Quaternion>;
	//struct NodeAnimation
	//{
	//	std::vector<Vector3> translate;
	//	std::vector<Quaternion> rotate;
	//	std::vector<Vector3>scale;
	//};
	template<typename tValue>
	struct AnimationCurve
	{
		std::vector<Keyframe<tValue>> keyframes;
	};
	struct NodeAnimation
	{
		AnimationCurve<Vector3> translate;
		AnimationCurve<Quaternion> rotate;
		AnimationCurve<Vector3>scale;
	};
	struct Animation
	{
		float duration;//アニメーション全体の尺
		std::map<std::string, NodeAnimation>nodeAnimations;
	};
	const uint32_t kNumMaxInfluence = 4;
	struct VertexInfluence
	{
		std::array<float, 4>weights;
		std::array<int32_t, 4>jointIndices;

	};
	struct WellForGPU
	{
		Matrix4x4 skeltonSpaceMatrix;//位置
		Matrix4x4 skeltonSpaceInverseTransposeMatrix;//法線
	};
	struct SkinCluster
	{
		std::vector<Matrix4x4>inverseBindPoseMatrices;

		Microsoft::WRL::ComPtr<ID3D12Resource>influenceResource;
		D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
		std::span<VertexInfluence>mappedInfluence;

		Microsoft::WRL::ComPtr<ID3D12Resource>paletteResource;
		std::span<WellForGPU>mappedPalette;
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE>paletteSrvHandle;
	};

	void ModelInitialize(ModelCommon* modelCommon, std::string objFilePath, std::string TextureFilePath);
	void AnimationInitialize(ModelCommon* modelCommon, std::string objFilePath, std::string TextureFilePath);
	void SkeltonInitialize(ModelCommon* modelCommon, std::string objFilePath, std::string TextureFilePath, SRVManager* srvManager);
	void Draw(ModelCommon* modelCommon, SRVManager* srvManager);
	void SkeltonDraw(ModelCommon* modelCommon, SRVManager* srvManager);
	void Memcpy();
	ComPtr<ID3D12Resource> CreateBufferResource(ModelCommon* modelCommon, size_t sizeInBytes);
	ModelData LoadModelFile(const std::string& directryPath, const std::string& filename);
	//MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	void MakeBufferView();
	Node ReadNode(aiNode* node);
	Animation LoadAnimationFile(const std::string& directoryPath, const std::string& filename);
	Skelton CreateSkelton(const Node& rootNode);
	int32_t CreateJoint(
		const Node& node,
		const std::optional<int32_t>parent,
		std::vector<Joint>&joints);
	SkinCluster CreateSkinCluster(
		const Skelton& skelton,
		const ModelData& modelData,
		const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>descriptorHeap,
		uint32_t descriptorSize);

	ModelData* GetModelData() { return &modelData; }
	Animation& GetAnimation() { return animation; }
	Skelton& GetSkelton() { return skelton; }
	const SkinCluster& GetSkinCluster() { return skinCluster; }
private:
	ModelData modelData;
	ModelCommon* modelCommon_;
	SRVManager* srvManager_ = nullptr;
	HRESULT hr = NULL;

	VertexData* vertexData = nullptr;
	ComPtr<ID3D12Resource> vertexResource = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	uint32_t* indexData = nullptr;
	ComPtr<ID3D12Resource> indexResource = nullptr;
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	Material* materialData = nullptr;
	ComPtr<ID3D12Resource> materialResource = nullptr;

	Animation animation;

	Skelton skelton;

	SkinCluster skinCluster;
	
	EulerTransform uvTransform
	{
			{1.0f,1.0f,1.0f},
			{0.0f,0.0f,0.0f},
			{0.0f,0.0f,0.0f},
	};
};

