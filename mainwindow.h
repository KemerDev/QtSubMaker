#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtMultimedia>
#include <QtMultimediaWidgets>
#include <QtCore>
#include <QtWidgets>
#include <QtGui>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void durationChanged(qint64 Duration);

    void positionChanged(qint64 Duration);

    void on_horizontalSlider_Duration_valueChanged(int value);

    void on_verticalSlider_Volume_valueChanged(int value);

    void on_pushButton_seek_Backwards_clicked();

    void on_pushButton_seek_Forwards_clicked();

    void on_pushButton_Play_Stop_clicked();

    void on_actionOpen_Video_Ctrl_Alt_O_triggered();

    void on_comboBox_currentTextChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QMediaPlayer *MediaPlayer;
    QAudioOutput *MediaAudioOutput;
    QVideoWidget *Video;
    qint64 mDuration;
    qint64 mDurationSeekFB;
    bool IS_Paused = true;
    bool IS_Muted = false;

    void updateDuration(qint64 Duration);
};
#endif // MAINWINDOW_H
