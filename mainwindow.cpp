#include "mainwindow.h"
#include "ui_infowindow.h"
#include "ui_mainwindow.h"
#include "infowindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(985, 690);

    MediaPlayer = new QMediaPlayer();
    MediaAudioOutput = new QAudioOutput();

    ui->pushButton_Play_Stop->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->pushButton_seek_Backwards->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    ui->pushButton_seek_Forwards->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));

    ui->verticalSlider_Volume->setRange(0, 100);
    ui->verticalSlider_Volume->setValue(20);

    float volumeF = qPow(10, (ui->verticalSlider_Volume->value() / 50.0) - 1);
    MediaAudioOutput->setVolume(volumeF);

    mDurationSeekFB = 10;

    QTime timeInit(((0 / 1000) / 3600) % 60, ((0 / 1000) / 60) % 60, (0 / 1000) % 60, 0 % 1000);
    vSubTimeSave = timeInit.toString(vFormat);

    connect(MediaPlayer, &QMediaPlayer::durationChanged, this, &MainWindow::durationChanged);
    connect(MediaPlayer, &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);

    ui->horizontalSlider_Duration->setRange(0, MediaPlayer->duration() / 1000);

    QShortcut *openVideoShort = new QShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_O), this);
    connect(openVideoShort, &QShortcut::activated, this, &MainWindow::on_actionOpen_Video_Ctrl_Alt_O_triggered);

    QShortcut *openInfoShort = new QShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_I), this);
    connect(openInfoShort, &QShortcut::activated, this, &MainWindow::on_actionVideo_Info_Ctrl_Alt_I_triggered);

    QShortcut *playPauseVideo = new QShortcut(QKeySequence(Qt::Key_Space), this);
    connect(playPauseVideo, &QShortcut::activated, this, &MainWindow::on_pushButton_Play_Stop_clicked);

    QShortcut *seekBackwards = new QShortcut(QKeySequence(Qt::Key_Left), this);
    connect(seekBackwards, &QShortcut::activated, this, &MainWindow::on_pushButton_seek_Backwards_clicked);

    QShortcut *seekForwards = new QShortcut(QKeySequence(Qt::Key_Right), this);
    connect(seekForwards, &QShortcut::activated, this, &MainWindow::on_pushButton_seek_Forwards_clicked);

    QShortcut *increaseVolume = new QShortcut(QKeySequence(Qt::Key_Up), this);
    connect(increaseVolume, &QShortcut::activated, this, &MainWindow::on_keyVolume_valueChange);

    QShortcut *decreaseVolume = new QShortcut(QKeySequence(Qt::Key_Down), this);
    connect(decreaseVolume, &QShortcut::activated, this, &MainWindow::on_keyVolume_valueChange);

    QShortcut *addSubTime = new QShortcut(QKeySequence(Qt::Key_Plus), this);
    connect(addSubTime, &QShortcut::activated, this, &MainWindow::on_addSubTimeButton_clicked);

    ui->tableWidget->setColumnWidth(0, 200);
    ui->tableWidget->setColumnWidth(1, 200);
    ui->tableWidget->setColumnWidth(2, 500);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::durationChanged(qint64 Duration)
{
    mDuration = Duration / 1000;
    ui->horizontalSlider_Duration->setMaximum(mDuration);
}

void MainWindow::positionChanged(qint64 Duration)
{
    if (!ui->horizontalSlider_Duration->isSliderDown())
    {
        ui->horizontalSlider_Duration->setValue(Duration / 1000);
    }
    updateDuration(Duration);
}

void MainWindow::updateDuration(qint64 Duration)
{
    if(Duration || mDuration)
    {
        QTime CurrentTime(((Duration / 1000) / 3600) % 60, ((Duration / 1000) / 60) % 60, (Duration / 1000) % 60, Duration % 1000);

        vSubTimeSave = CurrentTime.toString(vFormat);

        ui->label_current_Time->setText(CurrentTime.toString(vFormat));
    }
}

