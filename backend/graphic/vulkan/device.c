#include <vulkan/vulkan.h>

#include <stdbool.h>
#include <assert.h>

#include "backend/graphic/vulkan/vulkan.h"

// 0 - Graphic index
// 1 - Present Index
bool _mtr_create_vulkan_device(VkDevice *device, VkQueue *graphic, VkQueue *present, VkPhysicalDevice physical, unsigned graphic_index, unsigned present_index) {
    assert(device != NULL);
    assert(physical != NULL);

    VkDeviceCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .queueCreateInfoCount = (graphic_index == present_index ? 1 : 2),
        .pQueueCreateInfos = (VkDeviceQueueCreateInfo[2]) {
            (VkDeviceQueueCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .queueFamilyIndex = graphic_index - 1,
                .queueCount = 1,
                .pQueuePriorities = &(float) { 1 },
            },
            (VkDeviceQueueCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .queueFamilyIndex = present_index - 1,
                .queueCount = 1,
                .pQueuePriorities = &(float) { 1 },
            },
        },
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = 1,
        .ppEnabledExtensionNames = (const char *[]) {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        },
        .pEnabledFeatures = &(VkPhysicalDeviceFeatures) {0},
    };

    if(_mtr_throw_vulkan_error(vkCreateDevice(physical, &info, NULL, device))) return true;

    vkGetDeviceQueue(*device, graphic_index - 1, 0, graphic);
    vkGetDeviceQueue(*device, present_index - 1, 0, present);

    return false;
}

void _mtr_destroy_vulkan_device(VkDevice device) {
    vkDestroyDevice(device, NULL);
}
