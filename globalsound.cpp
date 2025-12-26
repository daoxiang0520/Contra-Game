#include <QObject>
#include <QAudioOutput>
#include <QMediaDevices>
#include <QAudioFormat>
#include <QMutex>
#include <QAudioDevice>
#include <QDebug>
#include <QList>

#include "globalsound.h"

GlobalSound* GlobalSound::instance()
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    static GlobalSound instance;
    return &instance;
}

GlobalSound::GlobalSound(QObject *parent)
    : QObject(parent)
    , m_audioOutput(nullptr)
    , m_isMuted(false)
    , m_lastVolume(80)  // 音效默认音量可与BGM不同
{
    m_audioOutput = new QAudioOutput(this);
    if (!m_audioOutput) {
        qCritical() << "音效输出对象初始化失败";
        return;
    }

    m_audioOutput->setVolume(0.8);  // 默认音量80%
}

int GlobalSound::volume() const
{
    QMutexLocker locker(&m_mutex);
    if (!m_audioOutput){
        return 80;
    }
    return qRound(m_audioOutput->volume() * 100);
}

void GlobalSound::setVolume(int volume) {
    QMutexLocker locker(&m_mutex);

    if (!m_audioOutput) {
        m_lastVolume = qBound(0, volume, 100);
        qWarning() << "音效输出未初始化，无法设置音量";
        return;
    }

    int newVolume = qBound(0, volume, 100);
    m_audioOutput->setVolume(static_cast<qreal>(newVolume) / 100.0);

    if (m_isMuted) {
        m_isMuted = false;
        emit mutedChanged(false);
    }

    m_lastVolume = newVolume;
    emit volumeChanged(newVolume);
}

bool GlobalSound::isMuted() const
{
    QMutexLocker locker(&m_mutex);
    return m_isMuted;
}

void GlobalSound::setMuted(bool muted) {
    QMutexLocker locker(&m_mutex);
    if (!m_audioOutput) {
        m_isMuted = muted;
        qWarning() << "音效输出未初始化，无法设置静音";
        return;
    }

    m_isMuted = muted;
    int currentVolume = qRound(m_audioOutput->volume() * 100);
    if (muted) {
        m_lastVolume = currentVolume;
        m_audioOutput->setVolume(0.0);
    } else {
        m_audioOutput->setVolume(static_cast<qreal>(m_lastVolume) / 100.0);
        currentVolume = m_lastVolume;
    }

    emit mutedChanged(muted);
    emit volumeChanged(currentVolume);
}

QList<QAudioDevice> GlobalSound::audioOutputDevices() const
{
    return QMediaDevices::audioOutputs();
}

void GlobalSound::setAudioDevice(const QAudioDevice &device)
{
    QMutexLocker locker(&m_mutex);
    if (device.isNull() || !m_audioOutput) return;

    m_audioOutput->deleteLater();
    m_audioOutput = new QAudioOutput(this);
    if (m_isMuted) {
        m_audioOutput->setVolume(0.0);
    } else {
        m_audioOutput->setVolume(static_cast<qreal>(m_lastVolume) / 100.0);
    }
}
