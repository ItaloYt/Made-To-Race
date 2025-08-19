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
