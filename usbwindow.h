#ifndef USBWINDOW_H
#define USBWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QMap>
#include <QString>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDateTime>
#include <QMessageBox>
#include <QDebug>
#include <QCloseEvent>
#include <QTextCursor>

#ifdef Q_OS_WIN
#include <windows.h>
#include <dbt.h>
#include <setupapi.h>
#include <cfgmgr32.h>
#endif

struct USBDeviceInfo {
    QString devicePath;
    QString deviceName;
    QString deviceType;
    QString volumeName;
    QStringList driveLetters;
    bool isRemovable = false;
#ifdef Q_OS_WIN
    DEVINST devInst = 0;
#endif
    QString instanceId;
};

class USBWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit USBWindow(QWidget *parent = nullptr);
    ~USBWindow();

signals:
    void deviceConnected(const QString &deviceName);
    void deviceDisconnected(const QString &deviceName);

protected:
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onEjectButtonClicked();
    void onRefreshButtonClicked();
    void onDeviceSelectionChanged();

private:
    void setupUI();
    void applyStyles();
    void scanUSBDevices();
    void addLogMessage(const QString &message, const QString &color = "#000000");

#ifdef Q_OS_WIN
    void handleDeviceArrival(DEV_BROADCAST_HDR *pHdr);
    void handleDeviceRemoval(DEV_BROADCAST_HDR *pHdr);
    QString getDeviceDescription(const QString &devicePath);
    QString getDeviceFriendlyName(HDEVINFO deviceInfoSet, SP_DEVINFO_DATA &deviceInfoData);
    QString getDeviceClass(HDEVINFO deviceInfoSet, SP_DEVINFO_DATA &deviceInfoData);
    QStringList getDriveLettersForDevice(const QString &instanceId);
    bool ejectDevice(const USBDeviceInfo &deviceInfo);
    bool isDeviceEjectable(const QString &devicePath);
    DEVINST getDeviceInstance(const QString &devicePath);
    QString getDeviceInstanceId(DEVINST devInst);
    bool isMyMouse(const QString &instanceId);
#endif

    // UI Components
    QListWidget *m_deviceList = nullptr;
    QTextEdit *m_logConsole = nullptr;
    QPushButton *m_ejectButton = nullptr;
    QPushButton *m_refreshButton = nullptr;

    // Device tracking
    QMap<QString, USBDeviceInfo> m_devices;
#ifdef Q_OS_WIN
    HDEVNOTIFY m_deviceNotifyHandle = nullptr;
#endif
};

#endif // USBWINDOW_H
