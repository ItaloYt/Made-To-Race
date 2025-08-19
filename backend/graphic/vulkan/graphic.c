#include <stdlib.h>
#include <vulkan/vk_enum_string_helper.h>

#include <stdio.h>
#include <stdbool.h>
#include <vulkan/vulkan_core.h>
#include <assert.h>

#include "game/graphic.h"
#include "game/window.h"
#include "game/shaders.h"
#include "backend/graphic/vulkan/vulkan.h"

// TODO: Learn more about render passes, pipeline layouts and pipelines
bool mtr_create_graphic(MTRGraphic *out, MTRWindow window) {
    assert(out != NULL);
    assert(window != NULL);

    MTRGraphic graphic = calloc(1, sizeof(struct MTRGraphic));

    if (graphic == NULL) return mtr_throw_graphic_error(MTR_GRAPHIC_ERROR_ALLOCATION_FAILED);

    *out = graphic;

    graphic->window = window;

    if (
        _mtr_create_vulkan_instance(&graphic->instance) ||
        _mtr_query_vulkan_surface(&graphic->surface, window, graphic->instance) ||
        _mtr_load_vulkan_physical(&graphic->physical, graphic->instance) ||
        _mtr_load_vulkan_queue_families(&graphic->graphic_index, &graphic->present_index, graphic->physical, window) ||
        _mtr_create_vulkan_device(&graphic->device, &graphic->graphic, &graphic->present, graphic->physical, graphic->graphic_index, graphic->present_index) ||
        _mtr_load_vulkan_surface_capabilities(&graphic->capabilities, &graphic->extent, graphic->physical, graphic->surface, window) ||
        _mtr_load_vulkan_surface_format(&graphic->format, graphic->physical, graphic->surface) ||
        _mtr_load_vulkan_surface_present_mode(&graphic->mode, graphic->physical, graphic->surface) ||
        _mtr_create_vulkan_swapchain(&graphic->swapchain, &graphic->capabilities, &graphic->format, &graphic->extent, graphic->device, graphic->surface, graphic->mode, graphic->graphic_index, graphic->present_index) ||
        _mtr_load_vulkan_swapchain_images(&graphic->images, &graphic->image_count, graphic->swapchain, graphic->device) ||
        _mtr_create_vulkan_swapchain_views(&graphic->views, graphic->images, graphic->device, graphic->format.format, graphic->image_count) ||
        _mtr_create_vulkan_shader_module(&graphic->base_vertex, graphic->device, shader_base_vert_spv, shader_base_vert_spv_size) ||
        _mtr_create_vulkan_shader_module(&graphic->base_fragment, graphic->device, shader_base_frag_spv, shader_base_frag_spv_size) ||
        _mtr_create_base_vulkan_render_pass(&graphic->base_render_pass, graphic->device, graphic->format.format) ||
        _mtr_create_base_vulkan_pipeline_layout(&graphic->base_layout, graphic->device) ||
        _mtr_create_base_vulkan_pipeline(&graphic->base_pipeline, graphic->device, graphic->base_layout, graphic->base_render_pass, graphic->base_vertex, graphic->base_fragment, &graphic->extent) ||
        _mtr_create_vulkan_framebuffers(&graphic->base_framebuffers, graphic->device, graphic->base_render_pass, graphic->views, &graphic->extent, graphic->image_count) ||
        _mtr_create_vulkan_command_pool(&graphic->graphic_pool, graphic->device, graphic->graphic_index) ||
        _mtr_create_vulkan_command_buffer(&graphic->graphic_cmd, graphic->device, graphic->graphic_pool) ||
        _mtr_create_vulkan_semaphores(&graphic->image_available, graphic->device, MTR_VULKAN_FRAMES_IN_FLIGHT) ||
        _mtr_create_vulkan_semaphores(&graphic->render_finished, graphic->device, graphic->image_count) || // We didn't implemented frames in flight yet, so no need to update this
        _mtr_create_vulkan_fences(&graphic->frame_ended, graphic->device, MTR_VULKAN_FRAMES_IN_FLIGHT)
    ) {
        mtr_destroy_graphic(out);

        return mtr_throw_graphic_error(MTR_GRAPHIC_ERROR_IMPLEMENTATION_FAILED);
    }

    return false;
}

bool mtr_create_graphic_object(MTRGraphicObject *object, MTRGraphic graphic, MTRGraphicObjectType type) {
    return false;
}

