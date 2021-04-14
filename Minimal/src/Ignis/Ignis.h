#ifndef IGNIS_H
#define IGNIS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>


/* You can #define IGNIS_ASSERT(x) before the #include to avoid using assert.h */
#ifndef IGNIS_ASSERT
#include <assert.h>
#define IGNIS_ASSERT(x) assert(x)
#endif

/* defines */
#define IGNIS_SUCCESS	1
#define IGNIS_FAILURE	0

/* Core */
#include "Core/Texture.h"
#include "Core/Shader.h"
#include "Core/Buffer.h"

/* Font */
#define IGNIS_FONT_FIRST_CHAR		32
#define IGNIS_FONT_NUM_CHARS		96	/* ASCII 32..126 is 95 glyphs */
#define IGNIS_FONT_BITMAP_WIDTH		512
#define IGNIS_FONT_BITMAP_HEIGHT	512

#include "Font.h"

/* Vertex Array */
#define IGNIS_BUFFER_ARRAY_INITIAL_SIZE		4
#define IGNIS_BUFFER_ARRAY_GROWTH_FACTOR	2

#include "VertexArray.h"

int ignisInit(int debug);

typedef enum
{
	IGNIS_WARN = 0,
	IGNIS_ERROR = 1,
	IGNIS_CRITICAL = 2
} ignisErrorLevel;

void ignisSetErrorCallback(void (*callback)(ignisErrorLevel, const char*));
void _ignisErrorCallback(ignisErrorLevel level, const char* fmt, ...);

GLuint ignisGetOpenGLTypeSize(GLenum type);

int ignisEnableBlend(GLenum sfactor, GLenum dfactor);

/* Color */
typedef struct
{
	float r, g, b, a;
} IgnisColorRGBA;

extern const IgnisColorRGBA IGNIS_WHITE;
extern const IgnisColorRGBA IGNIS_BLACK;
extern const IgnisColorRGBA IGNIS_RED;
extern const IgnisColorRGBA IGNIS_GREEN;
extern const IgnisColorRGBA IGNIS_BLUE;
extern const IgnisColorRGBA IGNIS_CYAN;
extern const IgnisColorRGBA IGNIS_MAGENTA;
extern const IgnisColorRGBA IGNIS_YELLOW;

extern const IgnisColorRGBA IGNIS_DARK_GREY;
extern const IgnisColorRGBA IGNIS_LIGHT_GREY;

IgnisColorRGBA* ignisBlendColorRGBA(IgnisColorRGBA* color, float alpha);

void ignisSetClearColor(IgnisColorRGBA color);
void ignisClearColorBuffer(IgnisColorRGBA color);

char* ignisReadFile(const char* path, size_t* sizeptr);

/* Infos */
const char* ignisGetGLVersion();
const char* ignisGetGLVendor();
const char* ignisGetGLRenderer();
const char* ignisGetGLSLVersion();

/* Memory */
typedef void* (*ignisMallocCallback) (void* allocator, size_t size);
typedef void* (*ignisReallocCallback)(void* allocator, void* block, size_t size);
typedef void  (*ignisFreeCallback)   (void* allocator, void* block);

void ignisSetAllocator(void* allocator, ignisMallocCallback malloc, ignisReallocCallback realloc, ignisFreeCallback free);

void* ignisMalloc(size_t size);
void* ignisRealloc(void* block, size_t size);
void  ignisFree(void* block);

#ifdef __cplusplus
}
#endif

#endif /* !IGNIS_H */