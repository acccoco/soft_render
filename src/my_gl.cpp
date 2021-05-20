
#include "my_gl.h"
#include <cassert>

using namespace std;

int view_port_x_offset;
int view_port_y_offset;
int view_port_width;
int view_port_height;


mat<4, 4> lookat(const vec3 &eye, const vec3 &target, const vec3 &up) {
    vec3 k_ = (eye - target).normalize();
    vec3 i_ = cross(up, k_).normalize();
    vec3 j_ = cross(k_, i_).normalize();

    mat<4, 4> m_inv = mat<4, 4>::identity();
    mat<4, 4> tr = mat<4, 4>::identity();

    for (int i = 0; i < 3; ++i) {
        m_inv[0][i] = i_[i];
        m_inv[1][i] = j_[i];
        m_inv[2][i] = k_[i];
        tr[i][3] = -eye[i];
    }

    return m_inv * tr;
}

mat<4, 4> projection(int width, int height, int near, int far) {
    assert(near > 0 && far > 0 && far > near);
    assert(width > 0 && height > 0);

    mat<4, 4> proj = mat<4, 4>::identity();

    double r = double(width) / 2;
    double t = double(height) / 2;

    proj[0][0] = -near / r;
    proj[1][1] = -near / t;
    proj[2][2] = double(far + near) / double(far - near);
    proj[2][3] = 2 * far * near / double(far - near);
    proj[3][2] = 1;
    proj[3][3] = 0;

    return proj;
}

void view_port(int x_offset, int y_offset, int width, int height) {
    assert(width > 0 && height > 0);

    view_port_x_offset = x_offset;
    view_port_y_offset = y_offset;
    view_port_width = width;
    view_port_height = height;
}

/* 重心坐标插值的参数 */
vec3 barycentric(const vec2 &A, const vec2 &B, const vec2 &C, const vec2 &P) {
    mat<3, 3> abc;
    abc[0] = embed<3>(A);
    abc[1] = embed<3>(B);
    abc[2] = embed<3>(C);

    if (abs(abc.det()) < 1e-3)
        return {-1, 1, 1};
    return abc.invert_transpose() * embed<3>(P);
}


/* 绘制三角形，接受世界坐标系的点 */
void triangle(TGAImage &image, vector<vector<z_buffer_t>> &z_buffer, Shader &shader, const vector<Location> &locations) {
    vec3 screen_poss[3];

    // 调用顶点着色器
    vec4 temp_vec4;
    vec3 temp_vec3;
    for (int i = 0; i < 3; ++i) {
        temp_vec4 = shader.vertex(locations[i], i);
        if (temp_vec4[3] == 0) return;

        // 透视除法：标准化设备坐标
        temp_vec3 = proj<3>(temp_vec4 / temp_vec4[3]);

        // 屏幕坐标
        temp_vec3.x = (temp_vec3.x + 1) * view_port_width / 2 + view_port_x_offset;
        temp_vec3.y = (temp_vec3.y + 1) * view_port_height / 2 + view_port_y_offset;
        temp_vec3.z = (temp_vec3.z + 1) * (Z_BUFFER_MAX - Z_BUFFER_MIN) / 2 + Z_BUFFER_MIN;
        screen_poss[i] = temp_vec3;
    }

    // 寻找三角形的边界
    int image_size[2] = {image.get_width(), image.get_height()};
    int border_min[2] = {image_size[0], image_size[1]};
    int border_max[2] = {0, 0};
    for (const auto &screen_pos : screen_poss) {
        border_min[0] = max(0, min(border_min[0], int(screen_pos.x)));
        border_min[1] = max(0, min(border_min[1], int(screen_pos.y)));
        border_max[0] = min(image_size[0], max(border_max[0], int(screen_pos.x)));
        border_max[1] = min(image_size[1], max(border_max[1], int(screen_pos.y)));
    }

#pragma omp parallel for
    // 光栅化：遍历边界范围内的所有点，绘制
    for (int x = border_min[0]; x <= border_max[0]; ++x) {
        for (int y = border_min[1]; y <= border_max[1]; ++y) {

            // 获得插值参数
            auto bary_coeff = barycentric(proj<2>(screen_poss[0]),
                                          proj<2>(screen_poss[1]),
                                          proj<2>(screen_poss[2]),
                                          vec2(x, y));

            // 判断点是否在三角形内
            if (bary_coeff.x < 0 || bary_coeff.y < 0 || bary_coeff.z < 0)
                continue;

            // z-buffer 测试
            z_buffer_t depth = bary_coeff * vec3(screen_poss[0].z, screen_poss[1].z, screen_poss[2].z);
            if (depth < Z_BUFFER_MIN || depth > Z_BUFFER_MAX)
                continue;
            if (depth > z_buffer[y][x]) continue;
            z_buffer[y][x] = depth;

            // 调用片段着色器绘制
            image.set(x, y, shader.fragment(bary_coeff));
        }
    }
}
