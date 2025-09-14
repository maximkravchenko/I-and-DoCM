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
            label->setPixmap(currentAnimation->frames[currentFrame]);
            return;
        }

        // показываем кадр
        label->setPixmap(currentAnimation->frames[currentFrame]);

        currentFrame++;
        if (currentFrame >= currentAnimation->frames.size()) {
            if (currentAnimation->loop) {
                currentFrame = 0;
            } else {
                currentAnimation = nullptr; // возвращаемся к лупу
                currentFrame = 0;
            }
        }
    }
    else if (!loopFrames.isEmpty()) {
        label->setPixmap(loopFrames[currentFrame]);
        currentFrame = (currentFrame + 1) % loopFrames.size();
    }
}

void AnimationPlayer::resumeFromPause()
{
    isPaused = false;
    currentFrame++;
}