// TODO: Learn more about synchronization objects and command buffers
bool mtr_render_graphic(MTRGraphic graphic) {
    assert(graphic != NULL);

    if (mtr_is_window_resizing(graphic->window) && _mtr_recreate_vulkan_swapchain(graphic)) return true;

    if (_mtr_throw_vulkan_error(vkWaitForFences(graphic->device, 1, graphic->frame_ended, VK_FALSE, -1))) return true;

    unsigned image_index;
    VkResult status = vkAcquireNextImageKHR(graphic->device, graphic->swapchain, -1, graphic->image_available[0], NULL, &image_index);

    if (status == VK_SUBOPTIMAL_KHR || status == VK_ERROR_OUT_OF_DATE_KHR) return _mtr_recreate_vulkan_swapchain(graphic);
    if (_mtr_throw_vulkan_error(status)) return true;
    if (_mtr_throw_vulkan_error(vkResetFences(graphic->device, 1, graphic->frame_ended))) return true;
    if (_mtr_throw_vulkan_error(vkResetCommandBuffer(graphic->graphic_cmd, 0))) return true;

    VkCommandBufferBeginInfo cmd_begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = 0,
        .pInheritanceInfo = NULL,
    };

    if (_mtr_throw_vulkan_error(vkBeginCommandBuffer(graphic->graphic_cmd, &cmd_begin_info))) return true;

    VkRenderPassBeginInfo render_pass_begin_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = NULL,
        .renderPass = graphic->base_render_pass,
        .framebuffer = graphic->base_framebuffers[image_index],
        .renderArea = {
            .offset = { .x = 0, .y = 0 },
            .extent = graphic->extent,
        },
        .clearValueCount = 1,
        .pClearValues = (VkClearValue[]) {
            (VkClearValue) {
                .color = { .float32 = { 0, 0, 0, 0 }, },
            },
        },
    };

    vkCmdBeginRenderPass(graphic->graphic_cmd, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(graphic->graphic_cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, graphic->base_pipeline);

    vkCmdSetViewport(graphic->graphic_cmd, 0, 1, (VkViewport[]) {
        (VkViewport) {
            .x = 0,
            .y = 0,
            .width = graphic->extent.width,
            .height = graphic->extent.height,
            .minDepth = 0,
            .maxDepth = 1,
        },
    });

    vkCmdSetScissor(graphic->graphic_cmd, 0, 1, (VkRect2D[]) {
        (VkRect2D) {
            .offset = { .x = 0, .y = 0 },
            .extent = graphic->extent,
        },
    });

    vkCmdDraw(graphic->graphic_cmd, 3, 1, 0, 0);

    vkCmdEndRenderPass(graphic->graphic_cmd);

    if (_mtr_throw_vulkan_error(vkEndCommandBuffer(graphic->graphic_cmd))) return true;

    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = graphic->image_available,
        .pWaitDstStageMask = (VkPipelineStageFlags[]) {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        },
        .commandBufferCount = 1,
        .pCommandBuffers = (VkCommandBuffer[]) {
            graphic->graphic_cmd,
        },
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = graphic->render_finished + image_index,
    };

    if (_mtr_throw_vulkan_error(vkQueueSubmit(graphic->graphic, 1, &submit_info, graphic->frame_ended[0]))) return true;

    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = graphic->render_finished + image_index,
        .swapchainCount = 1,
        .pSwapchains = (VkSwapchainKHR[]) {
            graphic->swapchain,
        },
        .pImageIndices = (unsigned[]) {
            image_index,
        },
        .pResults = NULL,
    };

    status = vkQueuePresentKHR(graphic->present, &present_info);

    if (status == VK_SUBOPTIMAL_KHR || status == VK_ERROR_OUT_OF_DATE_KHR) return _mtr_recreate_vulkan_swapchain(graphic);
    if (_mtr_throw_vulkan_error(status)) return true;

    return false;
}

void mtr_destroy_graphic(MTRGraphic *graphic) {
    assert(graphic != NULL);

    if (*graphic == NULL) return;

    if ((*graphic)->device != NULL) (void) vkDeviceWaitIdle((*graphic)->device);

    _mtr_destroy_vulkan_fences(&(*graphic)->frame_ended, (*graphic)->device, MTR_VULKAN_FRAMES_IN_FLIGHT);
    _mtr_destroy_vulkan_semaphores(&(*graphic)->render_finished, (*graphic)->device, (*graphic)->image_count);
    _mtr_destroy_vulkan_semaphores(&(*graphic)->image_available, (*graphic)->device, MTR_VULKAN_FRAMES_IN_FLIGHT);
    _mtr_destroy_vulkan_command_pool((*graphic)->graphic_pool, (*graphic)->device);
    _mtr_destroy_vulkan_framebuffers(&(*graphic)->base_framebuffers, (*graphic)->device, (*graphic)->image_count);
    _mtr_destroy_vulkan_pipeline((*graphic)->base_pipeline, (*graphic)->device);
    _mtr_destroy_vulkan_pipeline_layout((*graphic)->base_layout, (*graphic)->device);
    _mtr_destroy_vulkan_render_pass((*graphic)->base_render_pass, (*graphic)->device);
    _mtr_destroy_vulkan_shader_module((*graphic)->base_fragment, (*graphic)->device);
    _mtr_destroy_vulkan_shader_module((*graphic)->base_vertex, (*graphic)->device);
    _mtr_destroy_vulkan_swapchain_views(&(*graphic)->views, (*graphic)->device, (*graphic)->image_count);
    _mtr_destroy_vulkan_swapchain_images(&(*graphic)->images);
    _mtr_destroy_vulkan_swapchain((*graphic)->swapchain, (*graphic)->device);
    _mtr_destroy_vulkan_device((*graphic)->device);
    _mtr_destroy_vulkan_surface((*graphic)->surface, (*graphic)->instance);
    _mtr_destroy_vulkan_instance((*graphic)->instance);

    free(*graphic);

    *graphic = NULL;
}

bool _mtr_throw_vulkan_error(VkResult result) {
    if (result == VK_SUCCESS) return false;

    (void) fprintf(stderr, "Vulkan error %08x - %s\n", result, string_VkResult(result));

    return true;
}

bool _mtr_throw_vulkan_app_error(MTRVulkanAppError code) {
    switch (code) {
        case MTR_VULKAN_APP_ERROR_NO_GRAPHIC_FAMILY:
        return "MTR_VULKAN_APP_ERROR_NO_GRAPHIC_FAMILY";

        case MTR_VULKAN_APP_ERROR_NO_PRESENT_FAMILY:
        return "MTR_VULKAN_APP_ERROR_NO_PRESENT_FAMILY";

        case MTR_VULKAN_APP_ERROR_ALLOCATE:
        return "MTR_VULKAN_APP_ERROR_ALLOCATE";
    }

    return "NULL(This is an error)";
}
