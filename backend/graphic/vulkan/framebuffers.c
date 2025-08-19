#include <vulkan/vulkan.h>

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#include "backend/graphic/vulkan/vulkan.h"

bool _mtr_create_vulkan_framebuffers(VkFramebuffer **out, VkDevice device, VkRenderPass render_pass, const VkImageView *views, const VkExtent2D *extent, unsigned count) {
    assert(out != NULL);
    assert(device != NULL);
    assert(render_pass != NULL);
    assert(views != NULL);
    assert(extent != NULL);

    VkFramebuffer *framebuffers = calloc(count, sizeof(VkFramebuffer));
    if (framebuffers == NULL) return _mtr_throw_vulkan_app_error(MTR_VULKAN_APP_ERROR_ALLOCATE);

    *out = framebuffers;

    VkFramebufferCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .renderPass = render_pass,
        .attachmentCount = 1,
        .pAttachments = NULL, // Later definition
        .width = extent->width,
        .height = extent->height,
        .layers = 1,
    };

    for (unsigned i = 0; i < count; ++i) {
        info.pAttachments = views + i;

        if (_mtr_throw_vulkan_error(vkCreateFramebuffer(device, &info, NULL, framebuffers + i))) return true;
    }

    return false;
}

void _mtr_destroy_vulkan_framebuffers(VkFramebuffer **framebuffers, VkDevice device, unsigned count) {
    assert(framebuffers != NULL);

    if (*framebuffers == NULL) return;

    for (unsigned i = 0; i < count; ++i) {
        if ((*framebuffers)[i] == NULL) break;

        vkDestroyFramebuffer(device, (*framebuffers)[i], NULL);
    }

    free(*framebuffers);

    *framebuffers = NULL;
}
