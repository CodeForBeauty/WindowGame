#include "Pipeline.h"
#include "Renderer.h"

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <fstream>

using namespace renderer;


constexpr const char* mainShader = "shaders/main.spv";
constexpr const char* vertShaderFunc = "vertMain";
constexpr const char* fragShaderFunc = "fragMain";

Pipeline::Pipeline(vk::raii::Device& device, vk::Format outputFormat) {
	CreatePipeline(device, outputFormat);
}

void Pipeline::CreatePipeline(vk::raii::Device& device, vk::Format outputFormat) {
	std::vector<char> shaderCode = ReadFile(mainShader);

	vk::ShaderModuleCreateInfo shaderModuleCI{
		.codeSize = shaderCode.size() * sizeof(char),
		.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data())
	};
	vk::raii::ShaderModule shaderModule{ device, shaderModuleCI };

	vk::PipelineShaderStageCreateInfo vertShaderStageInfo{
		.stage = vk::ShaderStageFlagBits::eVertex,
		.module = shaderModule,
		.pName = vertShaderFunc
	};
	vk::PipelineShaderStageCreateInfo fragShaderStageInfo{
		.stage = vk::ShaderStageFlagBits::eFragment,
		.module = shaderModule,
		.pName = fragShaderFunc
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

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo{ .setLayoutCount = 0, .pushConstantRangeCount = 0 };

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

	commandBuffer.beginRendering(renderingInfo);

	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mGraphicsPipeline);

	commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(viewWidth),
		static_cast<float>(viewHeight), 0.0f, 1.0f));
	commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), { .width = static_cast<uint32_t>(viewWidth), .height = static_cast<uint32_t>(viewHeight) }));

	commandBuffer.bindVertexBuffers(0, *vertexBuffer, { 0 });

	commandBuffer.draw(vertexCount, 1, 0, 0);

	commandBuffer.endRendering();
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
