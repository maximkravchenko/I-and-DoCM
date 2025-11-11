#include "camerahandler.h"
#include "mainwindow.h" // Для полного определения MainWindow
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>

CameraHandler::CameraHandler(MainWindow *mainWindow, QVideoWidget *previewWidget, QLabel *cameraInfoLabel, QLabel *statusLabel, Ui_MainWindow *ui, QObject *parent)
    : QObject(parent), m_mainWindow(mainWindow), m_ui(ui), m_previewWidget(previewWidget), m_cameraInfoLabel(cameraInfoLabel), m_statusLabel(statusLabel)
{
    // Инициализация пути сохранения
    m_savePath = QDir::homePath() + "/Pictures/CameraLab";
    QDir().mkpath(m_savePath);
    m_ui->selectedFolderLabel->setText(m_savePath);

    // Инициализация сессии
    m_captureSession.setVideoOutput(m_previewWidget);

    // Заполнение списка камер
    const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    for (const QCameraDevice &cam : cameras) {
        m_ui->comboCameraList->addItem(cam.description(), cam.id());
    }

    // Выбор первой камеры по умолчанию
    if (!cameras.isEmpty()) {
        changeCamera(cameras.first().id());
    }

    // Подключение сигналов кнопок
    connect(m_ui->btnSelectFolder, &QPushButton::clicked, this, &CameraHandler::onSelectFolder);
    connect(m_ui->btnTakePhoto, &QPushButton::clicked, this, &CameraHandler::onTakePhoto);
    connect(m_ui->btnStartStopVideo, &QPushButton::clicked, this, &CameraHandler::onStartStopVideo);
    connect(m_ui->btnPreviewToggle, &QPushButton::clicked, this, &CameraHandler::onTogglePreview);
    connect(m_ui->btnHiddenRecord, &QPushButton::clicked, this, &CameraHandler::onHiddenRecord);
    connect(m_ui->btnCheckCamera, &QPushButton::clicked, this, &CameraHandler::onCheckCamera);
    connect(m_ui->comboCameraList, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CameraHandler::onCameraChanged);
    connect(m_camera, &QCamera::errorOccurred, this, [this](QCamera::Error error) {
        Q_UNUSED(error);
        m_statusLabel->setText("Ошибка камеры: " + m_camera->errorString());
    });

    // Кнопка видео по умолчанию "Видео" (start)
    m_ui->btnStartStopVideo->setText("Видео");
}

CameraHandler::~CameraHandler()
{
    unregisterHotkey();
    stopPreview();
    delete m_camera;
    delete m_imageCapture;
    delete m_recorder;
}

void CameraHandler::startPreview()
{
    if (m_camera && !m_isPreviewing) {
        m_camera->start();
        m_isPreviewing = true;
        m_previewWidget->show();
        m_ui->btnPreviewToggle->setText("Выкл предпросмотр");
    }
}

void CameraHandler::stopPreview()
{
    if (m_camera && m_isPreviewing) {
        m_camera->stop();
        m_isPreviewing = false;
        m_previewWidget->hide();
        m_ui->btnPreviewToggle->setText("Вкл предпросмотр");
    }
}

void CameraHandler::onSelectFolder()
{
    QString dir = QFileDialog::getExistingDirectory(m_mainWindow, "Выберите папку для сохранения", m_savePath);
    if (!dir.isEmpty()) {
        m_savePath = dir;
        m_ui->selectedFolderLabel->setText(m_savePath);
    }
}

void CameraHandler::onTakePhoto()
{
    if (m_imageCapture) {
        QString fileName = m_savePath + "/" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".jpg";
        m_imageCapture->captureToFile(fileName);
        m_statusLabel->setText("Фото сохранено: " + fileName);
    }
}

void CameraHandler::onStartStopVideo()
{
    if (!m_isRecording) {
        startVideo();
    } else {
        stopVideo();
    }
}

void CameraHandler::startVideo()
{
    if (m_recorder) {
        QString fileName = m_savePath + "/" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".mp4";
        m_recorder->setOutputLocation(QUrl::fromLocalFile(fileName));
        m_recorder->record();
        m_isRecording = true;
        m_ui->btnStartStopVideo->setText("Стоп");
        m_statusLabel->setText("Запись начата...");
    }
}

