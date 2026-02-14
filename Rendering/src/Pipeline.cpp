#include "Pipeline.h"
#include "Renderer.h"

#include "vertex.h"
#include "renderer_helpers.h"

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <fstream>

using namespace renderer;


constexpr const char* mainShader = "shaders/forward.spv";
constexpr const char* vertMainShaderFunc = "vertMain";
constexpr const char* fragMainShaderFunc = "fragMain";

Pipeline::Pipeline(vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice, vk::Format outputFormat) {
	CreatePipeline(device, physicalDevice, outputFormat);
}

void Pipeline::CreatePipeline(vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice, vk::Format outputFormat) {
	CreateUniformBuffers(device, physicalDevice);

	std::vector<char> shaderCode = ReadFile(mainShader);

	vk::ShaderModuleCreateInfo shaderModuleCI{
		.codeSize = shaderCode.size() * sizeof(char),
		.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data())
	};
	vk::raii::ShaderModule shaderModule{ device, shaderModuleCI };

	vk::PipelineShaderStageCreateInfo vertShaderStageInfo{
		.stage = vk::ShaderStageFlagBits::eVertex,
		.module = shaderModule,
		.pName = vertMainShaderFunc
	};
	vk::PipelineShaderStageCreateInfo fragShaderStageInfo{
		.stage = vk::ShaderStageFlagBits::eFragment,
		.module = shaderModule,
		.pName = fragMainShaderFunc
	};

	vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	std::vector dynamicStates = {
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor
	};

	vk::PipelineDynamicStateCreateInfo dynamicState{
		.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
		.pDynamicStates = dynamicStates.data()
	};

	vk::VertexInputBindingDescription bindingDescription{
		.binding = 0,
		.stride = sizeof(vertex),
		.inputRate = vk::VertexInputRate::eVertex
	};
	std::vector<vk::VertexInputAttributeDescription> attributeDescriptions{
		vk::VertexInputAttributeDescription{
			.location = 0,
			.binding = 0,
			.format = vk::Format::eR32G32B32Sfloat,
			.offset = offsetof(vertex, pos)
		},
		vk::VertexInputAttributeDescription{
			.location = 1,
			.binding = 0,
			.format = vk::Format::eR32G32Sfloat,
			.offset = offsetof(vertex, uv)
		},
	};

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
		.vertexBindingDescriptionCount = 1,
		.pVertexBindingDescriptions = &bindingDescription,
		.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
		.pVertexAttributeDescriptions = attributeDescriptions.data()
	};

	vk::PipelineInputAssemblyStateCreateInfo inputAssembly{ .topology = vk::PrimitiveTopology::eTriangleList };

	vk::PipelineViewportStateCreateInfo viewportState{ .viewportCount = 1, .scissorCount = 1 };


	vk::PipelineRasterizationStateCreateInfo rasterizer{
		.depthClampEnable = vk::False,
		.rasterizerDiscardEnable = vk::False,
		.polygonMode = vk::PolygonMode::eFill,
		.cullMode = vk::CullModeFlagBits::eBack,
		.frontFace = vk::FrontFace::eClockwise,
		.depthBiasEnable = vk::False,
		.depthBiasSlopeFactor = 1.0f,
		.lineWidth = 1.0f
	};

	vk::PipelineColorBlendAttachmentState colorBlendAttachment{
		.blendEnable = vk::False,
		.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
	};

	vk::PipelineMultisampleStateCreateInfo multisampling{
		.rasterizationSamples = vk::SampleCountFlagBits::e1,
		.sampleShadingEnable = vk::False
	};

	vk::PipelineColorBlendStateCreateInfo colorBlending{
		.logicOpEnable = vk::False,
		.logicOp = vk::LogicOp::eCopy,
		.attachmentCount = 1,
		.pAttachments = &colorBlendAttachment
	};

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo{
		.setLayoutCount = 1,
		.pSetLayouts = &*mDescriptorSetLayout,
		.pushConstantRangeCount = 0
	};

	mPipelineLayout = vk::raii::PipelineLayout{ device, pipelineLayoutInfo };

	vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo{
		.colorAttachmentCount = 1,
		.pColorAttachmentFormats = &outputFormat
	};
	vk::GraphicsPipelineCreateInfo pipelineInfo{
		.pNext = &pipelineRenderingCreateInfo,
		.stageCount = 2,
		.pStages = shaderStages,
		.pVertexInputState = &vertexInputInfo,
		.pInputAssemblyState = &inputAssembly,
		.pViewportState = &viewportState,
		.pRasterizationState = &rasterizer,
		.pMultisampleState = &multisampling,
		.pColorBlendState = &colorBlending,
		.pDynamicState = &dynamicState,
		.layout = mPipelineLayout,
		.renderPass = nullptr
	};

	mGraphicsPipeline = vk::raii::Pipeline(device, nullptr, pipelineInfo);

