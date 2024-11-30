#ifndef CHEMLAB_MATH
#define CHEMLAB_MATH

#include <stdint.h>

enum axis {
  AXIS_X,
  AXIS_Y,
  AXIS_Z
};

typedef struct {
  float x;
  float y;
} vec2_t;

typedef struct {
  float x;
  float y;
  float z;
} vec3_t;

typedef struct {
  float x;
  float y;
  float z;
  float w;
} vec4_t;

typedef union {
  float dim1[16];
  float dim2[4][4];
  struct {
    float a[4];
    float b[4];
    float c[4];
    float d[4];
  } formed;
} mat4_t;

vec2_t c_add(vec2_t a, vec2_t b);
vec2_t c_sub(vec2_t a, vec2_t b);
vec2_t c_mul(vec2_t a, vec2_t b);
vec2_t c_div(vec2_t a, vec2_t b);
vec2_t c_pow(vec2_t a, vec2_t b);

//TODO complex POW()
vec3_t v_add(vec3_t a, vec3_t b);
vec3_t v_sub(vec3_t a, vec3_t b);
vec3_t v_cross(vec3_t a, vec3_t b);
vec3_t v_scale(vec3_t v, float scalar);
float v_dot(vec3_t a, vec3_t b);
vec4_t v_to4d(vec3_t v);
vec3_t v_to3d(vec4_t v);
vec3_t v_norm(vec3_t v);

vec4_t m_apply(mat4_t a, vec4_t vector);
mat4_t m_mul(mat4_t a, mat4_t b);
mat4_t m_lookAt(vec3_t front, vec3_t up, vec3_t pos);
mat4_t m_rotate(vec3_t around, float angle);
mat4_t m_perspective(float fov, float aspect, float near, float far);
mat4_t m_orthogonal(float height, float aspect, float near, float far);

typedef vec2_t (*wave)(vec4_t pos);
void function_mesh(float xfrom, float yfrom, float zfrom, float time,
    float astep, float bstep, uint64_t acount, uint64_t bcount, 
    enum axis in, enum axis out, wave function, vec4_t *data);

#endif