void CameraHandler::stopVideo()
{
    if (m_recorder && m_isRecording) {
        m_recorder->stop();
        m_isRecording = false;
        m_ui->btnStartStopVideo->setText("Видео");
        m_statusLabel->setText("Запись остановлена и сохранена.");
    }
}

void CameraHandler::onTogglePreview()
{
    if (m_isPreviewing) {
        stopPreview();
    } else {
        startPreview();
    }
}

void CameraHandler::onHiddenRecord()
{
    if (!m_isRecording) {
        if (registerHotkey()) {
            m_captureSession.setVideoOutput(nullptr);  // Отключаем вывод
            startVideo();
            m_mainWindow->hide();
            m_statusLabel->setText("Скрытая запись начата. Ctrl+Alt+R — остановить.");
        } else {
            QMessageBox::warning(m_mainWindow, "Ошибка", "Не удалось зарегистрировать хоткей.");
        }
    }
}

void CameraHandler::restoreVideoOutput()
{
    m_captureSession.setVideoOutput(m_previewWidget);
    if (m_isPreviewing) {
        m_camera->start();  // Перезапускаем камеру, если предпросмотр был включён
    }
}

bool CameraHandler::registerHotkey()
{
    return RegisterHotKey((HWND)m_mainWindow->winId(), HOTKEY_ID, MOD_CONTROL | MOD_ALT, 'R');
}

void CameraHandler::unregisterHotkey()
{
    UnregisterHotKey((HWND)m_mainWindow->winId(), HOTKEY_ID);
}

void CameraHandler::onCheckCamera()
{
    bool wasPreviewing = m_isPreviewing;
    stopPreview();

    // Создаём временную камеру для проверки (чтобы не трогать основную)
    QCamera *testCamera = new QCamera(m_camera->cameraDevice(), this);
    QMediaCaptureSession testSession;
    testSession.setCamera(testCamera);

    // Подключаем сигналы
    connect(testCamera, &QCamera::errorOccurred, this, [this, testCamera, wasPreviewing](QCamera::Error error) {
        Q_UNUSED(error);
        m_statusLabel->setText("Камера используется" /*+ testCamera->errorString()*/);
        m_statusLabel->setStyleSheet("color: red;");
        testCamera->deleteLater();
        if (wasPreviewing) startPreview();
    });

    connect(testCamera, &QCamera::activeChanged, this, [this, testCamera, wasPreviewing](bool active) {
        if (active) {
            m_statusLabel->setText("Камера доступна.");
            m_statusLabel->setStyleSheet("color: lightgreen;");
            testCamera->stop();
            testCamera->deleteLater();
            if (wasPreviewing) startPreview();
        }
    });

    // Запускаем проверку
    testCamera->start();
}

void CameraHandler::onCameraChanged(int index)
{
    QByteArray deviceId = m_ui->comboCameraList->itemData(index).toByteArray();
    changeCamera(deviceId);
}

void CameraHandler::changeCamera(const QByteArray &deviceId)
{
    stopPreview();
    delete m_camera;
    delete m_imageCapture;
    delete m_recorder;

    QCameraDevice selectedDevice;
    const QList<QCameraDevice> devices = QMediaDevices::videoInputs();
    for (const QCameraDevice &device : devices) {
        if (device.id() == deviceId) {
            selectedDevice = device;
            break;
        }
    }

    if (selectedDevice.isNull()) {
        m_statusLabel->setText("Камера не найдена.");
        return;
    }

    m_camera = new QCamera(selectedDevice, this);
    m_captureSession.setCamera(m_camera);
    m_imageCapture = new QImageCapture(this);
    m_captureSession.setImageCapture(m_imageCapture);
    m_recorder = new QMediaRecorder(this);
    m_captureSession.setRecorder(m_recorder);
    updateCameraInfo();
}

void CameraHandler::updateCameraInfo()
{
    if (m_camera) {
        QCameraDevice dev = m_camera->cameraDevice();
        QString positionStr;
        switch (dev.position()) {
        case QCameraDevice::FrontFace: positionStr = "Фронтальная"; break;
        case QCameraDevice::BackFace: positionStr = "Задняя"; break;
        default: positionStr = "Неизвестно"; break;
        }
        QString info = "Камера: " + dev.description() + "\n"
                       + "ID: " + QString(dev.id());
        m_cameraInfoLabel->setText(info);
    }
}
