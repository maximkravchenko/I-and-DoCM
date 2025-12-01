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
    // Register for device notifications
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
#endif

    // Initial scan
    QTimer::singleShot(100, this, &USBWindow::scanUSBDevices);

    addLogMessage("=== USB Мониторинг запущен ===", "white");
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
    setFixedSize(1101, 702);


    setWindowIcon(QIcon(":/icons/app.ico"));

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Title
    QLabel *titleLabel = new QLabel("Мониторинг USB-устройств", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 23px; font-weight: 700; color: white; padding: 10px;");
    mainLayout->addWidget(titleLabel);


    m_deviceList = new QListWidget(this);
    m_deviceList->setMinimumHeight(180);
    mainLayout->addWidget(m_deviceList);

    // Buttons
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

    m_logConsole = new QTextEdit(this);
    m_logConsole->setReadOnly(true);
    m_logConsole->setMinimumHeight(200);
    mainLayout->addWidget(m_logConsole);

    // Connect signals
    connect(m_ejectButton, &QPushButton::clicked, this, &USBWindow::onEjectButtonClicked);
    connect(m_refreshButton, &QPushButton::clicked, this, &USBWindow::onRefreshButtonClicked);
    connect(m_deviceList, &QListWidget::itemSelectionChanged, this, &USBWindow::onDeviceSelectionChanged);
}

void USBWindow::applyStyles()
{
    QString styleSheet = R"(
        QMainWindow {
            background-color: #401120;
        }
        QListWidget {
            background-color: black;
            color: white;
            border: 2px solid #bdc3c7;
            padding: 5px;
            font-size: 15px;
        }
        QListWidget::item {
            color: white;
            font-weight: 600;
            padding: 8px;
        }
        QListWidget::item:selected {
            background-color: white;
            color: black;
        }
        QListWidget::item:hover {
            background-color: white;
            color: black;
        }
        QTextEdit {
            background-color: black;
            color: white;
            border: 2px solid white;
            padding: 8px;

            font-size: 13px;
        }
        QPushButton {
            background-color: white;
            color: black;
            border: none;
            border: 1px solid black;
            padding: 10px 20px;
            font-size: 13px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #2980b9;
        }
        QPushButton:pressed {
            background-color: #21618c;
        }
        QPushButton:disabled {
            background-color: none;
            border: 1px solid white;
            color: white;
        }
    )";

    setStyleSheet(styleSheet);
}

void USBWindow::scanUSBDevices()
{
    m_deviceList->clear();
    m_devices.clear();

    addLogMessage("Сканирование USB-устройств...", "white");

#ifdef Q_OS_WIN
    // Получаем набор устройств
    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(
        &GUID_DEVINTERFACE_USB_DEVICE,
        NULL,
        NULL,
        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
        );

    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        addLogMessage("Ошибка: не удалось получить список устройств", "Red");
        return;
    }

    // Перебираем устройства
    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    int deviceCount = 0;

    for (DWORD i = 0; SetupDiEnumDeviceInterfaces(deviceInfoSet, NULL, &GUID_DEVINTERFACE_USB_DEVICE, i, &deviceInterfaceData); i++) {
        // Получение деталей интервейса
        DWORD requiredSize = 0;
        SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, NULL, 0, &requiredSize, NULL);

        PSP_DEVICE_INTERFACE_DETAIL_DATA deviceInterfaceDetailData =
            (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(requiredSize);

        if (!deviceInterfaceDetailData) continue;

        // Получаем доп данные к устроййтсву DevicePath — уникальный путь к устройству в системе. SP_DEVINFO_DATA — дополнительные данные о устройстве, включая DevInst, который нужен для безопасного извлечения.
        deviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        SP_DEVINFO_DATA deviceInfoData;
        deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);



        if (SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData,
                                            deviceInterfaceDetailData, requiredSize, NULL, &deviceInfoData)) {

            // Пытаемся достать имя или стандартное описание устройства
            QString devicePath = QString::fromWCharArray(deviceInterfaceDetailData->DevicePath);
            QString deviceName = getDeviceDescription(devicePath);
            QString deviceType = getDeviceType(devicePath);
            QStringList driveLetters = getDriveLettersForDevice(devicePath);

            USBDeviceInfo info;
            info.devicePath = devicePath;
            info.deviceName = deviceName;
            info.deviceType = deviceType;
            info.driveLetters = driveLetters;
            info.isRemovable = isDeviceEjectable(devicePath);
            info.devInst = deviceInfoData.DevInst;
            info.instanceId = getDeviceInstanceId(deviceInfoData.DevInst);

            m_devices[devicePath] = info;

            QString displayText = QString("%1 [%2]").arg(deviceName, deviceType);
            if (!driveLetters.isEmpty()) {
                displayText += QString("").arg(driveLetters.join(", "));
            }

            QListWidgetItem *item = new QListWidgetItem(displayText);
            item->setData(Qt::UserRole, devicePath);
            m_deviceList->addItem(item);
            deviceCount++;

            addLogMessage(QString("✓ Обнаружено: %1").arg(displayText), "green");
        }

        free(deviceInterfaceDetailData);
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);

    addLogMessage(QString("Сканирование завершено. Найдено устройств: %1").arg(deviceCount), "white");
