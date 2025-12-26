#include "networkmanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent)
{
    m_tcpSocket = new QTcpSocket(this);
    m_udpSocket = new QUdpSocket(this);

    // TCP wiring
    connect(m_tcpSocket, &QTcpSocket::connected, this, &NetworkManager::onTcpConnected);
    connect(m_tcpSocket, &QTcpSocket::readyRead, this, &NetworkManager::onTcpReadyRead);
    connect(m_tcpSocket, &QTcpSocket::stateChanged, this, &NetworkManager::onTcpStateChanged);
    connect(m_tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, &NetworkManager::onTcpError);

    // UDP read
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &NetworkManager::onUdpReadyRead);
}

void NetworkManager::connectToServer(const QString& ip, uint16_t port, uint16_t udpPort)
{
    QMutexLocker locker(&m_mutex);
    m_serverIp = ip;
    m_tcpPort = port;
    if (udpPort != 0) m_udpPort = udpPort;
    else m_udpPort = port;

    qDebug() << "NetworkManager: connecting to" << m_serverIp << "tcp:" << m_tcpPort << "udp:" << m_udpPort;
    m_tcpSocket->connectToHost("47.100.161.93",10086);
    if (!m_udpSocket->isOpen()) {
        bool ok = m_udpSocket->bind(QHostAddress::AnyIPv4, 0, QUdpSocket::ShareAddress);
        qDebug() << "NetworkManager: udp bind ok=" << ok << " localPort=" << m_udpSocket->localPort();
    }
}

void NetworkManager::onTcpConnected()
{
    qDebug() << "NetworkManager: TCP connected to" << m_serverIp << ":" << m_tcpPort;
    emit tcpConnected();

    flushPendingRequestsIfConnected();
}

void NetworkManager::onTcpStateChanged(QAbstractSocket::SocketState state)
{
    qDebug() << "NetworkManager: TCP state changed =" << state;
}

void NetworkManager::onTcpError(QAbstractSocket::SocketError err)
{
    Q_UNUSED(err);
    QString s = m_tcpSocket->errorString();
    qWarning() << "NetworkManager: TCP error:" << s;
    emit roomError(s);
}

void NetworkManager::createRoom(int mapId)
{
    QMutexLocker locker(&m_mutex);
    if (m_tcpSocket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "NetworkManager: createRoom queued until connected, mapId=" << mapId;
        m_pendingCreateRoom = true;
        m_pendingCreateMapId = mapId;
        return;
    }
    QJsonObject obj;
    obj["magic"] = "A8YB61usP@%123";
    obj["option"] = 0; // create
    obj["mapid"] = mapId;
    sendTcpJson(obj);
}

void NetworkManager::joinRoom(int roomId)
{
    QMutexLocker locker(&m_mutex);
    if (m_tcpSocket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "NetworkManager: joinRoom queued until connected, roomId=" << roomId;
        m_pendingJoinRoom = true;
        m_pendingJoinRoomId = roomId;
        return;
    }
    QJsonObject obj;
    obj["magic"] = "A8YB61usP@%123";
    obj["option"] = 1; // join
    obj["roomid"] = roomId;
    sendTcpJson(obj);
}

void NetworkManager::sendTcpJson(const QJsonObject& obj)
{
    QJsonDocument doc(obj);
    QByteArray b = doc.toJson(QJsonDocument::Compact);
    qDebug() << "NetworkManager: sending TCP JSON:" << b;
    qint64 written = m_tcpSocket->write(b);
    qDebug()<<written;
    /*if (written <= 0) {
        qWarning() << "NetworkManager: failed to write TCP data:" << m_tcpSocket->errorString();
    } else {
        m_tcpSocket->flush();
    }*/
}

