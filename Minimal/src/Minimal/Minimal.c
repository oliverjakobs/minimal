#include "Minimal.h"

static uint64_t     _minimal_timer_frequency;
static uint64_t     _minimal_timer_offset;

static MinimalWindow* _minimal_current_context;

static uint64_t MinimalGetTimeValue()
{
    uint64_t value;
    QueryPerformanceCounter((LARGE_INTEGER*)&value);
    return value;
}

uint8_t MinimalInit()
{
    MinimalCreateKeyTable();

    /* init timer */
    if (!QueryPerformanceFrequency((LARGE_INTEGER*)&_minimal_timer_frequency))
    {
        MINIMAL_ERROR("High-resolution performance counter is not supported");
        return MINIMAL_FAIL;
    }

    _minimal_timer_offset = MinimalGetTimeValue();

    _minimal_current_context = NULL;

    return MINIMAL_OK;
}

double MinimalGetTime()
{
    return (double)(MinimalGetTimeValue() - _minimal_timer_offset) / _minimal_timer_frequency;
}

void MinimalMakeContextCurrent(MinimalWindow* context)	{ _minimal_current_context = context; }
MinimalWindow* MinimalGetCurrentContext()				{ return _minimal_current_context; }

/*
 * -----------------------------------------------------------------------------------
 *                               Minimal App
 * -----------------------------------------------------------------------------------
 */
#include "toolbox/tb_str.h"


static void MinimalGetGLVersion(char* version_str, int* major, int* minor)
{
    const char* sep = ".";
    char* maj_str = tb_strsep(&version_str, sep);
    char* min_str = tb_strsep(&version_str, sep);

    if (!maj_str || !min_str) return;

    *major = atoi(maj_str);
    *minor = atoi(min_str);
}

uint8_t MinimalLoad(MinimalApp* app, const char* title, int width, int height, char* gl_version)
{
	app->title = tb_strdup(title);
	if (!app->title)
	{
		MINIMAL_ERROR("[GLFW] Failed to allocate memory for title");
		return 0;
	}

	app->running = 0;
	app->debug = 0;
	app->vsync = 0;

	/* Minimal initialization */
	if (!MinimalInit())
	{
		MINIMAL_ERROR("[Minimal] Failed to initialize Minimal");
		return 0;
	}

	MINIMAL_INFO("[Minimal] Initialized Minimal");

	int gl_major, gl_minor;
	char* temp = tb_strdup(gl_version);
	MinimalGetGLVersion(temp, &gl_major, &gl_minor);
	free(temp);

	/* creating the window */
	if (!MinimalCreateWindow(&app->window, "Minimal", width, height))
	{
		MINIMAL_ERROR("[Minimal] Failed to create Minimal window");
		return 0;
	}

	MinimalMakeContextCurrent(&app->window);
	MinimalTimerReset(&app->timer);

	app->running = (app->on_load) ? app->on_load(app) : 1;
	return app->running;
}

void MinimalDestroy(MinimalApp* app)
{
	if (app->on_destroy) app->on_destroy(app);
	MinimalDestroyWindow(&app->window);
}

void MinimalRun(MinimalApp* app, void(*clear_buffer)())
{
	while (app->running)
	{
		MinimalTimerStart(&app->timer, MinimalGetTime());
		// InputUpdate(app->window);

		app->on_update(app, (float)app->timer.deltatime);

		if (clear_buffer) clear_buffer();

		app->on_render(app);

		if (app->debug) app->on_render_debug(app);

		MinimalPollEvent(&app->window);
		MinimalSwapBuffer(&app->window);

		MinimalTimerEnd(&app->timer, MinimalGetTime());
	}
}

void MinimalClose(MinimalApp* app) { app->running = 0; }
void MinimalEnableDebug(MinimalApp* app, int b) { app->debug = b; }
void MinimalEnableVsync(MinimalApp* app, int b) { /* glfwSwapInterval(b); */ app->vsync = b; }

void MinimalToggleDebug(MinimalApp* app) { MinimalEnableDebug(app, !app->debug); }
void MinimalToggleVsync(MinimalApp* app) { MinimalEnableVsync(app, !app->vsync); }

uint32_t MinimalGetFps(MinimalApp* app)
{
	return app->timer.fps;
}

void MinimalSetLoadCallback(MinimalApp* app, MinimalLoadCB cb)               { app->on_load = cb; }
void MinimalSetDestroyCallback(MinimalApp* app, MinimalDestroyCB cb)         { app->on_destroy = cb; }
void MinimalSetUpdateCallback(MinimalApp* app, MinimalUpdateCB cb)           { app->on_update = cb; }
void MinimalSetRenderCallback(MinimalApp* app, MinimalRenderCB cb)           { app->on_render = cb; }
void MinimalSetRenderDebugCallback(MinimalApp* app, MinimalRenderDebugCB cb) { app->on_render_debug = cb; }
void MinimalSetRenderGUICallback(MinimalApp* app, MinimalRenderGUICB cb)     { app->on_render_gui = cb; }
