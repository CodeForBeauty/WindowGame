#include "Renderer.h"

#include "vertex.h"
#include "renderer_helpers.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <stdexcept>
#include <ranges>

using namespace renderer;


constexpr vk::Format imageFormat{ vk::Format::eB8G8R8A8Srgb };

Renderer::Renderer(Window& window, const char* name) :  mWindow{ &window } {
	CreateInstance(name);
	CreateDevice();
	CreateSwapchain();
	CreateCommandPool();
	LoadRenderData();
	CreatePipeline();
}

void Renderer::Render(int width, int height) {
	auto fenceResult = mVkDevice.waitForFences(*mVkDrawFence, vk::True, UINT64_MAX);
	mVkDevice.resetFences(*mVkDrawFence);

	mVkCommandBuffer.begin({});

	vk::ImageMemoryBarrier2 barrier = {
		.srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		.srcAccessMask = {},
		.dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		.dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
		.oldLayout = vk::ImageLayout::eUndefined,
		.newLayout = vk::ImageLayout::eColorAttachmentOptimal,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = mVkSwapImages[mCurrentSwapImage],
		.subresourceRange = {
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};
	vk::DependencyInfo dependencyInfo = {
		.dependencyFlags = {},
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &barrier
	};
	mVkCommandBuffer.pipelineBarrier2(dependencyInfo);


	mPipeline.ApplyBasePass(mVkCommandBuffer, mVkImageViews[mCurrentSwapImage], width, height, mVkVertexBuffer, mTotalVertexCount);


	vk::ImageMemoryBarrier2 barrier1 = {
		.srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		.srcAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
		.dstStageMask = vk::PipelineStageFlagBits2::eBottomOfPipe,
		.dstAccessMask = {},
		.oldLayout = vk::ImageLayout::eColorAttachmentOptimal,
		.newLayout = vk::ImageLayout::ePresentSrcKHR,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = mVkSwapImages[mCurrentSwapImage],
		.subresourceRange = {
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};
	vk::DependencyInfo dependencyInfo1 = {
		.dependencyFlags = {},
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &barrier1
	};
	mVkCommandBuffer.pipelineBarrier2(dependencyInfo1);

	mVkCommandBuffer.end();

	auto [result, imageIndex] = mVkSwapchain.acquireNextImage(UINT64_MAX, mVkPresentCompleteSemaphore, nullptr);

	vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
	const vk::SubmitInfo submitInfo{
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &*mVkPresentCompleteSemaphore,
		.pWaitDstStageMask = &waitDestinationStageMask,
		.commandBufferCount = 1,
		.pCommandBuffers = &*mVkCommandBuffer,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &*mVkRenderFinishedSemaphore };

	mVkGraphicsQueue.submit(submitInfo, *mVkDrawFence);

	const vk::PresentInfoKHR presentInfoKHR{
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &*mVkRenderFinishedSemaphore,
		.swapchainCount = 1,
		.pSwapchains = &*mVkSwapchain,
		.pImageIndices = &imageIndex
	};

	result = mVkPresentQueue.presentKHR(presentInfoKHR);
}

void Renderer::UpdateData(std::vector<vertex>& vertices, std::vector<unsigned int>& indices) {
	mVkVertexBufferMemory = nullptr;
	mVkVertexBuffer = nullptr;

	mTotalVertexCount = vertices.size();

	vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	vk::raii::Buffer stagingBuffer = nullptr;
	vk::raii::DeviceMemory stagingMemory = nullptr;

	createBuffer(mVkDevice, mVkPhysicalDevice, bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingMemory);

	createBuffer(mVkDevice, mVkPhysicalDevice, bufferSize, vk::BufferUsageFlagBits::eVertexBuffer,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, mVkVertexBuffer, mVkVertexBufferMemory);
	
	{
		void* data = stagingMemory.mapMemory(0, bufferSize);
		memcpy(data, vertices.data(), bufferSize);
		stagingMemory.unmapMemory();
	}

	copyBuffer(mVkDevice, mVkCommandPool, mVkGraphicsQueue, stagingBuffer, mVkVertexBuffer, bufferSize);
}

void Renderer::Cleanup() {
	mVkDevice.waitIdle();
}

void Renderer::CreateInstance(const char* name) {
	vk::ApplicationInfo appInfo{
		.pApplicationName = name,
		.applicationVersion = VK_MAKE_VERSION(0, 0, 1),
		.apiVersion = vk::ApiVersion14 };

	SDL_Window* sdlWindow = mWindow->GetSDLWindow();

	std::vector<const char*> iExtensions{ vk::EXTDebugUtilsExtensionName };
	uint32_t extensionCount = 0;
	{
		const char* const* sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
		iExtensions.insert(iExtensions.end(), sdlExtensions, sdlExtensions + extensionCount);
	}

	vk::InstanceCreateInfo instanceCI{
		.pApplicationInfo = &appInfo,
		.enabledExtensionCount = extensionCount + 1,
		.ppEnabledExtensionNames = iExtensions.data(),
	};
	mVkInstance = vk::raii::Instance { mVkContext, instanceCI };

	VkSurfaceKHR tmpSurface;

	if (!SDL_Vulkan_CreateSurface(sdlWindow, *mVkInstance, nullptr, &tmpSurface)) {
		throw std::runtime_error("Failed to create vulkan surface");
	}

	mVkSurface = vk::raii::SurfaceKHR{ mVkInstance,  tmpSurface };

}

void Renderer::CreateDevice() {
	auto tmp = mVkInstance.enumeratePhysicalDevices();
	mVkPhysicalDevice = vk::raii::PhysicalDevice{ mVkInstance.enumeratePhysicalDevices()[0] };

	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = mVkPhysicalDevice.getQueueFamilyProperties();

	mVkGraphicsIndex = 0;

	for (vk::QueueFamilyProperties& qfp : queueFamilyProperties) {
		if ((qfp.queueFlags & vk::QueueFlagBits::eGraphics) != vk::QueueFlags{ 0 }) {
			break;
		}
		++mVkGraphicsIndex;
	}

	mVkPresentIndex = mVkPhysicalDevice.getSurfaceSupportKHR(mVkGraphicsIndex, *mVkSurface)
		? mVkGraphicsIndex
		: static_cast<uint32_t>(queueFamilyProperties.size());

	std::vector<const char*> deviceExtensions = { vk::KHRSwapchainExtensionName };

	vk::PhysicalDeviceFeatures2 features = mVkPhysicalDevice.getFeatures2();
	vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT extendedDynamicStateFeatures{
		.extendedDynamicState = vk::True
	};
	vk::PhysicalDeviceVulkan13Features vulkan13Features{
		.pNext = &extendedDynamicStateFeatures,
		.synchronization2 = vk::True,
		.dynamicRendering = vk::True,
	};
	features.pNext = &vulkan13Features;

	float queuePriority = 0.5f;
	vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
		.queueFamilyIndex = mVkGraphicsIndex,
		.queueCount = 1,
		.pQueuePriorities = &queuePriority
	};
	vk::DeviceCreateInfo deviceCreateInfo{
		.pNext = &features,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &deviceQueueCreateInfo,
		.enabledExtensionCount = (uint32_t)deviceExtensions.size(),
		.ppEnabledExtensionNames = deviceExtensions.data()
	};

	mVkDevice = vk::raii::Device(mVkPhysicalDevice, deviceCreateInfo);
	mVkGraphicsQueue = vk::raii::Queue(mVkDevice, mVkGraphicsIndex, 0);
	mVkPresentQueue = vk::raii::Queue(mVkDevice, mVkPresentIndex, 0);
}

void Renderer::CreateSwapchain() {
	vk::SurfaceCapabilitiesKHR surfaceCaps = mVkPhysicalDevice.getSurfaceCapabilitiesKHR(mVkSurface);

	vk::SwapchainCreateInfoKHR swapchainCI{
		.surface = mVkSurface,
		.minImageCount = surfaceCaps.minImageCount,
		.imageFormat = imageFormat,
		.imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear,
		.imageExtent{.width = surfaceCaps.currentExtent.width, .height = surfaceCaps.currentExtent.height },
		.imageArrayLayers = 1,
		.imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
		.preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity,
		.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
		.presentMode = vk::PresentModeKHR::eFifo
	};

	mVkSwapchain = mVkDevice.createSwapchainKHR(swapchainCI);

	mVkSwapImages = mVkSwapchain.getImages();

	vk::ImageViewCreateInfo imageViewCI{
		.viewType = vk::ImageViewType::e2D,
		.format = imageFormat,
		.subresourceRange { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 },
	};

	for (vk::Image& img : mVkSwapImages) {
		imageViewCI.image = img;
		mVkImageViews.emplace_back(mVkDevice, imageViewCI);
	}
}

void Renderer::CreateCommandPool() {
	vk::CommandPoolCreateInfo poolInfo{
		.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		.queueFamilyIndex = mVkGraphicsIndex
	};
	mVkCommandPool = vk::raii::CommandPool(mVkDevice, poolInfo);

	vk::CommandBufferAllocateInfo allocInfo{
		.commandPool = mVkCommandPool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = 1
	};
	mVkCommandBuffer = std::move(vk::raii::CommandBuffers(mVkDevice, allocInfo).front());
}

void Renderer::LoadRenderData() {
	std::vector<vertex> vertices{ {} };
	std::vector<unsigned int> indices;
	UpdateData(vertices, indices);
}

void Renderer::CreatePipeline() {
	mVkPresentCompleteSemaphore = vk::raii::Semaphore(mVkDevice, vk::SemaphoreCreateInfo());
	mVkRenderFinishedSemaphore = vk::raii::Semaphore(mVkDevice, vk::SemaphoreCreateInfo());
	mVkDrawFence = vk::raii::Fence(mVkDevice, { .flags = vk::FenceCreateFlagBits::eSignaled });

	mPipeline.CreatePipeline(mVkDevice, mVkPhysicalDevice, imageFormat);
}
