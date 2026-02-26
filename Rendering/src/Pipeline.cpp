#include "Pipeline.h"
#include "Renderer.h"

#include "vertex.h"
#include "renderer_helpers.h"

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <fstream>

using namespace renderer;


constexpr const char* mainSolidShader = "shaders/forwardSolid.spv";
constexpr const char* vertSolidMainShaderFunc = "vertMain";
constexpr const char* fragSolidMainShaderFunc = "fragMain";

constexpr const char* mainSkinnedShader = "shaders/forwardSkinned.spv";
constexpr const char* vertSkinnedMainShaderFunc = "vertMain";
constexpr const char* fragSkinnedMainShaderFunc = "fragMain";

Pipeline::Pipeline(vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice, vk::Format outputFormat,
		vk::Format depthFormat, unsigned int maxTextures) {
	CreatePipeline(device, physicalDevice, outputFormat, depthFormat, maxTextures);
}

void Pipeline::CreatePipeline(vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice, vk::Format outputFormat,
		vk::Format depthFormat, unsigned int maxTextures) {
	CreateUniformBuffers(device, physicalDevice, maxTextures);

	// Solid meshes pipeline
	std::vector<char> shaderCode = ReadFile(mainSolidShader);

	vk::ShaderModuleCreateInfo shaderModuleCI{
		.codeSize = shaderCode.size() * sizeof(char),
		.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data())
	};
	vk::raii::ShaderModule shaderModule{ device, shaderModuleCI };

	vk::PipelineShaderStageCreateInfo vertShaderStageInfo{
		.stage = vk::ShaderStageFlagBits::eVertex,
		.module = shaderModule,
		.pName = vertSolidMainShaderFunc
	};
	vk::PipelineShaderStageCreateInfo fragShaderStageInfo{
		.stage = vk::ShaderStageFlagBits::eFragment,
		.module = shaderModule,
		.pName = fragSolidMainShaderFunc
	};

	std::vector< std::pair<const char*, vk::ShaderStageFlagBits> > shaderStageFuncs{
		{ vertSolidMainShaderFunc, vk::ShaderStageFlagBits::eVertex },
		{ fragSolidMainShaderFunc, vk::ShaderStageFlagBits::eFragment },
	};

	std::vector<vk::VertexInputBindingDescription> bindingDescriptions{
		vk::VertexInputBindingDescription {
			.binding = 0,
			.stride = sizeof(vertex),
			.inputRate = vk::VertexInputRate::eVertex
		}
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

	vk::PushConstantRange staticPushConstantRange{
		.stageFlags = vk::ShaderStageFlagBits::eAllGraphics,
		.offset = 0,
		.size = sizeof(StaticDrawDataUB),
	};

	NewVkPipeline(device, mainSolidShader, shaderStageFuncs, bindingDescriptions, attributeDescriptions, staticPushConstantRange,
		outputFormat, depthFormat, mVkDescriptorSetLayout, mVkSolidPipelineLayout, mVkSolidPipeline);

	// Skinned meshes pipeline
	bindingDescriptions.push_back(
		vk::VertexInputBindingDescription{
			.binding = 1,
			.stride = sizeof(vertexSkinning),
			.inputRate = vk::VertexInputRate::eVertex
		}
	);

	attributeDescriptions.push_back(
		vk::VertexInputAttributeDescription{
			.location = 3,
			.binding = 1,
			.format = vk::Format::eR32G32B32A32Sint,
			.offset = offsetof(vertexSkinning, indices)
		}
	);
	attributeDescriptions.push_back(
		vk::VertexInputAttributeDescription{
			.location = 4,
			.binding = 1,
			.format = vk::Format::eR32G32B32A32Sfloat,
			.offset = offsetof(vertexSkinning, weights)
		}
	);

	NewVkPipeline(device, mainSkinnedShader, shaderStageFuncs, bindingDescriptions, attributeDescriptions, staticPushConstantRange,
		outputFormat, depthFormat, mVkDescriptorSetLayout, mVkSkinnedPipelineLayout, mVkSkinnedPipeline);

#ifndef NDEBUG
	mPipelineCreated = true;
#endif
}

void Pipeline::DrawSolidMeshes(vk::raii::CommandBuffer& commandBuffer, vk::raii::ImageView& swapImageView, vk::raii::ImageView& depthImageView,
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

	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mVkSolidPipeline);

	commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(viewWidth),
		static_cast<float>(viewHeight), 0.0f, 1.0f));
	commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), { .width = static_cast<uint32_t>(viewWidth), .height = static_cast<uint32_t>(viewHeight) }));

	commandBuffer.bindVertexBuffers(0, *vertexBuffer, { 0 });
	commandBuffer.bindIndexBuffer(*indexBuffer, 0, vk::IndexType::eUint32);

	uint32_t uniformOffset = 0;

	PerObjectData pod{};
	commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mVkSolidPipelineLayout, 0, *mVkMainDescriptorSet, nullptr);
	
	for (size_t i = 0; i < solidMeshes.size(); ++i) {
		pod.model = lm2::position3D(solidMeshes[i].data.position) * lm2::mat4(lm2::rotation3D(solidMeshes[i].data.rotation));
		commandBuffer.pushConstants<PerObjectData>(mVkSolidPipelineLayout, vk::ShaderStageFlagBits::eAllGraphics, 0, pod);

		uniformOffset++;

		commandBuffer.drawIndexed(solidMeshes[i].indexCount, 1, solidMeshes[i].indexOffset, solidMeshes[i].vertexOffset, 0);
	}

	commandBuffer.endRendering();
}

