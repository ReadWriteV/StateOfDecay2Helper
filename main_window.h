#pragma once

#include <Windows.h>

constexpr int width = 400, height = 600;

extern HHOOK keyHook;

class main_window
{
public:
    static LRESULT CALLBACK keyProc(int nCode, WPARAM wParam, LPARAM lParam)
    {
        KBDLLHOOKSTRUCT *pkbhs = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam);

        if (pkbhs->vkCode == VK_ESCAPE)
        {
            // stop rolling
        }
        return CallNextHookEx(keyHook, nCode, wParam, lParam);
    }

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        main_window *p = nullptr;

        if (uMsg == WM_NCCREATE)
        {
            CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT *>(lParam);
            p = reinterpret_cast<main_window *>(pCreate->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(p));

            p->hwnd = hwnd;
        }
        else
        {
            p = reinterpret_cast<main_window *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        }
        if (p)
        {
            return p->handle_message(uMsg, wParam, lParam);
        }
        else
        {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }

    ~main_window()
    {
        UnhookWindowsHookEx(keyHook);
    }

    BOOL create(PCWSTR lpWindowName, DWORD dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, DWORD dwExStyle = WS_EX_TOPMOST,
                int x = CW_USEDEFAULT, int y = CW_USEDEFAULT,
                int nWidth = width, int nHeight = height,
                HWND hWndParent = 0, HMENU hMenu = 0);
    HWND window() const { return hwnd; }

protected:
    inline PCWSTR class_name() const { return L"State of Decay Helper"; }
    LRESULT handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    HWND hwnd;
};
