#include "MainFrame.h"

#include <array>
#include <filesystem>

#include <atlimage.h>

constexpr WORD BEGIN_BTN_ID{1001};

BEGIN_MESSAGE_MAP(CMainFrameWnd, CFrameWnd)
ON_WM_CREATE()
ON_BN_CLICKED(BEGIN_BTN_ID, OnBeginBtnClicked)
END_MESSAGE_MAP()

bool CMainFrameWnd::InitOCR()
{
    if (!std::filesystem::exists("./tessdata/chi_sim.traineddata"))
    {
        AfxMessageBox(L"未找到chi_sim.traineddata", MB_OK | MB_ICONERROR);
        return false;
    }
    if (ocr.Init("./tessdata", "chi_sim"))
    {
        AfxMessageBox(L"初始化OCR引擎失败", MB_OK | MB_ICONERROR);
        return false;
    }
    return true;
}

void CMainFrameWnd::RollAt(std::stop_token stop_token, int slot_num)
{
    while (!stop_token.stop_requested())
    {
        if (rolling_flag == false)
        {
            std::this_thread::yield();
        }
        else
        {
            // int screenx = GetSystemMetrics(SM_CXSCREEN);
            // int screeny = GetSystemMetrics(SM_CYSCREEN);

            HDC hDCScreen = ::GetDC(NULL);
            int nBitPerPixel = GetDeviceCaps(hDCScreen, BITSPIXEL);
            CImage region_img;
            region_img.Create(slot_w, slot_h, nBitPerPixel);
            HDC hDCImg = region_img.GetDC();
            BitBlt(hDCImg, 0, 0, slot_w, slot_h, hDCScreen, slot3.left, slot3.top, SRCCOPY);

            // CDC *pDC = CDC::FromHandle(::GetDC(NULL));
            // CDC memDC;
            // memDC.CreateCompatibleDC(pDC);
            // CBitmap mem_bitmap;
            // mem_bitmap.CreateCompatibleBitmap(pDC, slot_w, slot_h);
            // memDC.SelectObject(mem_bitmap);
            // memDC.BitBlt(0, 0, slot_w, slot_h, pDC, slot3.left, slot3.top, SRCCOPY);

            std::array<unsigned char, slot_w * slot_h> img_buffer;

            // convert 32bit bitmap to 8bit gray image in image_bits
            // there is a problem during transform 32bit bitmap to 24bit gray image
            for (int row = 0; row < region_img.GetHeight(); row++)
            {
                for (int col = 0; col < region_img.GetWidth(); col++)
                {
                    auto pix = region_img.GetPixel(col, row);
                    auto r = GetRValue(pix);
                    auto g = GetGValue(pix);
                    auto b = GetBValue(pix);
                    auto target_idx = row * slot_w + col;
                    // auto gray = 0.3f * r + 0.59f * g + 0.11f * b;
                    // img_buffer.at(target_idx) = static_cast<unsigned char>(gray);
                    if (0.3f * r + 0.59f * g + 0.11f * b > 75.0f)
                    {
                        img_buffer.at(target_idx) = 255;
                    }
                    else
                    {
                        img_buffer.at(target_idx) = 0;
                    }
                }
            }
            ocr.SetImage(img_buffer.data(), slot_w, slot_h, 1, slot_w);
            std::unique_ptr<char[]> utf8_text(ocr.GetUTF8Text());

            // convert UTF8 text to wide char, and report ocr result in text box
            int wide_str_len =
                MultiByteToWideChar(CP_UTF8, 0, utf8_text.get(), static_cast<int>(strlen(utf8_text.get())), nullptr, 0);
            std::unique_ptr<wchar_t[]> wide_str = std::make_unique<wchar_t[]>(wide_str_len + 1);
            MultiByteToWideChar(CP_UTF8, 0, utf8_text.get(), static_cast<int>(strlen(utf8_text.get())), wide_str.get(),
                                wide_str_len);
            wide_str[wide_str_len] = '\0';

            ocr_result.SetWindowTextW(wide_str.get());

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

            CRect zcRect;
            snap_region_preview.GetClientRect(&zcRect);
            region_img.Draw(snap_region_preview.GetDC()->GetSafeHdc(),
                            zcRect.left,
                            zcRect.top,
                            zcRect.Width(),
                            zcRect.Height());

            region_img.ReleaseDC();
        }
    }
}

int CMainFrameWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

    // auto hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    // SetIcon(hIcon, TRUE);
    // SetIcon(hIcon, FALSE);

    // preview region width, height and position
    constexpr int preview_x = (client_w - slot_w) / 2, preview_y = 10;
    RECT preview_wnd_rect{.left = preview_x, .top = preview_y, .right = preview_x + slot_w, .bottom = preview_y + slot_h};
    snap_region_preview.Create(nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER | SS_BITMAP, preview_wnd_rect, this);

    RECT ocr_result_rect{.left = preview_x, .top = preview_wnd_rect.bottom + 10, .right = preview_x + slot_w, .bottom = preview_wnd_rect.bottom + 10 + slot_h};
    ocr_result.Create(nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER, ocr_result_rect, this);

    // setup button
    constexpr int btn_x = 40, btn_y = 300, btn_w = 100, btn_h = 50;
    RECT begin_btn_rect{.left = btn_x, .top = btn_y, .right = btn_x + btn_w, .bottom = btn_y + btn_h};
    begin_btn.Create(TEXT("开始"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, begin_btn_rect, this, BEGIN_BTN_ID);

    rolling_slot3 = std::jthread{[this](std::stop_token stop_token, int slot_num)
                                 { RollAt(stop_token, slot_num); },
                                 3};

    return CFrameWnd::OnCreate(lpCreateStruct);
}

void CMainFrameWnd::OnBeginBtnClicked()
{
    if (rolling_flag == false)
    {
        rolling_flag = true;
        begin_btn.SetWindowTextW(TEXT("停止"));
    }
    else
    {
        rolling_flag = false;
        begin_btn.SetWindowTextW(TEXT("开始"));
    }
}
