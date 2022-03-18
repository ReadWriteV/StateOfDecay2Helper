#include "main_window.h"

#include <Windows.h>

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, INT nCmdShow)
{
    main_window win(hInstance);

    if (!win.create(L"State of Decay Helper"))
    {
        return 0;
    }

    ShowWindow(win.get_window_handle(), nCmdShow);

    // Run the message loop.

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
