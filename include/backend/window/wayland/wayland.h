#ifndef MTR_WINDOW_WAYLAND_H
#define MTR_WINDOW_WAYLAND_H

#include <wayland-client.h>

#include <stdbool.h>

struct MTRWaylandRegistry {
    struct wl_registry *registry;
    struct wl_compositor *compositor;
    struct xdg_wm_base *shell;
    struct zxdg_decoration_manager_v1 *manager;
    bool initialized;
};
typedef struct MTRWaylandRegistry MTRWaylandRegistry;

struct MTRWaylandSurface {
    struct wl_surface *surface_wl;
    struct xdg_surface *surface_xdg;
    struct xdg_toplevel *toplevel;
    struct zxdg_toplevel_decoration_v1 *decoration;

    unsigned width, width_bound;
    unsigned height, height_bound;
    bool closed;
    bool invalid_state;
    bool initialized;
};
typedef struct MTRWaylandSurface MTRWaylandSurface;

struct MTRWindow {
    struct wl_display *display;
    MTRWaylandRegistry registry;
    MTRWaylandSurface surface;
};

enum MTRWaylandError {
    MTR_WAYLAND_ERROR_CONNECT_DISPLAY = 0x01,
    MTR_WAYLAND_ERROR_GET_REGISTRY = 0x02,
    MTR_WAYLAND_ERROR_ALLOCATE = 0x03,
    MTR_WAYLAND_ERROR_ADD_REGISTRY_LISTENER = 0x04,
    MTR_WAYLAND_ERROR_ROUNDTRIP_DISPLAY = 0x05,
    MTR_WAYLAND_ERROR_BIND_COMPOSITOR = 0x06,
    MTR_WAYLAND_ERROR_BIND_XDG_SHELL = 0x07,
    MTR_WAYLAND_ERROR_BIND_XDG_DECORATION = 0x08,
    MTR_WAYLAND_ERROR_CREATE_SURFACE = 0x09,
    MTR_WAYLAND_ERROR_CREATE_XDG_SURFACE = 0x0a,
    MTR_WAYLAND_ERROR_CREATE_TOPLEVEL = 0x0b,
    MTR_WAYLAND_ERROR_CREATE_DECORATION = 0x0c,
    MTR_WAYLAND_ERROR_ADD_SHELL_LISTENER = 0x0d,
    MTR_WAYLAND_ERROR_ADD_XDG_SURFACE_LISTENER = 0x0e,
    MTR_WAYLAND_ERROR_ADD_TOPLEVEL_LISTENER = 0x0f,
    MTR_WAYLAND_ERROR_ADD_DECORATION_LISTENER = 0x10,
    MTR_WAYLAND_ERROR_EXPECTED_SERVER_SIDE_DECORATION = 0x11,
};
typedef enum MTRWaylandError MTRWaylandError;

bool _mtr_connect_wayland_display(struct wl_display **display);
void _mtr_disconnect_wayland_display(struct wl_display *display);
bool _mtr_roundtrip_wayland_display(struct wl_display *display);

bool _mtr_create_wayland_registry(MTRWaylandRegistry *out, struct wl_display *display);
void _mtr_destroy_wayland_registry(MTRWaylandRegistry *registry);
bool _mtr_validate_wayland_registry(MTRWaylandRegistry *registry);

bool _mtr_create_wayland_surface(MTRWaylandSurface *surface, struct wl_compositor *compositor, struct xdg_wm_base *shell, struct zxdg_decoration_manager_v1 *manager);
void _mtr_destroy_wayland_surface(MTRWaylandSurface *surface);

const char *_mtr_convert_wayland_error_to_string(MTRWaylandError code);
bool _mtr_throw_wayland_error(MTRWaylandError code);

#endif
