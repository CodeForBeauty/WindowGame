#pragma once

#include "lm2.hpp"

#include "MeshTypes.h"

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <cstddef>
#include <vector>

namespace renderer {

constexpr uint32_t MAX_UNIFORM_COUNT = 1000;

struct TextureData;

class Pipeline {
public:
	Pipeline(nullptr_t) : mStaticDrawDataSize{ 0 } {};
	Pipeline(vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice, vk::Format outputFormat,
		vk::Format depthFormat, unsigned int maxTextures);

	void CreatePipeline(vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice, vk::Format outputFormat,
		vk::Format depthFormat, unsigned int maxTextures);

	void DrawSolidMeshes(vk::raii::CommandBuffer& commandBuffer, vk::raii::ImageView& swapImageView, vk::raii::ImageView& depthImageView,
		int viewWidth, int viewHeight, vk::raii::Buffer& vertexBuffer, vk::raii::Buffer& indexBuffer, const std::vector<SolidMesh>& solidMeshes);

	void DrawSkinnedMeshes(vk::raii::CommandBuffer& commandBuffer, vk::raii::ImageView& swapImageView, vk::raii::ImageView& depthImageView,
		int viewWidth, int viewHeight, vk::raii::Buffer& vertexBuffer, vk::raii::Buffer& indexBuffer, vk::raii::Buffer& skinningBuffer,
		const std::vector<SkinnedMesh>& skinnedMeshes);

	void UpdateTextures(const vk::raii::Device& device, const std::vector<TextureData>& textures);
	void UpdateSingleTexture(const vk::raii::Device& device, const TextureData& texture);

private:
#ifndef NDEBUG
	bool mPipelineCreated = false;
#endif

	void CreateUniformBuffers(vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice, unsigned int maxTextures);

	uint32_t mStaticDrawDataSize = 0;

	vk::raii::Buffer              mVkStaticDrawDataBuffer          = nullptr;
	vk::raii::DeviceMemory        mVkStaticDrawDataBufferMemory    = nullptr;
	void*                         mVkStaticDrawDataBufferMapped    = nullptr;

	vk::raii::DescriptorPool      mVkDescriptorPool                = nullptr;
	vk::raii::DescriptorSet       mVkMainDescriptorSet             = nullptr;

	vk::raii::DescriptorSetLayout mVkDescriptorSetLayout           = nullptr;

	vk::raii::PipelineLayout      mVkSolidPipelineLayout           = nullptr;
	vk::raii::Pipeline            mVkSolidPipeline                 = nullptr;

	vk::raii::PipelineLayout      mVkSkinnedPipelineLayout         = nullptr;
	vk::raii::Pipeline            mVkSkinnedPipeline               = nullptr;

	std::vector<char> ReadFile(const char* filepath);

	void NewVkPipeline(const vk::raii::Device& device, const char* shaderFile,
		std::vector<std::pair<const char*, vk::ShaderStageFlagBits>> shaderStagesFuncs,
		std::vector<vk::VertexInputBindingDescription> bindingDesc, std::vector<vk::VertexInputAttributeDescription> attribDescs,
		vk::PushConstantRange pushConstantRange, vk::Format outputFormat, vk::Format depthFormat,
		vk::raii::DescriptorSetLayout& descriptorSet, vk::raii::PipelineLayout& outPipelineLayout, vk::raii::Pipeline& outPipeline);
};

struct StaticDrawDataUB {
	lm2::mat4 view;
	lm2::mat4 proj;
};

struct PerObjectData {
	lm2::mat4 model;
};

} // namespace renderer
