#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "animationplayer.h"
#include "animations.h"
#include "iostream"

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

    usbWindow = new USBWindow(this);
    connect(ui->Lab5BTN, &QPushButton::clicked, this, [this]() {
        usbWindow->show();
        usbWindow->raise();
        usbWindow->activateWindow();
    });


    // Обновляем UI при изменении статуса батареи
    connect(batteryMonitor, &BatteryMonitor::statusChanged, this, &MainWindow::updateBatteryUI);
    connect(batteryMonitor, &BatteryMonitor::chargingChanged, this, &MainWindow::updateChargingAnimation);

    // Кнопки сна и гибернации
    connect(sleepBtn, &QPushButton::clicked, batteryMonitor, &BatteryMonitor::sleep);
    connect(hibernateBtn, &QPushButton::clicked, batteryMonitor, &BatteryMonitor::hibernate);


    // Начальное обновление UI
    updateBatteryUI();

    // Инициализация виджета PCI
    pciDevicesWidget = new PciDevicesWidget(ui->PCITable, this);
    
    QVideoWidget *previewWidget = new QVideoWidget(ui->Lab4_Page);
    previewWidget->hide();
    previewWidget->setGeometry(QRect(320,150, 471, 301)); // Подкорректируйте позицию/размер под дизайн

    QLabel *cameraInfoLabel = new QLabel(ui->Lab4_Page);
    cameraInfoLabel->setGeometry(QRect(870, 160, 200, 300));
    cameraInfoLabel->setStyleSheet("color: white; font-size: 17px;");
    cameraInfoLabel->setWordWrap(true);
    cameraInfoLabel->setText("Информация о камере...");

    QLabel *statusLabel = new QLabel(ui->Lab4_Page);
    statusLabel->setGeometry(QRect(870, 210, 161, 40 ));
    statusLabel->setStyleSheet("color: lightgreen; font-size: 14px;");
    statusLabel->setText("Статус камеры...");

    // Инициализация CameraHandler с этими виджетами
    cameraHandler = new CameraHandler(this, previewWidget, cameraInfoLabel, statusLabel, ui);



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
// -------------------------------------------------------------------------- Анимации L2 Window
    characterL2Anim = new AnimationPlayer(ui->GrimmL2, this);
    //Цикловая Анимация
    characterL2Anim->setLoopAnimation(getLoopFrames());

// -------------------------------------------------------------------------- Анимации L1 Window
    characterL4Anim = new AnimationPlayer(ui->GrimmL4, this);
    //Цикловая Анимация
    characterL4Anim->setLoopAnimation(getLoopFrames());

// -------------------------------------------------------------------------- Отдельные анимации
    //ПОКЛОН
    Animation reveranceAnim;
    reveranceAnim.frames = getReveranceFrames();
    reveranceAnim.loop = false;
    characterAnim->addAnimation("reverance", reveranceAnim);
    characterL1Anim->addAnimation("reverance", reveranceAnim);
    characterL4Anim->addAnimation("reverance", reveranceAnim);
    //ИНТРО
    Animation introAnim;
    introAnim.frames = getIntroFrames();
    introAnim.loop = false;
    characterAnim->addAnimation("intro", introAnim);
    characterL1Anim->addAnimation("intro", introAnim);
    characterL4Anim->addAnimation("intro", introAnim);
    characterL2Anim->addAnimation("intro", introAnim);
    //БЭК
    Animation backAnim;
    backAnim.frames = getBackFrames();
    backAnim.loop = false;
    characterL2Anim->addAnimation("back", backAnim);
    //ОУТРО
    Animation outroAnim;
    outroAnim.frames = getOutroFrames();
    outroAnim.loop = false;
    characterAnim->addAnimation("outro", outroAnim);
    characterL1Anim->addAnimation("outro", outroAnim);
    characterL4Anim->addAnimation("outro", outroAnim);
    //ЩЕЛЧЕК
    Animation handsOutAnim;
    handsOutAnim.frames = getHandsOutFrames();
    handsOutAnim.loop = false;
    characterAnim->addAnimation("handsOut", handsOutAnim);
    characterL1Anim->addAnimation("handsOut", handsOutAnim);
    characterL4Anim->addAnimation("handsOut", handsOutAnim);
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
    //Плащ
    Animation CloackAnim;
    CloackAnim.frames = getCloakFrames();
    CloackAnim.loop = false;
    characterAnim->addAnimation("cloackAnim", CloackAnim);



