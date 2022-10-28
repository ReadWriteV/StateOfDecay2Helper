#include "main_window.h"

#include <memory>
#include <cassert>
#include <string>

extern main_window *app_window_ptr;

LRESULT CALLBACK main_window::keyProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    KBDLLHOOKSTRUCT *pkbhs = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam);

    // press ESC to stop rolling
    if (pkbhs->vkCode == VK_ESCAPE)
    {
        // stop rolling
        app_window_ptr->stop_rolling();
    }
    return CallNextHookEx(app_window_ptr->get_hook_handle(), nCode, wParam, lParam);
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

main_window::main_window(HINSTANCE hInstance) : h_instance(hInstance), is_rolling(false), is_running(true)
{
}

main_window::~main_window()
{
    is_rolling = false;
    is_running = false;
    UnhookWindowsHookEx(keyHook);
    rolling_thread.join();
}

BOOL main_window::create(PCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int nWidth, int nHeight,
                         HWND hWndParent, HMENU hMenu)
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

    h_main_window =
        CreateWindowEx(dwExStyle, class_name(), lpWindowName, dwStyle, x, y, rect_size.right - rect_size.left,
                       rect_size.bottom - rect_size.top, hWndParent, hMenu, h_instance, this);

    keyHook = SetWindowsHookEx(WH_KEYBOARD_LL, main_window::keyProc, nullptr, 0);

    if (ocr.Init("./tessdata", "chi_sim"))
    {
        MessageBox(h_main_window, L"Init tesseract failed!", L"info", MB_OK);
        PostQuitMessage(0);
    }

    rolling_thread = std::thread(&main_window::rolling, this);

    return (h_main_window ? TRUE : FALSE);
}

void main_window::setup_ui()
{
    h_preview_box = CreateWindow(L"STATIC", nullptr,
                                 WS_VISIBLE | WS_CHILD | WS_BORDER | SS_BITMAP, // Styles
                                 pre_box_x,                                     // x position
                                 pre_box_y,                                     // y position
                                 pre_box_w,                                     // width
                                 pre_box_h,                                     // height
                                 h_main_window,                                 // Parent window
                                 reinterpret_cast<HMENU>(preview_box), h_instance, nullptr);

    h_text_box = CreateWindow(L"STATIC", L"Result ...",
                              WS_VISIBLE | WS_CHILD | WS_BORDER, // Styles
                              pre_box_x,                         // x position
                              pre_box_y + pre_box_h + 10,        // y position
                              pre_box_w,                         // width
                              pre_box_h,                         // height
                              h_main_window,                     // Parent window
                              nullptr, h_instance, nullptr);

    h_start_button = CreateWindow(L"BUTTON",                             // Predefined class; Unicode assumed
                                  L"START",                              // Button text
                                  WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, // Styles
                                  btn_x,                                 // x position
                                  btn_y,                                 // y position
                                  btn_w,                                 // Button width
                                  btn_h,                                 // Button height
                                  h_main_window,                         // Parent window
                                  reinterpret_cast<HMENU>(start_button), h_instance, nullptr);

    h_close_button = CreateWindow(L"BUTTON",                             // Predefined class; Unicode assumed
                                  L"CLOSE",                              // Button text
                                  WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, // Styles
                                  btn_x + btn_w + 20,                    // x position
                                  btn_y,                                 // y position
                                  btn_w,                                 // Button width
                                  btn_h,                                 // Button height
                                  h_main_window,                         // Parent window
                                  reinterpret_cast<HMENU>(close_button), h_instance, nullptr);
}

LRESULT main_window::handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        setup_ui();
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

        // case WM_PAINT:
        // {
        //     PAINTSTRUCT ps;
        //     HDC hdc = BeginPaint(h_main_window, &ps);
        //     FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_GRAYTEXT + 1));
        //     EndPaint(h_main_window, &ps);
        //     break;
        // }

    case WM_COMMAND:
    {
        if (LOWORD(wParam) == start_button)
        {
            on_start_button_click();
        }
        else if (LOWORD(wParam) == close_button)
        {
            on_close_button_click();
        }
        break;
    }

    default:
        return DefWindowProc(h_main_window, uMsg, wParam, lParam);
    }
    return DefWindowProc(h_main_window, uMsg, wParam, lParam);
}

void main_window::stop_rolling()
{
    is_rolling = false;
    SetWindowText(h_start_button, L"START");
}

