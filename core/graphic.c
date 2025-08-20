#include <stdbool.h>
#include <stdio.h>

#include "game/graphic.h"

bool _mtr_throw_graphic_error(MTRGraphicError code) {
    (void) fprintf(stderr, "Graphic Error 0x%02x - %s\n", code, _mtr_convert_graphic_error_to_string(code));

    return true;
}

const char *_mtr_convert_graphic_error_to_string(MTRGraphicError code) {
    switch (code) {
        case MTR_GRAPHIC_ERROR_ALLOCATE:
        return "MTR_GRAPHIC_ERROR_ALLOCATE";

        case MTR_GRAPHIC_ERROR_IMPLEMENTATION:
        return "MTR_GRAPHIC_ERROR_IMPLEMENTATION";
            
        case MTR_GRAPHIC_ERROR_ON_RENDER_CALLBACK:
        return "MTR_GRAPHIC_ERROR_ON_RENDER_CALLBACK";
    }

    return "NULL(This is an error)";
}
