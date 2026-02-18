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

void renderer::copyBuffer(const vk::raii::Device& device, const vk::CommandPool& cmdPool, const vk::raii::Queue graphicsQueue,
		vk::raii::Buffer& srcBuffer, vk::raii::Buffer& destBuffer, vk::DeviceSize size) {
	vk::CommandBufferAllocateInfo allocInfo{
		.commandPool = cmdPool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = 1
	};
	vk::raii::CommandBuffer commandCopyBuffer = std::move(device.allocateCommandBuffers(allocInfo).front());

	commandCopyBuffer.begin(vk::CommandBufferBeginInfo{ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

	commandCopyBuffer.copyBuffer(srcBuffer, destBuffer, vk::BufferCopy(0, 0, size));

	commandCopyBuffer.end();

	graphicsQueue.submit(vk::SubmitInfo{ .commandBufferCount = 1, .pCommandBuffers = &*commandCopyBuffer }, nullptr);
	graphicsQueue.waitIdle();
}
