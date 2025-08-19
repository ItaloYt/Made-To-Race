#include <wayland-client.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "backend/window/wayland/wayland.h"
#include "game/window.h"

bool mtr_create_window(MTRWindow *out) {
    assert(out != NULL);

    MTRWindow window = calloc(1, sizeof(struct MTRWindow));

    if (window == NULL) return _mtr_throw_window_error(MTR_WINDOW_ERROR_ALLOCATION_FAILED);

    *out = window;

    if (
        _mtr_connect_wayland_display(&window->display) ||
        _mtr_create_wayland_registry(&window->registry, window->display) ||
        _mtr_roundtrip_wayland_display(window->display) ||
        _mtr_validate_wayland_registry(&window->registry) ||
        _mtr_create_wayland_surface(&window->surface, window->registry.compositor, window->registry.shell, window->registry.manager) ||
        _mtr_roundtrip_wayland_display(window->display) ||
        window->surface.invalid_state
    ) {
        mtr_destroy_window(out);

        return _mtr_throw_window_error(MTR_WINDOW_ERROR_IMPLEMENTATION);
    }

    wl_surface_commit(window->surface.surface_wl);

    return false;
}

bool mtr_update_window(MTRWindow window) {
    assert(window != NULL);

    if (window->surface.resizing) {
        wl_surface_commit(window->surface.surface_wl);

        window->surface.resizing = false;
    }

    int status;

    while(1) {
        status = wl_display_dispatch_pending(window->display);

        if (status == -1) return _mtr_throw_wayland_error(MTR_WAYLAND_ERROR_ROUNDTRIP_DISPLAY);

        if (status == 0) break;
    }

    return false;
}

void mtr_destroy_window(MTRWindow *window) {
    assert(window != NULL);

    if (*window == NULL) return;

    _mtr_destroy_wayland_registry(&(*window)->registry);
    _mtr_disconnect_wayland_display((*window)->display);

    free(*window);

    *window = NULL;
}

unsigned mtr_get_window_width(MTRWindow window) { return window->surface.width; }
unsigned mtr_get_window_height(MTRWindow window) { return window->surface.height; }
bool mtr_is_window_closed(MTRWindow window) { return window->surface.closed; }
bool mtr_is_window_resizing(MTRWindow window) { return window->surface.resizing; }

bool _mtr_throw_wayland_error(MTRWaylandError code) {
    (void) fprintf(stderr, "Wayland Error %02x - %s\n", code, _mtr_convert_wayland_error_to_string(code));

    return true;
}

const char *_mtr_convert_wayland_error_to_string(MTRWaylandError code) {
    switch (code) {
        case MTR_WAYLAND_ERROR_CONNECT_DISPLAY:
        return "MTR_WAYLAND_ERROR_CONNECT_DISPLAY";

        case MTR_WAYLAND_ERROR_GET_REGISTRY:
        return "MTR_WAYLAND_ERROR_GET_REGISTRY";

        case MTR_WAYLAND_ERROR_ALLOCATE:
        return "MTR_WAYLAND_ERROR_ALLOCATE";

        case MTR_WAYLAND_ERROR_ADD_REGISTRY_LISTENER:
        return "MTR_WAYLAND_ERROR_ADD_REGISTRY_LISTENER";

        case MTR_WAYLAND_ERROR_ROUNDTRIP_DISPLAY:
        return "MTR_WAYLAND_ERROR_ROUNDTRIP_DISPLAY";

        case MTR_WAYLAND_ERROR_BIND_COMPOSITOR:
        return "MTR_WAYLAND_ERROR_BIND_COMPOSITOR";

        case MTR_WAYLAND_ERROR_BIND_XDG_SHELL:
        return "MTR_WAYLAND_ERROR_BIND_XDG_SHELL";

        case MTR_WAYLAND_ERROR_BIND_XDG_DECORATION:
        return "MTR_WAYLAND_ERROR_BIND_XDG_DECORATION";

        case MTR_WAYLAND_ERROR_CREATE_SURFACE:
        return "MTR_WAYLAND_ERROR_CREATE_SURFACE";

        case MTR_WAYLAND_ERROR_CREATE_XDG_SURFACE:
        return "MTR_WAYLAND_ERROR_CREATE_XDG_SURFACE";

        case MTR_WAYLAND_ERROR_CREATE_TOPLEVEL:
        return "MTR_WAYLAND_ERROR_CREATE_TOPLEVEL";

        case MTR_WAYLAND_ERROR_CREATE_DECORATION:
        return "MTR_WAYLAND_ERROR_CREATE_DECORATION";

        case MTR_WAYLAND_ERROR_ADD_SHELL_LISTENER:
        return "MTR_WAYLAND_ERROR_ADD_SHELL_LISTENER";

        case MTR_WAYLAND_ERROR_ADD_XDG_SURFACE_LISTENER:
        return "MTR_WAYLAND_ERROR_ADD_XDG_SURFACE_LISTENER";

        case MTR_WAYLAND_ERROR_ADD_TOPLEVEL_LISTENER:
        return "MTR_WAYLAND_ERROR_ADD_TOPLEVEL_LISTENER";

        case MTR_WAYLAND_ERROR_ADD_DECORATION_LISTENER:
        return "MTR_WAYLAND_ERROR_ADD_DECORATION_LISTENER";

        case MTR_WAYLAND_ERROR_EXPECTED_SERVER_SIDE_DECORATION:
        return "MTR_WAYLAND_ERROR_EXPECTED_SERVER_SIDE_DECORATION";
    }

    return "NULL(This is an error)";
}

#if MTR_VULKAN != 0
#define VK_USE_PLATFORM_WAYLAND_KHR
#include <vulkan/vulkan.h>

#include "backend/graphic/vulkan/vulkan.h"

void _mtr_query_vulkan_instance_extensions(const char ***exts, unsigned *count) {
    static const char *exts_default[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
    };

    static unsigned count_default = sizeof(exts_default) / sizeof(const char *);

    *exts = exts_default;
    *count = count_default;
}

bool _mtr_query_vulkan_surface(VkSurfaceKHR *surface, MTRWindow window, VkInstance instance) {
    assert(surface != NULL);
    assert(window != NULL);
    assert(instance != NULL);

    VkWaylandSurfaceCreateInfoKHR info = {
        .sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .display = window->display,
        .surface = window->surface.surface_wl,
    };

    return _mtr_throw_vulkan_error(vkCreateWaylandSurfaceKHR(instance, &info, NULL, surface));
}

bool _mtr_query_vulkan_physical_presentation_support(bool *is_supported, VkPhysicalDevice physical, unsigned family, MTRWindow window) {
    *is_supported = vkGetPhysicalDeviceWaylandPresentationSupportKHR(physical, family, window->display) == VK_TRUE;

    return false;
}
#endif