#else
    addLogMessage("USB мониторинг доступен только на Windows", "#e74c3c");
#endif
}

#ifdef Q_OS_WIN
QString USBWindow::getDeviceDescription(const QString &devicePath)
{
    // Создаем набор устройств
    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(
        &GUID_DEVINTERFACE_USB_DEVICE,
        NULL,
        NULL,
        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
        );

    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        return "Unknown Device";
    }

    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    // Перебираем
    for (DWORD i = 0; SetupDiEnumDeviceInterfaces(deviceInfoSet, NULL, &GUID_DEVINTERFACE_USB_DEVICE, i, &deviceInterfaceData); i++) {
        // Получение пути
        DWORD requiredSize = 0;
        SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, NULL, 0, &requiredSize, NULL);


        // Получаем имя если совпадают пути
        PSP_DEVICE_INTERFACE_DETAIL_DATA deviceInterfaceDetailData =
            (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(requiredSize);

        if (!deviceInterfaceDetailData) continue;

        deviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        SP_DEVINFO_DATA deviceInfoData;
        deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

        if (SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData,
                                            deviceInterfaceDetailData, requiredSize, NULL, &deviceInfoData)) {

            QString currentPath = QString::fromWCharArray(deviceInterfaceDetailData->DevicePath);

            if (currentPath == devicePath) {
                WCHAR buffer[256];
                if (SetupDiGetDeviceRegistryProperty(deviceInfoSet, &deviceInfoData,
                                                     SPDRP_DEVICEDESC, NULL, (PBYTE)buffer, sizeof(buffer), NULL)) {
                    free(deviceInterfaceDetailData);
                    SetupDiDestroyDeviceInfoList(deviceInfoSet);
                    return QString::fromWCharArray(buffer);
                }
            }
        }

        free(deviceInterfaceDetailData);
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    // Если имя не найдено
    return "USB Device";
}

QString USBWindow::getDeviceType(const QString &devicePath)
{
    if (devicePath.contains("vid_", Qt::CaseInsensitive)) {
        if (devicePath.contains("mi_00")) return "USB Composite Device";
        if (devicePath.contains("mi_01")) return "USB Interface";
    }

    // Try to determine type from description
    QString desc = getDeviceDescription(devicePath);
    if (desc.contains("Mouse", Qt::CaseInsensitive)) return "USB Mouse";
    if (desc.contains("Keyboard", Qt::CaseInsensitive)) return "USB Keyboard";
    if (desc.contains("Storage", Qt::CaseInsensitive)) return "USB Mass Storage";
    if (desc.contains("Hub", Qt::CaseInsensitive)) return "USB Hub";
    if (desc.contains("Camera", Qt::CaseInsensitive)) return "USB Camera";

    return "USB Device";
}

