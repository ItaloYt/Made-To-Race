#include <wayland-client.h>

#include <assert.h>
#include <stdbool.h>

#include "backend/window/wayland/xdg-shell.h"
#include "backend/window/wayland/xdg-decoration.h"

#include "game/math.h"
#include "backend/window/wayland/wayland.h"

static void shell_ping_event(MTRWaylandSurface *surface, struct xdg_wm_base *shell, unsigned serial);
static void xdg_configure_event(MTRWaylandSurface *surface, struct xdg_surface *xdg, unsigned serial);
static void toplevel_configure_event(MTRWaylandSurface *surface, struct xdg_toplevel *toplevel, int width, int height, struct wl_array *states);
static void toplevel_close_event(MTRWaylandSurface *surface, struct xdg_toplevel *toplevel);
static void toplevel_configure_bounds_event(MTRWaylandSurface *surface, struct xdg_toplevel *toplevel, int width, int height);
static void toplevel_wmcapabilities_event(MTRWaylandSurface *surface, struct xdg_toplevel *toplevel, struct wl_array *capabilities);
static void decoration_configure_event(MTRWaylandSurface *surface, struct zxdg_toplevel_decoration_v1 *decoration, unsigned mode);

bool _mtr_create_wayland_surface(MTRWaylandSurface *surface, struct wl_compositor *compositor, struct xdg_wm_base *shell, struct zxdg_decoration_manager_v1 *manager) {
    static const struct xdg_wm_base_listener shell_listener = {
        .ping = (void *) shell_ping_event,
    };
    static const struct xdg_surface_listener xdg_listener = {
        .configure = (void *) xdg_configure_event,
    };
    static const struct xdg_toplevel_listener toplevel_listener = {
        .configure = (void *) toplevel_configure_event,
        .close = (void *) toplevel_close_event,
        .configure_bounds = (void *) toplevel_configure_bounds_event,
        .wm_capabilities = (void *) toplevel_wmcapabilities_event,
    };
    static const struct zxdg_toplevel_decoration_v1_listener decoration_listener = {
        .configure = (void *) decoration_configure_event,
    };

    assert(surface != NULL);
    assert(compositor != NULL);
    assert(shell != NULL);
    assert(manager != NULL);

    if (xdg_wm_base_add_listener(shell, &shell_listener, surface) == -1) return _mtr_throw_wayland_error(MTR_WAYLAND_ERROR_ADD_SHELL_LISTENER);

    surface->surface_wl = wl_compositor_create_surface(compositor);
    if (surface->surface_wl == NULL) return _mtr_throw_wayland_error(MTR_WAYLAND_ERROR_CREATE_SURFACE);
    
    surface->initialized = true;

    surface->surface_xdg = xdg_wm_base_get_xdg_surface(shell, surface->surface_wl);
    if (surface->surface_xdg == NULL) {
        _mtr_destroy_wayland_surface(surface);

        return _mtr_throw_wayland_error(MTR_WAYLAND_ERROR_CREATE_XDG_SURFACE);
    }

    if (xdg_surface_add_listener(surface->surface_xdg, &xdg_listener, surface) == -1) {
        _mtr_destroy_wayland_surface(surface);

        return _mtr_throw_wayland_error(MTR_WAYLAND_ERROR_ADD_XDG_SURFACE_LISTENER);
    }

    surface->toplevel = xdg_surface_get_toplevel(surface->surface_xdg);
    if (surface->toplevel == NULL) {
        _mtr_destroy_wayland_surface(surface);

        return _mtr_throw_wayland_error(MTR_WAYLAND_ERROR_CREATE_TOPLEVEL);
    }

    if (xdg_toplevel_add_listener(surface->toplevel, &toplevel_listener, surface) == -1) {
        _mtr_destroy_wayland_surface(surface);

        return _mtr_throw_wayland_error(MTR_WAYLAND_ERROR_ADD_TOPLEVEL_LISTENER);
    }

    xdg_toplevel_set_title(surface->toplevel, "Made To Race");
    xdg_toplevel_set_app_id(surface->toplevel, "Made To Race");

    surface->decoration = zxdg_decoration_manager_v1_get_toplevel_decoration(manager, surface->toplevel);
    if (surface->decoration == NULL) {
        _mtr_destroy_wayland_surface(surface);

        return _mtr_throw_wayland_error(MTR_WAYLAND_ERROR_CREATE_DECORATION);
    }

    if (zxdg_toplevel_decoration_v1_add_listener(surface->decoration, &decoration_listener, surface) == -1) {
        _mtr_destroy_wayland_surface(surface);

        return _mtr_throw_wayland_error(MTR_WAYLAND_ERROR_ADD_DECORATION_LISTENER);
    }

    zxdg_toplevel_decoration_v1_set_mode(surface->decoration, ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE);

    wl_surface_commit(surface->surface_wl);

    return false;
}

void _mtr_destroy_wayland_surface(MTRWaylandSurface *surface) {
    if (surface == NULL || !surface->initialized) return;

    if (surface->decoration != NULL) zxdg_toplevel_decoration_v1_destroy(surface->decoration);
    if (surface->toplevel != NULL) xdg_toplevel_destroy(surface->toplevel);
    if (surface->surface_xdg != NULL) xdg_surface_destroy(surface->surface_xdg);
    if (surface->surface_wl != NULL) wl_surface_destroy(surface->surface_wl);

    surface->initialized = false;
}

static void shell_ping_event(MTRWaylandSurface *surface, struct xdg_wm_base *shell, unsigned serial) {
    xdg_wm_base_pong(shell, serial);
}

static void xdg_configure_event(MTRWaylandSurface *surface, struct xdg_surface *xdg, unsigned serial) {
    xdg_surface_ack_configure(xdg, serial);

    xdg_surface_set_window_geometry(surface->surface_xdg, 0, 0, surface->width, surface->height);
}

// TODO: Check for states
static void toplevel_configure_event(MTRWaylandSurface *surface, struct xdg_toplevel *toplevel, int width, int height, struct wl_array *states) {
    surface->width = (width == 0 ? mtr_min_int(1000, surface->width_bound) : width);
    surface->height = (height == 0 ? mtr_min_int(700, surface->height_bound) : height);
}

static void toplevel_close_event(MTRWaylandSurface *surface, struct xdg_toplevel *toplevel) {
    surface->closed = true;
}

static void toplevel_configure_bounds_event(MTRWaylandSurface *surface, struct xdg_toplevel *toplevel, int width, int height) {
    surface->width_bound = (width == 0 ? -1 : width);
    surface->height_bound = (height == 0 ? -1 : height);
}

// TODO: Check for capabilities
static void toplevel_wmcapabilities_event(MTRWaylandSurface *surface, struct xdg_toplevel *toplevel, struct wl_array *capabilities) {

}

static void decoration_configure_event(MTRWaylandSurface *surface, struct zxdg_toplevel_decoration_v1 *decoration, unsigned mode) {
    if (mode == ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE) {
        surface->invalid_state = true;

        _mtr_throw_wayland_error(MTR_WAYLAND_ERROR_EXPECTED_SERVER_SIDE_DECORATION);
    }
}