// -------------------------------------------------------------------------- Запуск анимаций
    characterAnim->playAnimation("intro");
    characterAnim->start();
    characterL1Anim->start();
    characterL2Anim->start();
    characterL4Anim->start();
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
        characterL1Anim->playSequence({"reverance", "outro"}, [this](){
            ui->GrimmL1->hide();
            QTimer::singleShot(500, this, [this]() {
                ui->stackedWidget->setCurrentIndex(0);
            });

        });
    });
    //Main->L2
    connect(ui->Lab2BTN, &QPushButton::clicked, this, [this]() {
        // защитный указатель на лейбл
        QPointer<QLabel> label = ui->CharacterLabel;
        if (!label) return;

        // сохраним исходную геометрию (если ещё не сохранена)
        if (!m_savedGeometries.contains(label)) {
            m_savedGeometries[label] = label->geometry();
        }
        const QRect origGeom = m_savedGeometries[label];

        // вычислим новую геометрию (увеличение в 1.5 раза по центру)
        const qreal scale = 1.8; // можно поставить 3.0 если нужен больший zoom
        const int newW = static_cast<int>(origGeom.width() * scale);
        const int newH = static_cast<int>(origGeom.height() * scale);
        const QPoint center = origGeom.center();
        const QRect bigGeom(center.x() - newW/2, center.y() - newH/2, newW, newH);

        // Немедленно устанавливаем увеличенную геометрию (без анимации)
        label->setGeometry(bigGeom);

        // Просим цикл событий нарисовать обновление до старта анимации персонажа.
        // Вместо QCoreApplication::processEvents() используем singleShot(0) — безопаснее.
        QTimer::singleShot(0, this, [this, label, origGeom]() {
            if (!label) {
                this->setEnabled(true);
                return;
            }

            // Теперь запускаем последовательность анимаций персонажа.
            // ВАЖНО: убедись, что имя анимации написано точно так же, как при добавлении.
            // В твоём коде есть getCloakFrames() — имя должно совпадать с тем, что ты добавляешь.
            characterAnim->playSequence({"cloackAnim"}, [this, label, origGeom]() {
                // Переключаем страницу (индекс ставь тот, который нужен)
                const int targetIndex = 2; // <- заменить в случае необходимости
                ui->stackedWidget->setCurrentIndex(targetIndex);

                // Восстанавливаем исходную геометрию сразу (без анимации)
                if (label) {
                    label->setGeometry(origGeom);
                }

                // Включаем интерфейс обратно
                this->setEnabled(true);
            });
        });
    });
    //L2->Main
    connect(ui->L2backToMain, &QPushButton::clicked, this, [=]() {
        characterL2Anim->playSequence({"back"}, [this](){
            ui->stackedWidget->setCurrentIndex(0);
            pciDevicesWidget->refreshDevices();
        });
    });

    connect(ui->Lab3BTN, &QPushButton::clicked, this, [this](){
        ui->stackedWidget->setCurrentIndex(3);
    });

    connect(ui->Back_BTN_NoLab, &QPushButton::clicked, this, [this](){
        ui->stackedWidget->setCurrentIndex(0);
    });
// LMainn -> L4
    connect(ui->Lab4BTN, &QPushButton::clicked, this, [this](){
        characterAnim->playSequence({"reverance", "outro"}, [this](){
            ui->stackedWidget->setCurrentIndex(4);
        });
    });


}



MainWindow::~MainWindow()
{
    delete ui;
    delete cameraHandler;
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
    case 2: // Лаба 2
        characterL2Anim->playAnimation("intro");
        pciDevicesWidget->refreshDevices();
        break;
    case 4: // Lab4
        //cameraHandler->startPreview();
        break;
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
            msg = QString("До полной зарядки: %1 ч %2 мин").arg(hours).arg(mins);
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
    ui->BatteryTypeLabel->setText("Тип аккумулятора: " + batteryMonitor->batteryChemistry());

}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
    if (eventType == "windows_generic_MSG") {
        MSG *msg = static_cast<MSG *>(message);
        if (msg->message == WM_HOTKEY && msg->wParam == CameraHandler::HOTKEY_ID) {
            show();  // Показываем окно
            cameraHandler->stopVideo();
            cameraHandler->unregisterHotkey();
            cameraHandler->restoreVideoOutput();  // Восстанавливаем вывод!
            *result = 0;
            return true;
        }
    }
    return QMainWindow::nativeEvent(eventType, message, result);
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

