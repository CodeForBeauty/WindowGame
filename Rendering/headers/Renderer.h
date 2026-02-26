#pragma once

#include "Window.h"
#include "Pipeline.h"

#include "vertex.h"
#include "MeshTypes.h"
#include "textures.h"
#include "models.h"

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <vector>

namespace renderer {

struct TextureData;

constexpr unsigned int MAX_TEXTURES = 64;

class Renderer {
public:
	Renderer(Window& window, const char* name);

	void Render(int width, int height);

	// data - vector of vertices and indices paired per mesh
	void UpdateSolidMeshes(std::vector<assets::SolidMeshData>& data);

	void Cleanup();

	size_t GetSolidMeshCount() const;
	MeshData* GetSolidMesh(size_t index);
	MeshData* CopySolidMesh(size_t index);

	// Returns index of the texture in a vector
	size_t LoadTexture(const char* filepath);

	void UploadTexture(const assets::TextureInfo& texture);
	void UploadTextures(const std::vector<assets::TextureInfo>& textures);

private:
	std::vector<SolidMesh> mSolidMeshes;

	Window* mWindow;

	vk::raii::Context mVkContext;

	vk::raii::Instance               mVkInstance                 = nullptr;
	vk::raii::SurfaceKHR             mVkSurface                  = nullptr;

	vk::raii::PhysicalDevice         mVkPhysicalDevice           = nullptr;
	vk::raii::Device                 mVkDevice                   = nullptr;
	vk::raii::Queue                  mVkGraphicsQueue            = nullptr;
	vk::raii::Queue                  mVkPresentQueue             = nullptr;

	vk::raii::SwapchainKHR           mVkSwapchain                = nullptr;
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

	vk::Format                       mVkDepthFormat              = vk::Format::eUndefined;
	vk::raii::Image                  mVkDepthImage               = nullptr;
	vk::raii::DeviceMemory           mVkDepthImageMemory         = nullptr;
	vk::raii::ImageView              mVkDepthImageView           = nullptr;
	
	uint32_t mVkGraphicsIndex = 0;
	uint32_t mVkPresentIndex = 0;

	Pipeline mPipeline = nullptr;

	int mCurrentSwapImage = 0;
	uint32_t mTotalVertexCount = 0;
	uint32_t mTotalIndexCount = 0;

	std::vector<TextureData> mAllTextures;


	void CreateInstance(const char* name);
	void CreateDevice();
	void CreateSwapchain();
	void CreateCommandPool();
	void LoadRenderData();
	void CreatePipeline();

	void TransitionImageView(const vk::raii::CommandBuffer& buffer, const vk::Image& image, vk::ImageLayout oldLayout,
		vk::ImageLayout newLayout, vk::AccessFlags2 srcAccessFlags, vk::AccessFlags2 dstAccessFlags,
		vk::PipelineStageFlags2 srcStageMask, vk::PipelineStageFlags2 dstStageMask, vk::ImageAspectFlags aspectMask);
};

struct TextureData {
	uint32_t index = 0;

	uint32_t width = 0;
	uint32_t height = 0;
	vk::Format format = vk::Format::eUndefined;

	vk::raii::DeviceMemory memory = nullptr;
	vk::raii::Image image = nullptr;
	vk::raii::ImageView imageView = nullptr;
	vk::raii::Sampler sampler = nullptr;
};

} // namespace renderer
