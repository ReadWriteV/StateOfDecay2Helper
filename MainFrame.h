#pragma once

#include <sdkddkver.h>
#include <afxwin.h>

#include <tesseract/baseapi.h>

#include <thread>

class CMainFrameWnd : public CFrameWnd
{
public:
    bool InitOCR();

private:
    void RollAt(std::stop_token stop_token, int slot_num);

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnBeginBtnClicked();
    DECLARE_MESSAGE_MAP()

public:
    static constexpr LONG client_w{300}, client_h{400};
    static constexpr RECT client{.left = 400, .top = 200, .right = 400 + client_w, .bottom = 200 + client_h};

    static constexpr LONG slot_w{200}, slot_h{70};
    static constexpr RECT slot3{.left = 1255, .top = 360, .right = 1255 + slot_w, .bottom = 360 + slot_h};

private:
    CStatic snap_region_preview;
    CStatic ocr_result;
    CButton begin_btn, end_btn;

    tesseract::TessBaseAPI ocr;

    std::jthread rolling_slot3;
    bool rolling_flag{false};
};
