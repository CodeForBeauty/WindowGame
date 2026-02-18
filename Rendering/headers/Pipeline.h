#pragma once

#include "lm2.hpp"

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <cstddef>
#include <vector>

namespace renderer {

class Pipeline {
public:
	Pipeline(nullptr_t) {};
	Pipeline(vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice, vk::Format outputFormat);

	void CreatePipeline(vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice, vk::Format outputFormat);

	void ApplyBasePass(vk::raii::CommandBuffer& commandBuffer, vk::raii::ImageView& swapImageView,
		int viewWidth, int viewHeight, vk::raii::Buffer& vertexBuffer, vk::raii::Buffer& indexBuffer, int indexCount);

private:
#ifndef NDEBUG
	bool mPipelineCreated = false;
#endif

	void CreateUniformBuffers(vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice);

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
