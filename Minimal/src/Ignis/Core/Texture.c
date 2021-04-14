#include "Texture.h"

#include "../Ignis.h"

#define STBI_MALLOC(size)			ignisMalloc(size)
#define STBI_REALLOC(block, size)	ignisRealloc(block, size)
#define STBI_FREE(block)			ignisFree(block)
#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"

int ignisGenerateTexture2D(IgnisTexture2D* texture, int width, int height, void* pixels, IgnisTextureConfig* configptr)
{
	if (!texture) return IGNIS_FAILURE;

	IgnisTextureConfig config = configptr ? *configptr : IGNIS_DEFAULT_CONFIG;
	texture->name = ignisGenerateTexture(GL_TEXTURE_2D, width, height, pixels, config);
	texture->width = width;
	texture->height = height;
	texture->rows = 1;
	texture->cols = 1;

	return texture->name;
}

int ignisGenerateTexStorage2D(IgnisTexture2D* texture, int width, int height, GLenum internal_format)
{
	if (!texture) return IGNIS_FAILURE;

	glGenTextures(1, &texture->name);
	glBindTexture(GL_TEXTURE_2D, texture->name);
	glTexStorage2D(GL_TEXTURE_2D, 8, internal_format, width, height);

	texture->width = width;
	texture->height = height;
	texture->rows = 1;
	texture->cols = 1;

	return texture->name;
}

int ignisCreateTexture2D(IgnisTexture2D* texture, const char* path, GLuint rows, GLuint columns, int flip_on_load, IgnisTextureConfig* configptr)
{
	if (!texture) return IGNIS_FAILURE;

	IgnisTextureConfig config = configptr ? *configptr : IGNIS_DEFAULT_CONFIG;

	stbi_set_flip_vertically_on_load(flip_on_load);

	int bpp = 0;

	size_t buffer_len;
	char* buffer = ignisReadFile(path, &buffer_len);

	unsigned char* pixels = stbi_load_from_memory(buffer, buffer_len, &texture->width, &texture->height, &bpp, 4);

	ignisFree(buffer);

	if (pixels)
	{
		/* check if bpp and format matches */
		if (bpp == 4 && (config.format != GL_RGBA || config.internal_format != GL_RGBA8))
			_ignisErrorCallback(IGNIS_WARN, "[Texture] Format mismatch for %s", path);
		else if (bpp == 3 && (config.format != GL_RGB || config.internal_format != GL_RGB8))
			_ignisErrorCallback(IGNIS_WARN, "[Texture] Format mismatch for %s", path);

		texture->name = ignisGenerateTexture(GL_TEXTURE_2D, texture->width, texture->height, pixels, config);
		texture->rows = rows;
		texture->cols = columns;

		stbi_image_free(pixels);

		return texture->name;
	}

	_ignisErrorCallback(IGNIS_ERROR, "[Texture] Failed to load texture(%s): %s", path, stbi_failure_reason());

	return IGNIS_FAILURE;
}

void ignisDeleteTexture2D(IgnisTexture2D* texture)
{
	glDeleteTextures(1, &texture->name);
}

GLuint ignisGenerateTexture(GLuint target, int width, int height, void* pixels, IgnisTextureConfig config)
{
	GLuint name;
	glGenTextures(1, &name);
	glBindTexture(target, name);

	glTexParameteri(target, GL_TEXTURE_WRAP_S, config.wrap_s);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, config.wrap_t);

	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, config.min_filter);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, config.mag_filter);

	switch (target)
	{
	case GL_TEXTURE_1D:
	case GL_PROXY_TEXTURE_1D:
		glTexImage1D(target, 0, config.internal_format, width, 0, config.format, GL_UNSIGNED_BYTE, pixels);
		break;
	case GL_TEXTURE_2D:
	case GL_PROXY_TEXTURE_2D:
	case GL_TEXTURE_1D_ARRAY:
	case GL_PROXY_TEXTURE_1D_ARRAY:
	case GL_TEXTURE_RECTANGLE:
	case GL_PROXY_TEXTURE_RECTANGLE:
	case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
	case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
	case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
	case GL_PROXY_TEXTURE_CUBE_MAP:
		glTexImage2D(target, 0, config.internal_format, width, height, 0, config.format, GL_UNSIGNED_BYTE, pixels);
		break;
	default:
		_ignisErrorCallback(IGNIS_ERROR, "[Texture] Unsupported target (%d)", target);
		glDeleteTextures(1, &name);
		return IGNIS_FAILURE;
	}
	glGenerateMipmap(target);

	return name;
}

void ignisBindTexture2D(const IgnisTexture2D* texture, GLuint slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, (texture) ? texture->name : 0);
}

