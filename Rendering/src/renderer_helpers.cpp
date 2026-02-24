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

	uint32_t memIdx = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

	vk::MemoryAllocateInfo memoryAllocateInfo{
		.allocationSize = memRequirements.size,
		.memoryTypeIndex = memIdx
	};

	outBufferMemory = vk::raii::DeviceMemory(device, memoryAllocateInfo);

	outBuffer.bindMemory(*outBufferMemory, 0);
}

void renderer::copyBuffer(const vk::raii::Device& device, const vk::CommandPool& cmdPool, const vk::raii::Queue graphicsQueue,
		vk::raii::Buffer& srcBuffer, vk::raii::Buffer& destBuffer, vk::DeviceSize size) {
	vk::raii::CommandBuffer commandCopyBuffer = startSingleTimeCommands(device, cmdPool);

	commandCopyBuffer.copyBuffer(srcBuffer, destBuffer, vk::BufferCopy(0, 0, size));

	endAndWaitSingleTimeCommands(commandCopyBuffer, graphicsQueue);
}

vk::Format renderer::findSupportedFormat(const vk::raii::PhysicalDevice& physicalDevice, const std::vector<vk::Format>& candidates,
		vk::ImageTiling tiling, vk::FormatFeatureFlags features) {
	for (const auto format : candidates) {
		vk::FormatProperties props = physicalDevice.getFormatProperties(format);

		if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}

void renderer::createImage2D(const vk::raii::Device& device, const vk::raii::PhysicalDevice& physicalDevice, uint32_t width, uint32_t height,
		vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties,
		vk::raii::Image& outImage, vk::raii::DeviceMemory& outImageMemory) {
	vk::ImageCreateInfo imageInfo{
		.imageType = vk::ImageType::e2D,
		.format = format,
		.extent = {width, height, 1},
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = vk::SampleCountFlagBits::e1,
		.tiling = tiling,
		.usage = usage,
		.sharingMode = vk::SharingMode::eExclusive
	};

	outImage = vk::raii::Image(device, imageInfo);

	vk::MemoryRequirements memRequirements = outImage.getMemoryRequirements();
	vk::MemoryAllocateInfo allocInfo{
		.allocationSize = memRequirements.size,
		.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties)
	};
	outImageMemory = vk::raii::DeviceMemory(device, allocInfo);
	outImage.bindMemory(outImageMemory, 0);
}

void renderer::copyBufferToImage2D(const vk::raii::Device& device, const vk::CommandPool& cmdPool, const vk::raii::Queue graphicsQueue,
		vk::raii::Buffer& srcBuffer, vk::raii::Image& destImage, uint32_t width, uint32_t height) {
	vk::raii::CommandBuffer commandCopyBuffer = startSingleTimeCommands(device, cmdPool);

	vk::BufferImageCopy region{
		.bufferOffset = 0,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource = { vk::ImageAspectFlagBits::eColor, 0, 0, 1 },
		.imageOffset = {0, 0, 0},
		.imageExtent = {width, height, 1}
	};

	commandCopyBuffer.copyBufferToImage(srcBuffer, destImage, vk::ImageLayout::eTransferDstOptimal, region);

	endAndWaitSingleTimeCommands(commandCopyBuffer, graphicsQueue);
}

void renderer::transitionImageLayout(const vk::raii::Device& device, const vk::CommandPool& cmdPool, const vk::raii::Queue graphicsQueue,
		vk::raii::Image& image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) {
	vk::raii::CommandBuffer cmdBuffer = startSingleTimeCommands(device, cmdPool);

	vk::ImageMemoryBarrier barrier{
		.oldLayout = oldLayout,
		.newLayout = newLayout,
		.image = image,
		.subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }
	};

	vk::PipelineStageFlags sourceStage;
	vk::PipelineStageFlags destinationStage;

	if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
		barrier.srcAccessMask = {};
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eTransfer;
	}
	else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		sourceStage = vk::PipelineStageFlagBits::eTransfer;
		destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	cmdBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {}, nullptr, barrier);

	endAndWaitSingleTimeCommands(cmdBuffer, graphicsQueue);
}

vk::raii::ImageView renderer::createImageView2D(const vk::raii::Device& device, const vk::raii::Image& image, vk::Format format, vk::ImageAspectFlags aspectFlags) {
	vk::ImageViewCreateInfo viewInfo{
		.image = image,
		.viewType = vk::ImageViewType::e2D,
		.format = format,
		.subresourceRange = { aspectFlags, 0, 1, 0, 1 }
	};

	return vk::raii::ImageView(device, viewInfo);
}

uint32_t renderer::findMemoryType(const vk::raii::PhysicalDevice& physicalDevice, uint32_t memoryTypeBits, vk::MemoryPropertyFlags properties) {
	vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();

	uint32_t memIdx{0};

	for (memIdx = 0; memIdx < memProperties.memoryTypeCount; ++memIdx) {
		if ((memoryTypeBits & (1 << memIdx)) && (memProperties.memoryTypes[memIdx].propertyFlags & properties) == properties) {
			break;
		}
	}

	return memIdx;
}

vk::raii::CommandBuffer renderer::startSingleTimeCommands(const vk::raii::Device& device, const vk::CommandPool& cmdPool) {
	vk::CommandBufferAllocateInfo allocInfo{
		.commandPool = cmdPool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = 1
	};
	vk::raii::CommandBuffer buffer = std::move(device.allocateCommandBuffers(allocInfo).front());

	buffer.begin(vk::CommandBufferBeginInfo{ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

	return buffer;
}

void renderer::endAndWaitSingleTimeCommands(vk::raii::CommandBuffer& buffer, const vk::raii::Queue queue) {
	buffer.end();

	queue.submit(vk::SubmitInfo{ .commandBufferCount = 1, .pCommandBuffers = &*buffer }, nullptr);
	queue.waitIdle();
}
