#include "usbwindow.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <dbt.h>
#include <setupapi.h>
#include <cfgmgr32.h>
#include <initguid.h>
#include <usbiodef.h>
#endif

USBWindow::USBWindow(QWidget *parent)
    : QMainWindow(parent)
#ifdef Q_OS_WIN
    , m_deviceNotifyHandle(nullptr)
#endif
{
    setupUI();
    applyStyles();

#ifdef Q_OS_WIN
    // Register for device notifications (USB device interface)
    DEV_BROADCAST_DEVICEINTERFACE notificationFilter;
    ZeroMemory(&notificationFilter, sizeof(notificationFilter));
    notificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    notificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    notificationFilter.dbcc_classguid = GUID_DEVINTERFACE_USB_DEVICE;

    m_deviceNotifyHandle = RegisterDeviceNotification(
        (HANDLE)this->winId(),
        &notificationFilter,
        DEVICE_NOTIFY_WINDOW_HANDLE
        );
    if (!m_deviceNotifyHandle) {
        addLogMessage("⚠ Не удалось зарегистрировать уведомления об устройствах", "#e67e22");
    }
#endif

    QTimer::singleShot(100, this, &USBWindow::scanUSBDevices);
    addLogMessage("=== USB Мониторинг запущен ===", "#0066cc");
}

USBWindow::~USBWindow()
{
#ifdef Q_OS_WIN
    if (m_deviceNotifyHandle) {
        UnregisterDeviceNotification(m_deviceNotifyHandle);
    }
#endif
}

void USBWindow::setupUI()
{
    setWindowTitle("ЛР5: Мониторинг USB-устройств");
    resize(900, 600);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *titleLabel = new QLabel("Мониторинг USB-устройств", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50; padding: 10px;");
    mainLayout->addWidget(titleLabel);

    QLabel *deviceLabel = new QLabel("Подключенные USB-устройства:", this);
    deviceLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #34495e;");
    mainLayout->addWidget(deviceLabel);

    m_deviceList = new QListWidget(this);
    m_deviceList->setMinimumHeight(380);
    mainLayout->addWidget(m_deviceList);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);

    m_ejectButton = new QPushButton("Безопасное извлечение", this);
    m_ejectButton->setEnabled(false);
    m_ejectButton->setMinimumHeight(40);

    m_refreshButton = new QPushButton("Обновить список", this);
    m_refreshButton->setMinimumHeight(40);

    buttonLayout->addWidget(m_ejectButton);
    buttonLayout->addWidget(m_refreshButton);
    mainLayout->addLayout(buttonLayout);

    QLabel *logLabel = new QLabel("Журнал событий:", this);
    logLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #34495e;");
    mainLayout->addWidget(logLabel);

    m_logConsole = new QTextEdit(this);
    m_logConsole->setReadOnly(true);
    m_logConsole->setMinimumHeight(200);
    mainLayout->addWidget(m_logConsole);

    connect(m_ejectButton, &QPushButton::clicked, this, &USBWindow::onEjectButtonClicked);
    connect(m_refreshButton, &QPushButton::clicked, this, &USBWindow::onRefreshButtonClicked);
    connect(m_deviceList, &QListWidget::itemSelectionChanged, this, &USBWindow::onDeviceSelectionChanged);
}

