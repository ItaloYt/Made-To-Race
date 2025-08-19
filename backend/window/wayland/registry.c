#include <string.h>
#include <wayland-client-core.h>
#include <wayland-client.h>

#include <assert.h>
#include <stdbool.h>

#include <stdio.h>

#include "backend/window/wayland/xdg-shell.h"
#include "backend/window/wayland/xdg-decoration.h"
#include "backend/window/wayland/wayland.h"

static void global_event(MTRWaylandRegistry *user, struct wl_registry *registry, unsigned name, const char *interface, unsigned version);
static void global_remote_event(MTRWaylandRegistry *user, struct wl_registry *registry, unsigned name);

bool _mtr_create_wayland_registry(MTRWaylandRegistry *registry, struct wl_display *display) {
    static struct wl_registry_listener listener = {
        .global = (void *) global_event,
        .global_remove = (void *) global_remote_event,
    };

    assert(registry != NULL);
    assert(!registry->initialized);
    assert(display != NULL);
   
    registry->registry = wl_display_get_registry(display);

    if (registry->registry == NULL) return _mtr_throw_wayland_error(MTR_WAYLAND_ERROR_GET_REGISTRY);

    registry->initialized = true;

    if (wl_registry_add_listener(registry->registry, &listener, registry) == -1) {
        _mtr_destroy_wayland_registry(registry);

        return _mtr_throw_wayland_error(MTR_WAYLAND_ERROR_ADD_REGISTRY_LISTENER);
    }

    return false;
}

void _mtr_destroy_wayland_registry(MTRWaylandRegistry *registry) {
    if (registry == NULL || !registry->initialized) return;

    if (registry->manager != NULL) zxdg_decoration_manager_v1_destroy(registry->manager);
    if (registry->shell != NULL) xdg_wm_base_destroy(registry->shell);
    if (registry->compositor != NULL) wl_compositor_destroy(registry->compositor);
    if (registry->registry != NULL) wl_registry_destroy(registry->registry);

    registry->initialized = false;
}

bool _mtr_validate_wayland_registry(MTRWaylandRegistry *registry) {
    assert(registry != NULL);
    assert(registry->initialized);

    if (registry->compositor == NULL) return _mtr_throw_wayland_error(MTR_WAYLAND_ERROR_BIND_COMPOSITOR);
    if (registry->shell == NULL) return _mtr_throw_wayland_error(MTR_WAYLAND_ERROR_BIND_XDG_SHELL);
    if (registry->manager == NULL) return _mtr_throw_wayland_error(MTR_WAYLAND_ERROR_BIND_XDG_DECORATION);

    return false;
}

static void global_event(MTRWaylandRegistry *user, struct wl_registry *registry, unsigned name, const char *interface, unsigned version) {
    if(strcmp(interface, wl_compositor_interface.name) == 0) {
        user->compositor = wl_registry_bind(registry, name, &wl_compositor_interface, version);

        return;
    }

    if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        user->shell = wl_registry_bind(registry, name, &xdg_wm_base_interface, version);

        return;
    }

    if (strcmp(interface, zxdg_decoration_manager_v1_interface.name) == 0) {
        user->manager = wl_registry_bind(registry, name, &zxdg_decoration_manager_v1_interface, version);

        return;
    }
}

static void global_remote_event(MTRWaylandRegistry *user, struct wl_registry *registry, unsigned name) {

}
