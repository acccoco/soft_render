
#include <random>
#include <iostream>

#include "2d.h"
#include "model.h"

using namespace std;


// 全局变量
// ============================================================================
const int width = 800;
const int height = 800;

// 绘制一个三角形
static auto triangle = triangle_barycentric;


/* 绘制三角形 */
void test_draw_triangle() {
    const char *tga_filename = "triangle.tga";

    TGAImage image = TGAImage(width, height, TGAImage::RGB);
    TGAColor white = TGAColor(255, 255, 255, 255);

    auto tri_1 = vector<Vec2i>({
                                       Vec2i(0, 10),
                                       Vec2i(50, 40),
                                       Vec2i(30, 60)
                               });
    triangle(tri_1[0], tri_1[1], tri_1[2], image, white);


    auto tri_2 = vector<Vec2i>({
                                       Vec2i(0, 80),
                                       Vec2i(20, 80),
                                       Vec2i(20, 100)
                               });
    triangle(tri_2[0], tri_2[1], tri_2[2], image, white);


    auto tri_3 = vector<Vec2i>({
                                       Vec2i(0, 0),
                                       Vec2i(10, 0),
                                       Vec2i(10, 10)
                               });
    triangle(tri_3[0], tri_3[1], tri_3[2], image, white);

    image.write_tga_file(tga_filename);
}

/* 使用三角形来绘制模型 */
void draw_triangle_obj() {
    const char *model_filename = "./obj/african_head.obj";
    const char *model_tga_filename = "./triangle_obj.tga";

    auto model = new Model(model_filename);
    auto image = TGAImage(width, height, TGAImage::RGB);

    default_random_engine e(3);
    uniform_int_distribution<uint8_t> u(0, 255);

    Vec2i screen_coords[3];
    for (int i = 0; i < model->nfaces(); i++) {
        for (int j = 0; j < 3; j++) {
            auto world_coords = model->vert(i, j);
            screen_coords[j] = Vec2i((world_coords.x + 1.) * width / 2., (world_coords.y + 1.) * height / 2.);
        }
        triangle(screen_coords[0], screen_coords[1], screen_coords[2], image,
                 TGAColor(u(e), u(e), u(e), 255));
    }

    image.write_tga_file(model_tga_filename);

    delete model;
}


/* 使用三角形来绘制模型，三角形的亮度与法线有关 */
void draw_obj_normal_light() {

    const char *model_filename = "./obj/african_head.obj";
    const char *model_tga_filename = "./triangle_obj_normal.tga";

    // 光照方向
    Vec3f light_dir(0, 0, -1);

    float gamma = 0.8;

    auto model = new Model(model_filename);
    auto image = TGAImage(width, height, TGAImage::RGB);

    Vec2i screen_coords[3];
    Vec3f world_coords[3];
    for (int i = 0; i < model->nfaces(); i++) {
        for (int j = 0; j < 3; ++j) {
            auto v = model->vert(i, j);     // 第 i 个三角形的第 j 个顶点
            world_coords[j] = Vec3f(v.x, v.y, v.z);
            screen_coords[j] = Vec2i((v.x + 1.) * width / 2., (v.y + 1.) * height / 2.);
        }

        // 叉积计算法向量
        Vec3f n = (world_coords[2] - world_coords[0]) ^(world_coords[1] - world_coords[0]);
        n.normalize();

        // 光照强度
        float intensity = n * light_dir;
        if (intensity < 0)      // 剔除背对着的三角形
            continue;
        int light = intensity * 255 * gamma;
        triangle(screen_coords[0], screen_coords[1], screen_coords[2],
                 image,
                 TGAColor(light, light, light, 255));
    }

    image.write_tga_file(model_tga_filename);
}

/* 基于 z-buffer 来绘制obj，三角形法线光照强度也有 */
void draw_obj_zbuffer() {

    const char *model_filename = "./obj/african_head.obj";
    const char *model_tga_filename = "./triangle_obj_zbuffer.tga";

    // 光照方向
    Vec3f light_dir(0, 0, -1);

    float gamma = 0.8;

    auto model = new Model(model_filename);
    auto image = TGAImage(width, height, TGAImage::RGB);

    // z-buffer
    auto zbuffer = vector<vector<float>>(height, vector<float>(width, INT32_MIN));

    Vec3f screen_coords[3];
    Vec3f world_coords[3];
    for (int i = 0; i < model->nfaces(); i++) {
        for (int j = 0; j < 3; ++j) {
            auto v = model->vert(i, j);     // 第 i 个三角形的第 j 个顶点
            world_coords[j] = Vec3f(v.x, v.y, v.z);
            screen_coords[j] = Vec3f((v.x + 1.f) * width / 2, (v.y + 1.f) * height / 2, v.z);
        }

        // 叉积计算法向量
        Vec3f n = (world_coords[2] - world_coords[0]) ^(world_coords[1] - world_coords[0]);
        n.normalize();

        // 光照强度
        float intensity = n * light_dir;
        if (intensity < 0)      // 剔除背对着的三角形
            continue;
        int light = intensity * 255 * gamma;
        triangle_z_buffer(screen_coords[0], screen_coords[1], screen_coords[2],
                          zbuffer, image,
                          TGAColor(light, light, light, 255));
    }

    image.write_tga_file(model_tga_filename);
}


// ============================================================================
int main() {
    test_draw_triangle();
    draw_triangle_obj();
    draw_obj_normal_light();
    draw_obj_zbuffer();

    cout << "write to file ojbk." << endl;
}
