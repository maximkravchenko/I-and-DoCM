#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "animationplayer.h"
#include "animations.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{
    ui->setupUi(this);
    connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, &MainWindow::onPageChanged);


    //Устанавливаем курсор
    QCursor cursorTarget = QCursor(QPixmap(":/coursor/resources/coursore/coursore.png"), 0, 0);
    this->setCursor(cursorTarget);

    // Подключение анимации к label
    characterAnim = new AnimationPlayer(ui->CharacterLabel, this);
    backgroundAnim = new AnimationPlayer(ui->BackgroundLabel, this);


    // Установка анимации которая будет циклом
    characterAnim->setLoopAnimation(getLoopFrames());
    backgroundAnim->setLoopAnimation(getBackgroundFrames());

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
    backgroundAnim->start();


    // Запуск анимации по кнопке
    connect(ui->Lab1BTN, &QPushButton::clicked, this, [this](){
        characterAnim->playSequence({"reverance", "handsOut"}, [this](){
            ui->stackedWidget->setCurrentIndex(1);
        });
    });

    connect(ui->Lab2BTN, &QPushButton::clicked, this, [this](){
        characterAnim->playAnimation("reverance");
    });





    connect(ui->Back, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentIndex(0);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::onPageChanged(int index)
{
    switch (index) {
    case 0: // Главная страница
        characterAnim->playAnimation("intro");
        break;
    case 1: // Вторая страница
        characterAnim->playAnimation("reverance"); // или любая анимация для страницы
        break;
    // Добавляем case для каждой страницы
    default:
        characterAnim->playAnimation("intro"); // запасной вариант
        break;
    }
}
