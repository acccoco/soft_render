#ifndef TINYRENDERER_2D_H
#define TINYRENDERER_2D_H

#include "tgaimage.h"
#include "geometry.h"
/* 绘制二维的线条：取整 */
void line_int(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor &color);

/* 绘制二维的线条：四舍五入 */
void line_round(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor &color);

/* 绘制二维的三角形：线扫描法 */
void triangle_line_sweep(Vec2i v1, Vec2i v2, Vec2i v3, TGAImage &image, TGAColor &color);

/* 绘制三角形：重心法 */
void triangle_barycentric(Vec2i v1, Vec2i v2, Vec2i v3, TGAImage &image, TGAColor &color);


#endif //TINYRENDERER_2D_H
