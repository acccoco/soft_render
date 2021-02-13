
#include <iostream>
#include "2d.h"
#include "model.h"


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
    TGAColor red = TGAColor(255, 0, 0, 255);

    auto tri_1 = std::vector<Vec2i>({
                                            Vec2i(0, 10),
                                            Vec2i(50, 40),
                                            Vec2i(30, 60)
                                    });
    triangle(tri_1[0], tri_1[1], tri_1[2], image, white);


    auto tri_2 = std::vector<Vec2i>({
                                            Vec2i(0, 80),
                                            Vec2i(20, 80),
                                            Vec2i(20, 100)
                                    });
    triangle(tri_2[0], tri_2[1], tri_2[2], image, white);


    auto tri_3 = std::vector<Vec2i>({
                                            Vec2i(0, 0),
                                            Vec2i(10, 0),
                                            Vec2i(10, 10)
                                    });
    triangle(tri_3[0], tri_3[1], tri_3[2], image, white);

    image.flip_vertically();
    image.write_tga_file(tga_filename);
}


// ============================================================================
int main() {
    test_draw_triangle();

    std::cout << "write to file ojbk." << std::endl;
}
