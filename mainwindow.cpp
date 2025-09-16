#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "animationplayer.h"
#include "animations.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{
    ui->setupUi(this);

    // -------------------- Инициализация Мониторинна Батареи --------------------
    batteryMonitor = new BatteryMonitor(this);

    // Подвязываем
    powerSourceLabel = ui->PowerSourceLabel;
    batteryLevelLabel = ui->BatteryLevelLabel;
    powerSavingLabel = ui->PowerSavingModeLabel;
    batteryTimeLabel = ui->BatteryTimeLabel;
    sleepBtn = ui->SleepButton;
    hibernateBtn = ui->HibernateButton;

    // Обновляем UI при изменении статуса батареи
    connect(batteryMonitor, &BatteryMonitor::statusChanged, this, &MainWindow::updateBatteryUI);
    connect(batteryMonitor, &BatteryMonitor::chargingChanged, this, &MainWindow::updateChargingAnimation);

    // Кнопки сна и гибернации
    connect(sleepBtn, &QPushButton::clicked, batteryMonitor, &BatteryMonitor::sleep);
    connect(hibernateBtn, &QPushButton::clicked, batteryMonitor, &BatteryMonitor::hibernate);


    // Начальное обновление UI
    updateBatteryUI();


    connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, &MainWindow::onPageChanged);
    this->setStatusBar(nullptr);

    //Устанавливаем курсор
    QCursor cursorTarget = QCursor(QPixmap(":/coursor/resources/coursore/coursore.png"), 0, 0);
    this->setCursor(cursorTarget);

// -------------------------------------------------------------------------- Анимации Main Window
    // Подключение анимации к label
    characterAnim = new AnimationPlayer(ui->CharacterLabel, this);
    backgroundAnim = new AnimationPlayer(ui->BackgroundLabel, this);

    characterAnim->setLoopAnimation(getLoopFrames());
    backgroundAnim->setLoopAnimation(getBackgroundFrames());
// -------------------------------------------------------------------------- Анимации L1 Window
    characterL1Anim = new AnimationPlayer(ui->GrimmL1, this);
    //Цикловая Анимация
    characterL1Anim->setLoopAnimation(getLoopFrames());

// -------------------------------------------------------------------------- Отдельные анимации
    //ПОКЛОН
    Animation reveranceAnim;
    reveranceAnim.frames = getReveranceFrames();
    reveranceAnim.loop = false;
    characterAnim->addAnimation("reverance", reveranceAnim);
    characterL1Anim->addAnimation("reverance", reveranceAnim);
    //ИНТРО
    Animation introAnim;
    introAnim.frames = getIntroFrames();
    introAnim.loop = false;
    characterAnim->addAnimation("intro", introAnim);
    characterL1Anim->addAnimation("intro", introAnim);
    //ЩЕЛЧЕК
    Animation handsOutAnim;
    handsOutAnim.frames = getHandsOutFrames();
    handsOutAnim.loop = false;
    characterAnim->addAnimation("handsOut", handsOutAnim);
    characterL1Anim->addAnimation("handsOut", handsOutAnim);
    //ИНТРО ЗАРЯДКА
    Animation IntroChargingAnim;
    IntroChargingAnim.frames = getIntroChargingFrames();
    IntroChargingAnim.loop = false;
    characterL1Anim->addAnimation("introCharging", IntroChargingAnim);
    //ОУТРО ЗАРЯДКА
    Animation OutroChargingAnim;
    OutroChargingAnim.frames = getOutroChargingFrames();
    OutroChargingAnim.loop = false;
    characterL1Anim->addAnimation("outroCharging", OutroChargingAnim);


// -------------------------------------------------------------------------- Запуск анимаций
    characterAnim->playAnimation("intro");
    characterAnim->start();
    characterL1Anim->start();
    backgroundAnim->start();

