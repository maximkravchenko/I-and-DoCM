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
}

MainWindow::~MainWindow()
{
    delete ui;
}
