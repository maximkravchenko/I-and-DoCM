#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <QVector>
#include <QPixmap>


inline QVector<QPixmap> getIntroFrames() {
    QVector<QPixmap> frames;
    frames.append(QPixmap(":/grimm_animation/t+s/resources/animations/grimm/teleport+stay/TeleToIdle_000.png"));
    frames.append(QPixmap(":/grimm_animation/t+s/resources/animations/grimm/teleport+stay/TeleToIdle_001.png"));
    frames.append(QPixmap(":/grimm_animation/t+s/resources/animations/grimm/teleport+stay/TeleToIdle_002.png"));
    frames.append(QPixmap(":/grimm_animation/t+s/resources/animations/grimm/teleport+stay/TeleToIdle_003.png"));
    frames.append(QPixmap(":/grimm_animation/t+s/resources/animations/grimm/teleport+stay/TeleToIdle_004.png"));
    frames.append(QPixmap(":/grimm_animation/t+s/resources/animations/grimm/teleport+stay/TeleToIdle_005.png"));
    frames.append(QPixmap(":/grimm_animation/t+s/resources/animations/grimm/teleport+stay/TeleToIdle_006.png"));
    frames.append(QPixmap(":/grimm_animation/t+s/resources/animations/grimm/teleport+stay/TeleToIdle_007.png"));
    return frames;
}


inline QVector<QPixmap> getLoopFrames() {
    QVector<QPixmap> frames;
    frames.append(QPixmap(":/grimm_animation/t+s/resources/animations/grimm/teleport+stay/TeleToIdle_008.png"));
    frames.append(QPixmap(":/grimm_animation/t+s/resources/animations/grimm/teleport+stay/TeleToIdle_009.png"));
    frames.append(QPixmap(":/grimm_animation/t+s/resources/animations/grimm/teleport+stay/TeleToIdle_010.png"));
    frames.append(QPixmap(":/grimm_animation/t+s/resources/animations/grimm/teleport+stay/TeleToIdle_011.png"));
    frames.append(QPixmap(":/grimm_animation/t+s/resources/animations/grimm/teleport+stay/TeleToIdle_012.png"));
    frames.append(QPixmap(":/grimm_animation/t+s/resources/animations/grimm/teleport+stay/TeleToIdle_013.png"));
    frames.append(QPixmap(":/grimm_animation/t+s/resources/animations/grimm/teleport+stay/TeleToIdle_014.png"));
    frames.append(QPixmap(":/grimm_animation/t+s/resources/animations/grimm/teleport+stay/TeleToIdle_015.png"));
    frames.append(QPixmap(":/grimm_animation/t+s/resources/animations/grimm/teleport+stay/TeleToIdle_016.png"));
    frames.append(QPixmap(":/grimm_animation/t+s/resources/animations/grimm/teleport+stay/TeleToIdle_017.png"));
    frames.append(QPixmap(":/grimm_animation/t+s/resources/animations/grimm/teleport+stay/TeleToIdle_018.png"));
    frames.append(QPixmap(":/grimm_animation/t+s/resources/animations/grimm/teleport+stay/TeleToIdle_019.png"));
    return frames;
}

