#include <vulkan/vulkan.h>

#include <stdbool.h>
#include <assert.h>

#include "backend/graphic/vulkan/vulkan.h"

bool _mtr_create_base_vulkan_pipeline(VkPipeline *pipeline, VkDevice device, VkPipelineLayout layout, VkRenderPass render_pass, VkShaderModule vertex, VkShaderModule fragment, const VkExtent2D *extent) {
    assert(pipeline != NULL);
    assert(device != NULL);
    assert(layout != NULL);
    assert(render_pass != NULL);
    assert(vertex != NULL);
    assert(fragment != NULL);
    assert(extent != NULL);

    VkGraphicsPipelineCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stageCount = 2,
        .pStages = (VkPipelineShaderStageCreateInfo[]) {
            (VkPipelineShaderStageCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = vertex,
                .pName = "main",
                .pSpecializationInfo = NULL,
            },
            (VkPipelineShaderStageCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = fragment,
                .pName = "main",
                .pSpecializationInfo = NULL,
            },
        },
        .pVertexInputState = &(VkPipelineVertexInputStateCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .vertexBindingDescriptionCount = 0,
            .pVertexBindingDescriptions = NULL,
            .vertexAttributeDescriptionCount = 0,
            .pVertexAttributeDescriptions = NULL,
        },
        .pInputAssemblyState = &(VkPipelineInputAssemblyStateCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE,
        },
        // .pTessellationState = &(VkPipelineTessellationStateCreateInfo) {
        //     .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
        //     .pNext = NULL,
        //     .flags = 0,
        //     .patchControlPoints = 0,
        // },
        .pTessellationState = NULL,
        .pViewportState = &(VkPipelineViewportStateCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .viewportCount = 1,
            .pViewports = (VkViewport[]) {
                (VkViewport) {
                    .x = 0,
                    .y = 0,
                    .width = extent->width,
                    .height = extent->height,
                    .minDepth = 0,
                    .maxDepth = 1,
                },
            },
            .scissorCount = 1,
            .pScissors = (VkRect2D[]) {
                (VkRect2D) {
                    .offset = { .x = 0, .y = 0 },
                    .extent = *extent,
                },
            },
        },
        .pRasterizationState = &(VkPipelineRasterizationStateCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_BACK_BIT,
            .frontFace = VK_FRONT_FACE_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0,
            .depthBiasClamp = 0,
            .depthBiasSlopeFactor = 0,
            .lineWidth = 1,
        },
        .pMultisampleState = &(VkPipelineMultisampleStateCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE,
            .minSampleShading = 0,
            .pSampleMask = NULL,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE,
        },
        // .pDepthStencilState = &(VkPipelineDepthStencilStateCreateInfo) {
        //     .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        //     .pNext = NULL,
        //     .flags = 0,
        //     .depthTestEnable = VK_FALSE,
        //     .depthWriteEnable = VK_FALSE,
        //     .depthCompareOp = VK_COMPARE_OP_LESS,
        //     .depthBoundsTestEnable = VK_FALSE,
        //     .stencilTestEnable = VK_FALSE,
        //     .front = {},
        //     .back = VK_STENCIL_OP_KEEP,
        //     .minDepthBounds = 0,
        //     .maxDepthBounds = 0,
        // }
        .pDepthStencilState = NULL,
        .pColorBlendState = &(VkPipelineColorBlendStateCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = 1,
            .pAttachments = (VkPipelineColorBlendAttachmentState[]) {
                (VkPipelineColorBlendAttachmentState) {
                    .blendEnable = VK_FALSE,
                    .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
                    .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
                    .colorBlendOp = VK_BLEND_OP_ADD,
                    .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
                    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
                    .alphaBlendOp = VK_BLEND_OP_ADD,
                    .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
                },
            },
            .blendConstants = { 0, 0, 0, 0 },
        },
        .pDynamicState = &(VkPipelineDynamicStateCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .dynamicStateCount = 0,
            .pDynamicStates = NULL,
        },
        .layout = layout,
        .renderPass = render_pass,
        .subpass = 0,
        .basePipelineHandle = NULL,
        .basePipelineIndex = -1,
    };

    return _mtr_throw_vulkan_error(vkCreateGraphicsPipelines(device, NULL, 1, &info, NULL, pipeline));
}

void _mtr_destroy_vulkan_pipeline(VkPipeline pipeline, VkDevice device) {
    vkDestroyPipeline(device, pipeline, NULL);
}
