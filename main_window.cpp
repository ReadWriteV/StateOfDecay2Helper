#include "main_window.h"

HHOOK keyHook;

BOOL main_window::create(PCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu)
{
    WNDCLASS wc = {0};

    wc.lpfnWndProc = main_window::WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = class_name();

    RegisterClass(&wc);

    hwnd = CreateWindowEx(
        dwExStyle, class_name(), lpWindowName, dwStyle, x, y,
        nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(NULL), this);

    keyHook = SetWindowsHookEx(WH_KEYBOARD_LL, main_window::keyProc, nullptr, 0);

    return (hwnd ? TRUE : FALSE);
}

LRESULT main_window::handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(window(), &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        EndPaint(window(), &ps);
    }
        return 0;

    default:
        return DefWindowProc(window(), uMsg, wParam, lParam);
    }
    return TRUE;
}
