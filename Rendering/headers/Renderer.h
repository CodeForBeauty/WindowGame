#pragma once

#include "Window.h"
#include "Pipeline.h"

#include "vertex.h"

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <vector>

namespace renderer {

class Renderer {
public:
	Renderer(Window& window, const char* name);

	void Render(int width, int height);

	void UpdateData(std::vector<vertex>& vertices, std::vector<uint16_t>& indices);

	void Cleanup();

private:
	Window* mWindow;

	vk::raii::Context mVkContext;

	vk::raii::Instance               mVkInstance               = nullptr;
	vk::raii::SurfaceKHR             mVkSurface                = nullptr;

	vk::raii::PhysicalDevice         mVkPhysicalDevice         = nullptr;
	vk::raii::Device                 mVkDevice                 = nullptr;
	vk::raii::Queue                  mVkGraphicsQueue          = nullptr;
	vk::raii::Queue                  mVkPresentQueue           = nullptr;

	vk::raii::SwapchainKHR           mVkSwapchain              = nullptr;
	std::vector<vk::Image>           mVkSwapImages;
	std::vector<vk::raii::ImageView> mVkImageViews;

	vk::raii::CommandPool            mVkCommandPool              = nullptr;
	vk::raii::CommandBuffer          mVkCommandBuffer            = nullptr;


	vk::raii::Semaphore              mVkPresentCompleteSemaphore = nullptr;
	vk::raii::Semaphore              mVkRenderFinishedSemaphore  = nullptr;
	vk::raii::Fence                  mVkDrawFence                = nullptr;

	vk::raii::Buffer                 mVkVertexBuffer             = nullptr;
	vk::raii::DeviceMemory           mVkVertexBufferMemory       = nullptr;
	vk::raii::Buffer                 mVkIndexBuffer              = nullptr;
	vk::raii::DeviceMemory           mVkIndexBufferMemory        = nullptr;
	
	uint32_t mVkGraphicsIndex = 0;
	uint32_t mVkPresentIndex = 0;

	Pipeline mPipeline = nullptr;

	int mCurrentSwapImage = 0;
	int mTotalVertexCount = 0;
	int mTotalIndexCount = 0;

	void CreateInstance(const char* name);
	void CreateDevice();
	void CreateSwapchain();
	void CreateCommandPool();
	void LoadRenderData();
	void CreatePipeline();
};

} // namespace renderer
