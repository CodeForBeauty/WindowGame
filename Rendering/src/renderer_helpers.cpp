#include "renderer_helpers.h"

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>


using namespace renderer;

void renderer::createBuffer(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, vk::DeviceSize size,
	vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Buffer& outBuffer, vk::raii::DeviceMemory& outBufferMemory) {

	vk::BufferCreateInfo bufferInfo{
		.size = size,
		.usage = usage,
		.sharingMode = vk::SharingMode::eExclusive
	};

	outBuffer = vk::raii::Buffer(device, bufferInfo);

	vk::MemoryRequirements memRequirements = outBuffer.getMemoryRequirements();

	vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();

	uint32_t typeFilter = memRequirements.memoryTypeBits;

	uint32_t memIdx;

	for (memIdx = 0; memIdx < memProperties.memoryTypeCount; ++memIdx) {
		if ((typeFilter & (1 << memIdx)) && (memProperties.memoryTypes[memIdx].propertyFlags & properties) == properties) {
			break;
		}
	}

	vk::MemoryAllocateInfo memoryAllocateInfo{
		.allocationSize = memRequirements.size,
		.memoryTypeIndex = memIdx
	};

	outBufferMemory = vk::raii::DeviceMemory(device, memoryAllocateInfo);

	outBuffer.bindMemory(*outBufferMemory, 0);
}
