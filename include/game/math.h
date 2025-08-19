#ifndef MTR_MATH_H
#define MTR_MATH_H

struct MTRVector3 {
    float x, y, z;
};
typedef struct MTRVector3 MTRVector3;

struct MTRQuaternion {
    float x, y, z, w;
};
typedef struct MTRQuaternion MTRQuaternion;

static inline int mtr_max_int(int a, int b) { return (a > b ? a : b); }
static inline int mtr_min_int(int a, int b) { return (a < b ? a : b); }
static inline int mtr_clamp_int(int value, int min, int max) { 
    return mtr_max_int(mtr_min_int(value, max), min);
}

#endif
