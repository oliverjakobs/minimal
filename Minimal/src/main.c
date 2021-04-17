
#include "Ignis/Ignis.h"
#include "Minimal/MinimalWindow.h"
#include "Minimal/MinimalInput.h"

#include "Graphics/Renderer.h"
#include "GUI/Gui.h"
#include "gjk.h"

MinimalTimer timer;
MinimalWindow window;

mat4 view;

GuiManager gui;

const float SCREEN_WIDTH = 1200.0f;
const float SCREEN_HEIGHT = 800.0f;

const float WIDTH = 12.0f;
const float HEIGHT = 8.0f;
const gjk_vec2 CENTER = { 512.0f, 400.0f };
gjk_vec2 mouse;

gjk_shape circle;
gjk_shape poly;
gjk_shape triangle;
gjk_vec2 simplex[3];

uint8_t collision = 0;

gjk_vec2 triangle_verts[] =
{
    { 2.0f, 4.0f },
    { 1.2f, 2.0f },
    { 3.0f, 1.0f }
};

/*
gjk_vec2 poly_verts[] =
{
    { 400.0f, 300.0f },
    { 500.0f, 260.0f },
    { 600.0f, 300.0f },
    { 620.0f, 500.0f },
    { 480.0f, 500.0f }
};
*/

gjk_vec2 poly_verts[] =
{
    {  2.0f,  0.0f },
    {  1.0f,  1.75f },
    { -1.0f,  1.75f },
    { -2.0f,  0.0f },
    { -1.0f, -1.75f },
    {  1.0f, -1.75f }
};

gjk_vec2 GetMousePos()
{
    gjk_vec2 pos = { 0 };
    MinimalGetCursorPos(&window, &pos.x, &pos.y);

    pos.x = (pos.x / 100.0f) - WIDTH * .5f;
    pos.y = HEIGHT - (pos.y / 100.0f) - HEIGHT * .5f;

    return pos;
}

void RenderPoint(gjk_vec2 pos, IgnisColorRGBA color)
{
    Primitives2DRenderCircle(pos.x, pos.y, .02f, color);
}

void RenderCircle(gjk_shape* circle, IgnisColorRGBA color)
{
    Primitives2DRenderCircle(circle->center.x, circle->center.y, circle->radius, color);
    RenderPoint(circle->center, color);
}

void RenderPoly(gjk_shape* shape, IgnisColorRGBA color)
{
    Primitives2DRenderPolygon((float*)shape->vertices, shape->count * 2, color);
    RenderPoint(shape->center, color);
}


static void IgnisErrorCallback(ignisErrorLevel level, const char* desc)
{
    switch (level)
    {
    case IGNIS_WARN:		MINIMAL_WARN("%s", desc); break;
    case IGNIS_ERROR:		MINIMAL_ERROR("%s", desc); break;
    case IGNIS_CRITICAL:	MINIMAL_CRITICAL("%s", desc); break;
    }
}

int OnInit()
{
    /* ingis initialization */
    ignisSetErrorCallback(IgnisErrorCallback);

    if (!ignisInit(0))
    {
        MINIMAL_ERROR("[Ignis] Failed to initialize Ignis");
        return 0;
    }

    ignisEnableBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ignisSetClearColor(IGNIS_DARK_GREY);

    MINIMAL_INFO("[OpenGL] Version: %s",        ignisGetGLVersion());
    MINIMAL_INFO("[OpenGL] Vendor: %s",         ignisGetGLVendor());
    MINIMAL_INFO("[OpenGL] Renderer: %s",       ignisGetGLRenderer());
    MINIMAL_INFO("[OpenGL] GLSL Version: %s",   ignisGetGLSLVersion());

    Renderer2DInit("res/shaders/renderer2D.vert", "res/shaders/renderer2D.frag");
    Primitives2DInit("res/shaders/primitives.vert", "res/shaders/primitives.frag");
    BatchRenderer2DInit("res/shaders/batchrenderer.vert", "res/shaders/batchrenderer.frag");
    FontRendererInit("res/shaders/font.vert", "res/shaders/font.frag");

    GuiInit(&gui, SCREEN_WIDTH, SCREEN_HEIGHT, "res/fonts.json", NULL);
    FontRendererBindFontColor(GuiGetFont(&gui, "gui"), IGNIS_WHITE);

    view = mat4_ortho(-WIDTH * .5f, WIDTH * .5f, -HEIGHT * .5f, HEIGHT * .5f, -1.0f, 1.0f);

    gjk_circle(&circle, CENTER, 4.0f);
    gjk_poly(&triangle, triangle_verts, 3);
    gjk_poly(&poly, poly_verts, 6);

    MinimalTimerReset(&timer);

    return 1;
}

