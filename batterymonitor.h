#pragma once
#include <QObject>
#include <QTimer>
#include <QString>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

class BatteryMonitor : public QObject
{
    Q_OBJECT
public:
    explicit BatteryMonitor(QObject *parent = nullptr);
    // ----------------- публичный API (читатели) -----------------
    QString powerSource() const;
    QString batteryType() const;
    int batteryLevel() const; // %
    QString powerSavingMode() const;
    int batteryLifeSeconds() const; // время работы оставшееся
    int batteryFullLifeSeconds() const; // время работы от полной зарядки
    bool isCharging() const { return m_charging; }

public slots:
    void updateStatus();
    void sleep();
    void hibernate();

signals:
    void statusChanged(); // сигнал для UI, когда данные обновились
    void chargingChanged(bool charging); // Сигнал когда подключение сети меняется

private:
    QTimer updateTimer;
#ifdef Q_OS_WIN
    SYSTEM_POWER_STATUS status;
#endif

    bool m_charging; // текущее состояние зарядки

};

