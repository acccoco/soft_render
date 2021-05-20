// 场景测试

#include <vector>
#include <random>
#include "model.h"
#include "tgaimage.h"
#include "geometry.h"
#include "my_gl.h"
#include "transform.h"

using namespace std;

void test_cube() {
    vec3 v1(0.5, -1.5, -1.0);
    vec3 v2(0.5, -0.5, -1.0);
    vec3 v3(0.5, -1.5, -2.0);
    vec3 v4(0.5, -0.5, -2.0);
    vec3 v5(1.5, -1.5, -1.0);
    vec3 v6(1.5, -0.5, -1.0);
    vec3 v7(1.5, -1.5, -2.0);
    vec3 v8(1.5, -0.5, -2.0);

    vector<vec3> face_left_1({v2, v3, v1});
    vector<vec3> face_left_2({v2, v4, v3});
    vector<vec3> face_far_1({v4, v7, v3});
    vector<vec3> face_far_2({v4, v8, v7});
    vector<vec3> face_right_1({v8, v5, v7});
    vector<vec3> face_right_2({v8, v6, v5});
    vector<vec3> face_near_1({v6, v1, v5});
    vector<vec3> face_near_2({v6, v2, v1});
    vector<vec3> face_bottom_1({v7, v1, v3});
    vector<vec3> face_bottom_2({v7, v5, v1});
    vector<vec3> face_top_1({v4, v6, v8});
    vector<vec3> face_top_2({v4, v2, v6});

    vector<vector<vec3>> faces({
                                       face_left_1, face_left_2,
                                       face_bottom_1, face_bottom_2,
                                       face_top_1, face_top_2,
                                       face_right_1, face_right_2,
                                       face_near_1, face_near_2,
                                       face_far_1, face_far_2,
                               });

    // 构造 location
    vector<vector<Location>> model;
    vec3 temp3;
    vec2 temp2;
    for (const auto &face : faces) {
        vector<Location> l_face;
        for (const auto &vert: face) {
            l_face.emplace_back(vert, temp3, temp2);
        }
        model.push_back(l_face);
    }

    int width = 800;
    int height = 800;

    const char *tga_filename = "../test_cube.tga";

    // image
    TGAImage image(width, height, TGAImage::RGB);
    auto z_buffer = vector<vector<z_buffer_t>>(height, vector<z_buffer_t>(width, Z_BUFFER_MAX));

    // 初始化 gl
    vec3 camera_pos(0, 0, 0);
    vec3 camera_target(0, 0, -1);
    vec3 camera_up(0, 1, 0);
    view_port(0, 0, width, width);
    mat<4, 4> projection_matrix = projection(120, 120, 100, 400);
    mat<4, 4> view_matrix = lookat(camera_pos, camera_target, camera_up);

    // 设置模型矩阵
    auto translate = translation(0, 0, -150);
    auto scale = scaling(80);
    auto model_matrix = translate * scale;

    RandomShader random_shader;
    random_shader.model_matrix = model_matrix;
    random_shader.view_matrix = view_matrix;
    random_shader.projection_matrix = projection_matrix;

    // 渲染三角形
    for (const auto &face: model) {
        triangle(image, z_buffer, random_shader, face);
    }

    image.write_tga_file(tga_filename);
}


int main() {
    test_cube();
    cout << "write ok" << endl;
}
