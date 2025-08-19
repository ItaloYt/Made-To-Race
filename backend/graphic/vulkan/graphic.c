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

bool mtr_create_graphic(MTRGraphic *out, MTRWindow window) {
    assert(out != NULL);
    assert(window != NULL);

    MTRGraphic graphic = calloc(1, sizeof(struct MTRGraphic));

    if (graphic == NULL) return mtr_throw_graphic_error(MTR_GRAPHIC_ERROR_ALLOCATION_FAILED);

    *out = graphic;

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
        _mtr_create_vulkan_shader_module(&graphic->base_pipeline.vertex, graphic->device, shader_base_vert_spv, shader_base_vert_spv_size) ||
        _mtr_create_vulkan_shader_module(&graphic->base_pipeline.fragment, graphic->device, shader_base_frag_spv, shader_base_frag_spv_size)
    ) {
        mtr_destroy_graphic(out);

        return mtr_throw_graphic_error(MTR_GRAPHIC_ERROR_IMPLEMENTATION_FAILED);
    }

    return false;
}

bool mtr_create_graphic_object(MTRGraphicObject *object, MTRGraphic graphic, MTRGraphicObjectType type) {
    return false;
}

bool mtr_render_graphic(MTRGraphic graphic) {
    assert(graphic != NULL);

    return false;
}

void mtr_destroy_graphic(MTRGraphic *graphic) {
    assert(graphic != NULL);

    if (*graphic == NULL) return;

    _mtr_destroy_vulkan_shader_module((*graphic)->base_pipeline.fragment, (*graphic)->device);
    _mtr_destroy_vulkan_shader_module((*graphic)->base_pipeline.vertex, (*graphic)->device);
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
