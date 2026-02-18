#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>


namespace renderer {

	void createBuffer(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, vk::DeviceSize size,
		vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Buffer& outBuffer, vk::raii::DeviceMemory& outBufferMemory);

	void copyBuffer(const vk::raii::Device& device, const vk::CommandPool& cmdPool, const vk::raii::Queue graphicsQueue,
		vk::raii::Buffer& srcBuffer, vk::raii::Buffer& destBuffer, vk::DeviceSize size);

} // namespace renderer
