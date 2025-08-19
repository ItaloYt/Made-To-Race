#include <vulkan/vulkan.h>

#include <stdbool.h>
#include <assert.h>

#include "backend/graphic/vulkan/vulkan.h"

bool _mtr_create_vulkan_shader_module(VkShaderModule *module, VkDevice device, const char *code, unsigned size) {
    assert(module != NULL);
    assert(device != NULL);
    assert(code != NULL);

    VkShaderModuleCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .codeSize = size,
        .pCode = (const unsigned *) code,
    };

    return _mtr_throw_vulkan_error(vkCreateShaderModule(device, &info, NULL, module));
}

void _mtr_destroy_vulkan_shader_module(VkShaderModule module, VkDevice device) {
    vkDestroyShaderModule(device, module, NULL);
}
