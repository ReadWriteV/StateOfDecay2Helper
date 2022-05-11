#pragma once

#include <Windows.h>

#include <tesseract/baseapi.h>

constexpr int client_width = 300, client_height = 400;
constexpr int btn_w = 100, btn_h = 50, btn_x = 40, btn_y = 300;

constexpr WORD start_button = 1001;
constexpr WORD close_button = 1002;
constexpr WORD preview_box = 1003;

constexpr int t_x = 1580, t_y = 290, t_w = 200, t_h = 125;
constexpr int p_x = 1572, p_y = 804;

constexpr int pre_box_w = t_w, pre_box_h = t_h;
constexpr int pre_box_x = (client_width - pre_box_w) / 2, pre_box_y = 10;

class main_window
{
public:
    static LRESULT CALLBACK keyProc(int nCode, WPARAM wParam, LPARAM lParam);

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    main_window(HINSTANCE hInstance) : h_instance(hInstance) {}

    ~main_window();

    BOOL create(PCWSTR lpWindowName, DWORD dwStyle = WS_SYSMENU | WS_CAPTION & ~WS_MINIMIZEBOX, DWORD dwExStyle = WS_EX_TOPMOST,
                int x = CW_USEDEFAULT, int y = CW_USEDEFAULT,
                int nWidth = client_width, int nHeight = client_height,
                HWND hWndParent = nullptr, HMENU hMenu = nullptr);

    HWND get_window_handle() const { return h_main_window; }

    BOOL show_window(INT nCmdShow) const { return ShowWindow(h_main_window, nCmdShow); }

protected:
    void on_start_button_click();

    void setup_ui();

    inline PCWSTR class_name() const { return L"State of Decay Helper"; }
    LRESULT handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    HINSTANCE h_instance;

    HWND h_main_window;
    HWND h_start_button;
    HWND h_close_button;

    HWND h_preview_box;
    HWND h_text_box;

    tesseract::TessBaseAPI ocr;

    HBITMAP preview_image;
};
