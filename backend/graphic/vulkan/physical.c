#include <vulkan/vulkan.h>

#include <assert.h>

#include "backend/graphic/vulkan/vulkan.h"

bool _mtr_load_vulkan_physical(VkPhysicalDevice *physical, VkInstance instance) {
    assert(physical != NULL);
    assert(instance != NULL);

    unsigned count;
    if (_mtr_throw_vulkan_error(vkEnumeratePhysicalDevices(instance, &count, NULL))) return true;

    VkPhysicalDevice physicals[count];
    if (_mtr_throw_vulkan_error(vkEnumeratePhysicalDevices(instance, &count, physicals))) return true;

    // TODO: Add a better physical device selection algorithm
    *physical = physicals[0];

    return false;
}

bool _mtr_load_vulkan_queue_families(unsigned *graphic_index, unsigned *present_index, VkPhysicalDevice physical, MTRWindow window) {
    assert(physical != NULL);
    assert(graphic_index != NULL);
    assert(present_index != NULL);
    assert(window != NULL);

    unsigned count;
    vkGetPhysicalDeviceQueueFamilyProperties(physical, &count, NULL);

    VkQueueFamilyProperties queues[count];
    vkGetPhysicalDeviceQueueFamilyProperties(physical, &count, queues);

    *graphic_index = 0;
    *present_index = 0;

    for (unsigned i = 0; i < count && (*graphic_index == 0 || *present_index == 0); ++i) {
        if (*graphic_index == 0 && queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) *graphic_index = i + 1;

        bool is_supported;
        if (_mtr_query_vulkan_physical_presentation_support(&is_supported, physical, i, window)) return true;

        if (!is_supported) continue;

        *present_index = i + 1;
    }

    if (*graphic_index == 0) return _mtr_throw_vulkan_app_error(MTR_VULKAN_APP_ERROR_NO_GRAPHIC_FAMILY);
    
    if (*present_index == 0) {
        *graphic_index = 0;

        return _mtr_throw_vulkan_app_error(MTR_VULKAN_APP_ERROR_NO_PRESENT_FAMILY);
    }

    return false;
}
