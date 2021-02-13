#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <cassert>
#include <iostream>


// double 任意维度向量
// ============================================================================


/* 任意维度向量，分量是 double  */
template<int n>
struct vec {
    vec() = default;

    double &operator[](const int i) {
        assert(i >= 0 && i < n);
        return data[i];
    }

    double operator[](const int i) const {
        assert(i >= 0 && i < n);
        return data[i];
    }

    double norm2() const { return (*this) * (*this); }

    double norm() const { return std::sqrt(norm2()); }

    double data[n] = {0};
};

// 向量 * 向量
template<int n>
double operator*(const vec<n> &lhs, const vec<n> &rhs) {
    double ret = 0;
    for (int i = n; i--; ret += lhs[i] * rhs[i]);
    return ret;
}

// 向量 + 向量
template<int n>
vec<n> operator+(const vec<n> &lhs, const vec<n> &rhs) {
    vec<n> ret = lhs;
    for (int i = n; i--; ret[i] += rhs[i]);
    return ret;
}

// 向量 - 向量
template<int n>
vec<n> operator-(const vec<n> &lhs, const vec<n> &rhs) {
    vec<n> ret = lhs;
    for (int i = n; i--; ret[i] -= rhs[i]);
    return ret;
}

// 浮点 * 向量
template<int n>
vec<n> operator*(const double &rhs, const vec<n> &lhs) {
    vec<n> ret = lhs;
    for (int i = n; i--; ret[i] *= rhs);
    return ret;
}

// 向量 * 浮点
template<int n>
vec<n> operator*(const vec<n> &lhs, const double &rhs) {
    vec<n> ret = lhs;
    for (int i = n; i--; ret[i] *= rhs);
    return ret;
}

// 向量 / 浮点
template<int n>
vec<n> operator/(const vec<n> &lhs, const double &rhs) {
    vec<n> ret = lhs;
    for (int i = n; i--; ret[i] /= rhs);
    return ret;
}

// 低维度向量转换为高维度
template<int n1, int n2>
vec<n1> embed(const vec<n2> &v, double fill = 1) {
    vec<n1> ret;
    for (int i = n1; i--; ret[i] = (i < n2 ? v[i] : fill));
    return ret;
}

// 高维度向量转换为低维度
template<int n1, int n2>
vec<n1> proj(const vec<n2> &v) {
    vec<n1> ret;
    for (int i = n1; i--; ret[i] = v[i]);
    return ret;
}

// 向量输出样式
template<int n>
std::ostream &operator<<(std::ostream &out, const vec<n> &v) {
    for (int i = 0; i < n; i++) out << v[i] << " ";
    return out;
}

// double 二维向量，三维向量
// ============================================================================


/* 二维向量，分量 double */
template<>
struct vec<2> {
    vec() = default;

    vec(double X, double Y) : x(X), y(Y) {}

    double &operator[](const int i) {
        assert(i >= 0 && i < 2);
        return i == 0 ? x : y;
    }

    double operator[](const int i) const {
        assert(i >= 0 && i < 2);
        return i == 0 ? x : y;
    }

    double norm2() const { return (*this) * (*this); }

    double norm() const { return std::sqrt(norm2()); }

    vec &normalize() {
        *this = (*this) / norm();
        return *this;
    }

    double x{}, y{};
};


/* 三维向量，分量 double */
template<>
struct vec<3> {
    vec() = default;

    vec(double X, double Y, double Z) : x(X), y(Y), z(Z) {}

    double &operator[](const int i) {
        assert(i >= 0 && i < 3);
        return i == 0 ? x : (1 == i ? y : z);
    }

    double operator[](const int i) const {
        assert(i >= 0 && i < 3);
        return i == 0 ? x : (1 == i ? y : z);
    }

    double norm2() const { return (*this) * (*this); }

    double norm() const { return std::sqrt(norm2()); }

    vec &normalize() {
        *this = (*this) / norm();
        return *this;
    }

    double x{}, y{}, z{};
};


// 前置声明
template<int n>
struct dt;


// double 矩阵
// ============================================================================


/* 矩阵相关定义 */
template<int nrows, int ncols>
struct mat {
    vec<ncols> rows[nrows] = {{}};

    mat() = default;

