#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTimer>
#include <QPixmap>
#include <QVector>
#include "animationplayer.h"

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

private:
    Ui::MainWindow *ui;
    AnimationPlayer* characterAnim;

};
#endif
