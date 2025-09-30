#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTimer>
#include <QPixmap>
#include <QVector>
#include "animationplayer.h"
#include "batterymonitor.h"
#include <QPushButton>
#include <QLabel>
#include <QPropertyAnimation>
#include <QMap>
#include "pcideviceswidget.h"


#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onPageChanged(int index); // Когда происходит смена окон
    void updateBatteryUI(); // слот для обновления UI батареи
    void updateChargingAnimation(bool charging); // Обновление анимац при смене тпа питания

private:
    Ui::MainWindow *ui;

    AnimationPlayer* characterAnim;
    AnimationPlayer* characterL1Anim;
    AnimationPlayer* characterL2Anim;
    AnimationPlayer* backgroundAnim;

    BatteryMonitor* batteryMonitor;

    // Метки и кнопки для батареи
    QLabel* powerSourceLabel;
    QLabel* batteryLevelLabel;
    QLabel* powerSavingLabel;
    QLabel* batteryTimeLabel;
    QPushButton* sleepBtn;
    QPushButton* hibernateBtn;
    QMap<QWidget*, QRect> m_savedGeometries;

    PciDevicesWidget *pciDevicesWidget;
};

#endif
