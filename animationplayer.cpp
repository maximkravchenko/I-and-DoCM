#include "animationplayer.h"
#include <QPainter>
#include <QDebug>

AnimationPlayer::AnimationPlayer(QLabel* targetLabel, QObject *parent)
    : QObject(parent), label(targetLabel)
{
    timer.setInterval(50); // 50 мс на кадр по умолчанию
    connect(&timer, &QTimer::timeout, this, &AnimationPlayer::updateFrame);

    pauseTimer.setSingleShot(true);
    connect(&pauseTimer, &QTimer::timeout, this, &AnimationPlayer::resumeFromPause);
}

void AnimationPlayer::addAnimation(const QString &name, const Animation &anim)
{
    animations[name] = anim;
}

void AnimationPlayer::playAnimation(const QString &name)
{
    if (!animations.contains(name)) {
        qWarning() << "Animation not found:" << name;
        return;
    }

    currentAnimationName = name;
    currentFrame = 0;
    isPaused = false;
}

void AnimationPlayer::setLoopAnimation(const QVector<QPixmap>& frames)
{
    loopFrames = frames;
}

void AnimationPlayer::start()
{
    if (!timer.isActive()) timer.start();
}

void AnimationPlayer::stop()
{
    timer.stop();
}

void AnimationPlayer::updateFrame()
{
    if (!label) return;

    if (!currentAnimationName.isEmpty() && animations.contains(currentAnimationName)) {
        const Animation& anim = animations[currentAnimationName];
        if (isPaused) return;

        // проверка паузы
        if (anim.pauseFrames.contains(currentFrame)) {
            isPaused = true;
            pauseTimer.start(anim.pauseFrames[currentFrame]);
            renderFrame(anim.frames[currentFrame]);
            return;
        }

        renderFrame(anim.frames[currentFrame]);

        currentFrame++;
        if (currentFrame >= anim.frames.size()) {
            if (anim.loop) {
                currentFrame = 0;
            } else {
                QString finishedName = currentAnimationName;
                currentAnimationName.clear();
                currentFrame = 0;
                emit animationFinished(finishedName);

                // Если идет последовательность
                if (!sequenceQueue.isEmpty()) {
                    QString nextAnim = sequenceQueue.takeFirst();
                    playAnimation(nextAnim);
                } else if (sequenceCallback) {
                    auto callback = sequenceCallback;
                    sequenceCallback = nullptr;
                    callback();
                }
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

    sequenceQueue = names;
    sequenceCallback = onFinish;

    QString firstAnim = sequenceQueue.takeFirst();
    playAnimation(firstAnim);
}

void AnimationPlayer::renderFrame(const QPixmap& pix)
{
    if (!label) return;
    if (pix.isNull()) return;

    QSize boxSize = label->size();
    QPixmap scaled = pix.scaled(boxSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPixmap base(boxSize);
    base.fill(Qt::transparent);

    QPainter painter(&base);
    painter.drawPixmap(
        (boxSize.width() - scaled.width()) / 2,
        (boxSize.height() - scaled.height()) / 2,
        scaled
        );

    label->setPixmap(base);
}

