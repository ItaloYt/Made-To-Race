#include <vulkan/vulkan.h>

#include <stdbool.h>
#include <assert.h>

#include "backend/graphic/vulkan/vulkan.h"

bool _mtr_create_vulkan_command_pool(VkCommandPool *pool, VkDevice device, unsigned queue_index) {
    assert(pool != NULL);
    assert(device != NULL);

    VkCommandPoolCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = queue_index - 1,
    };

    return _mtr_throw_vulkan_error(vkCreateCommandPool(device, &info, NULL, pool));
}

void _mtr_destroy_vulkan_command_pool(VkCommandPool pool, VkDevice device) {
    vkDestroyCommandPool(device, pool, NULL);
}

bool _mtr_create_vulkan_command_buffer(VkCommandBuffer *cmd, VkDevice device, VkCommandPool pool) {
    assert(cmd != NULL);
    assert(device != NULL);
    assert(pool != NULL);

    VkCommandBufferAllocateInfo info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    return _mtr_throw_vulkan_error(vkAllocateCommandBuffers(device, &info, cmd));
}
