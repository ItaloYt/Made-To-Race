#ifndef MTR_GRAPHIC_H
#define MTR_GRAPHIC_H

#include <stdbool.h>

#include "window.h"
#include "math.h"

typedef struct MTRGraphic *MTRGraphic;
typedef struct MTRGraphicObject *MTRGraphicObject;

enum MTRGraphicError {
    MTR_GRAPHIC_ERROR_ALLOCATE = 0x01,
    MTR_GRAPHIC_ERROR_IMPLEMENTATION = 0x02,
    MTR_GRAPHIC_ERROR_ON_RENDER_CALLBACK = 0x03,
};
typedef enum MTRGraphicError MTRGraphicError;

struct MTRBaseGraphicObjectProperty {
    
};
typedef struct MTRBaseGraphicObjectProperty MTRBaseGraphicObjectProperty;

typedef bool (*MTRGraphicOnRenderCallback)(void *user, MTRGraphic graphic);

bool mtr_create_graphic(MTRGraphic *graphic, MTRWindow window);
void mtr_destroy_graphic(MTRGraphic *graphic);

bool mtr_render_graphic(MTRGraphic graphic, MTRGraphicOnRenderCallback callback, void *user);

bool mtr_create_base_graphic_object(MTRGraphicObject *out, MTRGraphic graphic, const MTRVector3 *mesh, const unsigned *indices, unsigned mesh_length, unsigned indices_length);
bool mtr_draw_base_graphic_object(MTRGraphicObject object, MTRGraphic graphic);
void mtr_destroy_base_graphic_object(MTRGraphicObject *object, MTRGraphic graphic);

MTRBaseGraphicObjectProperty *mtr_get_base_graphic_object_properties(MTRGraphicObject object);
void mtr_update_graphic_object(MTRGraphicObject object);

const char *_mtr_convert_graphic_error_to_string(MTRGraphicError code);
bool _mtr_throw_graphic_error(MTRGraphicError code);

#endif
