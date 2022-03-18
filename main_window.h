#pragma once

#include <Windows.h>

constexpr int client_width = 400, client_height = 600;
constexpr int btn_w = 50, btn_h = 25, btn_x = 100, btn_y = 200;
constexpr WORD start_button = 1001;

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

            p->h_main_window = hwnd;
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

    main_window(HINSTANCE hInstance) : h_instance(hInstance) {}

    ~main_window()
    {
        UnhookWindowsHookEx(keyHook);
    }

    BOOL create(PCWSTR lpWindowName, DWORD dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, DWORD dwExStyle = WS_EX_TOPMOST,
                int x = CW_USEDEFAULT, int y = CW_USEDEFAULT,
                int nWidth = client_width, int nHeight = client_height,
                HWND hWndParent = nullptr, HMENU hMenu = nullptr);

    HWND get_window_handle() const { return h_main_window; }

protected:
    void on_start_button_click();

    BOOL save_file(LPCTSTR lpszFilePath, HBITMAP hBm);

    void setup_ui();

    inline PCWSTR class_name() const { return L"State of Decay Helper"; }
    LRESULT handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    HINSTANCE h_instance;

    HWND h_main_window;
    HWND h_start_button;
    HWND h_pause_button;
};
