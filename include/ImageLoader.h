#pragma once

unsigned char* LoadImageRGB(const char* path, int* width, int* height);
void FreeImage(unsigned char* image);