    vec<ncols> &operator[](const int idx) {
        assert(idx >= 0 && idx < nrows);
        return rows[idx];
    }

    const vec<ncols> &operator[](const int idx) const {
        assert(idx >= 0 && idx < nrows);
        return rows[idx];
    }

    vec<nrows> col(const int idx) const {
        assert(idx >= 0 && idx < ncols);
        vec<nrows> ret;
        for (int i = nrows; i--; ret[i] = rows[i][idx]);
        return ret;
    }

    void set_col(const int idx, const vec<nrows> &v) {
        assert(idx >= 0 && idx < ncols);
        for (int i = nrows; i--; rows[i][idx] = v[i]);
    }

    // 矩阵是否相同
    static mat<nrows, ncols> identity() {
        mat<nrows, ncols> ret;
        for (int i = nrows; i--;)
            for (int j = ncols; j--; ret[i][j] = (i == j));
        return ret;
    }

    // 计算行列式
    double det() const {
        return dt<ncols>::det(*this);
    }

    // 矩阵的余子式：去除某行某列后剩余的矩阵
    mat<nrows - 1, ncols - 1> get_minor(const int row, const int col) const {
        mat<nrows - 1, ncols - 1> ret;
        for (int i = nrows - 1; i--;)
            for (int j = ncols - 1; j--; ret[i][j] = rows[i < row ? i : i + 1][j < col ? j : j + 1]);
        return ret;
    }

    // 矩阵的代数余子式：矩阵的余子式，对应的行列式值
    double cofactor(const int row, const int col) const {
        return get_minor(row, col).det() * ((row + col) % 2 ? -1 : 1);
    }

    // 矩阵的伴随矩阵（伴随矩阵：原矩阵的每一个格子都是对应的代数余子式）
    mat<nrows, ncols> adjugate() const {
        mat<nrows, ncols> ret;
        for (int i = nrows; i--;)
            for (int j = ncols; j--; ret[i][j] = cofactor(i, j));
        return ret;
    }

    // 逆矩阵的转置矩阵
    mat<nrows, ncols> invert_transpose() const {
        mat<nrows, ncols> ret = adjugate();
        return ret / (ret[0] * rows[0]);
    }

    // 逆矩阵（逆矩阵是为了实现矩阵的除法）
    mat<nrows, ncols> invert() const {
        return invert_transpose().transpose();
    }

    // 矩阵转置
    mat<ncols, nrows> transpose() const {
        mat<ncols, nrows> ret;
        for (int i = ncols; i--; ret[i] = this->col(i));
        return ret;
    }
};


// 矩阵 * 向量
template<int nrows, int ncols>
vec<nrows> operator*(const mat<nrows, ncols> &lhs, const vec<ncols> &rhs) {
    vec<nrows> ret;
    for (int i = nrows; i--; ret[i] = lhs[i] * rhs);
    return ret;
}

// 矩阵 * 矩阵
template<int R1, int C1, int C2>
mat<R1, C2> operator*(const mat<R1, C1> &lhs, const mat<C1, C2> &rhs) {
    mat<R1, C2> result;
    for (int i = R1; i--;)
        for (int j = C2; j--; result[i][j] = lhs[i] * rhs.col(j));
    return result;
}

// 矩阵 * 浮点
template<int nrows, int ncols>
mat<nrows, ncols> operator*(const mat<nrows, ncols> &lhs, const double &val) {
    mat<nrows, ncols> result;
    for (int i = nrows; i--; result[i] = lhs[i] * val);
    return result;
}

// 矩阵 / 浮点
template<int nrows, int ncols>
mat<nrows, ncols> operator/(const mat<nrows, ncols> &lhs, const double &val) {
    mat<nrows, ncols> result;
    for (int i = nrows; i--; result[i] = lhs[i] / val);
    return result;
}

// 矩阵 + 矩阵
template<int nrows, int ncols>
mat<nrows, ncols> operator+(const mat<nrows, ncols> &lhs, const mat<nrows, ncols> &rhs) {
    mat<nrows, ncols> result;
    for (int i = nrows; i--;)
        for (int j = ncols; j--; result[i][j] = lhs[i][j] + rhs[i][j]);
    return result;
}

