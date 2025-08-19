#include <vulkan/vulkan.h>

#include <stdbool.h>
#include <assert.h>
#include <vulkan/vulkan_core.h>

#include "backend/graphic/vulkan/vulkan.h"
#include "game/math.h"

bool _mtr_create_vulkan_swapchain(VkSwapchainKHR *swapchain, const VkSurfaceCapabilitiesKHR *capabilities, const VkSurfaceFormatKHR *format, const VkExtent2D *extent, VkDevice device, VkSurfaceKHR surface, VkPresentModeKHR mode, unsigned graphic_index, unsigned present_index) {
    assert(swapchain != NULL);

    VkSwapchainCreateInfoKHR info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .surface = surface,
        .minImageCount = (capabilities->maxImageCount == 0 ? capabilities->minImageCount + 1 : mtr_clamp_int(capabilities->minImageCount + 1, capabilities->minImageCount, capabilities->maxImageCount)),
        .imageFormat = format->format,
        .imageColorSpace = format->colorSpace,
        .imageExtent = *extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_CONCURRENT,
        .queueFamilyIndexCount = 2,
        .pQueueFamilyIndices = (unsigned[]) { graphic_index, present_index },
        .preTransform = capabilities->currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = mode,
        .clipped = VK_TRUE,
        .oldSwapchain = NULL,
    };

    if (graphic_index == present_index) {
        info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        info.queueFamilyIndexCount = 0;
    }

    return _mtr_throw_vulkan_error(vkCreateSwapchainKHR(device, &info, NULL, swapchain));
}

void _mtr_destroy_vulkan_swapchain(VkSwapchainKHR swapchain, VkDevice device) {
    vkDestroySwapchainKHR(device, swapchain, NULL);
}

bool _mtr_recreate_vulkan_swapchain(MTRGraphic graphic) {
    (void) vkDeviceWaitIdle(graphic->device);

    _mtr_destroy_vulkan_framebuffers(&graphic->base_framebuffers, graphic->device, graphic->image_count);
    _mtr_destroy_vulkan_swapchain_views(&graphic->views, graphic->device, graphic->image_count);
    _mtr_destroy_vulkan_swapchain_images(&graphic->images);
    _mtr_destroy_vulkan_swapchain(graphic->swapchain, graphic->device);

    if (
        _mtr_load_vulkan_surface_capabilities(&graphic->capabilities, &graphic->extent, graphic->physical, graphic->surface, graphic->window) ||
        _mtr_load_vulkan_surface_format(&graphic->format, graphic->physical, graphic->surface) ||
        _mtr_load_vulkan_surface_present_mode(&graphic->mode, graphic->physical, graphic->surface) ||
        _mtr_create_vulkan_swapchain(&graphic->swapchain, &graphic->capabilities, &graphic->format, &graphic->extent, graphic->device, graphic->surface, graphic->mode, graphic->graphic_index, graphic->present_index) ||
        _mtr_load_vulkan_swapchain_images(&graphic->images, &graphic->image_count, graphic->swapchain, graphic->device) ||
        _mtr_create_vulkan_swapchain_views(&graphic->views, graphic->images, graphic->device, graphic->format.format, graphic->image_count) ||
        _mtr_create_vulkan_framebuffers(&graphic->base_framebuffers, graphic->device, graphic->base_render_pass, graphic->views, &graphic->extent, graphic->image_count)
    ) return true;

    return false;
}
