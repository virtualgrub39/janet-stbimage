#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <janet/janet.h>

static Janet cfun_load (int32_t argc, Janet *argv)
{
    janet_arity(argc, 1, 2);
    const char *filename = janet_getcstring (argv, 0);
    int req_comp = (argc == 2) ? janet_getinteger(argv, 1) : 0;

    int w, h, channels;
    unsigned char *data = stbi_load (filename, &w, &h, &channels, 0);
    if (!data) janet_panicf ("Failed to load image from `%s`: %s", filename, stbi_failure_reason());

    int actual_channels = req_comp ? req_comp : channels;
    int32_t data_len = w * h * actual_channels;

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

static Janet cfun_load_mem (int32_t argc, Janet *argv)
{
    janet_arity(argc, 1, 2);
    const JanetBuffer *raw_data = janet_getbuffer (argv, 0);
    const int req_comp = (argc == 2) ? janet_getinteger(argv, 1) : 0;

    int w, h, channels;
    unsigned char *data = stbi_load_from_memory (raw_data->data, raw_data->count, &w, &h, &channels, req_comp);
    if (!data) janet_panicf ("Failed to load image from memory: %s", stbi_failure_reason());

    int actual_channels = req_comp ? req_comp : channels;
    int32_t data_len = w * h * actual_channels;

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

static Janet cfun_loadf (int32_t argc, Janet *argv)
{
    janet_arity(argc, 1, 2);
    const char *filename = janet_getcstring (argv, 0);
    int req_comp = (argc == 2) ? janet_getinteger(argv, 1) : 0;

    int w, h, channels;
    float *data = stbi_loadf (filename, &w, &h, &channels, req_comp);
    if (!data) janet_panicf ("Failed to load image from `%s`: %s", filename, stbi_failure_reason());

    JanetArray *arr = janet_array (w * h * channels);
    for (size_t i = 0; i < w * h * channels; ++i)
    {
        janet_array_push (arr, janet_wrap_number (data[i])); // FIXME: get rid of copy here ("abstract type"?)
    }

    stbi_image_free (data);

    JanetKV *st = janet_struct_begin (4);
    janet_struct_put (st, janet_ckeywordv ("width"), janet_wrap_integer (w));
    janet_struct_put (st, janet_ckeywordv ("height"), janet_wrap_integer (h));
    janet_struct_put (st, janet_ckeywordv ("channels"), janet_wrap_integer (channels));
    janet_struct_put (st, janet_ckeywordv ("data"), janet_wrap_array (arr));

    return janet_wrap_struct (st);
}

static Janet cfun_loadf_mem (int32_t argc, Janet *argv)
{
    janet_arity(argc, 1, 2);
    const JanetBuffer *raw_data = janet_getbuffer (argv, 0);
    int req_comp = (argc == 2) ? janet_getinteger(argv, 1) : 0;

    int w, h, channels;
    float *data = stbi_loadf_from_memory (raw_data->data, raw_data->count, &w, &h, &channels, req_comp);
    if (!data) janet_panicf ("Failed to load image from memory: %s", stbi_failure_reason());

    JanetArray *arr = janet_array (w * h * channels);
    for (size_t i = 0; i < w * h * channels; ++i)
    {
        janet_array_push (arr, janet_wrap_number (data[i])); // FIXME: get rid of copy here ("abstract type"?)
    }

    stbi_image_free (data);

    JanetKV *st = janet_struct_begin (4);
    janet_struct_put (st, janet_ckeywordv ("width"), janet_wrap_integer (w));
    janet_struct_put (st, janet_ckeywordv ("height"), janet_wrap_integer (h));
    janet_struct_put (st, janet_ckeywordv ("channels"), janet_wrap_integer (channels));
    janet_struct_put (st, janet_ckeywordv ("data"), janet_wrap_array (arr));

    return janet_wrap_struct (st);
}

static Janet cfun_info (int32_t argc, Janet *argv)
{
    janet_fixarity(argc, 1);
    const char *filename = janet_getcstring (argv, 0);

    int w, h, channels;
    int ok = stbi_info (filename, &w, &h, &channels);
    if (!ok) return janet_wrap_false ();

    JanetKV *st = janet_struct_begin (4);
    janet_struct_put (st, janet_ckeywordv ("width"), janet_wrap_integer (w));
    janet_struct_put (st, janet_ckeywordv ("height"), janet_wrap_integer (h));
    janet_struct_put (st, janet_ckeywordv ("channels"), janet_wrap_integer (channels));

    return janet_wrap_struct (st);
}

// TODO: callback API?
// TODO: use stbi_write_*_to_func for actual error reporting or expose separate API for that

static Janet cfun_write_png (int32_t argc, Janet *argv)
{
    janet_fixarity (argc, 6);
    const char *filename = janet_getcstring (argv, 0);
    const int w = janet_getinteger (argv, 1);
    const int h = janet_getinteger (argv, 2);
    const int comp = janet_getinteger (argv, 3);
    const JanetBuffer *data = janet_getbuffer (argv, 4);
    const int stride = janet_getinteger (argv, 5);

    int ok = stbi_write_png (filename, w, h, comp, data->data, stride);

    if (ok) return janet_wrap_true();
    return janet_wrap_false();
}

static Janet cfun_write_bmp (int32_t argc, Janet *argv)
{
    janet_fixarity (argc, 5);
    const char *filename = janet_getcstring (argv, 0);
    const int w = janet_getinteger (argv, 1);
    const int h = janet_getinteger (argv, 2);
    const int comp = janet_getinteger (argv, 3);
    const JanetBuffer *data = janet_getbuffer (argv, 4);

    int ok = stbi_write_bmp (filename, w, h, comp, data->data);

    if (ok) return janet_wrap_true();
    return janet_wrap_false();
}

static Janet cfun_write_tga (int32_t argc, Janet *argv)
{
    janet_fixarity (argc, 5);
    const char *filename = janet_getcstring (argv, 0);
    const int w = janet_getinteger (argv, 1);
    const int h = janet_getinteger (argv, 2);
    const int comp = janet_getinteger (argv, 3);
    const JanetBuffer *data = janet_getbuffer (argv, 4);

    int ok = stbi_write_tga (filename, w, h, comp, data->data);

    if (ok) return janet_wrap_true();
    return janet_wrap_false();
}

static Janet cfun_write_jpg (int32_t argc, Janet *argv)
{
    janet_fixarity (argc, 6);
    const char *filename = janet_getcstring (argv, 0);
    const int w = janet_getinteger (argv, 1);
    const int h = janet_getinteger (argv, 2);
    const int comp = janet_getinteger (argv, 3);
    const JanetBuffer *data = janet_getbuffer (argv, 4);
    const int quality = janet_getinteger (argv, 5);

    int ok = stbi_write_jpg (filename, w, h, comp, data->data, quality);

    if (ok) return janet_wrap_true();
    return janet_wrap_false();
}

static Janet cfun_set_flip_vertically (int32_t argc, Janet *argv)
{
    janet_fixarity (argc, 1);
    int flip = janet_getboolean(argv, 0);
    stbi_set_flip_vertically_on_load(flip);
    return janet_wrap_nil ();
}

// static Janet cfun_write_hdr (int32_t argc, Janet *argv)
// {
//     janet_fixarity (argc, 5);
//     const char *filename = janet_getcstring (argv, 0);
//     const int w = janet_getinteger (argv, 1);
//     const int h = janet_getinteger (argv, 2);
//     const int comp = janet_getinteger (argv, 3);
//     const JanetArray *arr = janet_getarray (argv, 4);

//     // TODO: pain in the ass
// }

static const JanetReg cfuns[] = {
    { "load", cfun_load, "Loads an image." },
    { "load-mem", cfun_load_mem, "Loads an image from memory." },
    { "loadf", cfun_loadf, "Loads an image with float-per-channel data." },
    { "loadf-mem", cfun_loadf_mem, "Loads an image with float-per-channel data from memory." },
    { "info", cfun_info, "Loads image information." },
    { "write-png", cfun_write_png, "Writes image in png format." },
    { "write-bmp", cfun_write_bmp, "Writes image in bmp format." },
    { "write-tga", cfun_write_tga, "Writes image in tga format." },
    { "write-jpg", cfun_write_jpg, "Writes image in jpg format." },
    { "set-flip-vertically", cfun_set_flip_vertically, "" },
    {NULL, NULL, NULL}
};

JANET_MODULE_ENTRY (JanetTable *env)
{
    janet_cfuns (env, "_stbimage", cfuns);
}
