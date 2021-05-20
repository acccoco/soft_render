
#ifndef RENDER_SHADER_H
#define RENDER_SHADER_H

#include "geometry.h"
#include "tgaimage.h"
#include "model.h"
#include <cmath>
#include <utility>
#include <random>


struct Location {
    const vec3 local_pos;
    const vec3 local_normal;
    const vec2 uv;

    explicit Location(const vec3 &local_pos, const vec3 &local_normal, const vec2 &uv)
            : local_pos(local_pos), local_normal(local_normal), uv(uv) {}
};

struct Shader {
    virtual vec4 vertex(const Location &location, int ivert) = 0;

    virtual TGAColor fragment(const vec3 &barycent) = 0;
};

inline TGAColor get_diffuse(const TGAImage &image, const vec2 &uv) {
    return image.get(uv[0] * image.get_width(), uv[1] * image.get_height());
}

inline vec3 get_normal(const TGAImage &normal_texture, const vec2 &uv) {
    TGAColor c = normal_texture.get(uv[0] * normal_texture.get_width(), uv[1] * normal_texture.get_height());
    vec3 res;
    for (int i = 0; i < 3; i++)
        res[2 - i] = c[i] / 255. * 2 - 1;
    return res;
}

inline double get_specular(const TGAImage &specular_texture, const vec2 &uv) {
    return specular_texture.get(uv[0] * specular_texture.get_width(), uv[1] * specular_texture.get_height())[0];
}

struct PhongShader : public Shader {
    // 外面提供的
    vec3 light_pos;
    vec3 camera_pos;
    mat<4, 4> model_matrix;
    mat<4, 4> model_iv_matrix;
    mat<4, 4> view_matrix;
    mat<4, 4> projection_matrix;
    TGAImage diffuse_texture;
    TGAImage normal_texture;
    TGAImage specular_texture;

    // 传递给片段着色器的
    mat<3, 3> world_ps;
    mat<3, 3> world_ns;
    mat<2, 3> uvs;
    mat<3, 3> TBN;

    vec4 vertex(const Location &location, const int ivert) override {
        // 世界系中的坐标
        vec4 world_p = model_matrix * embed<4>(location.local_pos);

        // uv
        uvs.set_col(ivert, location.uv);

        world_ps.set_col(ivert, proj<3>(world_p));

        // 世界系中的法线坐标
        vec4 world_n = model_iv_matrix * embed<4>(location.local_normal);
        world_ns.set_col(ivert, proj<3>(world_n).normalize());

        // TBN矩阵
        if (ivert == 2) {
            vec3 e1 = world_ps.col(1) - world_ps.col(0);
            vec3 e2 = world_ps.col(2) - world_ps.col(0);
            double delta_u1 = uvs.col(1)[0] - uvs.col(0)[0];
            double delta_u2 = uvs.col(2)[0] - uvs.col(0)[0];
            double delta_v1 = uvs.col(1)[1] - uvs.col(0)[1];
            double delta_v2 = uvs.col(2)[1] - uvs.col(0)[1];
            double base = delta_u1 * delta_v2 - delta_u2 * delta_v1;
            vec3 T = (delta_v2 * e1 - delta_v1 * e2) / base;
            vec3 B = (delta_u1 * e2 - delta_u2 * e1) / base;
            TBN.set_col(0, T.normalize());
            TBN.set_col(1, B.normalize());
        }

        // 裁剪空间的坐标
        vec4 pos = projection_matrix * view_matrix * world_p;
        return pos;
    }

    TGAColor fragment(const vec3 &barycent) override {
        // 插值 uv
        vec2 uv = uvs * barycent;

        // 计算法向量
        vec3 n_inter = world_ns * barycent;
        TBN.set_col(2, n_inter.normalize());
        vec3 n_tangent = get_normal(normal_texture, uv);
        vec3 n = (TBN * n_tangent).normalize();

        // 获得颜色
        TGAColor color = get_diffuse(diffuse_texture, uv);

        // 光照方向
        vec3 pos = world_ps * barycent;
        vec3 light_dir = (pos - light_pos).normalize();

        // 漫反射强度
        double diffuse = std::max(0., -1 * n * light_dir);

        // 高光强度
        double spec_intens = get_specular(specular_texture, uv);
        vec3 r_light_dir = light_dir - 2 * n * (n * light_dir);
        vec3 pos2camera = (camera_pos - pos).normalize();
        double specular = pow(std::max(0., pos2camera * r_light_dir), spec_intens);

        // phong 光照的参数
        double ambient = 5.;
        double diffuse_coeff = 1.0;
        double spec_coeff = 0.2;
        for (int i = 0; i < 3; ++i)
            color[i] = std::min(255., ambient + color[i] * (diffuse_coeff * diffuse + spec_coeff * specular));

        return color;
    }
};


