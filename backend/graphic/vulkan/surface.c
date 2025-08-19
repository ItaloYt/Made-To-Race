#include <vulkan/vulkan.h>

#include <stdbool.h>
#include <assert.h>

#include "backend/graphic/vulkan/vulkan.h"
#include "game/window.h"
#include "game/math.h"

void _mtr_destroy_vulkan_surface(VkSurfaceKHR surface, VkInstance instance) {
    vkDestroySurfaceKHR(instance, surface, NULL);
}

bool _mtr_load_vulkan_surface_capabilities(VkSurfaceCapabilitiesKHR *capabilities, VkExtent2D *extent, VkPhysicalDevice physical, VkSurfaceKHR surface, MTRWindow window) {
    assert(capabilities != NULL);
    assert(extent != NULL);
    assert(physical != NULL);
    assert(surface != NULL);
    assert(window != NULL);

    if (_mtr_throw_vulkan_error(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical, surface, capabilities))) return true;

    if (capabilities->currentExtent.width > -1) {
        *extent = capabilities->currentExtent;

        return false;
    }

    extent->width = mtr_clamp_int(mtr_get_window_width(window), capabilities->minImageExtent.width, capabilities->maxImageExtent.width);
    extent->height = mtr_clamp_int(mtr_get_window_height(window), capabilities->minImageExtent.height, capabilities->maxImageExtent.height);

    return false;
}

bool _mtr_load_vulkan_surface_format(VkSurfaceFormatKHR *format, VkPhysicalDevice physical, VkSurfaceKHR surface) {
    assert(format != NULL);
    assert(physical != NULL);
    assert(surface != NULL);

    unsigned count;
    if (_mtr_throw_vulkan_error(vkGetPhysicalDeviceSurfaceFormatsKHR(physical, surface, &count, NULL))) return true;

    VkSurfaceFormatKHR formats[count];
    if (_mtr_throw_vulkan_error(vkGetPhysicalDeviceSurfaceFormatsKHR(physical, surface, &count, formats))) return true;

    *format = formats[0];
    
    for (unsigned i = 1; i < count; ++i) {
        if (formats[i].format != VK_FORMAT_B8G8R8A8_SRGB || formats[i].colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) continue;

        *format = formats[i];
        break;
    }

    return false;
}

bool _mtr_load_vulkan_surface_present_mode(VkPresentModeKHR *mode, VkPhysicalDevice physical, VkSurfaceKHR surface) {
    assert(mode != NULL);
    assert(physical != NULL);
    assert(surface != NULL);

    unsigned count;
    if (_mtr_throw_vulkan_error(vkGetPhysicalDeviceSurfacePresentModesKHR(physical, surface, &count, NULL))) return true;

    VkPresentModeKHR modes[count];
    if (_mtr_throw_vulkan_error(vkGetPhysicalDeviceSurfacePresentModesKHR(physical, surface, &count, modes))) return true;

    *mode = VK_PRESENT_MODE_FIFO_KHR;

    for (unsigned i = 0; i < count; ++i) {
        if (modes[i] != VK_PRESENT_MODE_MAILBOX_KHR) continue;

        *mode = modes[i];
        break;
    }

    return false;
}
