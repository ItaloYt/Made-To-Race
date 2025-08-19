#include <stdbool.h>
#include <stdio.h>

#include "game/graphic.h"

bool mtr_throw_graphic_error(MTRGraphicError code) {
    (void) fprintf(stderr, "Graphic Error 0x%02x - %s\n", code, mtr_convert_graphic_error_to_string(code));

    return true;
}

const char *mtr_convert_graphic_error_to_string(MTRGraphicError code) {
    switch (code) {
        case MTR_GRAPHIC_ERROR_ALLOCATION_FAILED:
        return "MTR_GRAPHIC_ERROR_ALLOCATION_FAILED";

        case MTR_GRAPHIC_ERROR_IMPLEMENTATION_FAILED:
        return "MTR_GRAPHIC_ERROR_IMPLEMENTATION_FAILED";
    }

    return "NULL(This is an error)";
}
