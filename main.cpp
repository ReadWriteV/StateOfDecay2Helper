#include "main_window.h"

#include <Windows.h>

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, INT nCmdShow)
{
    main_window win;

    if (!win.Create(L"State of Decay Helper", WS_OVERLAPPEDWINDOW))
    {
        return 0;
    }

    ShowWindow(win.Window(), nCmdShow);

    // Run the message loop.

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}