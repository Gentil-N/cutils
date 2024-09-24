#ifndef __CUTILS_MATH_H__
#define __CUTILS_MATH_H__

#ifdef __cplusplus
extern "C"
{
#endif

struct mat4f
{
    union
    {
        struct
        {
            float   m00, m01, m02, m03, // col #0
                    m10, m11, m12, m13, // col #1
                    m20, m21, m22, m23, // col #2
                    m30, m31, m32, m33; // col #3
        };
        float m[16];
    };
};

struct vec2f
{
    union
    {
        struct  { float x, y; };
        float v[2];
    };
};

struct vec3f
{
    union
    {
        struct { float x, y, z; };
        float v[3];
    };
};

struct vec4f
{
    union
    {
        struct { float x, y, z, w; };
        float v[4];
    };
};

static inline void vec4f_set_zero(struct vec4f *vec)
{
    vec->x = vec->y = vec->z = vec->w = 0.0f;
}

static inline struct  vec4f vec4f_zero()
{
    return (struct vec4f){0.0f, 0.0f, 0.0f, 0.0f};
}

static inline struct mat4f *mat4f_set_zero(struct mat4f *mat)
{
    mat->m00 = mat->m01 = mat->m02 = mat->m03 = 0.0f;
    mat->m10 = mat->m11 = mat->m12 = mat->m13 = 0.0f;
    mat->m20 = mat->m21 = mat->m22 = mat->m23 = 0.0f;
    mat->m30 = mat->m31 = mat->m32 = mat->m33 = 0.0f;
    return mat;
}

static inline struct mat4f *mat4f_set_identity(struct mat4f *mat)
{
    mat->m01 = mat->m02 = mat->m03 = 0.0f;
    mat->m10 = mat->m12 = mat->m13 = 0.0f;
    mat->m20 = mat->m21 = mat->m23 = 0.0f;
    mat->m30 = mat->m31 = mat->m32 = 0.0f;
    mat->m00 = mat->m11 = mat->m22 = mat->m33 = 1.0;
    return mat;
}

static inline struct mat4f mat4f_identity()
{
    struct mat4f mat;
    mat4f_set_identity(&mat);
    return mat;
}

static inline struct mat4f mat4f_zero()
{
    return (struct mat4f){0};
}

struct mat4f *mat4f_set_rot_x(struct mat4f *mat, float alpha);

struct mat4f *mat4f_set_rot_y(struct mat4f *mat, float alpha);

struct mat4f *mat4f_set_rot_z(struct mat4f *mat, float alpha);

struct mat4f *mat4f_rot_x(struct mat4f *mat, float alpha);

struct mat4f *mat4f_rot_y(struct mat4f *mat, float alpha);

struct mat4f *mat4f_rot_z(struct mat4f *mat, float alpha);

struct mat4f *mat4f_rot_x_ahead(struct mat4f *mat, float alpha);

struct mat4f *mat4f_rot_y_ahead(struct mat4f *mat, float alpha);

struct mat4f *mat4f_rot_z_ahead(struct mat4f *mat, float alpha);

struct mat4f *mat4f_set_translation(struct mat4f *mat, float x, float y, float z);

struct mat4f *mat4f_translate(struct mat4f *mat, float x, float y, float z);

struct mat4f *mat4f_translate_ahead(struct mat4f *mat, float x, float y, float z);

#ifdef __cplusplus
}
#endif

#endif // __CUTILS_MATH_H__
