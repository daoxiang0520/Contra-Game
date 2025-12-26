#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>

class SoundPlayer : public QObject
{
    Q_OBJECT
public:
    explicit SoundPlayer(QObject *parent = nullptr);

    // 播放音效（默认不循环）
    void playSound(const QString& filePath, int loops = 1);
    // 停止音效播放
    void stopSound();
    // 暂停/继续音效
    void toggleSound();

private:
    QMediaPlayer m_player;
    QAudioOutput m_audioOutput;
};

#endif // SOUNDPLAYER_H
