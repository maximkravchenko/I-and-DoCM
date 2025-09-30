#include "pcideviceswidget.h"
#include <windows.h>
#include <setupapi.h>
#include <QDebug>
#include <QMessageBox>

// Для MinGW убираем pragma comment и добавляем в .pro файл LIBS += -lsetupapi

PciDevicesWidget::PciDevicesWidget(QTableWidget *tableWidget, QObject *parent)
    : QObject(parent), m_tableWidget(tableWidget)
{
    setupTable();
}

void PciDevicesWidget::setupTable()
{
    if (!m_tableWidget) return;

    // Настраиваем таблицу
    m_tableWidget->setColumnCount(2);
    m_tableWidget->setHorizontalHeaderLabels(QStringList() << "Vendor ID" << "Device ID");

    // Настройка внешнего вида таблицы
    m_tableWidget->horizontalHeader()->setStretchLastSection(true);
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableWidget->setShowGrid(true);
    m_tableWidget->setStyleSheet("QTableWidget { gridline-color: #d0d0d0; }");
}

void PciDevicesWidget::refreshDevices()
{
    if (!m_tableWidget) return;

    // Очищаем таблицу перед обновлением
    m_tableWidget->setRowCount(0);
    populatePciDevices();

    // Автоматически подгоняем размер столбцов под содержимое
    m_tableWidget->resizeColumnsToContents();
}

void PciDevicesWidget::populatePciDevices()
{
    if (!m_tableWidget) return;
    m_tableWidget->setRowCount(0);

    HDEVINFO deviceInfoSet;
    SP_DEVINFO_DATA deviceInfoData;
    DWORD deviceIndex = 0;
    DWORD error = 0;

    // Получаем информацию о всех PCI устройствах
    deviceInfoSet = SetupDiGetClassDevs(NULL, TEXT("PCI"), NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);

    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        error = GetLastError();
        qDebug() << "Ошибка при получении списка PCI устройств. Код:" << error;
        QMessageBox::warning(nullptr, "Ошибка",
                             QString("Не удалось получить информацию об устройствах PCI. Код ошибки: %1\nПроверьте права администратора.").arg(error));
        return;
    }

    qDebug() << "Начинаем перечисление PCI устройств...";

    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    int devicesFound = 0;

    // Перебираем все устройства
    while (SetupDiEnumDeviceInfo(deviceInfoSet, deviceIndex, &deviceInfoData)) {
        WCHAR hardwareID[1024];
        DWORD requiredSize = 0;

        // Получаем Hardware ID устройства
        if (SetupDiGetDeviceRegistryPropertyW(deviceInfoSet, &deviceInfoData,
                                              SPDRP_HARDWAREID, NULL,
                                              (PBYTE)hardwareID, sizeof(hardwareID) - sizeof(WCHAR),
                                              &requiredSize)) {

            hardwareID[sizeof(hardwareID)/sizeof(WCHAR) - 1] = L'\0';
            QString hardwareIDStr = QString::fromWCharArray(hardwareID).toUpper();

            // Ищем VendorID и DeviceID в строке Hardware ID
            if (hardwareIDStr.contains("PCI\\VEN_", Qt::CaseInsensitive)) {
                QStringList parts = hardwareIDStr.split("&");
                QString vendorID, deviceID;

                for (const QString &part : parts) {
                    QString cleanPart = part.trimmed();
                    if (cleanPart.startsWith("PCI\\VEN_", Qt::CaseInsensitive)) {
                        // Извлекаем 4 символа после VEN_
                        QString venPart = cleanPart.mid(8);
                        if (venPart.length() >= 4) {
                            vendorID = venPart.left(4);
                        }
                    } else if (cleanPart.startsWith("DEV_", Qt::CaseInsensitive)) {
                        // Извлекаем 4 символа после DEV_
                        QString devPart = cleanPart.mid(4);
                        if (devPart.length() >= 4) {
                            deviceID = devPart.left(4);
                        }
                    } else if (cleanPart.startsWith("SUBSYS_", Qt::CaseInsensitive)) {
                        // Пропускаем SUBSYS
                        continue;
                    }
                }

                // Если нашли оба ID, добавляем в таблицу
                if (!vendorID.isEmpty() && !deviceID.isEmpty()) {
                    int row = m_tableWidget->rowCount();
                    m_tableWidget->insertRow(row);

                    m_tableWidget->setItem(row, 0, new QTableWidgetItem(vendorID.toUpper()));
                    m_tableWidget->setItem(row, 1, new QTableWidgetItem(deviceID.toUpper()));
                    devicesFound++;

                    qDebug() << "Добавлено устройство: VendorID:" << vendorID << "DeviceID:" << deviceID;
                } else {
                    qDebug() << "Не удалось извлечь ID из:" << hardwareIDStr;
                }
            }
        } else {
            error = GetLastError();
            if (error != ERROR_INVALID_DATA) {
                qDebug() << "Ошибка получения HardwareID для устройства" << deviceIndex << "Код ошибки:" << error;
            }
        }

        deviceIndex++;
        ZeroMemory(&deviceInfoData, sizeof(SP_DEVINFO_DATA));
        deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    }

    error = GetLastError();
    if (error != ERROR_NO_MORE_ITEMS) {
        qDebug() << "Ошибка при перечислении устройств. Код:" << error;
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);

    qDebug() << "Перечисление завершено. Всего найдено PCI устройств:" << devicesFound;

    if (devicesFound == 0) {
        QMessageBox::information(nullptr, "Информация",
                                 "PCI устройства не найдены. Возможные причины:\n"
                                 "1. Отсутствуют права администратора\n"
                                 "2. Проблема с драйверами PCI\n"
                                 "3. Аппаратная проблема\n"
                                 "4. Попробуйте запустить программу от имени администратора");
    }
}
