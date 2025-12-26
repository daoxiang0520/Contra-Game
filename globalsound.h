#ifndef GLOBALSOUND_H
#define GLOBALSOUND_H

#include <QObject>
#include <QAudioOutput>
#include <QMediaDevices>
#include <QMutex>
#include <QAudioDevice>
#include <QList>

class GlobalSound : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)

public:
    // 静态接口 获取全局唯一单例实例
    static GlobalSound* instance();

    int volume() const;
    void setVolume(int volume);

    bool isMuted() const;
    void setMuted(bool muted);

    QList<QAudioDevice> audioOutputDevices() const;
    void setAudioDevice(const QAudioDevice &device);

signals:
    void volumeChanged(int volume);
    void mutedChanged(bool muted);

private:
    explicit GlobalSound(QObject *parent = nullptr);
    ~GlobalSound() override = default;
    GlobalSound(const GlobalSound&) = delete;
    GlobalSound& operator=(const GlobalSound&) = delete;

    QAudioOutput* m_audioOutput;
    mutable QMutex m_mutex;
    bool m_isMuted;
    int m_lastVolume;
};

#endif // GLOBALSOUND_H
