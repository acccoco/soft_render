//
// Created by bigso on 2021/2/12.
//


void test_intersection() {
    auto v1 = Vec2i(10, 10);
    auto v2 = Vec2i(40, 60);
    auto v3 = Vec2i(60, 40);

    auto x1 = intersection(v2, v1, 41).x;
    std::cout << x1 << std::endl;

    auto x2 = intersection(v2, v3, 56).x;
    std::cout << x2 << std::endl;
}



void test_draw_line() {
    const char *filename = "line.tga";

    auto v1 = Vec2i(1, 0);
    auto v2 = Vec2i(11, 3);

    auto image = TGAImage(width, height, TGAImage::RGB);
    TGAColor red = TGAColor(255, 0, 0, 255);

    line(v1.x, v1.y, v2.x, v2.y, image, red);

    image.flip_vertically();
    image.write_tga_file(filename);
}


/* 输出指定的线条 */
void cout_face(Model *model, const int idx) {
    std::vector<int> face = model->face(idx);
    std::cout << "face " << idx << ": "
              << face[0] << ", "
              << face[1] << ", "
              << face[2] << std::endl;
}


/* 使用线条来绘制 obj 模型，绘制二维的模型 */
void test_draw_line_obj() {

    const char *obj_filename = "./obj/african_head.obj";
    const char *tga_filename = "line_obj.tga";

    TGAImage image = TGAImage(width, height, TGAImage::RGB);
    TGAColor white = TGAColor(255, 255, 255, 255);
    auto model = new Model(obj_filename);

    for (int i = 0; i < model->nfaces(); ++i) {
        std::vector<int> face = model->face(i);

        /*
         * 一个面3个顶点，绘制3条线：
         * 0 - 1， 1- 2， 2 - 0
         * */
        for (int j = 0; j < 3; ++j) {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j + 1) % 3]);
            int x0 = (v0.x + 1.f) * width / 2.f;
            int y0 = (v0.y + 1.f) * height / 2.f;
            int x1 = (v1.x + 1.f) * width / 2.f;
            int y1 = (v1.y + 1.f) * height / 2.f;
            line(x0, y0, x1, y1, image, white);
        }
    }

    image.flip_vertically();
    image.write_tga_file(tga_filename);

    delete model;
}
