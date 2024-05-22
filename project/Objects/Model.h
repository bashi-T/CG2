#pragma once
#include"Commons/ModelCommon.h"
#include"Managers/SRVManager.h"
#include<map>

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
	struct ModelData
	{
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

	void ModelInitialize(ModelCommon* modelCommon, std::string objFilePath, std::string TextureFilePath);
	void AnimationInitialize(ModelCommon* modelCommon, std::string objFilePath, std::string TextureFilePath);
	void SkeltonInitialize(ModelCommon* modelCommon, std::string objFilePath, std::string TextureFilePath);
	void Draw(ModelCommon* modelCommon, SRVManager* srvManager);
	void Memcpy();
	ComPtr<ID3D12Resource> CreateBufferResource(ModelCommon* modelCommon, size_t sizeInBytes);
	ModelData LoadModelFile(const std::string& directryPath, const std::string& filename);
	//MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	void MakeBufferView();
	Node ReadNode(aiNode* node);
	Animation LoadAnimationFile(const std::string& directoryPath, const std::string& filename);
	Skelton CreateSkelton(const Node& rootNode);
	int32_t CreateJoint(const Node& node,
		const std::optional<int32_t>parent,
		std::vector<Joint>&joints);

	ModelData* GetModelData() { return &modelData; }
	Animation* GetAnimation() { return &animation; }
	Skelton GetSkelton() { return skelton; }
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

	EulerTransform uvTransform
	{
			{1.0f,1.0f,1.0f},
			{0.0f,0.0f,0.0f},
			{0.0f,0.0f,0.0f},
	};
};