void Pipeline::DrawSkinnedMeshes(vk::raii::CommandBuffer& commandBuffer, vk::raii::ImageView& swapImageView, vk::raii::ImageView& depthImageView,
		int viewWidth, int viewHeight, vk::raii::Buffer& vertexBuffer, vk::raii::Buffer& indexBuffer, vk::raii::Buffer& skinningBuffer,
		const std::vector<SkinnedMesh>& skinnedMeshes) {
	vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
	vk::ClearValue clearDepth = vk::ClearDepthStencilValue(1.0f, 0);

	vk::RenderingAttachmentInfo colorAttachmentInfo = {
		.imageView = swapImageView,
		.imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
		.loadOp = vk::AttachmentLoadOp::eLoad,
		.storeOp = vk::AttachmentStoreOp::eStore,
		.clearValue = clearColor
	};
	vk::RenderingAttachmentInfo depthAttachmentInfo = {
		.imageView = depthImageView,
		.imageLayout = vk::ImageLayout::eDepthAttachmentOptimal,
		.loadOp = vk::AttachmentLoadOp::eLoad,
		.storeOp = vk::AttachmentStoreOp::eDontCare,
		.clearValue = clearDepth
	};

	vk::RenderingInfo renderingInfo = {
		.renderArea = {
			.offset = { 0, 0 },
			.extent = {.width = static_cast<uint32_t>(viewWidth), .height = static_cast<uint32_t>(viewHeight) }
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

	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mVkSolidPipeline);

	commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(viewWidth),
		static_cast<float>(viewHeight), 0.0f, 1.0f));
	commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), { .width = static_cast<uint32_t>(viewWidth), .height = static_cast<uint32_t>(viewHeight) }));

	commandBuffer.bindVertexBuffers(0, *vertexBuffer, { 0 });
	commandBuffer.bindVertexBuffers(1, *skinningBuffer, { 0 });
	commandBuffer.bindIndexBuffer(*indexBuffer, 0, vk::IndexType::eUint32);

	uint32_t uniformOffset = 0;

	PerObjectData pod{};
	commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mVkSolidPipelineLayout, 0, *mVkMainDescriptorSet, nullptr);

	for (size_t i = 0; i < skinnedMeshes.size(); ++i) {
		pod.model = lm2::position3D(skinnedMeshes[i].data.position) * lm2::mat4(lm2::rotation3D(skinnedMeshes[i].data.rotation));
		commandBuffer.pushConstants<PerObjectData>(mVkSolidPipelineLayout, vk::ShaderStageFlagBits::eAllGraphics, 0, pod);

		uniformOffset++;

		commandBuffer.drawIndexed(skinnedMeshes[i].indexCount, 1, skinnedMeshes[i].indexOffset, skinnedMeshes[i].vertexOffset, 0);
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

void Pipeline::NewVkPipeline(const vk::raii::Device& device, const char* shaderFile,
		std::vector<std::pair<const char*, vk::ShaderStageFlagBits>> shaderStagesFuncs,
		std::vector<vk::VertexInputBindingDescription> bindingDescs, std::vector<vk::VertexInputAttributeDescription> attribDescs,
		vk::PushConstantRange pushConstantRange, vk::Format outputFormat, vk::Format depthFormat,
		vk::raii::DescriptorSetLayout& descriptorSet, vk::raii::PipelineLayout& outPipelineLayout, vk::raii::Pipeline& outPipeline) {
	std::vector<char> shaderCode = ReadFile(shaderFile);

	vk::ShaderModuleCreateInfo shaderModuleCI{
		.codeSize = shaderCode.size() * sizeof(char),
		.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data())
	};
	vk::raii::ShaderModule shaderModule{ device, shaderModuleCI };

	std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;

	for (const auto& shaderStage : shaderStagesFuncs) {
		shaderStages.push_back(
			vk::PipelineShaderStageCreateInfo{
				.stage = shaderStage.second,
				.module = shaderModule,
				.pName = shaderStage.first
			});
	}

	std::vector dynamicStates = {
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor
	};

	vk::PipelineDynamicStateCreateInfo dynamicState{
		.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
		.pDynamicStates = dynamicStates.data()
	};

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
		.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescs.size()),
		.pVertexBindingDescriptions = bindingDescs.data(),
		.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribDescs.size()),
		.pVertexAttributeDescriptions = attribDescs.data()
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


	vk::PipelineLayoutCreateInfo pipelineLayoutInfo{
		.setLayoutCount = 1,
		.pSetLayouts = &*descriptorSet,
		.pushConstantRangeCount = 1,
		.pPushConstantRanges = &pushConstantRange
	};

	outPipelineLayout = vk::raii::PipelineLayout{ device, pipelineLayoutInfo };

	vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo{
		.colorAttachmentCount = 1,
		.pColorAttachmentFormats = &outputFormat,
		.depthAttachmentFormat = depthFormat,
	};
	vk::GraphicsPipelineCreateInfo pipelineInfo{
		.pNext = &pipelineRenderingCreateInfo,
		.stageCount = static_cast<uint32_t>(shaderStages.size()),
		.pStages = shaderStages.data(),
		.pVertexInputState = &vertexInputInfo,
		.pInputAssemblyState = &inputAssembly,
		.pViewportState = &viewportState,
		.pRasterizationState = &rasterizer,
		.pMultisampleState = &multisampling,
		.pDepthStencilState = &depthStencil,
		.pColorBlendState = &colorBlending,
		.pDynamicState = &dynamicState,
		.layout = outPipelineLayout,
		.renderPass = nullptr,
	};

	outPipeline = vk::raii::Pipeline(device, nullptr, pipelineInfo);
}