void USBWindow::applyStyles()
{
    QString styleSheet = R"(
        QMainWindow { background-color: #ecf0f1; }
        QListWidget {
            background-color: white;
            color: #2c3e50;
            border: 2px solid #bdc3c7;
            border-radius: 8px;
            padding: 5px;
            font-size: 12px;
        }
        QListWidget::item { color: #2c3e50; padding: 8px; border-bottom: 1px solid #ecf0f1; }
        QListWidget::item:selected { background-color: #3498db; color: white; }
        QListWidget::item:hover { background-color: #e8f4f8; }
        QTextEdit {
            background-color: #2c3e50; color: #ecf0f1; border: 2px solid #34495e;
            border-radius: 8px; padding: 8px; font-family: 'Consolas','Courier New',monospace; font-size: 11px;
        }
        QPushButton {
            background-color: #3498db; color: white; border: none; border-radius: 6px;
            padding: 10px 20px; font-size: 13px; font-weight: bold;
        }
        QPushButton:hover { background-color: #2980b9; }
        QPushButton:pressed { background-color: #21618c; }
        QPushButton:disabled { background-color: #95a5a6; color: #bdc3c7; }
    )";
    setStyleSheet(styleSheet);
}

void USBWindow::scanUSBDevices()
{
    m_deviceList->clear();
    m_devices.clear();
    addLogMessage("Сканирование USB-устройств...", "#3498db");

#ifdef Q_OS_WIN
    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(
        &GUID_DEVINTERFACE_USB_DEVICE,
        NULL,
        NULL,
        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
        );

    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        addLogMessage("Ошибка: не удалось получить список устройств", "#e74c3c");
        return;
    }

    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    int deviceCount = 0;
    for (DWORD i = 0; SetupDiEnumDeviceInterfaces(deviceInfoSet, NULL, &GUID_DEVINTERFACE_USB_DEVICE, i, &deviceInterfaceData); ++i) {
        DWORD requiredSize = 0;
        SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, NULL, 0, &requiredSize, NULL);
        PSP_DEVICE_INTERFACE_DETAIL_DATA deviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(requiredSize);
        if (!deviceInterfaceDetailData) continue;

        deviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        SP_DEVINFO_DATA deviceInfoData;
        deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

        if (SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, deviceInterfaceDetailData, requiredSize, NULL, &deviceInfoData)) {
            QString devicePath = QString::fromWCharArray(deviceInterfaceDetailData->DevicePath);

            // Get friendly name (preferred) and description
            QString friendly = getDeviceFriendlyName(deviceInfoSet, deviceInfoData);
            QString desc = getDeviceDescription(devicePath);
            QString devClass = getDeviceClass(deviceInfoSet, deviceInfoData);

            QString deviceName = !friendly.isEmpty() ? friendly : (!desc.isEmpty() ? desc : QString("USB Device"));

            // Heuristics to filter out unwanted devices:
            // - hide Bluetooth adapters (Intel Wireless Bluetooth)
            // - hide generic composite devices unless they look like a mouse or storage
            QString lowerName = deviceName.toLower();
            QString lowerDesc = desc.toLower();
            QString lowerClass = devClass.toLower();

            bool looksLikeMouse = (lowerName.contains("mouse") || lowerDesc.contains("mouse") || lowerName.contains("logitech") || lowerDesc.contains("logitech") || lowerName.contains("g305"));
            bool looksLikeStorage = (lowerName.contains("mass storage") || lowerDesc.contains("mass storage") ||
                                     lowerName.contains("removable") || lowerDesc.contains("usb flash") ||
                                     lowerName.contains("запоминающее") || lowerDesc.contains("flash") || lowerDesc.contains("disk"));
            bool isBluetooth = (lowerName.contains("bluetooth") || lowerDesc.contains("bluetooth") || lowerClass.contains("bluetooth"));

            // If device is obviously Bluetooth and not a mouse/storage -> skip
            if (isBluetooth && !looksLikeMouse && !looksLikeStorage) {
                free(deviceInterfaceDetailData);
                continue;
            }

            // If device is a generic composite/usual "USB Device" and doesn't look like mouse or storage -> skip
            bool isGenericComposite = (lowerName.contains("composite") || lowerDesc.contains("composite") ||
                                       lowerName == "usb device" || lowerDesc == "usb device");

            if (isGenericComposite && !looksLikeMouse && !looksLikeStorage) {
                free(deviceInterfaceDetailData);
                continue;
            }

            USBDeviceInfo info;
            // Only append drive letters for devices that look like storage
            QStringList driveLetters;
            if (looksLikeStorage) {
                driveLetters = getDriveLettersForDevice(info.instanceId);
            }


            info.devicePath = devicePath;
            info.deviceName = deviceName;
            info.deviceType = looksLikeMouse ? "USB Mouse" : (looksLikeStorage ? "USB Flash Drive" : "USB Device");
            info.driveLetters = driveLetters;
            info.isRemovable = isDeviceEjectable(devicePath);
            info.devInst = deviceInfoData.DevInst;
            info.instanceId = getDeviceInstanceId(deviceInfoData.DevInst);
            // -------- ОПРЕДЕЛЕНИЕ ТВОЕЙ МЫШКИ --------
            if (isMyMouse(info.instanceId)) {
                info.deviceType = "USB Mouse";
                info.deviceName = "Logitech G305";
            }


            // Build visible text
            QString displayText;
            if (info.deviceType == "USB Flash Drive" && !driveLetters.isEmpty()) {
                displayText = QString("%1 (%2)").arg(info.deviceName, driveLetters.join(", "));
            } else {
                displayText = QString("%1 [%2]").arg(info.deviceName, info.deviceType);
            }

            // Put in map and list
            m_devices[devicePath] = info;
            QListWidgetItem *item = new QListWidgetItem(displayText);
            item->setData(Qt::UserRole, devicePath);
            m_deviceList->addItem(item);
            deviceCount++;

            addLogMessage(QString("✓ Обнаружено: %1").arg(displayText), "#27ae60");
        }

        free(deviceInterfaceDetailData);
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    addLogMessage(QString("Сканирование завершено. Найдено устройств: %1").arg(deviceCount), "#3498db");
#else
    addLogMessage("USB мониторинг доступен только на Windows", "#e74c3c");
#endif
}

#ifdef Q_OS_WIN
QString USBWindow::getDeviceFriendlyName(HDEVINFO deviceInfoSet, SP_DEVINFO_DATA &deviceInfoData)
{
    wchar_t buffer[256];

    if (SetupDiGetDeviceRegistryPropertyW(
            deviceInfoSet,
            &deviceInfoData,
            SPDRP_FRIENDLYNAME,
            NULL,
            (PBYTE)buffer,
            sizeof(buffer),
            NULL))
    {
        return QString::fromWCharArray(buffer);
    }

    if (SetupDiGetDeviceRegistryPropertyW(
            deviceInfoSet,
            &deviceInfoData,
            SPDRP_DEVICEDESC,
            NULL,
            (PBYTE)buffer,
            sizeof(buffer),
            NULL))
    {
        return QString::fromWCharArray(buffer);
    }

    return "Unknown USB Device";
}


QString USBWindow::getDeviceDescription(const QString &devicePath)
{
    // Lookup device description by enumerating interfaces and comparing paths (fallback)
    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(&GUID_DEVINTERFACE_USB_DEVICE, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (deviceInfoSet == INVALID_HANDLE_VALUE) return "USB Device";

    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInterfaces(deviceInfoSet, NULL, &GUID_DEVINTERFACE_USB_DEVICE, i, &deviceInterfaceData); ++i) {
        DWORD requiredSize = 0;
        SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, NULL, 0, &requiredSize, NULL);
        PSP_DEVICE_INTERFACE_DETAIL_DATA detail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(requiredSize);
        if (!detail) continue;
        detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        SP_DEVINFO_DATA devInfo;
        devInfo.cbSize = sizeof(SP_DEVINFO_DATA);
        if (SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, detail, requiredSize, NULL, &devInfo)) {
            QString curPath = QString::fromWCharArray(detail->DevicePath);
            if (curPath == devicePath) {
                WCHAR buffer[512] = {0};
                if (SetupDiGetDeviceRegistryProperty(deviceInfoSet, &devInfo, SPDRP_DEVICEDESC, NULL, (PBYTE)buffer, sizeof(buffer), NULL)) {
                    QString res = QString::fromWCharArray(buffer);
                    free(detail);
                    SetupDiDestroyDeviceInfoList(deviceInfoSet);
                    return res;
                }
            }
        }
        free(detail);
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    return "USB Device";
}

QString USBWindow::getDeviceClass(HDEVINFO deviceInfoSet, SP_DEVINFO_DATA &deviceInfoData)
{
    wchar_t buffer[256];

    if (SetupDiGetDeviceRegistryPropertyW(
            deviceInfoSet,
            &deviceInfoData,
            SPDRP_CLASS,
            NULL,
            (PBYTE)buffer,
            sizeof(buffer),
            NULL))
    {
        return QString::fromWCharArray(buffer);
    }

    return "UnknownClass";
}


QStringList USBWindow::getDriveLettersForDevice(const QString &instanceId)
{
    QStringList result;

    DWORD mask = GetLogicalDrives();

    for (int i = 0; i < 26; i++) {
        if (!(mask & (1 << i)))
            continue;

        QString letter = QString("%1:").arg(QChar('A' + i));
        QString root   = letter + "\\";

        UINT type = GetDriveTypeW((LPCWSTR)root.utf16());

        if (type != DRIVE_REMOVABLE)
            continue;

        wchar_t volumePath[MAX_PATH] = {0};
        if (!GetVolumeNameForVolumeMountPointW((LPCWSTR)root.utf16(), volumePath, MAX_PATH))
            continue;

        // Приводим всё в верхний регистр
        QString volumeId = QString::fromWCharArray(volumePath).toUpper();
        QString instId   = instanceId.toUpper();

        // Проверяем VID/PID
        // Пример: \\?\Volume{GUID}\ -> далее содержит DeviceInstance
        if (volumeId.contains("VID_") && volumeId.contains("PID_"))
        {
            QString vidPid = instId.section('\\', 1, 1); // берем VID_xxx&PID_xxx
            if (volumeId.contains(vidPid)) {
                result << letter;
                break; // одна буква — выходим
            }
        }
    }

    return result;
}


bool USBWindow::isDeviceEjectable(const QString &devicePath)
{
    // Heuristic: if description contains root_hub or is not mass storage, keep non-ejectable
    if (devicePath.contains("root_hub", Qt::CaseInsensitive)) return false;
    return true;
}

DEVINST USBWindow::getDeviceInstance(const QString &devicePath)
{
    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(&GUID_DEVINTERFACE_USB_DEVICE, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (deviceInfoSet == INVALID_HANDLE_VALUE) return 0;

    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInterfaces(deviceInfoSet, NULL, &GUID_DEVINTERFACE_USB_DEVICE, i, &deviceInterfaceData); ++i) {
        DWORD requiredSize = 0;
        SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, NULL, 0, &requiredSize, NULL);
        PSP_DEVICE_INTERFACE_DETAIL_DATA detail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(requiredSize);
        if (!detail) continue;
        detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        SP_DEVINFO_DATA devInfo;
        devInfo.cbSize = sizeof(SP_DEVINFO_DATA);
        if (SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, detail, requiredSize, NULL, &devInfo)) {
            QString curPath = QString::fromWCharArray(detail->DevicePath);
            if (curPath == devicePath) {
                DEVINST inst = devInfo.DevInst;
                free(detail);
                SetupDiDestroyDeviceInfoList(deviceInfoSet);
                return inst;
            }
        }
        free(detail);
    }
    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    return 0;
}

QString USBWindow::getDeviceInstanceId(DEVINST devInst)
{
    if (!devInst) return QString();
    WCHAR buffer[MAX_DEVICE_ID_LEN];
    if (CM_Get_Device_ID(devInst, buffer, MAX_DEVICE_ID_LEN, 0) == CR_SUCCESS) {
        return QString::fromWCharArray(buffer);
    }
    return QString();
}

bool USBWindow::ejectDevice(const USBDeviceInfo &deviceInfo)
{
    if (!deviceInfo.isRemovable) {
        addLogMessage("⚠ Устройство не поддерживает безопасное извлечение", "#e67e22");
        return false;
    }

    addLogMessage(QString("Попытка безопасного извлечения: %1").arg(deviceInfo.deviceName), "#3498db");

    PNP_VETO_TYPE vetoType;
    WCHAR vetoName[MAX_PATH];

    CONFIGRET result = CM_Request_Device_Eject(
        deviceInfo.devInst,
        &vetoType,
        vetoName,
        MAX_PATH,
        0
        );

    if (result == CR_SUCCESS) {
        addLogMessage(QString("✓ Устройство '%1' безопасно извлечено").arg(deviceInfo.deviceName), "#27ae60");
        emit deviceDisconnected(deviceInfo.deviceName);
        return true;
    } else {
        QString errorMsg = QString("Код ошибки: %1").arg((int)result);
        addLogMessage(QString("✗ ОТКАЗ в безопасном извлечении: %1").arg(errorMsg), "#e74c3c");
        if (vetoName[0] != L'\0') {
            addLogMessage(QString("  Причина: %1").arg(QString::fromWCharArray(vetoName)), "#e74c3c");
        }
        return false;
    }
}
#endif

void USBWindow::addLogMessage(const QString &message, const QString &color)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString formattedMessage = QString("<span style='color: #95a5a6;'>[%1]</span> <span style='color: %2;'>%3</span>")
                                   .arg(timestamp, color, message);
    m_logConsole->append(formattedMessage);
    QTextCursor cursor = m_logConsole->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_logConsole->setTextCursor(cursor);
}

