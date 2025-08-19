#ifndef MTR_GRAPHIC_H
#define MTR_GRAPHIC_H

#include <stdbool.h>

#include "window.h"

typedef struct MTRGraphic *MTRGraphic;
typedef struct MTRGraphicObject *MTRGraphicObject;

enum MTRGraphicError {
    MTR_GRAPHIC_ERROR_ALLOCATION_FAILED = 0x01,
    MTR_GRAPHIC_ERROR_IMPLEMENTATION_FAILED = 0x02,
};
typedef enum MTRGraphicError MTRGraphicError;

enum MTRGraphicObjectType {
    MTR_GRAPHIC_OBJECT_TYPE_BASE = 0x01,
};
typedef enum MTRGraphicObjectType MTRGraphicObjectType;

bool mtr_create_graphic(MTRGraphic *graphic, MTRWindow window);
bool mtr_create_graphic_object(MTRGraphicObject *object, MTRGraphic graphic, MTRGraphicObjectType type);
bool mtr_render_graphic(MTRGraphic graphic);
void mtr_destroy_graphic(MTRGraphic *graphic);

// Always returns true
const char *mtr_convert_graphic_error_to_string(MTRGraphicError code);
bool mtr_throw_graphic_error(MTRGraphicError code);

#endif
