#ifndef IGNIS_FONT_H
#define IGNIS_FONT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "Core/Texture.h"

#include "stb/stb_truetype.h"

typedef struct
{
	int first_char;
	int num_chars;
	float height;

	stbtt_bakedchar* char_data;

	IgnisTexture2D texture;
} IgnisFont;

int ignisCreateFont(IgnisFont* font, const char* path, float size);
int ignisCreateFontConfig(IgnisFont* font, const char* path, float size, int first, int num, int bitmap_width, int bitmap_height);
void ignisDeleteFont(IgnisFont* font);

void ignisBindFont(IgnisFont* font, GLuint slot);

int ignisFontLoadCharQuad(IgnisFont* font, char c, float* x, float* y, float* vertices, size_t offset);

float ignisFontGetTextWidth(const IgnisFont* font, const char* text, size_t len);
float ignisFontGetHeight(const IgnisFont* font);

#ifdef __cplusplus
}
#endif

#endif /* !IGNIS_FONT_H */