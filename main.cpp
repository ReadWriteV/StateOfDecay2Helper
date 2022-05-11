#include "main_window.h"

#include <Windows.h>

main_window *app_window = nullptr;

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, INT nCmdShow)
{
    app_window = new main_window(hInstance);

    if (!app_window->create(L"State of Decay Helper"))
    {
        return 0;
    }

    app_window->show_window(nCmdShow);

    // Run the message loop.

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    delete app_window;
    return 0;
}
