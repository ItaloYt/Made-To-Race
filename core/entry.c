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

    mtr_destroy_graphic(&graphic);
    mtr_destroy_window(&window);
}
