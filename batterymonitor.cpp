#include "batterymonitor.h"
#include <QDebug>
#include <windows.h>
#include <powrprof.h> // Для перехода в спящий/гибернацию

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
    SYSTEM_POWER_STATUS oldStatus = status;
    if (GetSystemPowerStatus(&status))
    {
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
