#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "animationplayer.h"
#include "animations.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{
    ui->setupUi(this);

    //Устанавливаем курсор
    QCursor cursorTarget = QCursor(QPixmap(":/coursor/resources/coursore/coursore.png"), 0, 0);
    this->setCursor(cursorTarget);

    // Подключение анимации к label
    characterAnim = new AnimationPlayer(ui->CharacterLabel, this);

    // Установка анимации которая будет циклом
    characterAnim->setLoopAnimation(getLoopFrames());

    // Установка отдельной анимации ПОКЛОН
    Animation reveranceAnim;
    reveranceAnim.frames = getReveranceFrames();
    reveranceAnim.loop = false;
    characterAnim->addAnimation("reverance", reveranceAnim);
    // Установка анимации ИНТРО
    Animation introAnim;
    introAnim.frames = getIntroFrames();
    introAnim.loop = false;
    characterAnim->addAnimation("intro", introAnim);
    // Установка отдельной плащ
    Animation cloakAnim;
    cloakAnim.frames = getCloakFrames();
    cloakAnim.loop = false;
    characterAnim->addAnimation("cloak", cloakAnim);
    // Установка отдельной анимации Щелчек
    Animation handsOutAnim;
    handsOutAnim.frames = getHandsOutFrames();
    handsOutAnim.loop = false;
    characterAnim->addAnimation("handsOut", handsOutAnim);

    // Начало анимации персонажа
    characterAnim->playAnimation("intro");
    characterAnim->start();


    // Запуск анимации по кнопке
    connect(ui->Lab1BTN, &QPushButton::clicked, this, [=](){
        characterAnim->playAnimation("reverance");
    });

    connect(ui->Lab2BTN, &QPushButton::clicked, this, [=](){
        characterAnim->playAnimation("intro");
    });

    connect(ui->Lab3BTN, &QPushButton::clicked, this, [=](){
        characterAnim->playAnimation("cloak");
    });

    connect(ui->SKIP, &QPushButton::clicked, this, [this]() {
        characterAnim->playSequence({"reverance", "handsOut"}, [this]() {
            ui->stackedWidget->setCurrentIndex(1);
        });
    });





    connect(ui->Back, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentIndex(0);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