inline QVector<QPixmap> getReveranceFrames() {
    QVector<QPixmap> frames;
    frames.append(QPixmap(":/grimm_animatiom/bow/resources/animations/grimm/bow/Bow_000.png"));
    frames.append(QPixmap(":/grimm_animatiom/bow/resources/animations/grimm/bow/Bow_001.png"));
    frames.append(QPixmap(":/grimm_animatiom/bow/resources/animations/grimm/bow/Bow_002.png"));
    frames.append(QPixmap(":/grimm_animatiom/bow/resources/animations/grimm/bow/Bow_003.png"));
    frames.append(QPixmap(":/grimm_animatiom/bow/resources/animations/grimm/bow/Bow_004.png"));
    frames.append(QPixmap(":/grimm_animatiom/bow/resources/animations/grimm/bow/Bow_005.png"));
    frames.append(QPixmap(":/grimm_animatiom/bow/resources/animations/grimm/bow/Bow_006.png"));
    frames.append(QPixmap(":/grimm_animatiom/bow/resources/animations/grimm/bow/Bow_006.png"));
    frames.append(QPixmap(":/grimm_animatiom/bow/resources/animations/grimm/bow/Bow_005.png"));
    frames.append(QPixmap(":/grimm_animatiom/bow/resources/animations/grimm/bow/Bow_004.png"));
    frames.append(QPixmap(":/grimm_animatiom/bow/resources/animations/grimm/bow/Bow_003.png"));
    frames.append(QPixmap(":/grimm_animatiom/bow/resources/animations/grimm/bow/Bow_002.png"));
    frames.append(QPixmap(":/grimm_animatiom/bow/resources/animations/grimm/bow/Bow_001.png"));
    frames.append(QPixmap(":/grimm_animatiom/bow/resources/animations/grimm/bow/Bow_000.png"));
    return frames;
}

inline QVector<QPixmap> getCloakFrames() {
    QVector<QPixmap> frames;
    frames.append(QPixmap(":/grimm_animation/open_cloak/resources/animations/grimm/open_cloak/Absorb Start_000.png"));
    frames.append(QPixmap(":/grimm_animation/open_cloak/resources/animations/grimm/open_cloak/Absorb Start_001.png"));
    frames.append(QPixmap(":/grimm_animation/open_cloak/resources/animations/grimm/open_cloak/Absorb Start_002.png"));
    frames.append(QPixmap(":/grimm_animation/open_cloak/resources/animations/grimm/open_cloak/Absorb Start_003.png"));
    frames.append(QPixmap(":/grimm_animation/open_cloak/resources/animations/grimm/open_cloak/Absorb Start_004.png"));
    frames.append(QPixmap(":/grimm_animation/open_cloak/resources/animations/grimm/open_cloak/Absorb Start_005.png"));
    frames.append(QPixmap(":/grimm_animation/open_cloak/resources/animations/grimm/open_cloak/Absorb Start_006.png"));
    frames.append(QPixmap(":/grimm_animation/open_cloak/resources/animations/grimm/open_cloak/Absorb Start_007.png"));
    frames.append(QPixmap(":/grimm_animation/open_cloak/resources/animations/grimm/open_cloak/Absorb Start_008.png"));
    return frames;
}

inline QVector<QPixmap> getHandsOutFrames() {
    QVector<QPixmap> frames;
    frames.append(QPixmap(":/grimm_animation/hands_out/resources/animations/grimm/hands_out/Hand Out_0000.png"));
    frames.append(QPixmap(":/grimm_animation/hands_out/resources/animations/grimm/hands_out/Hand Out_0001.png"));
    frames.append(QPixmap(":/grimm_animation/hands_out/resources/animations/grimm/hands_out/Hand Out_0002.png"));
    frames.append(QPixmap(":/grimm_animation/hands_out/resources/animations/grimm/hands_out/Hand Out_0003.png"));
    frames.append(QPixmap(":/grimm_animation/hands_out/resources/animations/grimm/hands_out/Hand Out_0004.png"));
    frames.append(QPixmap(":/grimm_animation/hands_out/resources/animations/grimm/hands_out/Hand Out_0005.png"));
    frames.append(QPixmap(":/grimm_animation/hands_out/resources/animations/grimm/hands_out/Hand Out_0006.png"));
    frames.append(QPixmap(":/grimm_animation/hands_out/resources/animations/grimm/hands_out/Hand Out_0007.png"));
    frames.append(QPixmap(":/grimm_animation/hands_out/resources/animations/grimm/hands_out/Hand Out_0008.png"));
    frames.append(QPixmap(":/grimm_animation/hands_out/resources/animations/grimm/hands_out/Hand Out_0009.png"));
    frames.append(QPixmap(":/grimm_animation/hands_out/resources/animations/grimm/hands_out/Hand Out_0010.png"));
    return frames;
}
#endif // ANIMATIONS_H
