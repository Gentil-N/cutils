#include "cutils_math.h"

#include <math.h>

void mat4f_set_perspective(struct mat4f *mat, float aspect_ratio, float fov, float near_plane, float far_plane)
{
    float s = 1.0f / tanf(fov * M_PI / 180.0f / 2.0f);
    float coeff = - 1.0f / (far_plane - near_plane);
    mat4f_set_zero(mat);
    mat->m00 = s / aspect_ratio;
    mat->m11 = s;
    mat->m22 = far_plane * coeff;
    mat->m32 = far_plane * near_plane * coeff;
    mat->m23 = -1;
}

void mat4f_set_mul(struct mat4f *mat_a, struct mat4f *mat_b, struct mat4f *mat_res)
{
    mat_res->m00 = mat_a->m00 * mat_b->m00 + mat_a->m10 * mat_b->m01 + mat_a->m20 * mat_b->m02 + mat_a->m30 * mat_b->m03;
    mat_res->m01 = mat_a->m01 * mat_b->m00 + mat_a->m11 * mat_b->m01 + mat_a->m21 * mat_b->m02 + mat_a->m31 * mat_b->m03;
    mat_res->m02 = mat_a->m02 * mat_b->m00 + mat_a->m12 * mat_b->m01 + mat_a->m22 * mat_b->m02 + mat_a->m32 * mat_b->m03;
    mat_res->m03 = mat_a->m03 * mat_b->m00 + mat_a->m13 * mat_b->m01 + mat_a->m23 * mat_b->m02 + mat_a->m33 * mat_b->m03;

    mat_res->m10 = mat_a->m00 * mat_b->m10 + mat_a->m10 * mat_b->m11 + mat_a->m20 * mat_b->m12 + mat_a->m30 * mat_b->m13;
    mat_res->m11 = mat_a->m01 * mat_b->m10 + mat_a->m11 * mat_b->m11 + mat_a->m21 * mat_b->m12 + mat_a->m31 * mat_b->m13;
    mat_res->m12 = mat_a->m02 * mat_b->m10 + mat_a->m12 * mat_b->m11 + mat_a->m22 * mat_b->m12 + mat_a->m32 * mat_b->m13;
    mat_res->m13 = mat_a->m03 * mat_b->m10 + mat_a->m13 * mat_b->m11 + mat_a->m23 * mat_b->m12 + mat_a->m33 * mat_b->m13;

    mat_res->m20 = mat_a->m00 * mat_b->m20 + mat_a->m10 * mat_b->m21 + mat_a->m20 * mat_b->m22 + mat_a->m30 * mat_b->m23;
    mat_res->m21 = mat_a->m01 * mat_b->m20 + mat_a->m11 * mat_b->m21 + mat_a->m21 * mat_b->m22 + mat_a->m31 * mat_b->m23;
    mat_res->m22 = mat_a->m02 * mat_b->m20 + mat_a->m12 * mat_b->m21 + mat_a->m22 * mat_b->m22 + mat_a->m32 * mat_b->m23;
    mat_res->m23 = mat_a->m03 * mat_b->m20 + mat_a->m13 * mat_b->m21 + mat_a->m23 * mat_b->m22 + mat_a->m33 * mat_b->m23;

    mat_res->m30 = mat_a->m00 * mat_b->m30 + mat_a->m10 * mat_b->m31 + mat_a->m20 * mat_b->m32 + mat_a->m30 * mat_b->m33;
    mat_res->m31 = mat_a->m01 * mat_b->m30 + mat_a->m11 * mat_b->m31 + mat_a->m21 * mat_b->m32 + mat_a->m31 * mat_b->m33;
    mat_res->m32 = mat_a->m02 * mat_b->m30 + mat_a->m12 * mat_b->m31 + mat_a->m22 * mat_b->m32 + mat_a->m32 * mat_b->m33;
    mat_res->m33 = mat_a->m03 * mat_b->m30 + mat_a->m13 * mat_b->m31 + mat_a->m23 * mat_b->m32 + mat_a->m33 * mat_b->m33;
}

struct mat4f *mat4f_set_rot_x(struct mat4f *mat, float alpha)
{
    float sina = sinf(alpha);
    float cosa = cosf(alpha);
    mat4f_set_identity(mat);
    mat->m11 = cosa;
    mat->m12 = sina;
    mat->m21 = -sina;
    mat->m22 = cosa;
    return mat;
}