void MainWindow::on_actionOpen_Video_Ctrl_Alt_O_triggered()
{
    QString FileName = QFileDialog::getOpenFileName(this, tr("Select Video File"), "", tr("MP4 Files (*.mp4)"));

    Video = new QVideoWidget();

    Video->setGeometry(0, 0, ui->groupBox_Video->width(), ui->groupBox_Video->height());

    Video->setParent(ui->groupBox_Video);

    MediaPlayer->setVideoOutput(Video);

    connect(MediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::on_MediaStatusChanged);

    MediaPlayer->setSource(QUrl(FileName));

    MediaPlayer->setAudioOutput(MediaAudioOutput);

    Video->setVisible(true);

    Video->show();
}

void MainWindow::on_MediaStatusChanged()
{
    QMediaMetaData metaData = MediaPlayer->metaData();

    if (MediaPlayer->mediaStatus() == QMediaPlayer::LoadedMedia)
    {
        vTitle = metaData.stringValue(metaData.Title);
        vDescription = metaData.stringValue(metaData.Description);
        vDate = metaData.stringValue(metaData.Date);

        vMediaType = metaData.stringValue(metaData.MediaType);
        vFileFormat = metaData.stringValue(metaData.FileFormat);
        vDuration = metaData.stringValue(metaData.Duration);

        vVideoFrameRate = metaData.stringValue(metaData.VideoFrameRate);
        vVideoBitRate = metaData.stringValue(metaData.VideoBitRate);
        vVideoCodec = metaData.stringValue(metaData.VideoCodec);

        vAudioBitRate = metaData.stringValue(metaData.AudioBitRate);
        vAudioCodec = metaData.stringValue(metaData.AudioCodec);

        vOrientation = metaData.stringValue(metaData.Orientation);
        vResolution = metaData.stringValue(metaData.Resolution);
    }
}

void MainWindow::on_actionVideo_Info_Ctrl_Alt_I_triggered()
{
    InfoWindow *infoWindow = new InfoWindow();
    Ui::InfoWindow *infoWindowUi = infoWindow->getUi();

    vTitle.isEmpty() ? infoWindowUi->TitleTxtLabel->setText("No Data Available") : infoWindowUi->TitleTxtLabel->setText(vTitle);

    vDescription.isEmpty() ? infoWindowUi->DescriptionTxtLabel->setText("No Data Available") : infoWindowUi->DescriptionTxtLabel->setText(vDescription);

    QDateTime dateTime = QDateTime::fromString(vDate, "yyyy-MM-ddTHH:mm:ss.zzzZ");
    dateTime.isNull() ? infoWindowUi->DateTxtLabel->setText("No Data Available") : infoWindowUi->DateTxtLabel->setText(dateTime.toString("yyyy-MM-dd hh:mm:ss"));

    vMediaType.isEmpty() ? infoWindowUi->MediaTypeTxtLabel->setText("No Data Available") : infoWindowUi->MediaTypeTxtLabel->setText(vMediaType);
    vFileFormat.isEmpty() ? infoWindowUi->FileFormatTxtLabel->setText("No Data Available") : infoWindowUi->FileFormatTxtLabel->setText(vFileFormat);
    vDuration.isEmpty() ? infoWindowUi->DurationTxtLabel->setText("No Data Available") : infoWindowUi->DurationTxtLabel->setText(vDuration);

    vAudioBitRate.isEmpty() ? infoWindowUi->AudioBitRateTxtLabel->setText("No Data Available") : infoWindowUi->AudioBitRateTxtLabel->setText(vAudioBitRate + " bit/s");
    vAudioCodec.isEmpty() ? infoWindowUi->AudioCodecTxtLabel->setText("No Data Available") : infoWindowUi->AudioCodecTxtLabel->setText(vAudioCodec + " codec");

    vVideoFrameRate.isEmpty() ? infoWindowUi->VideoFrameRateTxtLabel->setText("No Data Available") : infoWindowUi->VideoFrameRateTxtLabel->setText(vVideoFrameRate + " frames/s");
    vVideoBitRate.isEmpty() ? infoWindowUi->VideoBitRateTxtLabel->setText("No Data Available") : infoWindowUi->VideoBitRateTxtLabel->setText(vVideoBitRate + " bit/s");
    vVideoCodec.isEmpty() ? infoWindowUi->VideoCodecTxtLabel->setText("No Data Available") : infoWindowUi->VideoCodecTxtLabel->setText(vVideoCodec + " codec");

    vOrientation.isEmpty() ? infoWindowUi->OrientationTxtLabel->setText("No Data Available") : infoWindowUi->OrientationTxtLabel->setText(vOrientation + " ratio");
    vResolution.isEmpty() ? infoWindowUi->ResolutionTxtLabel->setText("No Data Available") : infoWindowUi->ResolutionTxtLabel->setText(vResolution);

    infoWindow->show();
}