void NetworkManager::flushPendingRequestsIfConnected()
{
    QMutexLocker locker(&m_mutex);
    if (m_tcpSocket->state() == QAbstractSocket::ConnectedState) {
        if (m_pendingCreateRoom) {
            m_pendingCreateRoom = false;
            int mapId = m_pendingCreateMapId;
            locker.unlock();
            createRoom(mapId);
            locker.relock();
        }
        if (m_pendingJoinRoom) {
            m_pendingJoinRoom = false;
            int roomId = m_pendingJoinRoomId;
            locker.unlock();
            joinRoom(roomId);
            locker.relock();
        }
    }
}

void NetworkManager::onTcpReadyRead()
{
    QByteArray data = m_tcpSocket->readAll();
    qDebug() << "NetworkManager: TCP recv len=" << data.size() << " payload=" << data;
    QJsonParseError perr;
    QJsonDocument doc = QJsonDocument::fromJson(data, &perr);
    if (perr.error != QJsonParseError::NoError) {
        qWarning() << "NetworkManager: JSON parse error:" << perr.errorString();
        return;
    }
    if (!doc.isObject()) {
        qWarning() << "NetworkManager: TCP JSON not an object";
        return;
    }
    QJsonObject obj = doc.object();
    if (obj.contains("status") && obj["status"].toString() != "OK") {
        QString msg = obj.value("status").toString();
        qWarning() << "NetworkManager: server status error:" << msg;
        emit roomError(msg);
        emit mainRoom(msg);
        return;
    }
    if (obj.contains("token") && obj.contains("playerid")) {
        QString token = obj.value("token").toString();
        uint64_t playerid = 0;
        if (obj.value("playerid").isDouble()) {
            playerid = static_cast<uint64_t>(obj.value("playerid").toDouble());
        } else {
            playerid = obj.value("playerid").toString().toULongLong();
        }
        int roomid = obj.value("roomid").toInt(-1);
        qDebug() << "NetworkManager: roomJoined token=" << token << " playerid=" << playerid << " room=" << roomid;
        {
            QMutexLocker locker(&m_mutex);
            m_token = token;
            m_playerId = playerid;
            m_roomId = roomid;
        }
        emit roomJoined(roomid, token, playerid);
        return;
    }
    qDebug() << "NetworkManager: unhandled TCP JSON fields:" << obj.keys();
}

void NetworkManager::sendUdpInput(uint32_t inputBits)
{
    qDebug()<<"UDP SEND READY";
    QMutexLocker locker(&m_mutex);
    if (m_token.isEmpty()) {
        return;
    }
    game::ClientInput ci;
    ci.set_token(m_token.toStdString());
    ci.set_input_bits(inputBits);

    std::string ser = ci.SerializeAsString();
    if (ser.empty()) {
        qWarning() << "NetworkManager: protobuf serialize failed for ClientInput";
        return;
    }
    if (m_serverIp.isEmpty() || m_udpPort == 0) {
        qWarning() << "NetworkManager: no server ip/udp port set, cannot send input";
        return;
    }
    // debug
    qDebug() << "NetworkManager: UDP send ->" << m_serverIp << ":" << m_udpPort << " localPort=" << m_udpSocket->localPort();
    qint64 sent = m_udpSocket->writeDatagram(ser.data(), static_cast<qint64>(ser.size()),
                                             QHostAddress(m_serverIp), static_cast<quint16>(m_udpPort));
    if (sent < 0) {
        qWarning() << "NetworkManager: UDP send error:" << m_udpSocket->errorString();
    }
}

void NetworkManager::onUdpReadyRead()
{
    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(static_cast<int>(m_udpSocket->pendingDatagramSize()));
        QHostAddress sender;
        quint16 senderPort;
        m_udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        qDebug() << "NetworkManager: UDP datagram received from" << sender.toString() << ":" << senderPort << " len=" << datagram.size();

        game::ServerSnapshot snapshot;
        if (snapshot.ParseFromArray(datagram.constData(), datagram.size())) {
            emit snapshotReceived(snapshot);
        } else {
            qWarning() << "NetworkManager: failed to parse ServerSnapshot protobuf";
        }
    }
}
