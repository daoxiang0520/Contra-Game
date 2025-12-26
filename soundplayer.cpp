#include "soundplayer.h"
#include "globalsound.h"

SoundPlayer::SoundPlayer(QObject *parent) : QObject(parent) {
    // 关联全局音效音量
    m_player.setAudioOutput(&m_audioOutput);
    auto globalSound = GlobalSound::instance();

    // 同步音量变化
    connect(globalSound, &GlobalSound::volumeChanged, this, [this](int volume) {
        m_audioOutput.setVolume(static_cast<qreal>(volume) / 100.0);
    }, Qt::QueuedConnection);

    // 同步静音状态
    connect(globalSound, &GlobalSound::mutedChanged, this, [this](bool muted) {
        m_audioOutput.setMuted(muted);
    }, Qt::QueuedConnection);

    // 初始化音量和静音状态
    qreal initVolume = static_cast<qreal>(globalSound->volume()) / 100.0;
    bool initMuted = globalSound->isMuted();
    m_audioOutput.setVolume(initVolume);
    m_audioOutput.setMuted(initMuted);
}

void SoundPlayer::playSound(const QString& filePath, int loops) {
    m_player.setSource(QUrl::fromLocalFile(filePath));

    // 设置循环次数 默认一次不循环
    m_player.setLoops(loops);
    m_player.play();
}

void SoundPlayer::stopSound() {
    m_player.stop();
}

void SoundPlayer::toggleSound() {
    if (m_player.playbackState() == QMediaPlayer::PlayingState) {
        m_player.pause();
    } else {
        m_player.play();
    }
}
