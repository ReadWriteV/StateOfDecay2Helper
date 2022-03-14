#include "base_window.hpp"

class main_window : public base_window<main_window>
{
public:
    PCWSTR ClassName() const override { return L"Sample Window Class"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
};