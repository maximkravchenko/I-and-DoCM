#ifndef ANIMATIONPLAYER_H
#define ANIMATIONPLAYER_H

#include <QObject>
#include <QLabel>
#include <QPixmap>
#include <QVector>
#include <QMap>
#include <QTimer>
#include <QPainter>

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

private slots:
    void updateFrame();
    void resumeFromPause();

private:
    void renderFrame(const QPixmap& pix); // рисуем кадр в рамке

    QLabel* label;
    QTimer* timer;
    QTimer* pauseTimer;
    int currentFrame;
    bool isPaused;

    Animation* currentAnimation;

    QMap<QString, Animation> animations;
    QString currentAnimationName;
    QVector<QPixmap> loopFrames; // основной луп

signals:
    void animationFinished(const QString& name);
};
#endif // ANIMATIONPLAYER_H
