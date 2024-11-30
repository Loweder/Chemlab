#include "math.h"
#include <math.h>
#include <stdlib.h>

vec2_t c_add(vec2_t a, vec2_t b) {
  return (vec2_t) {a.x + b.x, a.y + b.y};
}
vec2_t c_sub(vec2_t a, vec2_t b) {
  return (vec2_t) {a.x - b.x, a.y - b.y};
}
vec2_t c_mul(vec2_t a, vec2_t b) {
  vec2_t result = {0, 0};
  result.x = (a.x * b.x) - (a.y * b.y);
  result.y = (a.x * b.y) + (a.y * b.x);
  return result;
}
vec2_t c_div(vec2_t a, vec2_t b) {
  vec2_t result = {0, 0};
  float divisor = (b.x * b.x) + (b.y * b.y);
  result.x = ((a.x*b.x)+(a.y*b.y))/divisor;
  result.y = ((a.y*b.x)-(a.x*b.y))/divisor;
  return result;
}
vec2_t c_pow(vec2_t a, vec2_t b) {
  float r = sqrt((a.x*a.x)+(a.y*a.y));
  float p = atan(a.y/a.x);
  vec2_t i1 = {pow(r, b.x) * exp(-b.y*p), 0 };
  vec2_t pr = {cos(b.y * log(r) + (b.x * p)), sin(b.y * log(r) + (b.x * p))};
  return c_mul(i1, pr);
}

vec3_t v_add(vec3_t a, vec3_t b) {
  return (vec3_t) {a.x + b.x, a.y + b.y, a.z + b.z};
}
vec3_t v_sub(vec3_t a, vec3_t b) {
  return (vec3_t) {a.x - b.x, a.y - b.y, a.z - b.z};
}
vec3_t v_cross(vec3_t a, vec3_t b) {
  vec3_t result = {0, 0, 0};
  result.x = (a.y * b.z) - (a.z * b.y);
  result.y = (a.z * b.x) - (a.x * b.z);
  result.z = (a.x * b.y) - (a.y * b.x);
  return result;
}
vec3_t v_scale(vec3_t v, float scalar) {
  return (vec3_t) {v.x * scalar, v.y * scalar, v.z * scalar};
}
float v_dot(vec3_t a, vec3_t b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}
vec4_t v_to4d(vec3_t v) {
  vec4_t result = {v.x, v.y, v.z, 1};
  return result;
}
vec3_t v_to3d(vec4_t v) {
  vec3_t result = {v.x / v.w, v.y / v.w, v.z / v.w};
  return result;
}
vec3_t v_norm(vec3_t v) {
  vec3_t result = {0, 0, 0};
  float magnitude = sqrt(v_dot(v, v));
  result.x = v.x/magnitude;
  result.y = v.y/magnitude;
  result.z = v.z/magnitude;
  return result;
}

vec4_t m_apply(mat4_t a, vec4_t vector) {
  float *v = (float*) &vector;
  vec4_t res = {0};
  float *result = (float*) &res;
  for (int x = 0; x < 4; x++) {
      float res = 0;
      for (int j = 0; j < 4; j++) {
	res += a.dim2[x][j] * v[j];
      }
      result[x] = res;
  }
  return res;
}
mat4_t m_mul(mat4_t a, mat4_t b) {
  mat4_t result = {0};
  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      float res = 0;
      for (int j = 0; j < 4; j++) {
	res += a.dim2[x][j] * b.dim2[j][y];
      }
      result.dim2[x][y] = res;
    }
  }
  return result;
}
mat4_t m_lookAt(vec3_t front, vec3_t world_up, vec3_t pos) {
  vec3_t right = v_norm(v_cross(front, world_up));
  vec3_t up = v_norm(v_cross(right, front));
  return (mat4_t) {.formed = {
    {right.x, right.y, right.z, -v_dot(pos, right)}, 
    {up.x,    up.y,    up.z, 	-v_dot(pos, up)}, 
    {-front.x, -front.y, -front.z, v_dot(pos, front)}, 
    {0, 0, 0, 1}
  }};
  /*return (mat4_t) {.formed = {*/
  /*  {right.x, up.x, front.x, pos.x}, */
  /*  {right.y, up.y, front.y, pos.y}, */
  /*  {right.z, up.z, front.z, pos.z}, */
  /*  {0, 0, 0, 1}*/
  /*}};*/
}
mat4_t m_rotate(vec3_t around, float angle) {
  float s = sin(angle), c = cos(angle), a = 1-c;
  float x = around.x, y = around.y, z = around.z;
  return (mat4_t) {.formed = {
    {c+(x*x*a), (x*y*a)-(z*s), (x*z*a)+(y*s), 0}, 
    {(y*x*a)+(z*s), c+(y*y*a), (y*z*a)-(x*s), 0}, 
    {(z*x*a)-(y*s), (z*y*a)+(x*s), c+(z*z*a), 0}, 
    {0, 0, 0, 1}
  }};
}
mat4_t m_perspective(float fov, float aspect, float near, float far) {
  float t = tan(fov/2);
  return (mat4_t) {.formed = {
    {1/(aspect*t), 0, 0, 0},
    {0, 1/t, 0, 0},
    {0, 0, -((far+near)/(far-near)), -((2*far*near)/(far-near))},
    {0, 0, -1, 0}
  }};
}
mat4_t m_orthogonal(float height, float aspect, float near, float far) {
  float right = -(height * aspect)/2, left = (height * aspect)/2, top = height/2, bottom = -height/2;
  return (mat4_t) {.formed = {
    {2/(right-left), 0, 0, -(right+left)/(right-left)},
    {0, 2/(top-bottom), 0, -(top+bottom)/(top-bottom)},
    {0, 0, -2/(far-near), (far+near)/(far-near)},
    {0, 0, 0, 1}
  }};
}

static inline vec4_t in_mesh_vec(float x, float y, float z, float t, float as, float bs, enum axis in) {
  switch (in) {
  case AXIS_X:
    return (vec4_t) {x, y + as, z + bs, t};
  case AXIS_Y:
    return (vec4_t) {x + as, y, z + bs, t};
  case AXIS_Z:
    return (vec4_t) {x + as, y + bs, z, t};
  }
}
static inline vec4_t out_mesh_vec(float x, float y, float z, vec2_t v, float as, float bs, enum axis out) {
  switch (out) {
  case AXIS_X:
    return (vec4_t) {v.x, y + as, z + bs, v.y};
  case AXIS_Y:
    return (vec4_t) {x + as, v.x, z + bs, v.y};
  case AXIS_Z:
    return (vec4_t) {x + as, y + bs, v.x, v.y};
  }
}

void function_mesh(float x, float y, float z, float t,
    float astep, float bstep, uint64_t acount, uint64_t bcount, 
    enum axis in, enum axis out, wave function, vec4_t *data) {
  for (uint64_t a = 0; a < acount; a++) {
    for (uint64_t b = 0; b < bcount; b++) {
      vec4_t position = in_mesh_vec(x, y, z, t, a*astep, b*bstep, in);
      vec2_t product = function(position);
      data[a*bcount + b] = out_mesh_vec(x, y, z, product, a*astep, b*bstep, out);
    }
  }
}

