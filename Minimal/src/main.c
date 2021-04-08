
#include "glad/glad.h"
#include "Minimal/MinimalWindow.h"

int main()
{
    MinimalWindow window;
    MinimalCreateWindow(&window, L"Minimal", 800, 600);

    gladLoadGL();
    glViewport(0, 0, 800, 600);

    while (!MinimalShouldClose(&window))
    {
        MinimalPollEvent(&window);
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        MinimalSwapBuffer(&window);
    }

    MinimalDestroyWindow(&window);

    return 0;
}