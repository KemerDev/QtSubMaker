#ifndef INFOWINDOW_H
#define INFOWINDOW_H

#include <QDialog>
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

private:
    Ui::InfoWindow *ui;
};

#endif // INFOWINDOW_H
