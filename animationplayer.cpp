#include "animationplayer.h"

AnimationPlayer::AnimationPlayer(QLabel* targetLabel, QObject *parent)
    : QObject(parent),
    label(targetLabel),
    timer(new QTimer(this)),
    pauseTimer(new QTimer(this)),
    currentFrame(0),
    isPaused(false),
    currentAnimation(nullptr)
{
    timer->setInterval(50); // 50 мс на кадр по умолчанию
    connect(timer, &QTimer::timeout, this, &AnimationPlayer::updateFrame);

    pauseTimer->setSingleShot(true);
    connect(pauseTimer, &QTimer::timeout, this, &AnimationPlayer::resumeFromPause);
}

void AnimationPlayer::addAnimation(const QString &name, const Animation &anim)
{
    animations[name] = anim;
}

void AnimationPlayer::playAnimation(const QString &name)
{
    if (!animations.contains(name)) return;

    currentAnimation = &animations[name];
    currentAnimationName = name;  // <-- сохраняем имя
    currentFrame = 0;
    isPaused = false;
}


void AnimationPlayer::setLoopAnimation(const QVector<QPixmap>& frames)
{
    loopFrames = frames;
}

void AnimationPlayer::start()
{
    timer->start();
}

void AnimationPlayer::stop()
{
    timer->stop();
}

void AnimationPlayer::updateFrame()
{
    if (currentAnimation) {
        if (isPaused) return;

        // проверка паузы
        if (currentAnimation->pauseFrames.contains(currentFrame)) {
            isPaused = true;
            pauseTimer->start(currentAnimation->pauseFrames[currentFrame]);
            renderFrame(currentAnimation->frames[currentFrame]);
            return;
        }

        // показываем кадр
        renderFrame(currentAnimation->frames[currentFrame]);

        currentFrame++;
        if (currentFrame >= currentAnimation->frames.size()) {
            if (currentAnimation->loop) {
                currentFrame = 0;
            } else {
                QString finishedName = currentAnimationName; // <-- берем из поля
                currentAnimation = nullptr;
                currentAnimationName.clear();
                currentFrame = 0;

                emit animationFinished(finishedName); // <-- сигналим
            }
        }
    }
    else if (!loopFrames.isEmpty()) {
        renderFrame(loopFrames[currentFrame]);
        currentFrame = (currentFrame + 1) % loopFrames.size();
    }
}

void AnimationPlayer::resumeFromPause()
{
    isPaused = false;
    currentFrame++;
}

void AnimationPlayer::playSequence(const QStringList& names, std::function<void()> onFinish)
{
    if (names.isEmpty()) {
        if (onFinish) onFinish();
        return;
    }

    // Копия списка в "умном" указателе
    auto sequence = QSharedPointer<QStringList>::create(names);

    // Копия колбэка (тоже в умный указатель, чтобы не потерялся)
    auto finishCallback = QSharedPointer<std::function<void()>>::create(onFinish);

    // Рекурсивная функция
    auto playNext = QSharedPointer<std::function<void()>>::create();

    *playNext = [this, sequence, playNext, finishCallback]() {
        if (sequence->isEmpty()) {
            if (*finishCallback) {
                (*finishCallback)();
            }
            return;
        }

        QString anim = sequence->takeFirst();
        playAnimation(anim);

        QMetaObject::Connection *c = new QMetaObject::Connection;
        *c = connect(this, &AnimationPlayer::animationFinished, this,
                     [this, anim, playNext, c](const QString& name) {
                         if (name == anim) {
                             disconnect(*c);
                             delete c; // освобождаем
                             (*playNext)();
                         }
                     });
    };

    // Запуск первой анимации
    (*playNext)();
}





void AnimationPlayer::renderFrame(const QPixmap& pix)
{
    // Размер рамки — как у QLabel
    QSize boxSize = label->size();

    // Холст фиксированного размера
    QPixmap base(boxSize);
    base.fill(Qt::transparent);

    // Рисуем кадр по центру
    QPainter painter(&base);
    painter.drawPixmap(
        (boxSize.width() - pix.width()) / 2,
        (boxSize.height() - pix.height()) / 2,
        pix
        );
    painter.end();

    // Отправляем в label
    label->setPixmap(base);
}
