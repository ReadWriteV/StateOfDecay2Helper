#include "main_window.h"

LRESULT main_window::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(Window(), &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        EndPaint(Window(), &ps);
    }
        return 0;

    default:
        return DefWindowProc(Window(), uMsg, wParam, lParam);
    }
    return TRUE;
}