#include <QObject>
#include <QAudioOutput>
#include <QMediaDevices>
#include <QAudioFormat>
#include <QMutex>
#include <QAudioDevice>
#include <QDebug>
#include <QList>

#include "globalaudio.h"

GlobalAudio* GlobalAudio::instance()
{
    static QMutex mutex;
    QMutexLocker locker(&mutex); // 锁定初始化过程
    static GlobalAudio instance;
    return &instance;
}

GlobalAudio::GlobalAudio(QObject *parent)
    : QObject(parent)
    , m_audioOutput(nullptr) // 初始输出对象指针为空
    , m_isMuted(false) // 默认不静音
    , m_lastVolume(80) // 初始状态，静音恢复时使用
{
    // Qt6.10唯一合法构造方式：无参数 + 父对象
    m_audioOutput = new QAudioOutput(this);
    if (!m_audioOutput) {
        qCritical() << "音频输出对象初始化失败";
        return;
    }

    // 设置初始音量
    m_audioOutput->setVolume(0.8);
}

int GlobalAudio::volume() const
{
    QMutexLocker locker(&m_mutex);
    if (!m_audioOutput){
        return 80;
    }
    return qRound(m_audioOutput->volume() * 100); // 四舍五入
}



// 同步修改 setVolume 函数（避免同类问题）
void GlobalAudio::setVolume(int volume) {
    QMutexLocker locker(&m_mutex); // 线程锁

    if (!m_audioOutput) {
        m_lastVolume = qBound(0, volume, 100);
        qWarning() << "音频输出未初始化，无法设置音量";
        return;
    }

    int newVolume = qBound(0, volume, 100);
    m_audioOutput->setVolume(static_cast<qreal>(newVolume) / 100.0); // 强转为浮点型

    if (m_isMuted) {
        m_isMuted = false;
        emit mutedChanged(false);
    }

    m_lastVolume = newVolume;

    emit volumeChanged(newVolume); // 直接用 newVolume，不调用 volume()，以免双重锁
}

bool GlobalAudio::isMuted() const
{
    QMutexLocker locker(&m_mutex); // 保护m_isMuted变量
    return m_isMuted;
}

void GlobalAudio::setMuted(bool muted) {
    QMutexLocker locker(&m_mutex); // 仅锁定一次
    if (!m_audioOutput) {
        m_isMuted = muted;
        qWarning() << "音频输出未初始化，无法设置静音";
        return;
    }

    m_isMuted = muted;
    int currentVolume = qRound(m_audioOutput->volume() * 100); // 直接获取，不调用 volume()
    if (muted) {
        m_lastVolume = currentVolume; // 用缓存值替代 volume()
        m_audioOutput->setVolume(0.0);
    } else {
        m_audioOutput->setVolume(static_cast<qreal>(m_lastVolume) / 100.0);
        currentVolume = m_lastVolume; // 恢复后的音量
    }

    // 发射信号（仅发射一次，避免重复触发）
    emit mutedChanged(muted);
    emit volumeChanged(currentVolume); // 用缓存的 currentVolume 替代 volume()
}

QList<QAudioDevice> GlobalAudio::audioOutputDevices() const
{
    return QMediaDevices::audioOutputs();
}

void GlobalAudio::setAudioDevice(const QAudioDevice &device)
{
    QMutexLocker locker(&m_mutex); // 互斥锁
    if (device.isNull() || !m_audioOutput) return;

    // Qt6.10 中无法动态切换设备，仅销毁旧对象重建
    m_audioOutput->deleteLater();
    m_audioOutput = new QAudioOutput(this);
    if (m_isMuted) {
        m_audioOutput->setVolume(0.0);
    } else {
        m_audioOutput->setVolume(static_cast<qreal>(m_lastVolume) / 100.0);
    }
}