void MainWindow::on_horizontalSlider_Duration_valueChanged(int value)
{
    MediaPlayer->setPosition(value * 1000);
}

void MainWindow::on_verticalSlider_Volume_valueChanged(int value)
{
    if (value == 0)
    {
        MediaAudioOutput->setVolume(0);
    }
    else
    {
        float volumeF = qPow(10, (value / 50.0) - 1);
        MediaAudioOutput->setVolume(volumeF);
    }
}

void MainWindow::on_keyVolume_valueChange()
{
    QShortcut *volShortcut = qobject_cast<QShortcut*>(sender());

    if (!volShortcut)
        return;

    if (volShortcut->key().toString() == "Up")
    {
        if (ui->verticalSlider_Volume->value() + 5 >= 100)
        {
            ui->verticalSlider_Volume->setValue(100);
        }
        else
        {
            ui->verticalSlider_Volume->setValue(ui->verticalSlider_Volume->value() + 5);
        }
    }
    else if (volShortcut->key().toString() == "Down")
    {
        if (ui->verticalSlider_Volume->value() - 5 <= 0)
        {
            ui->verticalSlider_Volume->setValue(0);
        }
        else
        {
            ui->verticalSlider_Volume->setValue(ui->verticalSlider_Volume->value() - 5);
        }
    }
}

void MainWindow::on_pushButton_seek_Backwards_clicked()
{
    qint64 milliseconds = MediaPlayer->position();
    qint64 timeSet = milliseconds - mDurationSeekFB;

    ui->horizontalSlider_Duration->setValue(timeSet / 1000);
    MediaPlayer->setPosition(timeSet);
}

void MainWindow::on_pushButton_seek_Forwards_clicked()
{
    qint64 milliseconds = MediaPlayer->position();
    qint64 timeSet = milliseconds + mDurationSeekFB;

    ui->horizontalSlider_Duration->setValue(timeSet / 1000);
    MediaPlayer->setPosition(timeSet);
}

void MainWindow::on_pushButton_Play_Stop_clicked()
{
    if(IS_Paused == true)
    {
        IS_Paused = false;
        MediaPlayer->play();
        ui->pushButton_Play_Stop->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    } else
    {
        IS_Paused = true;
        MediaPlayer->pause();
        ui->pushButton_Play_Stop->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    }
}

void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)
{
    QString numberString = arg1.left(arg1.indexOf("ms"));
    qint64 msSeek = numberString.toInt();

    mDurationSeekFB = msSeek;
}

void MainWindow::on_addSubTimeButton_clicked()
{
    if (MediaPlayer->isPlaying() || !MediaPlayer->isPlaying())
    {
        if (vCurrentState == start) {
            if (ui->tableWidget->rowCount() == 0){
                ui->tableWidget->insertRow(0);
            }
            else
            {
                ui->tableWidget->insertRow(ui->tableWidget->rowCount());
            }

            ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 0, new QTableWidgetItem(vSubTimeSave));

            vCurrentState = stop;

        } else if (vCurrentState == stop) {
            ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 1, new QTableWidgetItem(vSubTimeSave));

            vCurrentState = start;

            ui->tableWidget->verticalScrollBar()->setValue(ui->tableWidget->verticalScrollBar()->maximum());
        }
    }
}

void MainWindow::on_removeSubTimeButton_clicked()
{
    if (MediaPlayer->isPlaying() || !MediaPlayer->isPlaying())
    {
        if (vCurrentState == start)
        {
            QTableWidgetItem* itemToDelete = ui->tableWidget->takeItem(ui->tableWidget->rowCount() - 1, 1);
            delete itemToDelete;

            vCurrentState = stop;
        }
        else if (vCurrentState == stop)
        {
            ui->tableWidget->removeRow(ui->tableWidget->rowCount() - 1);

            vCurrentState = start;
        }
    }
}
