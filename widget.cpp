#include "widget.h"

#include <QtGui/QImage>
#include <QtGui/QScreen>
#include <QtGui/QGuiApplication>
#include <QtWidgets/QApplication>

#include <Windows.h>

constexpr qint32 t_x = 1580, t_y = 290, t_w = 200, t_h = 125;
constexpr qint32 p_x = 1572, p_y = 804;

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("State of Decay 2 Helper");
    setWindowFlag(Qt::WindowType::WindowStaysOnTopHint);

    is_rolling = false;

    main_box = new QVBoxLayout(this);

    preview = new QLabel;
    result = new QTextBrowser;
    button_box = new QHBoxLayout;

    begin_button = new QPushButton("开始");
    pause_button = new QPushButton("暂停(esc)");
    exit_button = new QPushButton("退出");
    pause_button->setEnabled(false);

    button_box->addWidget(begin_button);
    button_box->addWidget(pause_button);
    button_box->addWidget(exit_button);

    main_box->addWidget(preview);
    main_box->addWidget(result);
    main_box->addLayout(button_box);

    QObject::connect(begin_button, &QPushButton::clicked, this, &Widget::on_begin_button_click);
    QObject::connect(pause_button, &QPushButton::clicked, this, &Widget::on_pause_button_click);
    QObject::connect(exit_button, &QPushButton::clicked, this, &Widget::close);

    ocr = new tesseract::TessBaseAPI;
    if (ocr->Init("C:\\Program Files\\Tesseract-5.0.0\\bin\\tessdata", "chi_sim"))
    {
        result->setText("cannnot init tesseract");
        begin_button->setEnabled(false);
    }
}

Widget::~Widget()
{
    delete preview;

    delete main_box;
    delete button_box;

    delete begin_button;
    delete pause_button;
    delete exit_button;

    delete result;

    ocr->End();
    delete ocr;
}

void Widget::on_begin_button_click()
{
    is_rolling = true;
    begin_button->setEnabled(false);
    pause_button->setEnabled(true);

    QScreen *screen = QGuiApplication::primaryScreen();

    while (is_rolling)
    {
        QPixmap target = screen->grabWindow(0).copy(t_x, t_y, t_w, t_h);
        QImage gray_target = target.toImage().convertToFormat(QImage::Format_Grayscale8);

        // gray_target.applyColorTransform()

        for (int i = 0; i < gray_target.width(); i++)
        {
            for (int j = 0; j < gray_target.height(); j++)
            {
                QColor t = gray_target.pixel(i, j);
                int bt = t.red();
                if (bt > 70)
                {
                    gray_target.setPixel(i, j, qRgb(255, 255, 255));
                }
                else
                {
                    gray_target.setPixel(i, j, qRgb(0, 0, 0));
                }
            }
        }
        preview->setPixmap(QPixmap::fromImage(gray_target));

        ocr->SetImage(gray_target.bits(), gray_target.width(), gray_target.height(), 1, gray_target.width());
        char *utf8_text = ocr->GetUTF8Text();
        QString text(utf8_text);
        result->setText(text);
        delete[] utf8_text;

        if (text.contains("不灭") || text.contains("血疫幸存者"))
        {
            on_pause_button_click();
        }
        else
        {
            ::SetCursorPos(p_x, p_y);
            ::mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
            ::mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
        }
        QApplication::processEvents();
    }
}

void Widget::on_pause_button_click()
{
    is_rolling = false;
    begin_button->setEnabled(true);
    pause_button->setEnabled(false);
}

void Widget::pause_key_down()
{
    if (is_rolling)
    {
        on_pause_button_click();
    }
}