struct RandomShader: public Shader {

    mat<4, 4> model_matrix;
    mat<4, 4> view_matrix;
    mat<4, 4> projection_matrix;

    std::default_random_engine e;
    TGAColor color;

    vec4 vertex(const Location &location, int ivert) override {
        std::uniform_int_distribution<unsigned> u(0, 255);
        color = TGAColor(u(e), u(e), u(e), 255);
        return projection_matrix * view_matrix * model_matrix * embed<4>(location.local_pos);
    }

    TGAColor fragment(const vec3 &barycent) override {

        return color;
    }
};

//
///* 着色器的格式定义 */
//struct IShader {
//
//    /* 顶点着色器，返回false表示要剔除 */
//    virtual bool vertex(int iface, int ivert, vec3 &vert) = 0;
//
//    /* 片段着色器，返回false表示要剔除 */
//    virtual bool fragment(const vec3 &barycentric, TGAColor &color) = 0;
//
//    /* 计算屏幕坐标 */
//    static bool calcu_screen_coord(vec3 &screen_coor_out, const vec4 &world_vert,
//                                   const mat<4, 4> &view_m, const mat<4, 4> &proj_m, const mat<4, 4> &view_port_t) {
//        vec4 after_proj = proj_m * view_m * world_vert;
//        if (after_proj[3] == 0) return false;
//        after_proj = after_proj / after_proj[3];
//        after_proj[3] = 1;
//        screen_coor_out = proj<3>(view_port_t * after_proj);
//        return true;
//    }
//};
//
//struct MyShader : public IShader {
//    const Model &model;             // 模型
//    vec3 light_dir;                 // 光照方向
//    vec3 camera;                    // 相机的坐标
//    mat<4, 4> model_matrix;         // 模型矩阵
//    mat<4, 4> shadow_view_matrix;   // 渲染阴影时使用的 view 矩阵
//    const std::vector<std::vector<uint32_t>> shadow_buffer;
//    mat<4, 4> model_iv_matrix;      // 模型矩阵的逆转置矩阵
//
//    MyShader(const Model &model, vec3 &light_dir, const vec3 &camera, const mat<4, 4> &model_matrix,
//             const mat<4, 4> &shadow_view_matrix, std::vector<std::vector<uint32_t>> shadow_buffer)
//            : model(model), light_dir(light_dir.normalize()), camera(camera), model_matrix(model_matrix),
//              shadow_view_matrix(shadow_view_matrix), shadow_buffer(std::move(shadow_buffer)) {
//        this->model_iv_matrix = this->model_matrix.invert_transpose();
//    }
//
//    mat<3, 3> ns;                   // 三个顶点在世界坐标系中的法线
//    mat<2, 3> uvs;                  // 三个顶点的 UV
//    mat<3, 3> vs;                   // 三个顶点在世界坐标系中的坐标值
//
//    bool vertex(int iface, int ivert, vec3 &vert) override {
//        vec4 vert_local = embed<4>(this->model.vert(iface, ivert));
//        vec4 vert_world = model_matrix * vert_local;
//
//        // 计算屏幕坐标系的值
//        if (!calcu_screen_coord(vert, vert_world, view_matrix, projection_matrix, view_port_matrix))
//            return false;
//
//        // 为 fragment 着色器提供值
//        vec4 normal = this->model_iv_matrix * embed<4>(this->model.normal(iface, ivert));
//        this->ns.set_col(ivert, proj<3>(normal));
//        this->uvs.set_col(ivert, this->model.uv(iface, ivert));
//        this->vs.set_col(ivert, proj<3>(vert_world));
//
//        return true;
//    }
//
//    /* 计算TBN矩阵中的TB分量 */
//    mat<3, 3> calcuTB() const {
//        vec2 uv_A = this->uvs.col(0);
//        vec2 uv_B = this->uvs.col(1);
//        vec2 uv_C = this->uvs.col(2);
//        mat<2, 2> delta_uv;
//        delta_uv.set_col(0, vec2(uv_B[0] - uv_A[0], uv_C[0] - uv_A[0]));
//        delta_uv.set_col(1, vec2(uv_B[1] - uv_A[1], uv_C[1] - uv_A[1]));
//        mat<2, 2> delta_uv_inv = delta_uv.invert();
//        vec3 AB = this->vs.col(1) - this->vs.col(0);
//        vec3 AC = this->vs.col(2) - this->vs.col(0);
//        vec3 T = (delta_uv_inv[0][0] * AB + delta_uv_inv[0][1] * AC).normalize();
//        vec3 B = (delta_uv_inv[1][0] * AB + delta_uv_inv[1][1] * AC).normalize();
//        mat<3, 3> TBN;
//        TBN.set_col(0, T);
//        TBN.set_col(1, B);
//
//        return TBN;
//    }
//
//    bool fragment(const vec3 &barycentric, TGAColor &color) override {
//        // 插值得到 uv
//        vec2 uv = this->uvs * barycentric;
//        vec3 v = this->vs * barycentric;
//
//        // 读取当前点的颜色值
//        color = this->model.diffuse(uv);
//
//        // 计算当前点是否在阴影中
//        vec3 v_shadow_screen;
//        if (!calcu_screen_coord(v_shadow_screen, embed<4>(v), shadow_view_matrix, projection_matrix, view_port_matrix))
//            return false;
//        bool in_shadow = shadow_buffer[v_shadow_screen[1]][v_shadow_screen[0]] < v_shadow_screen[2] - 0.5;
//        double shadow_coeff = 0.3 + 0.7 * !in_shadow;
//
//        // 通过法线贴图计算法线
//        mat<3, 3> TBN = calcuTB();
//        vec3 n_interpolation = (this->ns * barycentric).normalize();
//        TBN.set_col(2, n_interpolation);
//        vec3 n_tangent = this->model.normal(uv);
//        vec3 n = (TBN * n_tangent).normalize();
//
//        // 通过法线得到的漫反射强度
//        double diff_intensity = std::max(0., n * this->light_dir);
//
//        // 得到高光强度
//        double specular = this->model.specular(uv);
//        vec3 r = 2 * n * (n * this->light_dir) - this->light_dir;
//        vec3 v2camera = (this->camera - v).normalize();
//        double spec_intensity;
//        if (specular < 1)
//            spec_intensity = 0;
//        else
//            spec_intensity = pow(std::max(0., v2camera * r), specular);
//
//        // phony ：环境光 + 漫反射强度 + 高光强度
//        double ambient = 1;
//        double diffuse_coeff = 0.9;
//        double spec_coeff = 0.6;
//
//        for (int i = 0; i < 3; ++i) {
//            color[i] = std::min(255., ambient + color[i] * shadow_coeff
//                                                * (diffuse_coeff * diff_intensity + spec_coeff * spec_intensity));
//        }
//
//        return true;
//    }
//};
//
//
///* 用于渲染阴影贴图的 */
//struct ShadowShader : public IShader {
//
//    const Model &model;
//    mat<4, 4> model_matrix;         // 模型矩阵
//
//    explicit ShadowShader(const Model &model, const mat<4, 4> &model_matrix)
//            : model(model), model_matrix(model_matrix) {}
//
//    mat<3, 3> vs;                   // 三个顶点在世界坐标系中的坐标值
//
//    bool vertex(int iface, int ivert, vec3 &vert) override {
//        vec4 vert_local = embed<4>(this->model.vert(iface, ivert));
//        vec4 vert_world = model_matrix * vert_local;
//
//        // 透视除法
//        if (!calcu_screen_coord(vert, vert_world, view_matrix, projection_matrix, view_port_matrix))
//            return false;
//
//        // 为片段着色器提供值
//        this->vs.set_col(ivert, proj<3>(vert_world));
//
//        return true;
//    }
//
//    bool fragment(const vec3 &barycentric, TGAColor &color) override {
//        vec3 vert = this->vs * barycentric;
//        color = TGAColor(vert[2], vert[2], vert[2], 255);
//        return true;
//    }
//};


#endif //RENDER_SHADER_H
