#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimer>
#include <QMutex>

#include "game.pb.h"

class NetworkManager : public QObject {
    Q_OBJECT
public:
    explicit NetworkManager(QObject *parent = nullptr);
    ~NetworkManager() override = default;

    // Public API
    void connectToServer(const QString& ip, uint16_t port, uint16_t udpPort = 0);
    void createRoom(int mapId);
    void joinRoom(int roomId);
    void sendUdpInput(uint32_t inputBits);

    QTcpSocket* getSocket() const { return m_tcpSocket; }

signals:
    void tcpConnected();
    void roomJoined(int roomId, QString token, uint64_t playerid);
    void mainRoom(QString msg);
    void roomError(QString msg);
    void snapshotReceived(const game::ServerSnapshot& snapshot);

private slots:
    void onTcpConnected();
    void onTcpReadyRead();
    void onTcpStateChanged(QAbstractSocket::SocketState state);
    void onTcpError(QAbstractSocket::SocketError err);

    void onUdpReadyRead();

private:
    // helpers
    void sendTcpJson(const QJsonObject& obj);
    void flushPendingRequestsIfConnected();

    QTcpSocket* m_tcpSocket = nullptr;
    QUdpSocket* m_udpSocket = nullptr;

    QString m_serverIp;
    uint16_t m_tcpPort = 0;
    uint16_t m_udpPort = 0;

    QString m_token;
    uint64_t m_playerId = 0;
    int m_roomId = -1;

    bool m_pendingCreateRoom = false;
    int m_pendingCreateMapId = 0;
    bool m_pendingJoinRoom = false;
    int m_pendingJoinRoomId = 0;

    QMutex m_mutex;
};
