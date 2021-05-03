#include "Minimal.h"

#include "MinimalWGL.h"
#include "MinimalWindow.h"

static uint64_t     _minimal_timer_frequency = 0;
static uint64_t     _minimal_timer_offset = 0;

static MinimalWindow* _minimal_current_context = NULL;

static uint64_t MinimalGetTimeValue()
{
    uint64_t value;
    QueryPerformanceCounter((LARGE_INTEGER*)&value);
    return value;
}

MinimalBool MinimalInit()
{
    MinimalCreateKeyTable();

    /* init timer */
    if (!QueryPerformanceFrequency((LARGE_INTEGER*)&_minimal_timer_frequency))
    {
        MinimalErrorCallback(MINIMAL_LOG_ERROR, "Timer", "High-resolution performance counter is not supported");
        return MINIMAL_FAIL;
    }

    _minimal_timer_offset = MinimalGetTimeValue();
    _minimal_current_context = NULL;

	if (!MinimalRegisterWindowClass())	return MINIMAL_FAIL;
	if (!MinimalCreateHelperWindow())	return MINIMAL_FAIL;

	if (!MinimalInitWGL())
	{
		MinimalErrorCallback(MINIMAL_LOG_ERROR, "Minimal", "Failed to initialize WGL");
		return MINIMAL_FAIL;
	}

    return MINIMAL_OK;
}

void MinimalTerminate()
{
	MinimalTerminateWGL();
	MinimalDestroyHelperWindow();
	MinimalUnregisterWindowClass();
}

double MinimalGetTime()
{
    return (double)(MinimalGetTimeValue() - _minimal_timer_offset) / _minimal_timer_frequency;
}

void MinimalMakeContextCurrent(MinimalWindow* context)	{ _minimal_current_context = context; }
MinimalWindow* MinimalGetCurrentContext()				{ return _minimal_current_context; }

void MinimalGetVersion(int* major, int* minor, int* rev)
{
	if (major != NULL) *major = MINIMAL_VERSION_MAJOR;
	if (minor != NULL) *minor = MINIMAL_VERSION_MINOR;
	if (rev != NULL)   *rev = MINIMAL_VERSION_REVISION;
}

#define MINIMAL_CONCAT_VERSION(m, n, r) #m "." #n "." #r
#define MINIMAL_MAKE_VERSION_STR(m, n, r) MINIMAL_CONCAT_VERSION(m, n, r)

const char* MinimalGetVersionString(void)
{
	return MINIMAL_MAKE_VERSION_STR(MINIMAL_VERSION_MAJOR, MINIMAL_VERSION_MINOR, MINIMAL_VERSION_REVISION);
}

/* --------------------------| minimal app |----------------------------- */
static void MinimalGetGLVersion(const char* version_str, int* major, int* minor)
{
	const char* sep = ".";
	const char* major_str = version_str;
	const char* minor_str = version_str + strcspn(version_str, sep) + 1;

	if (major_str && major) *major = atoi(major_str);
	if (minor_str && minor) *minor = atoi(minor_str);
}

MinimalBool MinimalLoad(MinimalApp* app, const char* title, uint32_t w, uint32_t h, const char* gl)
{
	app->debug = 0;
	app->vsync = 0;

	/* minimal initialization */
	if (!MinimalInit())
	{
		MinimalErrorCallback(MINIMAL_LOG_ERROR, "App", "Failed to initialize Minimal");
		return MINIMAL_FAIL;
	}

	MinimalWindowConfig wnd_config = { 0 };

#ifdef _DEBUG
	wnd_config.context_flags |= WGL_CONTEXT_DEBUG_BIT_ARB;
#endif // _DEBUG

	MinimalGetGLVersion(gl, &wnd_config.gl_major, &wnd_config.gl_minor);

	/* creating the window */
	app->window = MinimalCreateWindow(title, w, h, &wnd_config);
	if (!app->window)
	{
		MinimalErrorCallback(MINIMAL_LOG_ERROR, "App", "Failed to create Minimal window");
		return MINIMAL_FAIL;
	}

	MinimalSetEventDispatch(app->window, app, MinimalDispatchEvent);

	MinimalMakeContextCurrent(app->window);
	MinimalTimerReset(&app->timer);

	return (app->on_load) ? app->on_load(app, w, h) : MINIMAL_OK;
}

