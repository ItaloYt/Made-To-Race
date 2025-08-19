#include <wayland-client-core.h>
#include <wayland-client.h>

#include <assert.h>
#include <stdbool.h>

#include "backend/window/wayland/wayland.h"

bool _mtr_connect_wayland_display(struct wl_display **out) {
    assert(out != NULL);

    *out = wl_display_connect(NULL);

    if (*out == NULL) return _mtr_throw_wayland_error(MTR_WAYLAND_ERROR_CONNECT_DISPLAY);

    return false;
}

void _mtr_disconnect_wayland_display(struct wl_display *display) {
    if (display == NULL) return;

    wl_display_disconnect(display);
}

bool _mtr_roundtrip_wayland_display(struct wl_display *display) {
    assert(display != NULL);

    if (wl_display_roundtrip(display) == -1) return _mtr_throw_wayland_error(MTR_WAYLAND_ERROR_ROUNDTRIP_DISPLAY);

    return false;
}
