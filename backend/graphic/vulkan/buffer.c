#include <string.h>
#include <vulkan/vulkan.h>

#include <stdbool.h>
#include <assert.h>

#include "backend/graphic/vulkan/vulkan.h"

bool _mtr_create_vulkan_buffer(VkBuffer *buffer, VkDevice device, VkBufferUsageFlags usage, unsigned size) {
    assert(buffer != NULL);
    assert(device != NULL);

    VkBufferCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = NULL,
    };

    return _mtr_throw_vulkan_error(vkCreateBuffer(device, &info, NULL, buffer));
}

void _mtr_destroy_vulkan_buffer(VkBuffer buffer, VkDevice device) {
    vkDestroyBuffer(device, buffer, NULL);
}

bool _mtr_create_vulkan_buffer_memory(VkDeviceMemory *memory, VkDevice device, VkBuffer buffer, VkPhysicalDevice physical, VkMemoryPropertyFlags flags) {
    assert(memory != NULL);
    assert(device != NULL);
    assert(buffer != NULL);
    assert(physical != NULL);

    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(device, buffer, &requirements);

    VkMemoryAllocateInfo info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = requirements.size,
        .memoryTypeIndex = -1, // Later definition
    };

    VkPhysicalDeviceMemoryProperties properties;
    vkGetPhysicalDeviceMemoryProperties(physical, &properties);

    for (unsigned i = 0; i < properties.memoryTypeCount; ++i) {
        if (requirements.memoryTypeBits & (1 << i) && (properties.memoryTypes[i].propertyFlags & flags) == flags) {
            info.memoryTypeIndex = i;

            break;
        }
    }

    if (info.memoryTypeIndex == -1) return _mtr_throw_vulkan_app_error(MTR_VULKAN_APP_ERROR_MEMORY_FLAGS_NOT_FOUND);
    if (_mtr_throw_vulkan_error(vkAllocateMemory(device, &info, NULL, memory))) return true;

    return _mtr_throw_vulkan_error(vkBindBufferMemory(device, buffer, *memory, 0));
}

void _mtr_destroy_vulkan_memory(VkDeviceMemory memory, VkDevice device) {
    vkFreeMemory(device, memory, NULL);
}

bool _mtr_copy_vulkan_buffers(VkBuffer dst, VkBuffer src, VkDevice device, VkCommandBuffer cmd, VkQueue queue, unsigned size) {
    assert(dst != NULL);
    assert(src != NULL);
    assert(device != NULL);
    assert(cmd != NULL);

    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = NULL,
    };

    if (
        _mtr_throw_vulkan_error(vkResetCommandBuffer(cmd, 0)) ||
        _mtr_throw_vulkan_error(vkBeginCommandBuffer(cmd, &begin_info))
    ) return true;

    vkCmdCopyBuffer(cmd, src, dst, 1, (VkBufferCopy[]) {
        (VkBufferCopy) {
            .srcOffset = 0,
            .dstOffset = 0,
            .size = size,
        },
    });

    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = 0,
        .pWaitSemaphores = NULL,
        .pWaitDstStageMask = NULL,
        .commandBufferCount = 1,
        .pCommandBuffers = (VkCommandBuffer[]) { cmd },
        .signalSemaphoreCount = 0,
        .pSignalSemaphores = NULL,
    };

    return 
        _mtr_throw_vulkan_error(vkEndCommandBuffer(cmd)) ||
        _mtr_throw_vulkan_error(vkQueueSubmit(queue, 1, &submit_info, NULL)) ||
        _mtr_throw_vulkan_error(vkQueueWaitIdle(queue))
    ;
}

bool _mtr_create_staged_vulkan_buffer(VkBuffer *buffer, VkDeviceMemory *memory, VkDevice device, VkPhysicalDevice physical, VkCommandBuffer cmd, VkQueue queue, VkBufferUsageFlags usage, VkMemoryPropertyFlags flags, const void *data, unsigned size) {
    assert(buffer != NULL);
    assert(memory != NULL);
    assert(device != NULL);
    assert(physical != NULL);

    VkBuffer stage_buffer = NULL;
    VkDeviceMemory stage_memory = NULL;
    void *mapped;

    if (
        _mtr_create_vulkan_buffer(&stage_buffer, device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, size) ||
        _mtr_create_vulkan_buffer_memory(&stage_memory, device, stage_buffer, physical, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) ||
        _mtr_throw_vulkan_error(vkMapMemory(device, stage_memory, 0, size, 0, &mapped))
    ) {
        _mtr_destroy_vulkan_memory(stage_memory, device);
        _mtr_destroy_vulkan_buffer(stage_buffer, device);

        return true;
    }
    
    (void) memcpy(mapped, data, size);
    vkUnmapMemory(device, stage_memory);

    bool status = false;

    if (
        _mtr_create_vulkan_buffer(buffer, device, usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, size) ||
        _mtr_create_vulkan_buffer_memory(memory, device, *buffer, physical, flags | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) ||
        _mtr_copy_vulkan_buffers(*buffer, stage_buffer, device, cmd, queue, size)
    ) {
        _mtr_destroy_vulkan_memory(*memory, device);
        _mtr_destroy_vulkan_buffer(*buffer, device);

        status = true;
    }

    _mtr_destroy_vulkan_memory(stage_memory, device);
    _mtr_destroy_vulkan_buffer(stage_buffer, device);
    
    return status;
}