void MinimalDestroy(MinimalApp* app)
{
	if (app->on_destroy) app->on_destroy(app);
	MinimalDestroyWindow(app->window);
}

void MinimalRun(MinimalApp* app, void(*clear_buffer)())
{
	while (!MinimalShouldCloseWindow(app->window))
	{
		MinimalTimerStart(&app->timer, MinimalGetTime());
		MinimalWindowUpdateKeyStates(app->window);

		app->on_update(app, (float)app->timer.deltatime);

		if (clear_buffer) clear_buffer();

		app->on_render(app);

		if (app->debug) app->on_render_debug(app);

		MinimalPollEvent(app->window);
		MinimalSwapBuffer(app->window);

		MinimalTimerEnd(&app->timer, MinimalGetTime());
	}
}

void MinimalClose(MinimalApp* app) { MinimalCloseWindow(app->window); }

void MinimalSetLoadCallback(MinimalApp* app, MinimalLoadCB cb)			{ app->on_load = cb; }
void MinimalSetDestroyCallback(MinimalApp* app, MinimalDestroyCB cb)	{ app->on_destroy = cb; }
void MinimalSetEventCallback(MinimalApp* app, MinimalEventCB cb)		{ app->on_event = cb; }
void MinimalSetUpdateCallback(MinimalApp* app, MinimalUpdateCB cb)		{ app->on_update = cb; }
void MinimalSetRenderCallback(MinimalApp* app, MinimalRenderCB cb)		{ app->on_render = cb; }
void MinimalSetRenderDebugCallback(MinimalApp* app, MinimalRenderCB cb) { app->on_render_debug = cb; }
void MinimalSetRenderGUICallback(MinimalApp* app, MinimalRenderCB cb)	{ app->on_render_gui = cb; }

/* --------------------------| settings |-------------------------------- */
void MinimalSetTitle(MinimalApp* app, const char* title) { MinimalSetWindowTitle(app->window, title); }
void MinimalEnableDebug(MinimalApp* app, MinimalBool b)  { app->debug = b; }
void MinimalEnableVsync(MinimalApp* app, MinimalBool b)  { MinimalSwapIntervalWGL(b); app->vsync = b; }

void MinimalToggleDebug(MinimalApp* app) { MinimalEnableDebug(app, !app->debug); }
void MinimalToggleVsync(MinimalApp* app) { MinimalEnableVsync(app, !app->vsync); }

uint32_t MinimalGetFps(const MinimalApp* app)	{ return app->timer.fps; }

void MinimalDispatchEvent(MinimalApp* app, uint32_t type, uint32_t uParam, int32_t lParam, int32_t rParam)
{
	MinimalEvent e = { .type = type, .uParam = uParam, .lParam = lParam, .rParam = rParam };
	if (app->on_event) app->on_event(app, &e);
}

/* --------------------------| input |----------------------------------- */
MinimalBool MinimalKeyPressed(uint32_t keycode)
{
	const MinimalInputState* state = MinimalWindowGetKeyState(_minimal_current_context, keycode);
	return state && state->current == MINIMAL_PRESS && state->previous == MINIMAL_RELEASE;
}

MinimalBool MinimalKeyReleased(uint32_t keycode)
{
	const MinimalInputState* state = MinimalWindowGetKeyState(_minimal_current_context, keycode);
	return state && state->current == MINIMAL_RELEASE && state->previous == MINIMAL_PRESS;
}

MinimalBool MinimalKeyDown(uint32_t keycode)
{
	const MinimalInputState* state = MinimalWindowGetKeyState(_minimal_current_context, keycode);
	return state && state->action == MINIMAL_PRESS;
}

MinimalBool MinimalMouseButtonPressed(uint32_t button)
{
	int8_t state = MinimalWindowGetMouseButtonState(_minimal_current_context, button);
	return state >= 0 && state == MINIMAL_PRESS;
}

MinimalBool MinimalMouseButtonReleased(uint32_t button)
{
	int8_t state = MinimalWindowGetMouseButtonState(_minimal_current_context, button);
	return state >= 0 && state == MINIMAL_RELEASE;
}

void MinimalGetCursorPos(float* x, float* y)
{
	MinimalWindow* window = MinimalGetCurrentContext();
	if (window) MinimalWindowGetCursorPos(window, x, y);
}