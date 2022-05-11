﻿#include "main_window.h"

HHOOK keyHook;

extern main_window *app_window;

LRESULT CALLBACK main_window::keyProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    KBDLLHOOKSTRUCT *pkbhs = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam);

    if (pkbhs->vkCode == VK_ESCAPE)
    {
        // stop rolling
    }
    return CallNextHookEx(keyHook, nCode, wParam, lParam);
}

LRESULT CALLBACK main_window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

main_window::~main_window()
{
    // UnhookWindowsHookEx(keyHook);
    if (preview_image)
    {
        DeleteObject(preview_image);
    }
}

BOOL main_window::create(PCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu)
{
    WNDCLASS wc = {0};

    wc.lpfnWndProc = main_window::WindowProc;
    wc.hInstance = h_instance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.lpszClassName = class_name();

    RegisterClass(&wc);

    RECT rect_size;

    rect_size.left = 0;
    rect_size.top = 0;
    rect_size.right = nWidth;
    rect_size.bottom = nHeight;

    AdjustWindowRect(&rect_size, dwStyle, hMenu != nullptr);

    h_main_window = CreateWindowEx(
        dwExStyle, class_name(), lpWindowName, dwStyle, x, y,
        rect_size.right - rect_size.left, rect_size.bottom - rect_size.top, hWndParent, hMenu, h_instance, this);

    // keyHook = SetWindowsHookEx(WH_KEYBOARD_LL, main_window::keyProc, nullptr, 0);

    if (ocr.Init("./tessdata", "chi_sim"))
    {
        MessageBox(h_main_window, L"Init tesseract failed!", L"info", MB_OK);
        PostQuitMessage(0);
    }

    return (h_main_window ? TRUE : FALSE);
}

void main_window::setup_ui()
{
    h_preview_box = CreateWindow(
        L"STATIC",
        NULL,
        WS_VISIBLE | WS_CHILD | WS_BORDER | SS_BITMAP, // Styles
        pre_box_x,                                     // x position
        pre_box_y,                                     // y position
        pre_box_w,                                     // width
        pre_box_h,                                     // height
        h_main_window,                                 // Parent window
        reinterpret_cast<HMENU>(preview_box),
        h_instance,
        NULL);

    h_text_box = CreateWindow(
        L"STATIC",
        L"Result ...",
        WS_VISIBLE | WS_CHILD | WS_BORDER, // Styles
        pre_box_x,                         // x position
        pre_box_y + pre_box_h + 10,        // y position
        pre_box_w,                         // width
        pre_box_h,                         // height
        h_main_window,                     // Parent window
        NULL,
        h_instance,
        NULL);

    h_start_button = CreateWindow(
        L"BUTTON",                             // Predefined class; Unicode assumed
        L"START",                              // Button text
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, // Styles
        btn_x,                                 // x position
        btn_y,                                 // y position
        btn_w,                                 // Button width
        btn_h,                                 // Button height
        h_main_window,                         // Parent window
        reinterpret_cast<HMENU>(start_button),
        h_instance,
        NULL);

    h_close_button = CreateWindow(
        L"BUTTON",                             // Predefined class; Unicode assumed
        L"CLOSE",                              // Button text
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, // Styles
        btn_x + btn_w + 20,                    // x position
        btn_y,                                 // y position
        btn_w,                                 // Button width
        btn_h,                                 // Button height
        h_main_window,                         // Parent window
        reinterpret_cast<HMENU>(close_button),
        h_instance,
        NULL);
}

LRESULT main_window::handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        setup_ui();
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(h_main_window, &ps);
        // FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_GRAYTEXT + 1));
        EndPaint(h_main_window, &ps);
        return 0;
    }

    case WM_COMMAND:
    {
        if (LOWORD(wParam) == start_button)
        {
            on_start_button_click();
        }
        else if (LOWORD(wParam) == close_button)
        {
            PostQuitMessage(0);
        }
        return 0;
    }

    default:
        return DefWindowProc(h_main_window, uMsg, wParam, lParam);
    }
    return DefWindowProc(h_main_window, uMsg, wParam, lParam);
}

void main_window::on_start_button_click()
{
    // SetWindowText(h_start_button, L"Pause (ESC)");

    HDC hdc = GetDC(NULL);

    // int screenx = GetSystemMetrics(SM_CXSCREEN);
    // int screeny = GetSystemMetrics(SM_CYSCREEN);

    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, t_w, t_h);
    HDC hdcMem = CreateCompatibleDC(hdc);

    SelectObject(hdcMem, hBitmap);
    BitBlt(hdcMem, 0, 0, t_w, t_h, hdc, t_x, t_y, SRCCOPY);

    BITMAP bm;
    GetObject(hBitmap, sizeof(bm), &bm);
    if (bm.bmBitsPixel == 32)
    {
        SetWindowText(h_text_box, L"32 BitsPixel");
    }

    // note: 24bit bmp image
    // HBITMAP hBitmap = reinterpret_cast<HBITMAP>(LoadImage(NULL, L"m.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));

    // TODO: convert 32bit bitmap to 1 bit gray image

    HBITMAP hold = reinterpret_cast<HBITMAP>(SendMessage(h_preview_box, STM_SETIMAGE, IMAGE_BITMAP, reinterpret_cast<LPARAM>(hBitmap)));

    // clear the old image
    if (hold && hold != hBitmap)
    {
        DeleteObject(hold);
    }

    // unsigned char *image_data = new unsigned char[t_w * t_h * 4];

    // GetBitmapBits(hBitmap, t_w * t_h * 4, image_data);
    // cv::Mat image(t_h, t_w, CV_8UC4, image_data);
    // cv::Mat image_gray;
    // cv::cvtColor(image, image_gray, cv::COLOR_BGRA2GRAY);
    // cv::Mat dst;
    // cv::threshold(image_gray, dst, 70, 255, cv::THRESH_BINARY);

    // cv::imshow("image.jpg", dst);
    // cv::waitKey(0);

    // ocr.SetImage(dst.data, dst.cols, dst.rows, dst.channels(), dst.cols);
    // char *utf8_text = ocr.GetUTF8Text();

    // int wcscLen = MultiByteToWideChar(CP_UTF8, NULL, utf8_text, static_cast<int>(strlen(utf8_text)), NULL, 0);
    // wchar_t *wszcString = new wchar_t[wcscLen + 1];
    // MultiByteToWideChar(CP_UTF8, NULL, utf8_text, static_cast<int>(strlen(utf8_text)), wszcString, wcscLen);
    // wszcString[wcscLen] = '\0';
    // MessageBox(h_main_window, wszcString, L"info", MB_OK);

    DeleteObject(hBitmap);
    ReleaseDC(h_main_window, hdcMem);
    ReleaseDC(h_main_window, hdc);
    // delete[] utf8_text;
    // delete[] wszcString;
}
