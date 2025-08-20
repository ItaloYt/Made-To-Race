#include <stdio.h>

#include "game/window.h"
#include "game/graphic.h"

struct MTRGame {
    MTRWindow window;
    MTRGraphic graphic;
    MTRGraphicObject object;
};
typedef struct MTRGame MTRGame;

static bool on_render(MTRGame *game, MTRGraphic graphic);

int main() {
    MTRGame game = {0};
    if (mtr_create_window(&game.window)) return 1;

    if (mtr_create_graphic(&game.graphic, game.window)) {
        mtr_destroy_window(&game.window);

        return 1;
    }

    if (mtr_create_base_graphic_object(
        &game.object, 
        game.graphic, 
        (MTRVector3[]) {
            (MTRVector3) { -0.5, -0.5, 0 },
            (MTRVector3) { 0.5, -0.5, 0 },
            (MTRVector3) { 0.5, 0.5, 0 },
            (MTRVector3) { -0.5, 0.5, 0 },
        }, 
        (unsigned[]) {
            0, 1, 2,
            2, 3, 0,
        },
        4,
        6
    )) {
        mtr_destroy_graphic(&game.graphic);
        mtr_destroy_window(&game.window);

        return 1;
    }

    while (!mtr_is_window_closed(game.window)) {
        if (mtr_update_window(game.window)) break;
        if (mtr_render_graphic(game.graphic, (void *) on_render, &game)) break;
    }

    mtr_destroy_base_graphic_object(&game.object, game.graphic);
    mtr_destroy_graphic(&game.graphic);
    mtr_destroy_window(&game.window);
}

static bool on_render(MTRGame *game, MTRGraphic graphic) {
    return mtr_draw_base_graphic_object(game->object, graphic);
}
