#ifndef INFOWINDOW_H
#define INFOWINDOW_H

#include <QtCore>
#include <QtWidgets>
#include <QtGui>

QT_BEGIN_NAMESPACE
namespace Ui {
class InfoWindow;
}
QT_END_NAMESPACE

class InfoWindow : public QWidget
{
    Q_OBJECT
public:
    InfoWindow(QWidget *parent = 0);
    ~InfoWindow();

    Ui::InfoWindow* getUi();

private slots:

    void on_closeInfoWindow_clicked();

private:
    Ui::InfoWindow *ui;
};

#endif // INFOWINDOW_H