void USBWindow::onEjectButtonClicked()
{
    QListWidgetItem *currentItem = m_deviceList->currentItem();
    if (!currentItem) return;

    QString devicePath = currentItem->data(Qt::UserRole).toString();
    if (!m_devices.contains(devicePath)) {
        addLogMessage("Ошибка: устройство не найдено", "#e74c3c");
        return;
    }

#ifdef Q_OS_WIN
    USBDeviceInfo deviceInfo = m_devices[devicePath];
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Безопасное извлечение");
    msgBox.setText(QString("Вы уверены, что хотите извлечь устройство?\n\n%1").arg(deviceInfo.deviceName));
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);

    if (msgBox.exec() == QMessageBox::Yes) {
        bool success = ejectDevice(deviceInfo);
        if (success) {
            QTimer::singleShot(1000, this, &USBWindow::scanUSBDevices);
        }
    }
#else
    addLogMessage("Безопасное извлечение доступно только на Windows", "#e74c3c");
#endif
}

void USBWindow::onRefreshButtonClicked()
{
    scanUSBDevices();
}

void USBWindow::onDeviceSelectionChanged()
{
    QListWidgetItem *currentItem = m_deviceList->currentItem();
    m_ejectButton->setEnabled(currentItem != nullptr);
}

bool USBWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
#ifdef Q_OS_WIN
    if (eventType == "windows_generic_MSG") {
        MSG *msg = static_cast<MSG *>(message);
        if (msg->message == WM_DEVICECHANGE) {
            DEV_BROADCAST_HDR *pHdr = reinterpret_cast<DEV_BROADCAST_HDR*>(msg->lParam);
            switch (msg->wParam) {
            case DBT_DEVICEARRIVAL:
                addLogMessage("=== СОБЫТИЕ: Подключение нового устройства ===", "#27ae60");
                handleDeviceArrival(pHdr);
                QTimer::singleShot(500, this, &USBWindow::scanUSBDevices);
                break;
            case DBT_DEVICEREMOVECOMPLETE:
                addLogMessage("=== СОБЫТИЕ: Устройство отключено ===", "#e67e22");
                handleDeviceRemoval(pHdr);
                QTimer::singleShot(500, this, &USBWindow::scanUSBDevices);
                break;
            case DBT_DEVICEQUERYREMOVE:
                addLogMessage("→ Запрос на безопасное извлечение устройства", "#3498db");
                break;
            case DBT_DEVICEQUERYREMOVEFAILED:
                addLogMessage("✗ НЕБЕЗОПАСНОЕ ИЗВЛЕЧЕНИЕ: Запрос отклонен!", "#e74c3c");
                break;
            case DBT_DEVICEREMOVEPENDING:
                addLogMessage("⏳ Ожидание завершения извлечения устройства...", "#f39c12");
                break;
            }
        }
    }
