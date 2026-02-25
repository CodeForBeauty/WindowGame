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

	void ApplyBasePass(vk::raii::CommandBuffer& commandBuffer, vk::raii::ImageView& swapImageView, vk::raii::ImageView& depthImageView,
		int viewWidth, int viewHeight, vk::raii::Buffer& vertexBuffer, vk::raii::Buffer& indexBuffer, const std::vector<SolidMesh>& solidMeshes);

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
	vk::raii::PipelineLayout      mVkPipelineLayout                = nullptr;
	vk::raii::Pipeline            mVkGraphicsPipeline              = nullptr;

	std::vector<char> ReadFile(const char* filepath);
};

struct StaticDrawDataUB {
	lm2::mat4 view;
	lm2::mat4 proj;
};

struct PerObjectData {
	lm2::mat4 model;
};

} // namespace renderer
