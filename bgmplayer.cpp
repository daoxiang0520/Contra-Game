#include "bgmplayer.h"
#include "globalaudio.h"

BgmPlayer::BgmPlayer(QObject *parent) : QObject(parent) {
    // 关联全局音量到BGM播放器
    m_player.setAudioOutput(&m_audioOutput);
    auto globalAudio = GlobalAudio::instance();
    // 监听全局音量变化，同步到BGM播放器
    connect(globalAudio, &GlobalAudio::volumeChanged, this, [this](int volume) {
        // 转换为QtMediaPlayer需要的0.0~1.0浮点音量
        m_audioOutput.setVolume(static_cast<qreal>(volume) / 100.0);
    }, Qt::QueuedConnection);
    // 监听全局静音变化，同步到BGM播放器
    connect(globalAudio, &GlobalAudio::mutedChanged, this, [this](bool muted) {
        m_audioOutput.setMuted(muted);
    }, Qt::QueuedConnection);
    // 初始音量和静音同步
    qreal initVolume = static_cast<qreal>(globalAudio->volume()) / 100.0;
    bool initMuted = globalAudio->isMuted();
    m_audioOutput.setVolume(initVolume);
    m_audioOutput.setMuted(initMuted);
}

// 播放BGM
void BgmPlayer::playBgm(const QString& filePath) {
    m_player.setSource(QUrl::fromLocalFile(filePath)); // 检索播放源
    m_player.setLoops(QMediaPlayer::Infinite); // 循环播放
    m_player.play();
}

// 暂停/继续BGM
void BgmPlayer::toggleBgm() {
    if (m_player.playbackState() == QMediaPlayer::PlayingState) {
        m_player.pause();
    } else {
        m_player.play();
    }
}
