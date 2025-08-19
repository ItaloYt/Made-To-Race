#include <stdio.h>

#include "game/window.h"

bool _mtr_throw_window_error(MTRWindowError code) {
    (void) fprintf(stderr, "Window Error 0x%02x - %s\n", code, _mtr_convert_window_error_to_string(code));

    return true;
}

const char *_mtr_convert_window_error_to_string(MTRWindowError code) {
    switch (code) {
        case MTR_WINDOW_ERROR_ALLOCATION_FAILED:
        return "MTR_WINDOW_ERROR_ALLOCATION_FAILED";
            
        case MTR_WINDOW_ERROR_IMPLEMENTATION:
        return "MTR_WINDOW_ERROR_IMPLEMENTATION";
    }

    return "NULL(This is an error)";
}