QStringList USBWindow::getDriveLettersForDevice(const QString &devicePath)
{
    QStringList drives;

    DWORD driveMask = GetLogicalDrives();

    for (char letter = 'A'; letter <= 'Z'; letter++) {
        if (driveMask & (1 << (letter - 'A'))) {
            QString drivePath = QString("%1:\\").arg(letter);
            UINT driveType = GetDriveType(drivePath.toStdWString().c_str());

            if (driveType == DRIVE_REMOVABLE || driveType == DRIVE_FIXED) {
                // Check if this is a USB drive
                WCHAR volumeName[MAX_PATH];
                if (GetVolumeNameForVolumeMountPoint(drivePath.toStdWString().c_str(), volumeName, MAX_PATH)) {
                    QString volName = QString::fromWCharArray(volumeName);
                    if (driveType == DRIVE_REMOVABLE) {
                        drives.append(QString("%1:").arg(letter));
                    }
                }
            }
        }
    }

    return drives;
}

bool USBWindow::isDeviceEjectable(const QString &devicePath)
{
    // Most USB devices are ejectable except for some system devices
    return !devicePath.contains("root_hub", Qt::CaseInsensitive);
}

DEVINST USBWindow::getDeviceInstance(const QString &devicePath)
{
    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(
        &GUID_DEVINTERFACE_USB_DEVICE,
        NULL,
        NULL,
        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
        );

    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        return 0;
    }

    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInterfaces(deviceInfoSet, NULL, &GUID_DEVINTERFACE_USB_DEVICE, i, &deviceInterfaceData); i++) {
        DWORD requiredSize = 0;
        SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, NULL, 0, &requiredSize, NULL);

        PSP_DEVICE_INTERFACE_DETAIL_DATA deviceInterfaceDetailData =
            (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(requiredSize);

        if (!deviceInterfaceDetailData) continue;

        deviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        SP_DEVINFO_DATA deviceInfoData;
        deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

        if (SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData,
                                            deviceInterfaceDetailData, requiredSize, NULL, &deviceInfoData)) {

            QString currentPath = QString::fromWCharArray(deviceInterfaceDetailData->DevicePath);

            if (currentPath == devicePath) {
                DEVINST devInst = deviceInfoData.DevInst;
                free(deviceInterfaceDetailData);
                SetupDiDestroyDeviceInfoList(deviceInfoSet);
                return devInst;
            }
        }

        free(deviceInterfaceDetailData);
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    return 0;
}

QString USBWindow::getDeviceInstanceId(DEVINST devInst)
{
    WCHAR buffer[MAX_DEVICE_ID_LEN];
    if (CM_Get_Device_ID(devInst, buffer, MAX_DEVICE_ID_LEN, 0) == CR_SUCCESS) {
        return QString::fromWCharArray(buffer);
    }
    return QString();
}

