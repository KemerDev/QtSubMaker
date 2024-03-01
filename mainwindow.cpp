#include "mainwindow.h"
#include "ui_infowindow.h"
#include "ui_mainwindow.h"
#include "infowindow.h"

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

        QString Format = "hh:mm:ss,zzz";

        ui->label_current_Time->setText(CurrentTime.toString(Format));
    }
}

void MainWindow::on_actionOpen_Video_Ctrl_Alt_O_triggered()
{
    QString FileName = QFileDialog::getOpenFileName(this, tr("Select Video File"), "", tr("MP4 Files (*.mp4)"));

    Video = new QVideoWidget();

    Video->setGeometry(0, 0, ui->groupBox_Video->width(), ui->groupBox_Video->height());

    Video->setParent(ui->groupBox_Video);

    MediaPlayer->setVideoOutput(Video);

    connect(MediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::onMediaStatusChanged);

    MediaPlayer->setSource(QUrl(FileName));

    MediaPlayer->setAudioOutput(MediaAudioOutput);

    Video->setVisible(true);

    Video->show();
}

void MainWindow::onMediaStatusChanged()
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

        vAudioBitRate = metaData.metaDataKeyToString(metaData.AudioBitRate);
        vAudioCodec = metaData.metaDataKeyToString(metaData.AudioCodec);

        vOrientation = metaData.metaDataKeyToString(metaData.Orientation);
        vResolution = metaData.metaDataKeyToString(metaData.Resolution);
    }
}

void MainWindow::on_actionVideo_Info_Ctrl_Alt_I_triggered()
{
    InfoWindow *infoWindow = new InfoWindow();
    Ui::InfoWindow *infoWindowUi = infoWindow->getUi();

    if (MediaPlayer->mediaStatus() == QMediaPlayer::LoadedMedia)
    {
        infoWindowUi->TitleTxtLabel->setText(vTitle);
        infoWindowUi->DescriptionTxtLabel->setText(vDescription);

        QDateTime dateTime = QDateTime::fromString(vDate, "yyyy-MM-ddTHH:mm:ss.zzzZ");
        infoWindowUi->DateTxtLabel->setText(dateTime.toString("yyyy-MM-dd hh:mm:ss"));

        infoWindowUi->MediaTypeTxtLabel->setText(vMediaType);
        infoWindowUi->FileFormatTxtLabel->setText(vFileFormat);
        infoWindowUi->DurationTxtLabel->setText(vDuration);

        infoWindowUi->VideoFrameRateTxtLabel->setText(vVideoFrameRate + " frames/s");
        infoWindowUi->VideoBitRateTxtLabel->setText(vVideoBitRate + " bit/s");
        infoWindowUi->VideoCodecTxtLabel->setText(vVideoCodec + " codec");

    }
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