struct mat4f *mat4f_set_rot_y(struct mat4f *mat, float alpha)
{
    float sina = sinf(alpha);
    float cosa = cosf(alpha);
    mat4f_set_identity(mat);
    mat->m00 = cosa;
    mat->m20 = sina;
    mat->m02 = -sina;
    mat->m22 = cosa;
    return mat;
}

struct mat4f *mat4f_set_rot_z(struct mat4f *mat, float alpha)
{
    float sina = sinf(alpha);
    float cosa = cosf(alpha);
    mat4f_set_identity(mat);
    mat->m00 = cosa;
    mat->m01 = sina;
    mat->m10 = -sina;
    mat->m11 = cosa;
    return mat;
}

struct mat4f *mat4f_rot_x(struct mat4f *mat, float alpha)
{
    float sina = sinf(alpha);
    float cosa = cosf(alpha);
    float nm10 = mat->m10 * cosa + mat->m20 * sina;
    float nm20 = -mat->m10 * sina + mat->m20 * cosa;
    float nm11 = mat->m11 * cosa + mat->m21 * sina;
    float nm21 = -mat->m11 * sina + mat->m21 * cosa;
    float nm12 = mat->m12 * cosa + mat->m22 * sina;
    float nm22 = -mat->m12 * sina + mat->m22 * cosa;
    float nm13 = mat->m13 * cosa + mat->m23 * sina;
    float nm23 = -mat->m13 * sina + mat->m23 * cosa;
    mat->m10 = nm10;
    mat->m11 = nm11;
    mat->m12 = nm12;
    mat->m13 = nm13;
    mat->m20 = nm20;
    mat->m21 = nm21;
    mat->m22 = nm22;
    mat->m23 = nm23;
    return mat;
}

struct mat4f *mat4f_rot_y(struct mat4f *mat, float alpha)
{
    float sina = sinf(alpha);
    float cosa = cosf(alpha);
    float nm00 = mat->m00 * cosa - mat->m20 * sina;
    float nm20 = mat->m00 * sina + mat->m20 * cosa;
    float nm01 = mat->m01 * cosa - mat->m21 * sina;
    float nm21 = mat->m01 * sina + mat->m21 * cosa;
    float nm02 = mat->m02 * cosa - mat->m22 * sina;
    float nm22 = mat->m02 * sina + mat->m22 * cosa;
    float nm03 = mat->m03 * cosa - mat->m23 * sina;
    float nm23 = mat->m03 * sina + mat->m23 * cosa;
    mat->m00 = nm00;
    mat->m01 = nm01;
    mat->m02 = nm02;
    mat->m03 = nm03;
    mat->m20 = nm20;
    mat->m21 = nm21;
    mat->m22 = nm22;
    mat->m23 = nm23;
    return mat;
}

struct mat4f *mat4f_rot_z(struct mat4f *mat, float alpha)
{
    float sina = sinf(alpha);
    float cosa = cosf(alpha);
    float nm00 = mat->m00 * cosa + mat->m10 * sina;
    float nm10 = -mat->m00 * sina + mat->m10 * cosa;
    float nm01 = mat->m01 * cosa + mat->m11 * sina;
    float nm11 = -mat->m01 * sina + mat->m11 * cosa;
    float nm02 = mat->m02 * cosa + mat->m12 * sina;
    float nm12 = -mat->m02 * sina + mat->m12 * cosa;
    float nm03 = mat->m03 * cosa + mat->m13 * sina;
    float nm13 = -mat->m03 * sina + mat->m13 * cosa;
    mat->m00 = nm00;
    mat->m01 = nm01;
    mat->m02 = nm02;
    mat->m03 = nm03;
    mat->m10 = nm10;
    mat->m11 = nm11;
    mat->m12 = nm12;
    mat->m13 = nm13;
    return mat;
}

struct mat4f *mat4f_rot_x_ahead(struct mat4f *mat, float alpha)
{
    float sina = sinf(alpha);
    float cosa = cosf(alpha);
    float nm01 = mat->m01 * cosa - mat->m02 * sina;
    float nm02 = mat->m01 * sina + mat->m02 * cosa;
    float nm11 = mat->m11 * cosa - mat->m12 * sina;
    float nm12 = mat->m11 * sina + mat->m12 * cosa;
    float nm21 = mat->m21 * cosa - mat->m22 * sina;
    float nm22 = mat->m21 * sina + mat->m22 * cosa;
    float nm31 = mat->m31 * cosa - mat->m32 * sina;
    float nm32 = mat->m31 * sina + mat->m32 * cosa;
    mat->m01 = nm01;
    mat->m02 = nm02;
    mat->m11 = nm11;
    mat->m12 = nm12;
    mat->m21 = nm21;
    mat->m22 = nm22;
    mat->m31 = nm31;
    mat->m32 = nm32;
    return mat;
}