#ifndef NDEBUG
	mPipelineCreated = true;
#endif
}

void Pipeline::ApplyBasePass(vk::raii::CommandBuffer& commandBuffer, vk::raii::ImageView& swapImageView,
	int viewWidth, int viewHeight, vk::raii::Buffer& vertexBuffer, int vertexCount) {

	vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
	vk::RenderingAttachmentInfo attachmentInfo = {
		.imageView = swapImageView,
		.imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
		.loadOp = vk::AttachmentLoadOp::eClear,
		.storeOp = vk::AttachmentStoreOp::eStore,
		.clearValue = clearColor
	};

	vk::RenderingInfo renderingInfo = {
		.renderArea = {.offset = { 0, 0 }, .extent = { .width = static_cast<uint32_t>(viewWidth), .height = static_cast<uint32_t>(viewHeight) } },
		.layerCount = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments = &attachmentInfo
	};

	MainMeshUB ubo{
		lm2::position3d<float>({ 0.1f, 0, -5 }),
		lm2::identity4x4<float>(),
		//lm2::ortho<float>(1, 1, 0.5f, 15.0f),
		lm2::perspective<float>(45.0f, 0.5f, 10.0f, 1),
	};

	commandBuffer.beginRendering(renderingInfo);


	memcpy(mMainBufferMapped, &ubo, sizeof(ubo));

	commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mPipelineLayout, 0, *mMainDescriptorSet, nullptr);


	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mGraphicsPipeline);

	commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(viewWidth),
		static_cast<float>(viewHeight), 0.0f, 1.0f));
	commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), { .width = static_cast<uint32_t>(viewWidth), .height = static_cast<uint32_t>(viewHeight) }));

	commandBuffer.bindVertexBuffers(0, *vertexBuffer, { 0 });

	commandBuffer.draw(vertexCount, 1, 0, 0);

	commandBuffer.endRendering();
}

void Pipeline::CreateUniformBuffers(vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice) {
	vk::DescriptorSetLayoutBinding uboLayoutBinding{ 0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex, nullptr };
	vk::DescriptorSetLayoutCreateInfo layoutInfo{ .bindingCount = 1, .pBindings = &uboLayoutBinding };
	mDescriptorSetLayout = vk::raii::DescriptorSetLayout(device, layoutInfo);

	vk::DeviceSize bufferSize = sizeof(MainMeshUB);

	createBuffer(device, physicalDevice, bufferSize, vk::BufferUsageFlagBits::eUniformBuffer,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, mMainBuffer, mMainBufferMemory);
	
	mMainBufferMapped = mMainBufferMemory.mapMemory(0, bufferSize);


	vk::DescriptorPoolSize poolSize(vk::DescriptorType::eUniformBuffer, 1);
	vk::DescriptorPoolCreateInfo poolInfo{
		.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
		.maxSets = 1,
		.poolSizeCount = 1,
		.pPoolSizes = &poolSize
	};

	mDescriptorPool = vk::raii::DescriptorPool(device, poolInfo);

	vk::DescriptorSetLayout layouts{ *mDescriptorSetLayout };
	vk::DescriptorSetAllocateInfo allocInfo{
		.descriptorPool = mDescriptorPool,
		.descriptorSetCount = 1,
		.pSetLayouts = &layouts
	};
	auto descriptorSets = device.allocateDescriptorSets(allocInfo);
	mMainDescriptorSet = std::move(descriptorSets[0]);

	vk::DescriptorBufferInfo bufferInfo{
		.buffer = mMainBuffer,
		.offset = 0,
		.range = sizeof(MainMeshUB)
	};
	vk::WriteDescriptorSet descriptorWrite{
		.dstSet = mMainDescriptorSet,
		.dstBinding = 0,
		.dstArrayElement = 0,
		.descriptorCount = 1,
		.descriptorType = vk::DescriptorType::eUniformBuffer,
		.pBufferInfo = &bufferInfo
	};
	device.updateDescriptorSets(descriptorWrite, {});
}

std::vector<char> Pipeline::ReadFile(const char* filepath) {
	std::ifstream file(filepath, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	std::vector<char> output(file.tellg());

	file.seekg(0, std::ios::beg);
	file.read(output.data(), static_cast<std::streamsize>(output.size()));

	file.close();
	return output;
}
