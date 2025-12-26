#pragma once

#include <QWidget>
#include <QSet>
#include <QTimer>
#include <QHash>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include<QLabel>
#include "networkmanager.h"
#include "game.pb.h"
#include "mapdata.h"

namespace InputBit {
enum Mask {
    LEFT  = 1 << 0,
    RIGHT = 1 << 1,
    JUMP  = 1 << 2,
    FIRE  = 1 << 3
};
}

class OnlineGameWidget : public QWidget {
    Q_OBJECT
public:
    explicit OnlineGameWidget(NetworkManager* net, int mapId = 1, QWidget *parent = nullptr);
    ~OnlineGameWidget() override;

    // Call after server returns token/playerid
    void setAuthTokenAndId(int roomId,const QString &token,  uint64_t myEntityId);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void sendInputLoop();
    void onSnapshotUpdate(const game::ServerSnapshot &snapshot);
signals:
    void toStart();

private:
    NetworkManager* m_net = nullptr; // non-singleton pointer

    // map & rendering
    int m_mapId = 1;
    mapdata m_map; // uses your existing mapdata class
    QGraphicsView* m_view = nullptr;
    QGraphicsScene* m_scene = nullptr;
    QLabel* m_roomLabel = nullptr;
    // auth & network
    QString m_token;
    int m_roomId = -1;
    uint64_t m_myEntityId = 0;
    QTimer* m_networkTimer = nullptr;

    // input
    QSet<int> m_pressedKeys;

    // network-driven entities
    QHash<uint64_t, QGraphicsPixmapItem*> m_netEntities;
    QHash<uint64_t, QGraphicsRectItem*>   m_netHpBars;

    // helpers
    int last=0;
    void buildMapIntoScene();
    QPixmap createBulletPixmap(bool isMine) const;
    void updateOrCreateEntity(const game::EntityState &state);
    void cleanMissingEntities(const QSet<uint64_t> &activeIds);
    void centerOnView(const QPointF &pos);
    void clearNetworkEntities();
};
