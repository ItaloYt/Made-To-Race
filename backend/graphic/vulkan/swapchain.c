#include <vulkan/vulkan.h>

#include <stdbool.h>
#include <assert.h>
#include <vulkan/vulkan_core.h>

#include "backend/graphic/vulkan/vulkan.h"
#include "game/math.h"
#include "game/window.h"

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

bool _mtr_recreate_vulkan_swapchain(VkSwapchainKHR *swapchain, VkFramebuffer **framebuffers, VkImageView **views, VkImage **images, unsigned *image_count, VkSurfaceCapabilitiesKHR *capabilities, VkExtent2D *extent, VkSurfaceFormatKHR *format, VkPresentModeKHR *mode, VkDevice device, VkPhysicalDevice physical, VkSurfaceKHR surface, VkRenderPass render_pass, MTRWindow window, unsigned graphic_index, unsigned present_index) {
    assert(swapchain != NULL);
    assert(framebuffers != NULL);
    assert(views != NULL);
    assert(images != NULL);
    assert(image_count != NULL);
    assert(capabilities != NULL);
    assert(extent != NULL);
    assert(format != NULL);
    assert(mode != NULL);
    assert(device != NULL);
    assert(physical != NULL);
    assert(surface != NULL);
    assert(render_pass != NULL);
    assert(window != NULL);

    (void) vkDeviceWaitIdle(device);

    _mtr_destroy_vulkan_framebuffers(framebuffers, device, *image_count);
    _mtr_destroy_vulkan_swapchain_views(views, device, *image_count);
    _mtr_destroy_vulkan_swapchain_images(images);
    _mtr_destroy_vulkan_swapchain(*swapchain, device);

    if (
        _mtr_load_vulkan_surface_capabilities(capabilities, extent, physical, surface, window) ||
        _mtr_load_vulkan_surface_format(format, physical, surface) ||
        _mtr_load_vulkan_surface_present_mode(mode, physical, surface) ||
        _mtr_create_vulkan_swapchain(swapchain, capabilities, format, extent, device, surface, *mode, graphic_index, present_index) ||
        _mtr_load_vulkan_swapchain_images(images, image_count, *swapchain, device) ||
        _mtr_create_vulkan_swapchain_views(views, *images, device, format->format, *image_count) ||
        _mtr_create_vulkan_framebuffers(framebuffers, device, render_pass, *views, extent, *image_count)
    ) return true;

    return false;
}
