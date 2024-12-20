#pragma once

#include "framework/vulkanapp.h"
#include "framework/camera.h"

struct RTAccelerationStructure {
	vulkanhelpers::Buffer buffer;
	VkAccelerationStructureKHR accelerationStructure;
	VkDeviceAddress handle;
};

struct RTMesh {
	uint32_t numVertices;
	uint32_t numFaces;

	vulkanhelpers::Buffer positions;
	vulkanhelpers::Buffer attribs;
	vulkanhelpers::Buffer indices;
	vulkanhelpers::Buffer faces;
	vulkanhelpers::Buffer matIDs;

	RTAccelerationStructure blas;
};

struct RTMaterial {
	vulkanhelpers::Image texture;
};

struct RTScene {
	std::vector<RTMesh> meshes;
	std::vector<RTMaterial> materials;
	RTAccelerationStructure topLevelAS;

	// shader resources stuff
	std::vector<VkDescriptorBufferInfo> matIDsBufferInfos;
	std::vector<VkDescriptorBufferInfo> attribsBufferInfos;
	std::vector<VkDescriptorBufferInfo> facesBufferInfos;
	std::vector<VkDescriptorImageInfo> texturesInfos;

	void BuildBLAS(VkDevice device, VkCommandPool cmdPool, VkQueue queue);
	void BuildTLAS(VkDevice device, VkCommandPool cmdPool, VkQueue queue);
};


class SBTHelper {
public:
	SBTHelper();
	~SBTHelper() = default;

	void Initialize(const uint32_t numHitGroups, const uint32_t numMissGroups, const uint32_t shaderHandleSize, const uint32_t shaderGroupAlignment);
	void Destroy();
	void SetRaygenStage(const VkPipelineShaderStageCreateInfo& stage);
	void AddStageToHitGroup(const std::vector<VkPipelineShaderStageCreateInfo>& stages, const uint32_t groupIndex);
	void AddStageToMissGroup(const VkPipelineShaderStageCreateInfo& stage, const uint32_t groupIndex);

	uint32_t GetGroupsStride() const;
	uint32_t GetNumGroups() const;
	uint32_t GetRaygenOffset() const;
	uint32_t GetRaygenSize() const;
	uint32_t GetHitGroupsOffset() const;
	uint32_t GetHitGroupsSize() const;
	uint32_t GetMissGroupsOffset() const;
	uint32_t GetMissGroupsSize() const;

	uint32_t                                    GetNumStages() const;
	const VkPipelineShaderStageCreateInfo* GetStages() const;
	const VkRayTracingShaderGroupCreateInfoKHR* GetGroups() const;

	uint32_t GetSBTSize() const;
	bool CreateSBT(VkDevice device, VkPipeline rtPipeline);
	VkDeviceAddress GetSBTAddress() const;

private:
	uint32_t mShaderHandleSize;
	uint32_t mShaderGroupAlignment;
	uint32_t mNumHitGroups;
	uint32_t mNumMissGroups;
	std::vector<uint32_t> mNumHitShaders;
	std::vector<uint32_t> mNumMissShaders;
	std::vector<VkPipelineShaderStageCreateInfo> mStages;
	std::vector<VkRayTracingShaderGroupCreateInfoKHR> mGroups;
	vulkanhelpers::Buffer mSBTBuffer;
};


class RtxApp : public VulkanApp {
public:
	RtxApp();
	~RtxApp();

protected:
	virtual void InitSettings() override;
	virtual void InitApp() override;
	virtual void FreeResources() override;
	virtual void FillCommandBuffer(VkCommandBuffer commandBuffer, const size_t imageIndex) override;

	virtual void OnMouseMove(const float x, const float y) override;
	virtual void OnMouseButton(const int button, const int action, const int mods) override;
	virtual void OnKey(const int key, const int scancode, const int action, const int mods) override;
	virtual void Update(const size_t imageIndex, const float dt) override;

private:
	void LoadSceneGeometry(String);
	void CreateScene();
	void CreateCamera();
	void UpdateCameraParams(struct UniformParams* params, const float dt);
	void CreateDescriptorSetsLayouts();
	void CreateRaytracingPipelineAndSBT();
	void UpdateDescriptorSets();

private:
	std::vector<VkDescriptorSetLayout> mRTDescriptorSetsLayouts;
	VkPipelineLayout mRTPipelineLayout;
	VkPipeline mRTPipeline;
	VkDescriptorPool mRTDescriptorPool;
	std::vector<VkDescriptorSet> mRTDescriptorSets;

	SBTHelper mSBT;

	RTScene mScene;
	vulkanhelpers::Image mEnvTexture;
	VkDescriptorImageInfo mEnvTextureDescInfo;

	// camera a& user input
	Camera mCamera;
	vulkanhelpers::Buffer mCameraBuffer;
	bool mWKeyDown;
	bool mAKeyDown;
	bool mSKeyDown;
	bool mDKeyDown;
	bool mShiftDown;
	bool mUKeyDown;
	bool mLMBDown;
	vec2 mCursorPos;
};
