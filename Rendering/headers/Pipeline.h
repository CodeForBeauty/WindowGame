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
		int viewWidth, int viewHeight, vk::raii::Buffer& vertexBuffer, int vertexCount);

private:
#ifndef NDEBUG
	bool mPipelineCreated = false;
#endif

	void CreateUniformBuffers(vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice);

	vk::raii::Buffer mMainBuffer = nullptr;
	vk::raii::DeviceMemory mMainBufferMemory = nullptr;
	void* mMainBufferMapped = nullptr;

	vk::raii::DescriptorPool mDescriptorPool = nullptr;
	vk::raii::DescriptorSet mMainDescriptorSet = nullptr;

	vk::raii::DescriptorSetLayout mDescriptorSetLayout = nullptr;
	vk::raii::PipelineLayout mPipelineLayout = nullptr;
	vk::raii::Pipeline mGraphicsPipeline = nullptr;

	std::vector<char> ReadFile(const char* filepath);
};

struct MainMeshUB {
	lm2::mat4 model;
	lm2::mat4 view;
	lm2::mat4 proj;
};

} // namespace renderer
