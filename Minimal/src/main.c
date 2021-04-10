
#include "glad/glad.h"
#include "Minimal/MinimalWindow.h"

int main()
{
    MinimalWindow window;
    MinimalCreateWindow(&window, L"Minimal", 800, 600);

    gladLoadGL();
    glViewport(0, 0, 800, 600);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

    MINIMAL_INFO("[OpenGL] Version: %s",        glGetString(GL_VERSION));
    MINIMAL_INFO("[OpenGL] Vendor: %s",         glGetString(GL_VENDOR));
    MINIMAL_INFO("[OpenGL] Renderer: %s",       glGetString(GL_RENDERER));
    MINIMAL_INFO("[OpenGL] GLSL Version: %s",   glGetString(GL_SHADING_LANGUAGE_VERSION));

    while (!MinimalShouldClose(&window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        MinimalPollEvent(&window);
        MinimalSwapBuffer(&window);
    }

    MinimalDestroyWindow(&window);

    return 0;
}