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
	Pipeline(nullptr_t) : mMainUniformAlignment{ 0 } {};
	Pipeline(vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice, vk::Format outputFormat,
		vk::Format depthFormat, const std::vector<TextureData>& textures);

	void CreatePipeline(vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice, vk::Format outputFormat,
		vk::Format depthFormat, const std::vector<TextureData>& textures);

	void ApplyBasePass(vk::raii::CommandBuffer& commandBuffer, vk::raii::ImageView& swapImageView, vk::raii::ImageView& depthImageView,
		int viewWidth, int viewHeight, vk::raii::Buffer& vertexBuffer, vk::raii::Buffer& indexBuffer, const std::vector<SolidMesh>& solidMeshes);

	void UpdateTextures(const vk::raii::Device& device, const std::vector<TextureData>& textures);

private:
#ifndef NDEBUG
	bool mPipelineCreated = false;
#endif

	void CreateUniformBuffers(vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice, const std::vector<TextureData>& textures);

	uint32_t mMainUniformAlignment = 0;

	vk::raii::Buffer              mVkMainBuffer          = nullptr;
	vk::raii::DeviceMemory        mVkMainBufferMemory    = nullptr;
	void*                         mVkMainBufferMapped    = nullptr;

	vk::raii::DescriptorPool      mVkDescriptorPool      = nullptr;
	vk::raii::DescriptorSet       mVkMainDescriptorSet   = nullptr;

	vk::raii::DescriptorSetLayout mVkDescriptorSetLayout = nullptr;
	vk::raii::PipelineLayout      mVkPipelineLayout      = nullptr;
	vk::raii::Pipeline            mVkGraphicsPipeline    = nullptr;

	std::vector<char> ReadFile(const char* filepath);
};

struct MainMeshUB {
	lm2::mat4 model;
	lm2::mat4 view;
	lm2::mat4 proj;
};

} // namespace renderer
