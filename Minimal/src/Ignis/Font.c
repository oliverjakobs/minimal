#include "Font.h"

#include "Ignis.h"

#define STBTT_malloc(x,u)  ((void)(u),ignisMalloc(x))
#define STBTT_free(x,u)    ((void)(u),ignisFree(x))
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"

int ignisCreateFont(IgnisFont* font, const char* path, float size)
{
	return ignisCreateFontConfig(font, path, size, IGNIS_FONT_FIRST_CHAR, IGNIS_FONT_NUM_CHARS, IGNIS_FONT_BITMAP_WIDTH, IGNIS_FONT_BITMAP_HEIGHT);
}

int ignisCreateFontConfig(IgnisFont* font, const char* path, float size, int first, int num, int bitmap_width, int bitmap_height)
{
	if (!font) return 0;

	char* buffer = ignisReadFile(path, NULL);
	if (!buffer)
	{
		_ignisErrorCallback(IGNIS_ERROR, "[Font] Failed to read file: %s", path);
		return 0;
	}

	GLubyte* bitmap = ignisMalloc(sizeof(GLubyte) * bitmap_width * bitmap_height);
	if (!bitmap)
	{
		_ignisErrorCallback(IGNIS_ERROR, "[Font] Failed to allocate memory for bitmap");
		ignisFree(buffer);
		return 0;
	}

	/* init stbfont */
	stbtt_fontinfo info;
	if (!stbtt_InitFont(&info, buffer, 0))
	{
		_ignisErrorCallback(IGNIS_ERROR, "[Font] Failed to load font info: %s", path);
		ignisFree(bitmap);
		ignisFree(buffer);
		return 0;
	}

	font->first_char = first;
	font->num_chars = num;

	/* load char data */
	font->char_data = ignisMalloc(sizeof(stbtt_bakedchar) * font->num_chars);
	if (!font->char_data)
	{
		_ignisErrorCallback(IGNIS_ERROR, "[Font] Failed to allocate memory for char data");
		ignisFree(bitmap);
		ignisFree(buffer);
		return 0;
	}

	/* get height and scale */
	int ascent, descent, linegap;
	stbtt_GetFontVMetrics(&info, &ascent, &descent, &linegap);
	float scale = stbtt_ScaleForMappingEmToPixels(&info, size);
	font->height = (ascent - descent + linegap) * scale + 0.5;

	/* load glyphs */
	float s = stbtt_ScaleForMappingEmToPixels(&info, 1) / stbtt_ScaleForPixelHeight(&info, 1);
	stbtt_BakeFontBitmap(buffer, 0, size * s, bitmap, bitmap_width, bitmap_height, font->first_char, font->num_chars, font->char_data);

	ignisFree(buffer);

	IgnisTextureConfig config = IGNIS_DEFAULT_CONFIG;
	config.internal_format = GL_R8;
	config.format = GL_RED;

	int tex_name = ignisGenerateTexture2D(&font->texture, bitmap_width, bitmap_height, bitmap, &config);
	if (tex_name == 0)
	{
		_ignisErrorCallback(IGNIS_ERROR, "[Font] Failed to create texture");
		ignisFree(font->char_data);
	}

	ignisFree(bitmap);
	return tex_name;
}

void ignisDeleteFont(IgnisFont* font)
{
	if (font->char_data) ignisFree(font->char_data);
	ignisDeleteTexture2D(&font->texture);
}

void ignisBindFont(IgnisFont* font, GLuint slot)
{
	ignisBindTexture2D(font ? &font->texture : NULL, slot);
}

int ignisFontLoadCharQuad(IgnisFont* font, char c, float* x, float* y, float* vertices, size_t offset)
{
	if (c >= font->first_char && c < font->first_char + font->num_chars)
	{
		stbtt_aligned_quad q;
		stbtt_GetBakedQuad(font->char_data, font->texture.width, font->texture.height, c - font->first_char, x, y, &q, 1);

		vertices[offset + 0] = q.x0;
		vertices[offset + 1] = q.y0;
		vertices[offset + 2] = q.s0;
		vertices[offset + 3] = q.t0;
		vertices[offset + 4] = q.x0;
		vertices[offset + 5] = q.y1;
		vertices[offset + 6] = q.s0;
		vertices[offset + 7] = q.t1;
		vertices[offset + 8] = q.x1;
		vertices[offset + 9] = q.y1;
		vertices[offset + 10] = q.s1;
		vertices[offset + 11] = q.t1;
		vertices[offset + 12] = q.x1;
		vertices[offset + 13] = q.y0;
		vertices[offset + 14] = q.s1;
		vertices[offset + 15] = q.t0;

		return 1;
	}

	return 0;
}

float ignisFontGetTextWidth(const IgnisFont* font, const char* text, size_t len)
{
	float x = 0.0f;
	float y = 0.0f;
	for (size_t i = 0; i < len; i++)
	{
		if (text[i] >= font->first_char && text[i] < font->first_char + font->num_chars)
		{
			stbtt_aligned_quad q;
			stbtt_GetBakedQuad(font->char_data, font->texture.width, font->texture.height, text[i] - font->first_char, &x, &y, &q, 1);
		}
	}
	return x;
}

float ignisFontGetHeight(const IgnisFont* font) { return font->height; }