void OnDestroy()
{
    FontRendererDestroy();
    Primitives2DDestroy();
    BatchRenderer2DDestroy();
    Renderer2DDestroy();

    GuiDestroy(&gui);
}

void OnUpdate(float deltatime)
{
    mouse = GetMousePos();
    gjk_set_center(&triangle, mouse);

    collision = gjk_collision(&triangle, &poly, simplex);
}

void OnRender()
{
    Primitives2DStart(view.v);

    gjk_vec2 d = gjk_negate(mouse);
    Primitives2DRenderLineDir(CENTER.x, CENTER.y, d.x, d.y, 1.0f, IGNIS_WHITE);

    RenderPoly(&poly, IGNIS_WHITE);
    RenderPoint(gjk_furthest_point(&poly, gjk_negate(d)), IGNIS_WHITE);

    RenderPoly(&triangle, collision ? IGNIS_RED : IGNIS_WHITE);
    RenderPoint(gjk_furthest_point(&triangle, d), IGNIS_WHITE);

    if (collision)
    {
        Primitives2DRenderPolygon((float*)simplex, 6, IGNIS_GREEN);

        epa_edge edge;
        epa_closest_edge(simplex, 3, &edge);

        Primitives2DRenderLine(edge.p.x, edge.p.y, edge.q.x, edge.q.y, IGNIS_BLUE);

        gjk_vec2 n;
        float d = epa(&triangle, &poly, simplex, &n);
        Primitives2DRenderLineDir(triangle.center.x, triangle.center.y, n.x, n.y, d, IGNIS_RED);
    }

    Primitives2DFlush();
}

void OnRenderDebug()
{
    FontRendererStart(GuiGetScreenProjPtr(&gui));

    /* fps */
    FontRendererRenderTextFormat(8.0f, 8.0f, "FPS: %d", timer.fps);

    FontRendererFlush();
}

void KeyCallback(MinimalWindow* wnd, UINT keycode, UINT scancode, UINT action, UINT mods)
{
    if (action == MINIMAL_PRESS)
        MINIMAL_TRACE("KeyPressed: %d, %d", keycode, scancode);
    else
        MINIMAL_TRACE("KeyReleased : %d, %d", keycode, scancode);
}

void MouseButtonCallback(MinimalWindow* wnd, UINT button, UINT action, UINT mods)
{
    if (action == MINIMAL_PRESS)
        MINIMAL_TRACE("ButtonPressed: %d", button);
    else
        MINIMAL_TRACE("ButtonReleased : %d", button);
}

int main()
{
    MinimalCreateWindow(&window, "Minimal", (int)SCREEN_WIDTH, (int)SCREEN_HEIGHT);

    MinimalCreateKeyTable();

    MinimalSetKeyCallback(&window, KeyCallback);
    MinimalSetMButtonCallback(&window, MouseButtonCallback);

    if (!OnInit()) return -1;

    while (!MinimalShouldClose(&window))
    {
        MinimalTimerStart(&timer);

        glClear(GL_COLOR_BUFFER_BIT);

        if (MinimalKeyPressed(&window, MINIMAL_KEY_ESCAPE))
            MinimalCloseWindow(&window);

        OnUpdate(timer.deltatime);

        OnRender();
        OnRenderDebug();

        MinimalPollEvent(&window);
        MinimalSwapBuffer(&window);
        MinimalTimerEnd(&timer);
    }

    OnDestroy();
    MinimalDestroyWindow(&window);

    return 0;
}