#ifndef MTR_WINDOW_H
#define MTR_WINDOW_H

#include <stdbool.h>

typedef struct MTRWindow *MTRWindow;

enum MTRWindowError {
    MTR_WINDOW_ERROR_ALLOCATION_FAILED = 0x01,
    MTR_WINDOW_ERROR_IMPLEMENTATION = 0x02,
};
typedef enum MTRWindowError MTRWindowError;

bool mtr_create_window(MTRWindow *window);
bool mtr_update_window(MTRWindow window);
void mtr_destroy_window(MTRWindow *window);

unsigned mtr_get_window_width(MTRWindow window);
unsigned mtr_get_window_height(MTRWindow window);

bool mtr_is_window_closed(MTRWindow window);

const char *_mtr_convert_window_error_to_string(MTRWindowError code);
bool _mtr_throw_window_error(MTRWindowError code);

#endif