bool USBWindow::ejectDevice(const USBDeviceInfo &deviceInfo)
{

    // Проверка возможности извлечения
    if (!deviceInfo.isRemovable) {
        addLogMessage("Устройство не поддерживает безопасное извлечение", "red");
        return false;
    }

    addLogMessage(QString("Попытка безопасного извлечения: %1").arg(deviceInfo.deviceName), "yellow");

    // Вызов Windows API CM_Request_Device_Eject
    PNP_VETO_TYPE vetoType; // Почему нельзя
    WCHAR vetoName[MAX_PATH]; // Служба отказа

    CONFIGRET result = CM_Request_Device_Eject(
        deviceInfo.devInst, // Идентификатор устрайства
        &vetoType,
        vetoName,
        MAX_PATH,
        0
        );

    if (result == CR_SUCCESS) {
        addLogMessage(QString("✓ Устройство '%1' безопасно извлечено").arg(deviceInfo.deviceName), "green");
        emit deviceDisconnected(deviceInfo.deviceName);
        return true;
    } else {
        QString errorMsg;
        switch (vetoType) {
        case PNP_VetoTypeUnknown:
            errorMsg = "Неизвестная причина отказа";
            break;
        case PNP_VetoLegacyDevice:
            errorMsg = "Устройство используется legacy драйвером";
            break;
        case PNP_VetoPendingClose:
            errorMsg = "Устройство используется приложением";
            break;
        case PNP_VetoWindowsApp:
            errorMsg = "Устройство используется приложением Windows";
            break;
        case PNP_VetoWindowsService:
            errorMsg = "Устройство используется службой Windows";
            break;
        case PNP_VetoOutstandingOpen:
            errorMsg = "Устройство имеет открытые дескрипторы";
            break;
        case PNP_VetoDevice:
            errorMsg = "Устройство отклонило запрос";
            break;
        case PNP_VetoDriver:
            errorMsg = "Драйвер отклонил запрос";
            break;
        case PNP_VetoIllegalDeviceRequest:
            errorMsg = "Недопустимый запрос для устройства";
            break;
        case PNP_VetoInsufficientPower:
            errorMsg = "Недостаточно питания";
            break;
        case PNP_VetoNonDisableable:
            errorMsg = "Устройство не может быть отключено";
            break;
        case PNP_VetoLegacyDriver:
            errorMsg = "Используется legacy драйвер";
            break;
        case PNP_VetoInsufficientRights:
            errorMsg = "Недостаточно прав доступа";
            break;
        default:
            errorMsg = QString("Код ошибки: %1").arg(result);
        }

        addLogMessage(QString("✗ ОТКАЗ в безопасном извлечении: %1").arg(errorMsg), "red");
        addLogMessage(QString("  Устройство: %1").arg(deviceInfo.deviceName), "white");

        if (vetoName[0] != L'\0') {
            QString vetoNameStr = QString::fromWCharArray(vetoName);
            addLogMessage(QString("  Причина: %1").arg(vetoNameStr), "red");
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

    // Auto-scroll to bottom
    QTextCursor cursor = m_logConsole->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_logConsole->setTextCursor(cursor);
}

void USBWindow::onEjectButtonClicked()
{
    QListWidgetItem *currentItem = m_deviceList->currentItem();
    if (!currentItem) return;


    // Берем путь устройства
    QString devicePath = currentItem->data(Qt::UserRole).toString();

    if (!m_devices.contains(devicePath)) {
        addLogMessage("Ошибка: устройство не найдено", "red");
        return;
    }

#ifdef Q_OS_WIN
    USBDeviceInfo deviceInfo = m_devices[devicePath];

    // Подтверждение пользователя
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Безопасное извлечение");
    msgBox.setText(QString("Вы уверены, что хотите извлечь устройство?\n\n%1").arg(deviceInfo.deviceName));
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);

    if (msgBox.exec() == QMessageBox::Yes) {
        // через функцию Plug and Play API отправляя дескриптор устройства извлекаем
        bool success = ejectDevice(deviceInfo);

        if (success) {
            // Refresh the list after a short delay
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
            DEV_BROADCAST_HDR *pHdr = (DEV_BROADCAST_HDR *)msg->lParam;

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
    if (pHdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
        DEV_BROADCAST_DEVICEINTERFACE *pDevInf = (DEV_BROADCAST_DEVICEINTERFACE *)pHdr;
        QString devicePath = QString::fromWCharArray(pDevInf->dbcc_name);
        QString deviceName = getDeviceDescription(devicePath);

        addLogMessage(QString("✓ Подключено: %1").arg(deviceName), "#27ae60");
        emit deviceConnected(deviceName);
    }
}

void USBWindow::handleDeviceRemoval(DEV_BROADCAST_HDR *pHdr)
{
    if (pHdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
        DEV_BROADCAST_DEVICEINTERFACE *pDevInf = (DEV_BROADCAST_DEVICEINTERFACE *)pHdr;
        QString devicePath = QString::fromWCharArray(pDevInf->dbcc_name);

        // Try to find device name from our cache
        QString deviceName = "Unknown Device";
        if (m_devices.contains(devicePath)) {
            deviceName = m_devices[devicePath].deviceName;
        }

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
