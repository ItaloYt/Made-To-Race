#include <vulkan/vulkan.h>

#include <stdbool.h>
#include <assert.h>

#include "backend/graphic/vulkan/vulkan.h"

// TODO: add pipeline layout helper functions
bool _mtr_create_base_vulkan_pipeline_layout(VkPipelineLayout *layout, VkDevice device) {
    assert(layout != NULL);
    assert(device != NULL);

    VkPipelineLayoutCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .setLayoutCount = 0,
        .pSetLayouts = NULL,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL,
    };

    return _mtr_throw_vulkan_error(vkCreatePipelineLayout(device, &info, NULL, layout));
}

void _mtr_destroy_vulkan_pipeline_layout(VkPipelineLayout layout, VkDevice device) {
    vkDestroyPipelineLayout(device, layout, NULL);
}
