#pragma once

#include <QObject>
#include <QLabel>
#include <QPixmap>
#include <QVector>
#include <QMap>
#include <QTimer>
#include <QPointer>
#include <functional>

struct Animation {
    QVector<QPixmap> frames;
    QMap<int,int> pauseFrames; // кадр -> миллисекунды паузы
    bool loop = false;         // зацикливать или нет
};

class AnimationPlayer : public QObject
{
    Q_OBJECT
public:
    explicit AnimationPlayer(QLabel* targetLabel, QObject *parent = nullptr);

    void addAnimation(const QString& name, const Animation& anim);
    void playAnimation(const QString& name); // временная анимация
    void setLoopAnimation(const QVector<QPixmap>& frames); // основной зацикленный луп
    void playSequence(const QStringList& names, std::function<void()> onFinish = {});
    void start(); // запуск таймера
    void stop();  // остановка анимации

signals:
    void animationFinished(const QString& name) const;

private slots:
    void updateFrame();
    void resumeFromPause();

private:
    void renderFrame(const QPixmap& pix);

    QPointer<QLabel> label;       // Non-owning
    QTimer timer;
    QTimer pauseTimer;

    int currentFrame = 0;
    bool isPaused = false;

    QString currentAnimationName;
    QMap<QString, Animation> animations;
    QVector<QPixmap> loopFrames;  // основной луп

    // Для playSequence
    QStringList sequenceQueue;
    std::function<void()> sequenceCallback;
};
