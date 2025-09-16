#include "batterymonitor.h"
#include <QDebug>
#include <windows.h>
#include <powrprof.h> // Для перехода в спящий/гибернацию
#include <setupapi.h>
#include <initguid.h>
#include <batclass.h>
#include <devguid.h>


BatteryMonitor::BatteryMonitor(QObject *parent)
    : QObject(parent)
{
    connect(&updateTimer, &QTimer::timeout, this, &BatteryMonitor::updateStatus);
    updateTimer.start(1000); // обновляем каждую секунду
    updateStatus();
    m_charging = (status.ACLineStatus == 1);

}

void BatteryMonitor::updateStatus()
{
#ifdef Q_OS_WIN
    //SYSTEM_POWER_STATUS oldStatus = status;
    if (GetSystemPowerStatus(&status))
    {
        // обновляем химию аккумулятора (раз в update)
        m_batteryChemistry = queryBatteryChemistry();

        emit statusChanged();

        bool nowCharging = (status.ACLineStatus == 1);
        if (nowCharging != m_charging) {
            m_charging = nowCharging;
            emit chargingChanged(m_charging);
        }
    }
#endif
}


QString BatteryMonitor::powerSource() const
{
#ifdef Q_OS_WIN
    switch (status.ACLineStatus) {
    case 0: return "Батарея";
    case 1: return "Сеть";
    default: return "Неизвестно";
    }
#else
    return "Не поддерживается";
#endif
}

QString BatteryMonitor::batteryType() const
{
#ifdef Q_OS_WIN
    return "Аккумулятор"; // стандартное обозначение в Windows
#else
    return "Не поддерживается";
#endif
}

int BatteryMonitor::batteryLevel() const
{
#ifdef Q_OS_WIN
    return status.BatteryLifePercent; // %
#else
    return -1;
#endif
}

QString BatteryMonitor::powerSavingMode() const
{
#ifdef Q_OS_WIN
    GUID *pSchemeGuid = nullptr;
    if (PowerGetActiveScheme(NULL, &pSchemeGuid) == ERROR_SUCCESS) {
        // Читаем "дружественное имя" схемы
        WCHAR friendlyName[256];
        DWORD size = sizeof(friendlyName);
        if (PowerReadFriendlyName(NULL, pSchemeGuid, NULL, NULL,
                                  (UCHAR*)friendlyName, &size) == ERROR_SUCCESS)
        {
            LocalFree(pSchemeGuid);
            return QString::fromWCharArray(friendlyName);
        }
        LocalFree(pSchemeGuid);
    }
    return "Не удалось определить";
#else
    return "Не поддерживается";
#endif
}



int BatteryMonitor::batteryLifeSeconds() const
{
#ifdef Q_OS_WIN
    return status.BatteryLifeTime; // секунды до разрядки
#else
    return -1;
#endif
}

int BatteryMonitor::batteryFullLifeSeconds() const
{
#ifdef Q_OS_WIN
    return status.BatteryFullLifeTime; // полная зарядка
#else
    return -1;
#endif
}

void BatteryMonitor::sleep()
{
#ifdef Q_OS_WIN
    SetSuspendState(FALSE, TRUE, TRUE); // спящий режим
#endif
}

void BatteryMonitor::hibernate()
{
#ifdef Q_OS_WIN
    SetSuspendState(TRUE, TRUE, TRUE); // гибернация
#endif
}

QString BatteryMonitor::queryBatteryChemistry() const
{
#ifdef Q_OS_WIN
    HDEVINFO hDevInfo = SetupDiGetClassDevsW(&GUID_DEVICE_BATTERY, nullptr, nullptr,
                                             DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (hDevInfo == INVALID_HANDLE_VALUE) return "Неизвестно";

    SP_DEVICE_INTERFACE_DATA did{};
    did.cbSize = sizeof(did);
    if (!SetupDiEnumDeviceInterfaces(hDevInfo, nullptr, &GUID_DEVICE_BATTERY, 0, &did)) {
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return "Неизвестно";
    }

    DWORD reqSize = 0;
    SetupDiGetDeviceInterfaceDetailW(hDevInfo, &did, nullptr, 0, &reqSize, nullptr);
    std::vector<BYTE> buf(reqSize);
    auto pDetail = reinterpret_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA_W>(buf.data());
    pDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);

    if (!SetupDiGetDeviceInterfaceDetailW(hDevInfo, &did, pDetail, reqSize, nullptr, nullptr)) {
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return "Неизвестно";
    }

    HANDLE hBattery = CreateFileW(pDetail->DevicePath,
                                  GENERIC_READ | GENERIC_WRITE,
                                  FILE_SHARE_READ | FILE_SHARE_WRITE,
                                  nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    SetupDiDestroyDeviceInfoList(hDevInfo);
    if (hBattery == INVALID_HANDLE_VALUE) return "Неизвестно";

    BATTERY_QUERY_INFORMATION bqi{};
    DWORD bytes = 0;
    if (!DeviceIoControl(hBattery, IOCTL_BATTERY_QUERY_TAG,
                         nullptr, 0, &bqi.BatteryTag, sizeof(bqi.BatteryTag),
                         &bytes, nullptr) || bqi.BatteryTag == 0) {
        CloseHandle(hBattery);
        return "Неизвестно";
    }

    BATTERY_QUERY_INFORMATION q{};
    q.BatteryTag = bqi.BatteryTag;
    q.InformationLevel = BatteryInformation;
    BATTERY_INFORMATION bi{};
    if (!DeviceIoControl(hBattery, IOCTL_BATTERY_QUERY_INFORMATION,
                         &q, sizeof(q), &bi, sizeof(bi),
                         &bytes, nullptr)) {
        CloseHandle(hBattery);
        return "Неизвестно";
    }
    CloseHandle(hBattery);

    char chem[5]{};
    memcpy(chem, bi.Chemistry, 4);
    chem[4] = '\0';

    return QString::fromUtf8(chem);
#else
    return "Не поддерживается";
#endif
}

