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

Pipeline::Pipeline(vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice, vk::Format outputFormat,
		vk::Format depthFormat, unsigned int maxTextures) {
	CreatePipeline(device, physicalDevice, outputFormat, depthFormat, maxTextures);
}

void Pipeline::CreatePipeline(vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice, vk::Format outputFormat,
		vk::Format depthFormat, unsigned int maxTextures) {
	CreateUniformBuffers(device, physicalDevice, maxTextures);

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
			.format = vk::Format::eR32G32B32Sfloat,
			.offset = offsetof(vertex, normal)
		},
		vk::VertexInputAttributeDescription{
			.location = 2,
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
		.frontFace = vk::FrontFace::eCounterClockwise,
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

	// TODO: Add stencil tests
	vk::PipelineDepthStencilStateCreateInfo depthStencil{
		.depthTestEnable = vk::True,
		.depthWriteEnable = vk::True,
		.depthCompareOp = vk::CompareOp::eLess,
		.depthBoundsTestEnable = vk::False,
		.stencilTestEnable = vk::False
	};

	vk::PushConstantRange staticPushConstantRange{
		.stageFlags = vk::ShaderStageFlagBits::eAllGraphics,
		.offset = 0,
		.size = sizeof(StaticDrawDataUB),
	};

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo{
		.setLayoutCount = 1,
		.pSetLayouts = &*mVkDescriptorSetLayout,
		.pushConstantRangeCount = 1,
		.pPushConstantRanges = &staticPushConstantRange
	};

	mVkPipelineLayout = vk::raii::PipelineLayout{ device, pipelineLayoutInfo };

	vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo{
		.colorAttachmentCount = 1,
		.pColorAttachmentFormats = &outputFormat,
		.depthAttachmentFormat = depthFormat,
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
		.pDepthStencilState = &depthStencil,
		.pColorBlendState = &colorBlending,
		.pDynamicState = &dynamicState,
		.layout = mVkPipelineLayout,
		.renderPass = nullptr,
	};

	mVkGraphicsPipeline = vk::raii::Pipeline(device, nullptr, pipelineInfo);

#ifndef NDEBUG
	mPipelineCreated = true;
#endif
}

void Pipeline::ApplyBasePass(vk::raii::CommandBuffer& commandBuffer, vk::raii::ImageView& swapImageView, vk::raii::ImageView& depthImageView,
	int viewWidth, int viewHeight, vk::raii::Buffer& vertexBuffer, vk::raii::Buffer& indexBuffer, const std::vector<SolidMesh>& solidMeshes) {

	vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
	vk::ClearValue clearDepth = vk::ClearDepthStencilValue(1.0f, 0);

	vk::RenderingAttachmentInfo colorAttachmentInfo = {
		.imageView = swapImageView,
		.imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
		.loadOp = vk::AttachmentLoadOp::eClear,
		.storeOp = vk::AttachmentStoreOp::eStore,
		.clearValue = clearColor
	};
	vk::RenderingAttachmentInfo depthAttachmentInfo = {
		.imageView = depthImageView,
		.imageLayout = vk::ImageLayout::eDepthAttachmentOptimal,
		.loadOp = vk::AttachmentLoadOp::eClear,
		.storeOp = vk::AttachmentStoreOp::eDontCare,
		.clearValue = clearDepth
	};

	vk::RenderingInfo renderingInfo = {
		.renderArea = {
			.offset = { 0, 0 },
			.extent = { .width = static_cast<uint32_t>(viewWidth), .height = static_cast<uint32_t>(viewHeight) }
		},
		.layerCount = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments = &colorAttachmentInfo,
		.pDepthAttachment = &depthAttachmentInfo,
	};

	StaticDrawDataUB staticUbo{
		lm2::position3D<float>({ 0.5f, 0, -5.0f }),
		lm2::perspective<float>(45.0f, 0.5f, 100.0f, static_cast<float>(viewHeight) / viewWidth),
	};
	memcpy(mVkStaticDrawDataBufferMapped, &staticUbo, sizeof(staticUbo));

	commandBuffer.beginRendering(renderingInfo);

	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mVkGraphicsPipeline);

	commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(viewWidth),
		static_cast<float>(viewHeight), 0.0f, 1.0f));
	commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), { .width = static_cast<uint32_t>(viewWidth), .height = static_cast<uint32_t>(viewHeight) }));

	commandBuffer.bindVertexBuffers(0, *vertexBuffer, { 0 });
	commandBuffer.bindIndexBuffer(*indexBuffer, 0, vk::IndexType::eUint16);

	uint32_t uniformOffset = 0;

	PerObjectData pod{};
	
	for (size_t i = 0; i < solidMeshes.size(); ++i) {
		pod.model = lm2::position3D(solidMeshes[i].data.position) * lm2::mat4(lm2::rotation3D(solidMeshes[i].data.rotation));
		commandBuffer.pushConstants<PerObjectData>(mVkPipelineLayout, vk::ShaderStageFlagBits::eAllGraphics, 0, pod);

		commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mVkPipelineLayout, 0, *mVkMainDescriptorSet, uniformOffset * mStaticDrawDataSize);
		uniformOffset++;

		commandBuffer.drawIndexed(solidMeshes[i].indexCount, 1, solidMeshes[i].indexOffset, solidMeshes[i].vertexOffset, 0);
	}

	commandBuffer.endRendering();
}

