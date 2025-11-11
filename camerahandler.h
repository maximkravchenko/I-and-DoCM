#pragma once
#include <QObject>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QImageCapture>
#include <QMediaRecorder>
#include <QVideoWidget>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QMainWindow>
#include <QDateTime>
#include <QDir>
#include <QFileDialog>
#include <QMediaDevices>
#include <windows.h> // Для WinAPI (RegisterHotKey)

class MainWindow; // Forward declaration
class Ui_MainWindow; // Forward declaration из ui_mainwindow.h

class CameraHandler : public QObject
{
    Q_OBJECT
public:
    explicit CameraHandler(MainWindow *mainWindow, QVideoWidget *previewWidget, QLabel *cameraInfoLabel, QLabel *statusLabel, Ui_MainWindow *ui, QObject *parent = nullptr);
    ~CameraHandler();
    void startPreview();
    void stopPreview();

private slots:
    void onSelectFolder();
    void onTakePhoto();
    void onStartStopVideo();
    void onTogglePreview();
    void onHiddenRecord();
    void onCheckCamera();
    void onCameraChanged(int index);
    void updateCameraInfo();

public:
    static const int HOTKEY_ID = 1;
    void restoreVideoOutput();
    void stopVideo();
    void unregisterHotkey();



private:
    void changeCamera(const QByteArray &deviceId);

    void startVideo();
    bool registerHotkey();

    MainWindow *m_mainWindow;
    Ui_MainWindow *m_ui;
    QCamera *m_camera = nullptr;
    QMediaCaptureSession m_captureSession;
    QImageCapture *m_imageCapture = nullptr;
    QMediaRecorder *m_recorder = nullptr;
    QVideoWidget *m_previewWidget;
    QLabel *m_cameraInfoLabel;
    QLabel *m_statusLabel;
    QString m_savePath;
    bool m_isPreviewing = false;
    bool m_isRecording = false;

};
