#ifndef PCIDEVICESWIDGET_H
#define PCIDEVICESWIDGET_H

#include <QObject>
#include <QTableWidget>
#include <QHeaderView>
#include <QTableWidgetItem>

class PciDevicesWidget : public QObject
{
    Q_OBJECT

public:
    explicit PciDevicesWidget(QTableWidget *tableWidget, QObject *parent = nullptr);
    void refreshDevices();

private:
    QTableWidget *m_tableWidget;
    void setupTable();
    void populatePciDevices();
};

#endif // PCIDEVICESWIDGET_H
