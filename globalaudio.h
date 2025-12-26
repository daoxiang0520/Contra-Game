#ifndef GLOBALAUDIO_H
#define GLOBALAUDIO_H

#include <QObject>
#include <QAudioOutput>
#include <QMediaDevices>
#include <QAudioFormat>
#include <QMutex>
#include <QAudioDevice>
#include <QList>

class GlobalAudio : public QObject
{
    Q_OBJECT
    // 声明volume属性：可读、可写、值变化时发信号
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    // 声明muted属性：可读、可写、状态变化时发信号
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)

public:
    // 静态接口 获取全局唯一单例实例
    static GlobalAudio* instance();

    int volume() const; // 获取音量
    void setVolume(int volume); // 设置音量

    bool isMuted() const; // 获取静音
    void setMuted(bool muted); // 设置静音

    QList<QAudioDevice> audioOutputDevices() const;  // 获取音频输出设备列表
    void setAudioDevice(const QAudioDevice &device); // 设置音频输出设备列表

signals:
    void volumeChanged(int volume); // 音量变换信号
    void mutedChanged(bool muted); // 静音变换信号

private:
    explicit GlobalAudio(QObject *parent = nullptr);
    ~GlobalAudio() override = default;
    //确保单例
    GlobalAudio(const GlobalAudio&) = delete; // 禁止拷贝
    GlobalAudio& operator=(const GlobalAudio&) = delete; // 禁止赋值

    QAudioOutput* m_audioOutput;
    mutable QMutex m_mutex; // 线程锁 确保单例
    bool m_isMuted;
    int m_lastVolume;
};

#endif // GLOBALAUDIO_H