void Pipeline::UpdateTextures(const vk::raii::Device& device, const std::vector<TextureData>& textures) {
	std::vector<vk::DescriptorImageInfo> imageInfos(textures.size());
	for (size_t i = 0; i < textures.size(); ++i) {
		imageInfos[i] = vk::DescriptorImageInfo{
			.sampler = textures[i].sampler,
			.imageView = textures[i].imageView,
			.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
		};
	}
	vk::WriteDescriptorSet descriptorWrite{
		.dstSet = mVkMainDescriptorSet,
		.dstBinding = 1,
		.dstArrayElement = 0,
		.descriptorCount = static_cast<uint32_t>(imageInfos.size()),
		.descriptorType = vk::DescriptorType::eCombinedImageSampler,
		.pImageInfo = imageInfos.data(),
	};
	device.updateDescriptorSets(descriptorWrite, {});
}

void Pipeline::UpdateSingleTexture(const vk::raii::Device& device, const TextureData& texture) {
	vk::DescriptorImageInfo imageInfo{
		.sampler = texture.sampler,
		.imageView = texture.imageView,
		.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
	};
	vk::WriteDescriptorSet descriptorWrite{
		.dstSet = mVkMainDescriptorSet,
		.dstBinding = 1,
		.dstArrayElement = texture.index,
		.descriptorCount = 1,
		.descriptorType = vk::DescriptorType::eCombinedImageSampler,
		.pImageInfo = &imageInfo,
	};
	device.updateDescriptorSets(descriptorWrite, {});
}

void Pipeline::CreateUniformBuffers(vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice, unsigned int maxTextures) {
	/*auto properties = physicalDevice.getProperties();
	uint32_t minUboAlignment = static_cast<uint32_t>(properties.limits.minUniformBufferOffsetAlignment);
	if (minUboAlignment > 0) {
		mStaticDrawDataSize = (mStaticDrawDataSize + minUboAlignment - 1) & ~(minUboAlignment - 1);
	}*/
	mStaticDrawDataSize = sizeof(StaticDrawDataUB);

	std::array uboLayoutBindings{
		vk::DescriptorSetLayoutBinding{ 0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex, nullptr },
		vk::DescriptorSetLayoutBinding{ 1, vk::DescriptorType::eCombinedImageSampler, maxTextures, vk::ShaderStageFlagBits::eFragment, nullptr },
	};
	vk::DescriptorSetLayoutCreateInfo layoutInfo{
		.bindingCount = static_cast<uint32_t>(uboLayoutBindings.size()),
		.pBindings = uboLayoutBindings.data()
	};
	mVkDescriptorSetLayout = vk::raii::DescriptorSetLayout(device, layoutInfo);

	vk::DeviceSize bufferSize = mStaticDrawDataSize;

	createBuffer(device, physicalDevice, bufferSize, vk::BufferUsageFlagBits::eUniformBuffer,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, mVkStaticDrawDataBuffer, mVkStaticDrawDataBufferMemory);
	
	mVkStaticDrawDataBufferMapped = mVkStaticDrawDataBufferMemory.mapMemory(0, bufferSize);


	std::array poolSizes{
		vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, 1),
		vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, maxTextures),
	};
	vk::DescriptorPoolCreateInfo poolInfo{
		.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
		.maxSets = 1,
		.poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
		.pPoolSizes = poolSizes.data()
	};

	mVkDescriptorPool = vk::raii::DescriptorPool(device, poolInfo);

	vk::DescriptorSetLayout layouts{ *mVkDescriptorSetLayout };
	vk::DescriptorSetAllocateInfo allocInfo{
		.descriptorPool = mVkDescriptorPool,
		.descriptorSetCount = 1,
		.pSetLayouts = &layouts
	};
	auto descriptorSets = device.allocateDescriptorSets(allocInfo);
	mVkMainDescriptorSet = std::move(descriptorSets[0]);

	vk::DescriptorBufferInfo bufferInfo{
		.buffer = mVkStaticDrawDataBuffer,
		.offset = 0,
		.range = mStaticDrawDataSize
	};
	std::vector<vk::WriteDescriptorSet> descriptorWrites{
		vk::WriteDescriptorSet{
			.dstSet = mVkMainDescriptorSet,
			.dstBinding = 0,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = vk::DescriptorType::eUniformBuffer,
			.pBufferInfo = &bufferInfo
		}
	};
	device.updateDescriptorSets(descriptorWrites, {});
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
