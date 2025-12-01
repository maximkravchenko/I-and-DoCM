#ifndef BLUETOOTHAPP_H
#define BLUETOOTHAPP_H

#include <QDialog> // Обязательно первым делом
#include <QWidget> // На всякий случай явно
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QTimer>

// Bluetooth
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QBluetoothLocalDevice>

// Multimedia
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMediaDevices>
#include <QAudioDevice>

// Путь к файлу по умолчанию
#define AUDIO_FILE_PATH "C:/FINAL/audio/file.mp3"

class BluetoothApp : public QDialog // Наследуемся от QDialog
{
    Q_OBJECT

public:
    explicit BluetoothApp(QWidget *parent = nullptr);
    ~BluetoothApp();

private slots:
    // --- Логика Друга (Мониторинг) ---
    void onMonitorToggleClicked();
    void startScanLoop();
    void scanFinished();
    void addDiscoveredDevice(const QBluetoothDeviceInfo &device);

    // Сопряжение
    void onDeviceListClicked(QListWidgetItem *item);
    void onPairClicked();
    void pairingDone(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing status);

    // --- Логика Плеера (Отправка звука) ---
    void updateAudioDevices();
    void updateDeviceHighlights();
    void onAudioOutputChanged(int index);

    void onSelectSongClicked();
    void onPlayPauseClicked();
    void onPositionChanged(int position);
    void updatePosition(qint64 position);
    void updateDuration(qint64 duration);
    void updatePlaybackState(QMediaPlayer::PlaybackState newState);

private:
    void checkBluetoothStatus();

    // UI элементы
    QPushButton *monitorToggleButton;
    QListWidget *deviceListWidget;
    QLabel *statusLabel;
    QPushButton *pairButton;

    // Плеер UI
    QLabel *songLabel;
    QPushButton *playPauseButton;
    QSlider *positionSlider;
    QComboBox *audioOutputComboBox;
    QPushButton *selectSongButton;

    // Bluetooth Logic
    QBluetoothDeviceDiscoveryAgent *m_discoveryAgent;
    QBluetoothLocalDevice *m_localDevice;
    QBluetoothAddress m_selectedDeviceAddress;
    QTimer *m_scanTimer;

    // Audio Logic
    QMediaPlayer *m_player;
    QAudioOutput *m_audioOutput;
    QString m_songPath;
    QList<QAudioDevice> m_audioDevices;
};

#endif // BLUETOOTHAPP_H
