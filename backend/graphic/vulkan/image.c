#include <vulkan/vulkan.h>

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#include "backend/graphic/vulkan/vulkan.h"

bool _mtr_load_vulkan_swapchain_images(VkImage **out, unsigned *count, VkSwapchainKHR swapchain, VkDevice device) {
    assert(out != NULL);
    assert(count != NULL);
    assert(swapchain != NULL);
    assert(device != NULL);

    if (_mtr_throw_vulkan_error(vkGetSwapchainImagesKHR(device, swapchain, count, NULL))) return true;

    VkImage *images = malloc((*count) * sizeof(VkImage));
    if (images == NULL) return _mtr_throw_vulkan_app_error(MTR_VULKAN_APP_ERROR_ALLOCATE);

    *out = images;

    if (_mtr_throw_vulkan_error(vkGetSwapchainImagesKHR(device, swapchain, count, images))) {
        _mtr_destroy_vulkan_swapchain_images(out);

        return true;
    }

    return false;
}

void _mtr_destroy_vulkan_swapchain_images(VkImage **images) {
    assert(images != NULL);

    if (*images == NULL) return;

    free(*images);

    *images = NULL;
}

bool _mtr_create_vulkan_swapchain_views(VkImageView **out, const VkImage *images, VkDevice device, VkFormat format, unsigned count) {
    assert(out != NULL);
    assert(images != NULL);
    assert(device != NULL);

    VkImageView *views = calloc(count, sizeof(VkImageView));
    if (views == NULL) return _mtr_throw_vulkan_app_error(MTR_VULKAN_APP_ERROR_ALLOCATE);

    *out = views;

    VkImageViewCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .image = NULL, // Later definition
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .components = (VkComponentMapping) { VK_COMPONENT_SWIZZLE_IDENTITY },
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };

    for (unsigned i = 0; i < count; ++i) {
        info.image = images[i];

        if (_mtr_throw_vulkan_error(vkCreateImageView(device, &info, NULL, views + i))) {
            _mtr_destroy_vulkan_swapchain_views(out, device, count);

            return true;
        }
    }

    return false;
}

void _mtr_destroy_vulkan_swapchain_views(VkImageView **views, VkDevice device, unsigned count) {
    assert(views != NULL);

    if (*views == NULL) return;

    for (unsigned i = 0; i < count; ++i) {
        if ((*views)[i] == NULL) break;

        vkDestroyImageView(device, (*views)[i], NULL);
    }

    free(*views);

    *views = NULL;
}
