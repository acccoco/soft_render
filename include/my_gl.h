
#ifndef MY_TINY_RENDER_MY_GL_H
#define MY_TINY_RENDER_MY_GL_H

#include "geometry.h"
#include "shader.h"
#include "tgaimage.h"

typedef uint8_t z_buffer_t;
const z_buffer_t Z_BUFFER_MAX = 255;
const z_buffer_t Z_BUFFER_MIN = 0;

mat<4, 4> lookat(const vec3 &eye, const vec3 &target, const vec3 &up);

mat<4, 4> projection(int width, int height, int near, int far);

void view_port(int x_offset, int y_offset, int width, int height);

void triangle(TGAImage &image, std::vector<std::vector<z_buffer_t>> &z_buffer, Shader &shader, const std::vector<Location> &locations);


#endif //MY_TINY_RENDER_MY_GL_H