void main_window::rolling()
{
    while (is_running)
    {
        if (is_rolling == false)
        {
            std::this_thread::yield();
        }
        else
        {
            HDC hdc = GetDC(NULL);

            // int screenx = GetSystemMetrics(SM_CXSCREEN);
            // int screeny = GetSystemMetrics(SM_CYSCREEN);

            HBITMAP hBitmap = CreateCompatibleBitmap(hdc, t_w, t_h);
            HDC hdcMem = CreateCompatibleDC(hdc);

            SelectObject(hdcMem, hBitmap);
            BitBlt(hdcMem, 0, 0, t_w, t_h, hdc, t_x, t_y, SRCCOPY);

            BITMAP bm;
            GetObject(hBitmap, sizeof(bm), &bm);

            auto size = bm.bmHeight * bm.bmWidthBytes;

            auto buffer = std::make_unique<unsigned char[]>(size);

            GetBitmapBits(hBitmap, size, buffer.get());

            // note: 24bit bmp image
            // HBITMAP hBitmap = reinterpret_cast<HBITMAP>(LoadImage(NULL, L"m.bmp", IMAGE_BITMAP, 0, 0,
            // LR_LOADFROMFILE));

            // convert 32bit bitmap to 1 bit gray image in image_bits
            // there is a problem during transform 32bit bitmap to 1 bit gray image
            for (std::size_t row = 0; row < bm.bmHeight; row++)
            {
                for (std::size_t col = 0; col < bm.bmWidth; col++)
                {
                    auto source_index = row * bm.bmWidthBytes + col * bm.bmBitsPixel / 8;
                    auto r = buffer[source_index];
                    auto g = buffer[source_index + 1];
                    auto b = buffer[source_index + 2];

                    auto target_index_x = col % 8;
                    if (0.3f * r + 0.59f * g + 0.11f * b > 75.0f)
                    {
                        image_bits.at(row).at(col / 8) |= 0x00000001 << (7 - target_index_x);
                    }
                    else
                    {
                        image_bits.at(row).at(col / 8) &= 0x00000001 << (7 - target_index_x) ^ 0x11111111;
                    }
                }
            }

            // create preview image handle
            HBITMAP preview_image = CreateBitmap(t_w, t_h, 1, 1, image_bits.data()->data());
            assert(preview_image != nullptr);

            HBITMAP hold = reinterpret_cast<HBITMAP>(
                SendMessage(h_preview_box, STM_SETIMAGE, IMAGE_BITMAP, reinterpret_cast<LPARAM>(preview_image)));

            // delete the old image
            if (hold && hold != preview_image)
            {
                DeleteObject(hold);
            }

            ocr.SetImage(buffer.get(), t_w, t_h, 4, 800);
            std::unique_ptr<char[]> utf8_text(ocr.GetUTF8Text());

            // convert UTF8 text to wide char, and report ocr result in text box
            int wide_str_len =
                MultiByteToWideChar(CP_UTF8, 0, utf8_text.get(), static_cast<int>(strlen(utf8_text.get())), nullptr, 0);
            std::unique_ptr<wchar_t[]> wide_str = std::make_unique<wchar_t[]>(wide_str_len + 1);
            MultiByteToWideChar(CP_UTF8, 0, utf8_text.get(), static_cast<int>(strlen(utf8_text.get())), wide_str.get(),
                                wide_str_len);
            wide_str[wide_str_len] = '\0';

            SetWindowText(h_text_box, wide_str.get());

            if (std::wstring(wide_str.get()).find(L"血疫幸存者") == std::wstring::npos)
            {
                INPUT press_key_t[2] = {};
                press_key_t[0].type = INPUT_KEYBOARD;
                press_key_t[0].ki.wVk = 'T';
                press_key_t[1].type = INPUT_KEYBOARD;
                press_key_t[1].ki.wVk = 'T';
                press_key_t[1].ki.dwFlags = KEYEVENTF_KEYUP;
                auto send = SendInput(ARRAYSIZE(press_key_t), press_key_t, sizeof(INPUT));
            }
            else
            {
                stop_rolling();
            }

            DeleteObject(hBitmap);
            ReleaseDC(h_main_window, hdcMem);
            ReleaseDC(h_main_window, hdc);
        }
    }
}

void main_window::on_start_button_click()
{
    if (is_rolling == false)
    {
        is_rolling = true;
        SetWindowText(h_start_button, L"Pause (ESC)");
    }
    else
    {
        stop_rolling();
    }
}

void main_window::on_close_button_click()
{
    is_rolling = false;
    is_running = false;
    PostQuitMessage(0);
}
