#ifndef ANIMATIONPLAYER_H
#define ANIMATIONPLAYER_H

#include <QObject>
#include <QLabel>
#include <QPixmap>
#include <QVector>
#include <QMap>
#include <QTimer>

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
    void start(); // запуск таймера
    void stop();  // остановка анимации

private slots:
    void updateFrame();
    void resumeFromPause();

private:
    QLabel* label;
    QTimer* timer;
    QTimer* pauseTimer;
    int currentFrame;
    bool isPaused;

    Animation* currentAnimation;
    QMap<QString, Animation> animations;

    QVector<QPixmap> loopFrames; // основной луп
};
#endif // ANIMATIONPLAYER_H