// 矩阵 - 矩阵
template<int nrows, int ncols>
mat<nrows, ncols> operator-(const mat<nrows, ncols> &lhs, const mat<nrows, ncols> &rhs) {
    mat<nrows, ncols> result;
    for (int i = nrows; i--;)
        for (int j = ncols; j--; result[i][j] = lhs[i][j] - rhs[i][j]);
    return result;
}

// 显示矩阵
template<int nrows, int ncols>
std::ostream &operator<<(std::ostream &out, const mat<nrows, ncols> &m) {
    for (int i = 0; i < nrows; i++) out << m[i] << std::endl;
    return out;
}


// double 行列式
// ============================================================================


/* 行列式 */
template<int n>
struct dt {
    // 计算行列式的值：按第 1 行展开
    static double det(const mat<n, n> &src) {
        double ret = 0;
        for (int i = n; i--; ret += src[0][i] * src.cofactor(0, i));
        return ret;
    }
};

template<>
struct dt<1> {
    static double det(const mat<1, 1> &src) {
        return src[0][0];
    }
};


// 常用向量的类型定义
// ============================================================================
typedef vec<2> vec2;
typedef vec<3> vec3;
typedef vec<4> vec4;

vec3 cross(const vec3 &v1, const vec3 &v2);


// 没有基础类型的向量
// ============================================================================
template<class t>
struct Vec2 {
    union {
        struct {
            t u, v;
        };
        struct {
            t x, y;
        };
        t raw[2];
    };

    Vec2() : u(0), v(0) {}

    Vec2(t _u, t _v) : u(_u), v(_v) {}

    inline Vec2<t> operator+(const Vec2<t> &V) const { return Vec2<t>(u + V.u, v + V.v); }

    inline Vec2<t> operator-(const Vec2<t> &V) const { return Vec2<t>(u - V.u, v - V.v); }

    inline Vec2<t> operator*(float f) const { return Vec2<t>(u * f, v * f); }

    t &operator[](const int i) {
        assert(i >= 0 && i < 2);
        return raw[i];
    }

    t operator[](const int i) const {
        assert(i >= 0 && i < 2);
        return raw[i];
    }

    template<class>
    friend std::ostream &operator<<(std::ostream &s, Vec2<t> &v);
};

template<class t>
struct Vec3 {
    union {
        struct {
            t x, y, z;
        };
        struct {
            t ivert, iuv, inorm;
        };
        t raw[3];
    };

    Vec3() : x(0), y(0), z(0) {}

    Vec3(t _x, t _y, t _z) : x(_x), y(_y), z(_z) {}

    inline Vec3<t> operator^(const Vec3<t> &v) const {
        return Vec3<t>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }

    inline Vec3<t> operator+(const Vec3<t> &v) const { return Vec3<t>(x + v.x, y + v.y, z + v.z); }

    inline Vec3<t> operator-(const Vec3<t> &v) const { return Vec3<t>(x - v.x, y - v.y, z - v.z); }

    inline Vec3<t> operator*(float f) const { return Vec3<t>(x * f, y * f, z * f); }

    inline t operator*(const Vec3<t> &v) const { return x * v.x + y * v.y + z * v.z; }

    t &operator[](const int i) {
        assert(i >= 0 && i < 3);
        return raw[i];
    }

    t operator[](const int i) const {
        assert(i >= 0 && i < 3);
        return raw[i];
    }

    float norm() const { return std::sqrt(x * x + y * y + z * z); }

    Vec3<t> &normalize(t l = 1) {
        *this = (*this) * (l / norm());
        return *this;
    }

    template<class>
    friend std::ostream &operator<<(std::ostream &s, Vec3<t> &v);
};

typedef Vec2<float> Vec2f;
typedef Vec2<int> Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int> Vec3i;

template<class t>
std::ostream &operator<<(std::ostream &s, Vec2<t> &v) {
    s << "(" << v.x << ", " << v.y << ")\n";
    return s;
}

template<class t>
std::ostream &operator<<(std::ostream &s, Vec3<t> &v) {
    s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
    return s;
}

template<typename t>
Vec3<t> cross(const Vec3<t> &v1, const Vec3<t> &v2) {
    return Vec3<t>{v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x};
}

#endif //__GEOMETRY_H__

