#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <cstddef>
#include <vector>

namespace renderer {

class Pipeline {
public:
	Pipeline(nullptr_t) {};
	Pipeline(vk::raii::Device& device, vk::Format outputFormat);

	void CreatePipeline(vk::raii::Device& device, vk::Format outputFormat);

	void ApplyBasePass(vk::raii::CommandBuffer& commandBuffer, vk::raii::ImageView& swapImageView,
		int viewWidth, int viewHeight, vk::raii::Buffer& vertexBuffer, int vertexCount);

private:
#ifndef NDEBUG
	bool mPipelineCreated = false;
#endif

	vk::raii::Pipeline mGraphicsPipeline = nullptr;
	vk::raii::PipelineLayout mPipelineLayout = nullptr;

	std::vector<char> ReadFile(const char* filepath);
};

} // namespace renderer
