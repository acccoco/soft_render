
#include <cassert>
#include <cmath>
#include <vector>
#include "2d.h"

using namespace std;

/*  绘制直线：取整版本 */
void line_int(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor &color) {
    // 确保 x0 - x1 是最长的
    bool is_tranpose = false;
    if (abs(x0 - x1) < abs(y0 - y1)) {
        is_tranpose = true;
        swap(x0, y0);
        swap(x1, y1);
    }

    // 确保 x0 <= x1
    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }

    /* 绘制：直接取整数版本 */
    for (int x = x0; x <= x1; ++x) {
        float t = (x - x0) / float(x1 - x0);
        int y = y0 + t * (y1 - y0);
        if (is_tranpose) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
    }
}


/* 绘制直线：四舍五入版本 */
void line_round(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor &color) {
    // 确保 x0 - x1 是最长的
    bool is_tranpose = false;
    if (abs(x0 - x1) < abs(y0 - y1)) {
        is_tranpose = true;
        swap(x0, y0);
        swap(x1, y1);
    }

    // 确保 x0 <= x1
    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }


    /* 绘制：四舍五入版本 */
#if flase
    int dx = x1 - x0;
    int dy = y1 - y0;
    float d_error = abs(dy / float(dx));
    float error = 0;
    int y = y0;

    for (int x = x0; x <= x1; ++x) {
        if (is_tranpose)
            image.set(y, x, color);
        else
            image.set(x, y, color);
        error += d_error;
        if (error > 0.5f) {
            y += (y1 > y0 ? 1 : -1);
            error -= 1;
        }
    }
#endif

    /* 绘制：四舍五入优化版本 */
#if true
    int dx = x1 - x0;
    int dy = y1 - y0;
    // d_error 是理解的关键，原版是 dy/dx，为了消除浮点数才出现这个值的
    int d_error = abs(dy) * 2;
    int error = 0;
    int y = y0;
    const int y_incr = (y1 > y0 ? 1 : -1);

    for (int x = x0; x <= x1; ++x) {
        if (is_tranpose)
            image.set(y, x, color);
        else
            image.set(x, y, color);
        error += d_error;
        if (error >= dx) {
            y += y_incr;
            error -= dx * 2;
        }
    }
#endif
}


/* 绘制水平线，不关心 x1 和 x2 的大小 */
void horizon_line(int x1, int x2, int y, TGAImage &image, TGAColor &color) {
    assert(x1 > 0);
    assert(x2 > 0);

    // 确保 x1 <= x2
    if (x1 > x2)
        swap(x1, x2);

    // 绘制
    for (int x = x1; x <= x2; ++x)
        image.set(x, y, color);
}


/* 直线与 y = "y" 的交点 */
Vec2i intersection(Vec2i v1, Vec2i v2, int y) {
    assert(v1.y != v2.y);           // 除零异常

    float t = abs(float(y - v1.y)) / abs(v1.y - v2.y);
    return v1 + (v2 - v1) * t;
}

void triangle_line_sweep(Vec2i v1, Vec2i v2, Vec2i v3, TGAImage &image, TGAColor &color) {
    // 三角形必须具有实体
    if (v1.y == v2.y && v2.y == v3.y) return;
    if (v1.x == v2.x && v2.x == v3.x) return;

    /* 排序 y 坐标， v1.y < v2.y < v3.y */
    if (v1.y > v2.y) swap(v1, v2);
    if (v1.y > v3.y) swap(v1, v3);
    if (v2.y > v3.y) swap(v2, v3);

    /* 上半部分：y ∈ (v2.y, v3.y] */
    for (int y = v2.y; y <= v3.y; ++y) {
        int x_1 = intersection(v3, v1, y).x;
        int x_2 = intersection(v3, v2, y).x;

        horizon_line(x_1, x_2, y, image, color);
    }

    /* 中间部分：y = v2.y */
    int _x;
    if (v1.y == v2.y) _x = v1.x;
    else if (v2.y == v3.y) _x = v3.x;
    else _x = intersection(v1, v3, v2.y).x;
    horizon_line(v2.x, _x, v2.y, image, color);

    /* 下半部分：y ∈ [v1.y, v2.y) */
    for (int y = v1.y; y < v2.y; ++y) {
        int x_1 = intersection(v1, v2, y).x;
        int x_2 = intersection(v1, v3, y).x;

        horizon_line(x_1, x_2, y, image, color);
    }
}