#endif
    return QMainWindow::nativeEvent(eventType, message, result);
}

#ifdef Q_OS_WIN
void USBWindow::handleDeviceArrival(DEV_BROADCAST_HDR *pHdr)
{
    if (!pHdr) return;
    if (pHdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
        auto *pDevInf = reinterpret_cast<DEV_BROADCAST_DEVICEINTERFACE*>(pHdr);
        QString devicePath = QString::fromWCharArray(pDevInf->dbcc_name);
        QString name = getDeviceDescription(devicePath);
        addLogMessage(QString("✓ Подключено: %1").arg(name), "#27ae60");
        emit deviceConnected(name);
    }
}

void USBWindow::handleDeviceRemoval(DEV_BROADCAST_HDR *pHdr)
{
    if (!pHdr) return;
    if (pHdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
        auto *pDevInf = reinterpret_cast<DEV_BROADCAST_DEVICEINTERFACE*>(pHdr);
        QString devicePath = QString::fromWCharArray(pDevInf->dbcc_name);
        QString deviceName = "Unknown Device";
        if (m_devices.contains(devicePath)) deviceName = m_devices[devicePath].deviceName;
        addLogMessage(QString("✓ Отключено: %1").arg(deviceName), "#e67e22");
        emit deviceDisconnected(deviceName);
    }
}
#endif

void USBWindow::closeEvent(QCloseEvent *event)
{
    addLogMessage("=== USB Мониторинг остановлен ===", "#95a5a6");
    QMainWindow::closeEvent(event);
}

bool USBWindow::isMyMouse(const QString &instanceId)
{
    QString id = instanceId.toUpper();

    // ТВОЯ МЫШЬ Logitech G305 — VID 046D, PID C53F
    return (id.contains("VID_046D") && id.contains("PID_C53F"));
}