// ---------- ПЕРЕХОД МЕЖДУ ОКНАМИ------------------------
    //Main->L1
    connect(ui->Lab1BTN, &QPushButton::clicked, this, [this](){
        characterAnim->playSequence({"reverance", "handsOut"}, [this](){
            ui->stackedWidget->setCurrentIndex(1);
        });
    });
    //L1->Main
    connect(ui->Back, &QPushButton::clicked, this, [=]() {
        characterL1Anim->playSequence({"reverance", "intro"}, [this](){
            ui->GrimmL1->hide();
            QTimer::singleShot(500, this, [this]() {
                ui->stackedWidget->setCurrentIndex(0);
            });

        });
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Действия при смене страницы
void MainWindow::onPageChanged(int index)
{
    switch (index) {
    case 0: // Главная страница
        characterAnim->playAnimation("intro");
        break;
    case 1: // Вторая страница
        ui->BACK_BTNLabel->hide();ui->Back->hide();ui->BatteryLevelLabel->hide();ui->BatteryTimeLabel->hide();ui->BatteryTypeLabel->hide();
        ui->GrimmL1->hide();ui->HibernateButton->hide();ui->Hibernate_BTNLabel_3->hide();ui->Info_table->hide();ui->PowerSavingModeLabel->hide();
        ui->PowerSourceLabel->hide();ui->SLEEP_BTNLabel_2->hide();ui->SleepButton->hide();

        // 1 кадр
        ui->Background1Lab->setPixmap(QPixmap(":/bg/resources/img/Background/1Lab/Bg1frame.png"));

        // Через 1 сек второй кадр
        QTimer::singleShot(1000, this, [this]() {
            ui->Background1Lab->setPixmap(QPixmap(":/bg/resources/img/Background/1Lab/Bg2frame.png"));

            // Через 1 сек персонажа
            QTimer::singleShot(1000, this, [this]() {
                ui->GrimmL1->show();

                // Стартовая анимация персонажа
                characterL1Anim->playSequence({"intro", "handsOut"}, [this]() {
                    ui->Background1Lab->setPixmap(QPixmap(":/bg/resources/img/Background/1Lab/backgroundClear4k.png"));
                    ui->BACK_BTNLabel->show();ui->Back->show();ui->BatteryLevelLabel->show();ui->BatteryTimeLabel->show();
                    ui->BatteryTypeLabel->show();ui->HibernateButton->show();ui->Hibernate_BTNLabel_3->show();
                    ui->Info_table->show();ui->PowerSavingModeLabel->show();ui->PowerSourceLabel->show();ui->SLEEP_BTNLabel_2->show();ui->SleepButton->show();
                    characterL1Anim->setLoopAnimation(getLoopFrames());

                    // После стартовой анимации проверяем зарядку
                    updateChargingAnimation(batteryMonitor->isCharging());
                });
            });
        });
        break;
    // Добавляем case для каждой страницы
    default:
        characterAnim->playAnimation("intro"); // запасной вариант
        break;
    }
}

void MainWindow::updateBatteryUI()
{
    // ------------------ Источник питания ------------------
    ui->PowerSourceLabel->setText("Источник питания: " + batteryMonitor->powerSource());

    // ------------------ Уровень заряда -------------------
    batteryLevelLabel->setText("Заряд: " + QString::number(batteryMonitor->batteryLevel()) + "%");

    // ------------------ Время работы батареи -----------
    QString msg;
    if (batteryMonitor->isCharging()) {
        if (batteryMonitor->batteryLevel() >= 100) {
            msg = "Аккумулятор полностью заряжен";
        } else {
            int remainingSec = batteryMonitor->batteryFullLifeSeconds();
            if (remainingSec < 0) remainingSec = 0;
            int hours = remainingSec / 3600;
            int mins = (remainingSec % 3600) / 60;
            msg = QString("До полной зарядки осталось: %1 ч %2 мин").arg(hours).arg(mins);
        }
    } else {
        int remainingSec = batteryMonitor->batteryLifeSeconds();
        if (remainingSec < 0) remainingSec = 0;
        int hours = remainingSec / 3600;
        int mins = (remainingSec % 3600) / 60;
        msg = QString("До полной разрядки осталось: %1 ч %2 мин").arg(hours).arg(mins);
    }
    ui->BatteryTimeLabel->setText(msg);

    // ------------------ Режим энергосбережения ----------------
    QString savingMode = batteryMonitor->powerSavingMode();
    ui->PowerSavingModeLabel->setText("Режим энергосбережения: " + savingMode);

    // ------------------ Тип аккумулятора -----------------
    ui->BatteryTypeLabel->setText("Тип аккумулятора: " + batteryMonitor->batteryType());
}


void MainWindow::updateChargingAnimation(bool charging)
{
    // Если мы не на странице 2, ничего не делаем
    if(ui->stackedWidget->currentIndex() != 1)
        return;

    if(charging) {
        // Зарядка подключена → IntroCharging → LoopCharging
        characterL1Anim->playSequence({"introCharging"}, [this]() {
            characterL1Anim->setLoopAnimation(getLoopChargingFrames());
        });
    } else {
        // Зарядка отключена → OutroCharging → обычный loop
        characterL1Anim->playSequence({"outroCharging"}, [this]() {
            characterL1Anim->setLoopAnimation(getLoopFrames());
        });
    }
}

