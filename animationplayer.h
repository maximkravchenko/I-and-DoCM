#pragma once

#include <QObject>
#include <QLabel>
#include <QPixmap>
#include <QVector>
#include <QMap>
#include <QTimer>
#include <QPointer>
#include <functional>

// Анимация
struct Animation {
    QVector<QPixmap> frames;
    QMap<int,int> pauseFrames;
    bool loop = false;
};


class AnimationPlayer : public QObject
{
    Q_OBJECT
public:
    explicit AnimationPlayer(QLabel* targetLabel, QObject *parent = nullptr);

    // Добавление новой анимации к объекту
    void addAnimation(const QString& name, const Animation& anim);
    // Запуск анимации
    void playAnimation(const QString& name);
    // Установка крговой анимации
    void setLoopAnimation(const QVector<QPixmap>& frames);
    // Вызов действия после проигрыша анимации
    void playSequence(const QStringList& names, std::function<void()> onFinish = {});

    // Контроль таймера
    void start();
    void stop();

signals:
    // Сигнал о том, что анимация закончена
    void animationFinished(const QString& name) const;

private slots:
    // Слот для обновления таймера
    void updateFrame();
    // Слот воспроизведения после конца паузы
    void resumeFromPause();

private:
    // Отрисовка кадра на QLable
    void renderFrame(const QPixmap& pix);

    // поля состояний
    QPointer<QLabel> label;
    QTimer timer;
    QTimer pauseTimer;

    int currentFrame = 0;
    bool isPaused = false;

    QString currentAnimationName;
    QMap<QString, Animation> animations;
    QVector<QPixmap> loopFrames;

    // Поля для очереди
    QStringList sequenceQueue;
    std::function<void()> sequenceCallback;
};
