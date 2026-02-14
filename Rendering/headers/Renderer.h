#pragma once

#include "Window.h"
#include "Pipeline.h"

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <vector>

namespace renderer {

class Renderer {
public:
	Renderer(Window& window, const char* name);

	void Render(int width, int height);

	void Cleanup();

private:
	Window* mWindow;

	vk::raii::Instance               mVkInstance               = nullptr;
	vk::raii::SurfaceKHR             mVkSurface                = nullptr;

	vk::raii::PhysicalDevice         mVkPhysicalDevice         = nullptr;
	vk::raii::Device                 mVkDevice                 = nullptr;
	vk::raii::Queue                  mVkGraphicsQueue          = nullptr;
	vk::raii::Queue                  mVkPresentQueue           = nullptr;

	vk::raii::SwapchainKHR           mVkSwapchain              = nullptr;
	std::vector<vk::Image>           mSwapImages;
	std::vector<vk::raii::ImageView> mImageViews;

	vk::raii::CommandPool            mCommandPool              = nullptr;
	vk::raii::CommandBuffer          mCommandBuffer            = nullptr;


	vk::raii::Semaphore              mPresentCompleteSemaphore = nullptr;
	vk::raii::Semaphore              mRenderFinishedSemaphore  = nullptr;
	vk::raii::Fence                  mDrawFence                = nullptr;

	vk::raii::Buffer                 mVertexBuffer             = nullptr;
	vk::raii::DeviceMemory           mVertexBufferMemory       = nullptr;
	
	uint32_t mGraphicsIndex = 0;
	uint32_t mPresentIndex = 0;

	Pipeline mPipeline = nullptr;

	int mCurrentSwapImage = 0;
	int mTotalVertexCount = 0;

	void CreateInstance(const char* name);
	void CreateDevice();
	void CreateSwapchain();
	void CreateCommandPool();
	void LoadRenderData();
	void CreatePipeline();
};

} // namespace renderer
