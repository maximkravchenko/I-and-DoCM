#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <QVector>
#include <QPixmap>


inline QVector<QPixmap> getIntroFrames() {
    static QVector<QPixmap> f = []{
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
    }();
    return f;
}
inline QVector<QPixmap> getLoopFrames() {
    static QVector<QPixmap> f = []{
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
    }();
    return f;

}

inline QVector<QPixmap>& getReveranceFrames() {
    static QVector<QPixmap> f = []{
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
    }();
    return f;


}

inline QVector<QPixmap>& getCloakFrames() {
    static QVector<QPixmap> f = []{
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
    }();
    return f;


}

inline QVector<QPixmap>& getHandsOutFrames() {
    static QVector<QPixmap> f = []{
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
    }();
    return f;
}



inline QVector<QPixmap>& getBackgroundFrames() {
    static QVector<QPixmap> f = []{
        QVector<QPixmap> frames;
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_001.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_002.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_003.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_004.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_005.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_006.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_007.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_008.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_009.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_010.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_011.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_012.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_013.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_014.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_015.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_016.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_017.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_018.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_019.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_020.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_021.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_022.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_023.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_024.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_025.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_026.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_027.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_028.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_029.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_030.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_031.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_032.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_033.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_034.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_035.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_036.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_037.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_038.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_039.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_040.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_041.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_042.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_043.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_044.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_045.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_046.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_047.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_048.jpg"));
        frames.append(QPixmap(":/animated/background/resources/animations/background/background_frame_049.jpg"));
        return frames;
    }();
    return f;
}
#endif // ANIMATIONS_H
