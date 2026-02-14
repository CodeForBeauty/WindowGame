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
	auto fenceResult = mVkDevice.waitForFences(*mDrawFence, vk::True, UINT64_MAX);
	mVkDevice.resetFences(*mDrawFence);

	mCommandBuffer.begin({});

	vk::ImageMemoryBarrier2 barrier = {
		.srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		.srcAccessMask = {},
		.dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		.dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
		.oldLayout = vk::ImageLayout::eUndefined,
		.newLayout = vk::ImageLayout::eColorAttachmentOptimal,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = mSwapImages[mCurrentSwapImage],
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
	mCommandBuffer.pipelineBarrier2(dependencyInfo);


	mPipeline.ApplyBasePass(mCommandBuffer, mImageViews[mCurrentSwapImage], width, height, mVertexBuffer, mTotalVertexCount);


	vk::ImageMemoryBarrier2 barrier1 = {
		.srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		.srcAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
		.dstStageMask = vk::PipelineStageFlagBits2::eBottomOfPipe,
		.dstAccessMask = {},
		.oldLayout = vk::ImageLayout::eColorAttachmentOptimal,
		.newLayout = vk::ImageLayout::ePresentSrcKHR,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = mSwapImages[mCurrentSwapImage],
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
	mCommandBuffer.pipelineBarrier2(dependencyInfo1);

	mCommandBuffer.end();


	vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
	const vk::SubmitInfo submitInfo{
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &*mPresentCompleteSemaphore,
		.pWaitDstStageMask = &waitDestinationStageMask,
		.commandBufferCount = 1,
		.pCommandBuffers = &*mCommandBuffer,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &*mRenderFinishedSemaphore };

	mVkGraphicsQueue.submit(submitInfo, *mDrawFence);

	auto [result, imageIndex] = mVkSwapchain.acquireNextImage(UINT64_MAX, mPresentCompleteSemaphore, nullptr);

	const vk::PresentInfoKHR presentInfoKHR{
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &*mRenderFinishedSemaphore,
		.swapchainCount = 1,
		.pSwapchains = &*mVkSwapchain,
		.pImageIndices = &imageIndex
	};

	result = mVkPresentQueue.presentKHR(presentInfoKHR);
}

void Renderer::Cleanup() {
	mVkDevice.waitIdle();
}

void Renderer::CreateInstance(const char* name) {
	vk::raii::Context context;

	vk::ApplicationInfo appInfo{
		.pApplicationName = name,
		.applicationVersion = VK_MAKE_VERSION(0, 0, 1),
		.apiVersion = vk::ApiVersion14 };

	SDL_Window* sdlWindow = mWindow->GetSDLWindow();

	std::vector<const char*> iExtensions;
	uint32_t extensionCount = 0;
	{
		const char* const* sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
		iExtensions.insert(iExtensions.end(), sdlExtensions, sdlExtensions + extensionCount);
	}

	vk::InstanceCreateInfo instanceCI{
		.pApplicationInfo = &appInfo,
		.enabledExtensionCount = extensionCount,
		.ppEnabledExtensionNames = iExtensions.data(),
	};
	mVkInstance = vk::raii::Instance { context, instanceCI };

	VkSurfaceKHR tmpSurface;

	if (!SDL_Vulkan_CreateSurface(sdlWindow, *mVkInstance, nullptr, &tmpSurface)) {
		throw std::runtime_error("Failed to create vulkan surface");
	}

	mVkSurface = vk::raii::SurfaceKHR{ mVkInstance,  tmpSurface };
}

void Renderer::CreateDevice() {
	mVkPhysicalDevice = vk::raii::PhysicalDevice{ mVkInstance.enumeratePhysicalDevices()[0] };

	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = mVkPhysicalDevice.getQueueFamilyProperties();

	mGraphicsIndex = 0;

	for (vk::QueueFamilyProperties& qfp : queueFamilyProperties) {
		if ((qfp.queueFlags & vk::QueueFlagBits::eGraphics) != vk::QueueFlags{ 0 }) {
			break;
		}
		++mGraphicsIndex;
	}

	mPresentIndex = mVkPhysicalDevice.getSurfaceSupportKHR(mGraphicsIndex, *mVkSurface)
		? mGraphicsIndex
		: static_cast<uint32_t>(queueFamilyProperties.size());

	std::vector<const char*> deviceExtensions = { vk::KHRSwapchainExtensionName };

	vk::PhysicalDeviceFeatures2 features = mVkPhysicalDevice.getFeatures2();
	vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT extendedDynamicStateFeatures{
		.extendedDynamicState = vk::True
	};
	vk::PhysicalDeviceVulkan13Features vulkan13Features{
		.pNext = &extendedDynamicStateFeatures,
		.dynamicRendering = vk::True,
	};
	features.pNext = &vulkan13Features;

	float queuePriority = 0.5f;
	vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
		.queueFamilyIndex = mGraphicsIndex,
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
	mVkGraphicsQueue = vk::raii::Queue(mVkDevice, mGraphicsIndex, 0);
	mVkPresentQueue = vk::raii::Queue(mVkDevice, mPresentIndex, 0);
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

	mSwapImages = mVkSwapchain.getImages();

	vk::ImageViewCreateInfo imageViewCI{
		.viewType = vk::ImageViewType::e2D,
		.format = imageFormat,
		.subresourceRange { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 },
	};

	for (vk::Image& img : mSwapImages) {
		imageViewCI.image = img;
		mImageViews.emplace_back(mVkDevice, imageViewCI);
	}
}

void Renderer::CreateCommandPool() {
	vk::CommandPoolCreateInfo poolInfo{
		.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		.queueFamilyIndex = mGraphicsIndex
	};
	mCommandPool = vk::raii::CommandPool(mVkDevice, poolInfo);

	vk::CommandBufferAllocateInfo allocInfo{
		.commandPool = mCommandPool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = 1
	};
	mCommandBuffer = std::move(vk::raii::CommandBuffers(mVkDevice, allocInfo).front());
}

void Renderer::LoadRenderData() {
	const std::vector<vertex> vertices = {
		{{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f}},
		{{ 0.5f,  0.5f, 0.0f}, {1.0f, 0.0f}},
		{{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f}},

		{{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f}},
		{{ 0.5f, -0.5f, 0.0f}, {1.0f, 1.0f}},
		{{ 0.5f,  0.5f, 0.0f}, {1.0f, 0.0f}},
	};

	mTotalVertexCount = vertices.size();

	vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	createBuffer(mVkDevice, mVkPhysicalDevice, bufferSize, vk::BufferUsageFlagBits::eVertexBuffer,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, mVertexBuffer, mVertexBufferMemory);

	{
		void* data = mVertexBufferMemory.mapMemory(0, bufferSize);
		memcpy(data, vertices.data(), bufferSize);
		mVertexBufferMemory.unmapMemory();
	}
}

void Renderer::CreatePipeline() {
	mPresentCompleteSemaphore = vk::raii::Semaphore(mVkDevice, vk::SemaphoreCreateInfo());
	mRenderFinishedSemaphore = vk::raii::Semaphore(mVkDevice, vk::SemaphoreCreateInfo());
	mDrawFence = vk::raii::Fence(mVkDevice, { .flags = vk::FenceCreateFlagBits::eSignaled });

	mPipeline.CreatePipeline(mVkDevice, mVkPhysicalDevice, imageFormat);
}
