
#include <random>
#include <iostream>
#include "model.h"
#include "my_gl.h"
#include "shader.h"
#include "transform.h"

using namespace std;


void render_obj() {
    int width = 1024;
    int height = 1024;

    // 载入模型，材质文件
    const char *model_filename = "../obj/diablo3_pose/diablo3_pose.obj";
    const char *diffuse_filename = "../obj/diablo3_pose/diablo3_pose_diffuse.tga";
    const char *normal_filename = "../obj/diablo3_pose/diablo3_pose_nm_tangent.tga";
    const char *spec_filename = "../obj/diablo3_pose/diablo3_pose_spec.tga";
    const char *tga_filename = "../render.tga";
    Model model = Model(model_filename);
    TGAImage diffuse_texture;
    diffuse_texture.read_tga_file(diffuse_filename);
    diffuse_texture.flip_vertically();
    TGAImage normal_texture;
    normal_texture.read_tga_file(normal_filename);
    normal_texture.flip_vertically();
    TGAImage spec_texture;
    spec_texture.read_tga_file(spec_filename);
    spec_texture.flip_vertically();

    // 设置模型矩阵
    auto rotate = rotate_y(0);
    auto translate = translation(0, 0, -200);
    auto scale = scaling(80);
    auto model_matrix = translate * scale * rotate;

    // image, view_port
    TGAImage out_image(width, height, TGAImage::RGB);
    auto z_buffer = vector<vector<z_buffer_t>>(height, vector<z_buffer_t>(width, Z_BUFFER_MAX));
    view_port(0, 0, width, height);

    // 构造 locations
    vector<vector<Location>> location_model;
    for (int i = 0; i < model.nfaces(); ++i) {
        vector<Location> location_face;
        for (int j = 0; j < 3; ++j) {
            vec3 pos = model.vert(i, j);
            vec3 normal = model.normal(i, j);
            vec2 uv = model.uv(i, j);
            location_face.emplace_back(pos, normal, uv);
        }
        location_model.push_back(location_face);
    }

    // 摄像机和光照方向
    vec3 camera_pos(0, 0, 0);
    vec3 camera_target(0, 0, -1);
    vec3 light_pos(0, 0.3, 1);
    vec3 y_up(0, 1, 0);

    // view, projection, view_port 矩阵
    mat<4, 4> view_matrix = lookat(camera_pos, camera_target, y_up);
    mat<4, 4> projection_matrix = projection(100, 100, 100, 400);

    // shader
    PhongShader phong_shader;
    phong_shader.light_pos = light_pos;
    phong_shader.camera_pos = camera_pos;
    phong_shader.model_matrix = model_matrix;
    phong_shader.model_iv_matrix = model_matrix.invert();
    phong_shader.view_matrix = view_matrix;
    phong_shader.projection_matrix = projection_matrix;
    phong_shader.diffuse_texture = diffuse_texture;
    phong_shader.normal_texture = normal_texture;
    phong_shader.specular_texture = spec_texture;

    // 绘制模型
    for (const auto &f: location_model) {
        triangle(out_image, z_buffer, phong_shader, f);
    }

    out_image.write_tga_file(tga_filename);
}


// ============================================================================
int main() {
    render_obj();
    cout << "wirte to file objk." << endl;
}
