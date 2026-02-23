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

	TransitionImageView(mVkCommandBuffer, mVkSwapImages[mCurrentSwapImage],
		vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal,
		{}, vk::AccessFlagBits2::eColorAttachmentWrite,
		vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		vk::ImageAspectFlagBits::eColor);
	TransitionImageView(mVkCommandBuffer, *mVkDepthImage,
		vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthAttachmentOptimal,
		vk::AccessFlagBits2::eDepthStencilAttachmentWrite, vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
		vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
		vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
		vk::ImageAspectFlagBits::eDepth);


	mPipeline.ApplyBasePass(mVkCommandBuffer, mVkImageViews[mCurrentSwapImage], mVkDepthImageView, width, height,
		mVkVertexBuffer, mVkIndexBuffer, mSolidMeshes);


	TransitionImageView(mVkCommandBuffer, mVkSwapImages[mCurrentSwapImage],
		vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR,
		vk::AccessFlagBits2::eColorAttachmentWrite, {},
		vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::PipelineStageFlagBits2::eBottomOfPipe,
		vk::ImageAspectFlagBits::eColor);

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

void Renderer::UpdateSolidMeshes(std::vector< std::pair< std::vector<vertex>, std::vector<uint16_t> > >& meshesData) {
	mVkVertexBufferMemory = nullptr;
	mVkVertexBuffer = nullptr;

	mVkIndexBufferMemory = nullptr;
	mVkIndexBuffer = nullptr;

	mTotalVertexCount = 0;
	mTotalIndexCount = 0;

	size_t vertexSize = sizeof(vertex);
	size_t indexSize = sizeof(uint16_t);

	mSolidMeshes.clear();

	for (size_t i = 0; i < meshesData.size(); ++i) {
		mSolidMeshes.emplace_back(
			mTotalVertexCount,
			static_cast<uint32_t>(meshesData[i].first.size()),
			mTotalIndexCount,
			static_cast<uint32_t>(meshesData[i].second.size())
		);

		mTotalVertexCount += static_cast<uint32_t>(meshesData[i].first.size());
		mTotalIndexCount += static_cast<uint32_t>(meshesData[i].second.size());
	}

	vk::DeviceSize vertBufSize = vertexSize * mTotalVertexCount;

	vk::raii::Buffer vertStagingBuffer = nullptr;
	vk::raii::DeviceMemory vertStagingMemory = nullptr;

	createBuffer(mVkDevice, mVkPhysicalDevice, vertBufSize, vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, vertStagingBuffer, vertStagingMemory);

	createBuffer(mVkDevice, mVkPhysicalDevice, vertBufSize, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
		vk::MemoryPropertyFlagBits::eDeviceLocal, mVkVertexBuffer, mVkVertexBufferMemory);
	
	{
		char* data = reinterpret_cast<char*>(vertStagingMemory.mapMemory(0, vertBufSize));
		for (size_t i = 0; i < meshesData.size(); ++i) {
			size_t size = meshesData[i].first.size() * vertexSize;
			memcpy(data, meshesData[i].first.data(), size);
			data = data + size;
		}
		vertStagingMemory.unmapMemory();
	}

	copyBuffer(mVkDevice, mVkCommandPool, mVkGraphicsQueue, vertStagingBuffer, mVkVertexBuffer, vertBufSize);

	// Index buffer
	vk::DeviceSize indexBufSize = indexSize * mTotalIndexCount;

	vk::raii::Buffer indStagingBuffer = nullptr;
	vk::raii::DeviceMemory indStagingMemory = nullptr;

	createBuffer(mVkDevice, mVkPhysicalDevice, indexBufSize, vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, indStagingBuffer, indStagingMemory);

	createBuffer(mVkDevice, mVkPhysicalDevice, indexBufSize, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
		vk::MemoryPropertyFlagBits::eDeviceLocal, mVkIndexBuffer, mVkIndexBufferMemory);

	{
		char* data = reinterpret_cast<char*>(indStagingMemory.mapMemory(0, indexBufSize));
		for (size_t i = 0; i < meshesData.size(); ++i) {
			size_t size = meshesData[i].second.size() * indexSize;
			memcpy(data, meshesData[i].second.data(), size);
			data = data + size;
		}
		indStagingMemory.unmapMemory();
	}

	copyBuffer(mVkDevice, mVkCommandPool, mVkGraphicsQueue, indStagingBuffer, mVkIndexBuffer, indexBufSize);
}

void Renderer::Cleanup() {
	mVkDevice.waitIdle();
}

size_t Renderer::GetSolidMeshCount() const {
	return mSolidMeshes.size();
}

MeshData* Renderer::GetSolidMesh(size_t index) {
	if (index >= mSolidMeshes.size()) {
		return nullptr;
	}

	return &mSolidMeshes[index].data;
}

MeshData* Renderer::CopySolidMesh(size_t index) {
	size_t size = mSolidMeshes.size();
	if (index >= size) {
		return nullptr;
	}

	mSolidMeshes.emplace_back(mSolidMeshes[index]);
	return &mSolidMeshes[size].data;
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

	// Depth
	mVkDepthFormat = findSupportedFormat(mVkPhysicalDevice,
		{ vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
		vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);

	createImage2D(mVkDevice, mVkPhysicalDevice, surfaceCaps.currentExtent.width, surfaceCaps.currentExtent.height, mVkDepthFormat, vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal, mVkDepthImage, mVkDepthImageMemory);
	mVkDepthImageView = createImageView2D(mVkDevice, mVkDepthImage, mVkDepthFormat, vk::ImageAspectFlagBits::eDepth);
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
	std::vector<uint16_t> indices{ 0 };
	std::vector< std::pair< std::vector<vertex>, std::vector<uint16_t> > > data{
		{ { {} }, {0} }
	};
	UpdateSolidMeshes(data);
}

void Renderer::CreatePipeline() {
	mVkPresentCompleteSemaphore = vk::raii::Semaphore(mVkDevice, vk::SemaphoreCreateInfo());
	mVkRenderFinishedSemaphore = vk::raii::Semaphore(mVkDevice, vk::SemaphoreCreateInfo());
	mVkDrawFence = vk::raii::Fence(mVkDevice, { .flags = vk::FenceCreateFlagBits::eSignaled });

	mPipeline.CreatePipeline(mVkDevice, mVkPhysicalDevice, imageFormat, mVkDepthFormat);
}

void Renderer::TransitionImageView(const vk::raii::CommandBuffer& buffer, const vk::Image& image, vk::ImageLayout oldLayout,
		vk::ImageLayout newLayout, vk::AccessFlags2 srcAccessFlags, vk::AccessFlags2 dstAccessFlags,
		vk::PipelineStageFlags2 srcStageMask, vk::PipelineStageFlags2 dstStageMask, vk::ImageAspectFlags aspectMask) {
	vk::ImageMemoryBarrier2 barrier = {
		.srcStageMask = srcStageMask,
		.srcAccessMask = srcAccessFlags,
		.dstStageMask = dstStageMask,
		.dstAccessMask = dstAccessFlags,
		.oldLayout = oldLayout,
		.newLayout = newLayout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = image,
		.subresourceRange = {
			.aspectMask = aspectMask,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};

	vk::DependencyInfo dependencyInfo = {
		.dependencyFlags = {},
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &barrier,
	};
	buffer.pipelineBarrier2(dependencyInfo);
}
