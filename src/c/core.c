#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <janet/janet.h>

static Janet cfun_load (int32_t argc, Janet *argv)
{
    janet_fixarity(argc, 1);
    const char *filename = janet_getcstring (argv, 0);

    int w, h, channels;
    unsigned char *data = stbi_load (filename, &w, &h, &channels, 0);
    if (!data) janet_panicf ("Failed to load image from `%s`: %s", filename, stbi_failure_reason());

    JanetBuffer *buf = janet_buffer (w * h * channels);
    janet_buffer_push_bytes (buf, data, w * h * channels); // FIXME: get rid of copy here ("abstract type"?)

    stbi_image_free (data);

    JanetKV *st = janet_struct_begin (4);
    janet_struct_put (st, janet_ckeywordv ("width"), janet_wrap_integer (w));
    janet_struct_put (st, janet_ckeywordv ("height"), janet_wrap_integer (h));
    janet_struct_put (st, janet_ckeywordv ("channels"), janet_wrap_integer (channels));
    janet_struct_put (st, janet_ckeywordv ("data"), janet_wrap_buffer (buf));

    return janet_wrap_struct (st);
}

static const JanetReg cfuns[] = {
    { "load", cfun_load, "Loads an image." },
    {NULL, NULL, NULL}
};

JANET_MODULE_ENTRY (JanetTable *env)
{
    janet_cfuns (env, "_stbimage", cfuns);
}
