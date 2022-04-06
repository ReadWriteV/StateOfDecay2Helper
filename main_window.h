#pragma once

#include <Windows.h>

#include <tesseract/baseapi.h>

constexpr int client_width = 300, client_height = 400;
constexpr int btn_w = 100, btn_h = 50, btn_x = (client_width - btn_w) / 2, btn_y = 200;
constexpr WORD start_button = 1001;

constexpr int t_x = 1580, t_y = 290, t_w = 200, t_h = 125;
constexpr int p_x = 1572, p_y = 804;

class main_window
{
public:
    static LRESULT CALLBACK keyProc(int nCode, WPARAM wParam, LPARAM lParam);

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    main_window(HINSTANCE hInstance) : h_instance(hInstance) {}

    ~main_window();

    BOOL create(PCWSTR lpWindowName, DWORD dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, DWORD dwExStyle = WS_EX_TOPMOST,
                int x = CW_USEDEFAULT, int y = CW_USEDEFAULT,
                int nWidth = client_width, int nHeight = client_height,
                HWND hWndParent = nullptr, HMENU hMenu = nullptr);

    HWND get_window_handle() const { return h_main_window; }

protected:
    void on_start_button_click();

    void setup_ui();

    inline PCWSTR class_name() const { return L"State of Decay Helper"; }
    LRESULT handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    HINSTANCE h_instance;

    HWND h_main_window;
    HWND h_start_button;
    HWND h_pause_button;

    tesseract::TessBaseAPI ocr;
};