struct mat4f *mat4f_rot_y_ahead(struct mat4f *mat, float alpha)
{
    float sina = sinf(alpha);
    float cosa = cosf(alpha);
    float nm00 = mat->m00 * cosa + mat->m02 * sina;
    float nm02 = -mat->m00 * sina + mat->m02 * cosa;
    float nm10 = mat->m10 * cosa + mat->m12 * sina;
    float nm12 = -mat->m10 * sina + mat->m12 * cosa;
    float nm20 = mat->m20 * cosa + mat->m22 * sina;
    float nm22 = -mat->m20 * sina + mat->m22 * cosa;
    float nm30 = mat->m30 * cosa + mat->m32 * sina;
    float nm32 = -mat->m30 * sina + mat->m32 * cosa;
    mat->m00 = nm00;
    mat->m02 = nm02;
    mat->m10 = nm10;
    mat->m12 = nm12;
    mat->m20 = nm20;
    mat->m22 = nm22;
    mat->m30 = nm30;
    mat->m32 = nm32;
    return mat;
}

struct mat4f *mat4f_rot_z_ahead(struct mat4f *mat, float alpha)
{
    float sina = sinf(alpha);
    float cosa = cosf(alpha);
    float nm00 = mat->m00 * cosa - mat->m01 * sina;
    float nm01 = mat->m01 * sina + mat->m01 * cosa;
    float nm10 = mat->m10 * cosa - mat->m11 * sina;
    float nm11 = mat->m10 * sina + mat->m11 * cosa;
    float nm20 = mat->m20 * cosa - mat->m21 * sina;
    float nm21 = mat->m20 * sina + mat->m21 * cosa;
    float nm30 = mat->m30 * cosa - mat->m31 * sina;
    float nm31 = mat->m30 * sina + mat->m31 * cosa;
    mat->m00 = nm00;
    mat->m01 = nm01;
    mat->m10 = nm10;
    mat->m11 = nm11;
    mat->m20 = nm20;
    mat->m21 = nm21;
    mat->m30 = nm30;
    mat->m31 = nm31;
    return mat;
}

struct mat4f *mat4f_set_translation(struct mat4f *mat, float x, float y, float z)
{
    mat4f_set_identity(mat);
    mat->m30 = x;
    mat->m31 = y;
    mat->m32 = z;
    return mat;
}

struct mat4f *mat4f_translate(struct mat4f *mat, float x, float y, float z)
{
    float nm30 = mat->m00 * x + mat->m10 * y + mat->m20 * z + mat->m30;
    float nm31 = mat->m01 * x + mat->m11 * y + mat->m21 * z + mat->m31;
    float nm32 = mat->m02 * x + mat->m12 * y + mat->m22 * z + mat->m32;
    float nm33 = mat->m03 * x + mat->m13 * y + mat->m23 * z + mat->m33;
    mat->m30 = nm30;
    mat->m31 = nm31;
    mat->m32 = nm32;
    mat->m33 = nm33;
    return mat;
}

struct mat4f *mat4f_translate_ahead(struct mat4f *mat, float x, float y, float z)
{
    float nm00 = mat->m00 + mat->m03 * x;
    float nm10 = mat->m10 + mat->m13 * x;
    float nm20 = mat->m20 + mat->m23 * x;
    float nm30 = mat->m30 + mat->m33 * x;

    float nm01 = mat->m01 + mat->m03 * y;
    float nm11 = mat->m11 + mat->m13 * y;
    float nm21 = mat->m21 + mat->m23 * y;
    float nm31 = mat->m31 + mat->m33 * y;

    float nm02 = mat->m02 + mat->m03 * z;
    float nm12 = mat->m12 + mat->m13 * z;
    float nm22 = mat->m22 + mat->m23 * z;
    float nm32 = mat->m32 + mat->m33 * z;

    mat->m00 = nm00;
    mat->m10 = nm10;
    mat->m20 = nm20;
    mat->m30 = nm30;

    mat->m01 = nm01;
    mat->m11 = nm11;
    mat->m21 = nm21;
    mat->m31 = nm31;

    mat->m02 = nm02;
    mat->m12 = nm12;
    mat->m22 = nm22;
    mat->m32 = nm32;

    return mat;
}
