#include "infowindow.h"
#include "ui_infowindow.h"

InfoWindow::InfoWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::InfoWindow)
{
    ui->setupUi(this);
    this->setFixedSize(640, 480);
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
}

InfoWindow::~InfoWindow()
{
    delete ui;
}

Ui::InfoWindow* InfoWindow::getUi()
{
    return ui;
}

void InfoWindow::on_closeInfoWindow_clicked()
{
    this->close();
}
