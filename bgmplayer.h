#ifndef BGMPLAYER_H
#define BGMPLAYER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>

class BgmPlayer : public QObject
{
    Q_OBJECT
public:
    explicit BgmPlayer(QObject *parent = nullptr);

    // 播放BGM
    void playBgm(const QString& filePath);
    // 暂停/继续BGM
    void toggleBgm();

private:
    QMediaPlayer m_player;
    QAudioOutput m_audioOutput; // Qt6播放音频必须搭配QAudioOutput
};

#endif // BGMPLAYER_H