/* 判断一个点是否在三角形里面，或者三角形的边上 */
bool is_in_triangle(Vec2i A, Vec2i B, Vec2i C, Vec2i P) {
    Vec3i v1(C.x - A.x, B.x - A.x, A.x - P.x);
    Vec3i v2(C.y - A.y, B.y - A.y, A.y - P.y);
    Vec3i v = cross(v1, v2);
    /* 由叉积运算可知，只有当 AB 和 AC 是平行的时候，v[2] 才会是0 */
    if (v.z == 0)
        return false;

    /* u 和 v 范围在 [0, 1]，说明点在三角形内 */
    if (v.x / float(v.z) < 0 || v.y / float(v.z) < 0 || 1.f - (v.x + v.y) / float(v.z) < 0)
        return false;
    return true;
}


/* 设直线 (P.x, P.y, ?) 和三角形 ABC 有交点为 P
 * 并给出交点的向量表示
 *      AP = u(AB) + v(AC)
 *      即：P = (1 - u - v)A + uB + vC
 *      即：P = [u, v, (1- u - v)] [A, B, C]T
 */
Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3i P) {
    Vec3f vx(C.x - A.x, B.x - A.x, A.x - P.x);
    Vec3f vy(C.y - A.y, B.y - A.y, A.y - P.y);
    Vec3f v = cross(vx, vy);

    // 当 A B C 在一条直线上时，v[2] 是0
    if (abs(v.z) < 1e-2)
        return {-1, 1, 1};
    return {1.f - v.x / v.z - v.y / v.z, v.x / v.z, v.y / v.z};
}

void triangle_z_buffer(Vec3f A, Vec3f B, Vec3f C,
                       vector<vector<float>> &z_buffer, TGAImage &image, const TGAColor & color) {
    assert(image.get_height() == z_buffer.size());
    assert(image.get_width() == z_buffer[0].size());

    // 寻找三角形的矩形边界，注意不能超过画布的边界
    Vec2i canvas(image.get_width() - 1, image.get_height() - 1);
    Vec2f boundry_min(canvas.x, canvas.y);
    Vec2f boundry_max(0, 0);
    for (auto v : vector<Vec3f>({A, B, C})) {
        boundry_min.x = max(0.f, min(boundry_min.x, v.x));
        boundry_min.y = max(0.f, min(boundry_min.y, v.y));
        boundry_max.x = min(float(canvas.x), max(boundry_max.x, v.x));
        boundry_max.y = min(float(canvas.y), max(boundry_max.y, v.y));
    }

    // 对于矩形边界内的每一个点，判断是否在三角形内，并绘制
    Vec3i P;
    for (P.x = boundry_min.x; P.x <= boundry_max.x; ++P.x) {
        for (P.y = boundry_min.y; P.y <= boundry_max.y; ++P.y) {
            auto bary = barycentric(A, B, C, P);

            // 判断是否在三角形内
            if (bary.x < 0 || bary.y < 0 || bary.z < 0)
                continue;

            // z-buffer测试
            float depth = 0;
            for (auto v : vector<Vec3f>({A, B, C}))     // 计算 P 点的 z 值
                depth += v.z;
            if (z_buffer[P.y][P.x] < depth) {     // 摄像机朝向 -z 方向，所以 z 大的会覆盖 z 小的点
                z_buffer[P.y][P.x] = depth;
                image.set(P.x, P.y, color);
            }
        }
    }
}


// 由于存在浮点误差，边界上的某些点可能无法被正确渲染
void triangle_barycentric(Vec2i v1, Vec2i v2, Vec2i v3, TGAImage &image, const TGAColor &color) {
    // 寻找三角形的矩形边界，注意不能超过画布的边界
    Vec2i canvas(image.get_width() - 1, image.get_height() - 1);
    Vec2i boundry_min(canvas.x, canvas.y);
    Vec2i boundry_max(0, 0);
    for (auto v : vector<Vec2i>({v1, v2, v3})) {
        boundry_min.x = max(0, min(boundry_min.x, v.x));
        boundry_min.y = max(0, min(boundry_min.y, v.y));
        boundry_max.x = min(canvas.x, max(boundry_max.x, v.x));
        boundry_max.y = min(canvas.y, max(boundry_max.y, v.y));
    }

    // 对于矩形边界内的每一个点，判断是否在三角形内，并绘制
    Vec2i P;
    for (P.x = boundry_min.x; P.x <= boundry_max.x; ++P.x) {
        for (P.y = boundry_min.y; P.y <= boundry_max.y; ++P.y) {
            if (!is_in_triangle(v1, v2, v3, P))
                continue;
            image.set(P.x, P.y, color);
        }
    }
}
