#ifndef GUI_H
#define GUI_H

#include "math/math.h"
#include "Ignis/Ignis.h"

#include "toolbox/tb_mem.h"
#include "toolbox/tb_hashmap.h"

typedef struct
{
	mat4 screen_projection;
	
	tb_hashmap fonts;	/* <str,IgnisFont> */
} GuiManager;


int GuiInit(GuiManager* gui, float w, float h, const char* path, tb_allocator* allocator);
void GuiDestroy(GuiManager* gui);

IgnisFont* GuiAddFont(GuiManager* gui, const char* name, const char* path, float size);

IgnisFont* GuiGetFont(const GuiManager* gui, const char* name);
const char* GuiGetFontName(const GuiManager* gui, const IgnisFont* font);

void GuiSetViewport(GuiManager* gui, float w, float h);
const float* GuiGetScreenProjPtr(const GuiManager* gui);

#endif // !GUI_H
