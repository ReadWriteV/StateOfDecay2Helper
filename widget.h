#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>

#include <tesseract/baseapi.h>

class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void pause_key_down();

private slots:
    void on_begin_button_click();
    void on_pause_button_click();

private:
    bool is_rolling;

    QLabel *preview;

    QVBoxLayout *main_box;
    QHBoxLayout *button_box;

    QPushButton *begin_button;
    QPushButton *pause_button;
    QPushButton *exit_button;

    QTextBrowser *result;

    tesseract::TessBaseAPI *ocr;
};