#include "game/window.h"
#include "game/graphic.h"

int main() {
    MTRWindow window;
    if (mtr_create_window(&window)) return 1;

    MTRGraphic graphic;
    if (mtr_create_graphic(&graphic, window)) {
        mtr_destroy_window(&window);

        return 1;
    }

    while (!mtr_is_window_closed(window)) {
        if (mtr_update_window(window)) break;

        if (mtr_render_graphic(graphic)) break;
    }

    mtr_destroy_graphic(&graphic);
    mtr_destroy_window(&window);
}
