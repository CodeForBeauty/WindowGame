#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>


namespace renderer {

	void createBuffer(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, vk::DeviceSize size,
		vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Buffer& outBuffer, vk::raii::DeviceMemory& outBufferMemory);

	void copyBuffer(const vk::raii::Device& device, const vk::CommandPool& cmdPool, const vk::raii::Queue graphicsQueue,
		vk::raii::Buffer& srcBuffer, vk::raii::Buffer& destBuffer, vk::DeviceSize size);

	vk::Format findSupportedFormat(const vk::raii::PhysicalDevice& physicalDevice, const std::vector<vk::Format>& candidates,
		vk::ImageTiling tiling, vk::FormatFeatureFlags features);

	void createImage2D(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, uint32_t width, uint32_t height,
		vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties,
		vk::raii::Image& outImage, vk::raii::DeviceMemory& outImageMemory);

	// The image layout is assumed to be Transfer optimal
	void copyBufferToImage2D(const vk::raii::Device& device, const vk::CommandPool& cmdPool, const vk::raii::Queue graphicsQueue,
		vk::raii::Buffer& srcBuffer, vk::raii::Image& destImage, uint32_t width, uint32_t height);

	void transitionImageLayout(const vk::raii::Device& device, const vk::CommandPool& cmdPool, const vk::raii::Queue graphicsQueue,
		vk::raii::Image& image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

	vk::raii::ImageView createImageView2D(const vk::raii::Device& device, const vk::raii::Image& image, vk::Format format, vk::ImageAspectFlags aspectFlags);


	uint32_t findMemoryType(const vk::raii::PhysicalDevice& physicalDevice, uint32_t memoryTypeBits, vk::MemoryPropertyFlags properties);

	vk::raii::CommandBuffer startSingleTimeCommands(const vk::raii::Device& device, const vk::CommandPool& cmdPool);
	void endAndWaitSingleTimeCommands(vk::raii::CommandBuffer& buffer, const vk::raii::Queue queue);

} // namespace renderer
