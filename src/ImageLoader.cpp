#include "ImageLoader.h"

#ifdef __EMSCRIPTEN__
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

unsigned char* LoadImageRGB(const char* path, int* width, int* height)
{
    return stbi_load(path, width, height, nullptr, STBI_rgb);
}

void FreeImage(unsigned char* image)
{
    stbi_image_free(image);
}
#else
#include <SOIL/SOIL.h>

unsigned char* LoadImageRGB(const char* path, int* width, int* height)
{
    return SOIL_load_image(path, width, height, nullptr, SOIL_LOAD_RGB);
}

void FreeImage(unsigned char* image)
{
    SOIL_free_image_data(image);
}
#endif
