#include "main_window.h"

#include <Windows.h>
#include <memory>

main_window *app_window_ptr = nullptr;

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, INT nCmdShow)
{
    auto app_window = std::make_unique<main_window>(hInstance);
    app_window_ptr = app_window.get();

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
    return 0;
}
