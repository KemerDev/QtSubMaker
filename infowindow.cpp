#include "infowindow.h"
#include "ui_infowindow.h"

InfoWindow::InfoWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::InfoWindow)
{
    ui->setupUi(this);
    this->setFixedSize(640, 480);
}

InfoWindow::~InfoWindow()
{
    delete ui;
}

Ui::InfoWindow* InfoWindow::getUi()
{
    return ui;
}
