#ifndef MATHGL_H
#define MATHGL_H
#include <QDebug>

#include <math.h>


struct vec3 { float x,y,z; };
struct mat4 { float m[16]; };

inline float len(const vec3& a)
{

}

inline vec3 normal(const vec3& a)
{

}

inline float dot(const vec3& a, const vec3& b)
{

}

inline vec3 cross(const vec3& a, const vec3& b)
{

}


inline mat4 identity() {
    mat4 mret;
    memset(mret.m, 0, 16 * sizeof(float));
    mret.m[0] = mret.m[5] = mret.m[10] = mret.m[15] = 1.0f;
    return mret;
}

inline mat4 rotation_mat(float angle, float x, float y, float z) {
    mat4 mret = identity();
    float c = cos(angle);
    float s = sin(angle);
    float t = 1.0f - c;
    float len = sqrt(x*x + y*y + z*z);
    if (len > 0) { x/=len; y/=len; z/=len; }

    mret.m[0] = t*x*x + c;     mret.m[4] = t*x*y - s*z;   mret.m[8] = t*x*z + s*y;
    mret.m[1] = t*x*y + s*z;   mret.m[5] = t*y*y + c;     mret.m[9] = t*y*z - s*x;
    mret.m[2] = t*x*z - s*y;   mret.m[6] = t*y*z + s*x;   mret.m[10] = t*z*z + c;
    return mret;
}

inline mat4 perspective(float fovy, float aspect, float znear, float zfar) {
    mat4 M;
    memset(M.m, 0, 16 * sizeof(float));
    float tanHalfFovy = tan(fovy / 2.0f);
    M.m[0] = 1.0f / (aspect * tanHalfFovy);
    M.m[5] = 1.0f / tanHalfFovy;
    M.m[10] = -(zfar + znear) / (zfar - znear);
    M.m[11] = -1.0f;
    M.m[14] = -(2.0f * zfar * znear) / (zfar - znear);
    return M;
}

inline mat4 translation_mat(float x, float y, float z)
{
    mat4 mret = identity();
    mret.m[12] = x;
    mret.m[13] = y;
    mret.m[14] = z;
    return mret;
}



inline mat4 scale_mat(float x, float y, float z)
{
    mat4 mret = identity();
    //...

    return mret;
}


inline mat4 inversed(const mat4& mat)
{
    mat4 mret = identity();
    //...

    return mret;
}

inline mat4 transposed(const mat4& mat)
{
    mat4 mret = identity();
    //...

    return mret;
}

inline mat4 mul(const mat4& m1, const mat4& m2)
{
    mat4 mret;
    //...
    return mret;
}

inline vec3 mul(const mat4& m1, const vec3& v)
{
    vec3 vret;
    //...
    return vret;
}


#endif // MATHGL_H
