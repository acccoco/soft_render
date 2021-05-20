
#ifndef RENDER_TRANSFORM_H
#define RENDER_TRANSFORM_H

#include "geometry.h"

/* 生成位移矩阵 */
inline mat<4, 4> translation(double x, double y, double z) {
    mat<4, 4> m = mat<4, 4>::identity();

    m[0][3] = x;
    m[1][3] = y;
    m[2][3] = z;

    return m;
}

/* 生产均匀缩放矩阵 */
inline mat<4, 4> scaling(double s) {
    assert(s > 0);

    mat<4, 4> m = mat<4, 4>::identity();

    m[0][0] = s;
    m[1][1] = s;
    m[2][2] = s;

    return m;
}


/* 计算法线 */
inline vec3 get_normal(const vec3 &A, const vec3 &B, const vec3 &C) {
    return cross(B - A, C - A).normalize();
}

/* 绕 x 轴旋转的角度，通过右手定则判断旋转正方向 */
inline mat<4, 4> rotate_x(double theta) {
    mat<4, 4> r;
    theta = theta * M_PI / 180;     // 角度转化为弧度
    r[0] = embed<4>(vec3(1, 0, 0), 0);
    r[1] = embed<4>(vec3(0, cos(theta), -sin(theta)), 0);
    r[2] = embed<4>(vec3(0, sin(theta), cos(theta)), 0);
    r[3] = embed<4>(vec3(0, 0, 0), 1);
    return r;
}

inline mat<4, 4> rotate_y(double theta) {
    mat<4, 4> r;
    theta = theta * M_PI / 180;
    r[0] = embed<4>(vec3(cos(theta), 0, sin(theta)), 0);
    r[1] = embed<4>(vec3(0, 1, 0), 0);
    r[2] = embed<4>(vec3(-sin(theta), 0, cos(theta)), 0);
    r[3] = embed<4>(vec3(0, 0, 0), 1);
    return r;
}

inline mat<4, 4> rotate_z(double theta) {
    mat<4, 4> r;
    theta = theta * M_PI / 180;
    r[0] = embed<4>(vec3(cos(theta), -sin(theta), 0), 0);
    r[1] = embed<4>(vec3(sin(theta), cos(theta), 0), 0);
    r[2] = embed<4>(vec3(0, 0, 1), 0);
    r[3] = embed<4>(vec3(0, 0, 0), 1);
    return r;
}

#endif //RENDER_TRANSFORM_H
