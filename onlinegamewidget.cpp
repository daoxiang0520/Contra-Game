#include "onlinegamewidget.h"
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QPainter>
#include <QDateTime>
#include <QDebug>

OnlineGameWidget::OnlineGameWidget(NetworkManager* net, int mapId, QWidget *parent)
    : QWidget(parent), m_net(net), m_mapId(mapId), m_map(mapId)
{
    // create scene & view
    m_scene = new QGraphicsScene(this);
    m_view = new QGraphicsView(m_scene, this);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setRenderHint(QPainter::Antialiasing, false);
    m_view->setFocusPolicy(Qt::NoFocus);

    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(0,0,0,0);
    lay->setSpacing(0);
    lay->addWidget(m_view);
    setLayout(lay);

    m_roomLabel = new QLabel(this);
    m_roomLabel->setText(QString()); // 初始为空
    m_roomLabel->setStyleSheet(
        "QLabel { color: white; background-color: rgba(0,0,0,160); "
        " padding: 4px 8px; border-radius: 4px; font-weight: bold; }"
        );
    m_roomLabel->setAttribute(Qt::WA_TransparentForMouseEvents); // 不拦截鼠标
    m_roomLabel->adjustSize();
    m_roomLabel->move(8, 8);
    m_roomLabel->show();
    m_roomLabel->raise();
    // draw map
    try {
        buildMapIntoScene();
    } catch (...) {
        qWarning() << "OnlineGameWidget: buildMapIntoScene threw exception";
    }

    m_networkTimer = new QTimer(this);
    connect(m_networkTimer, &QTimer::timeout, this, &OnlineGameWidget::sendInputLoop);
    if (m_net) {
        connect(m_net,&NetworkManager::roomJoined,this,&OnlineGameWidget::setAuthTokenAndId);
        connect(m_net, &NetworkManager::snapshotReceived, this, &OnlineGameWidget::onSnapshotUpdate);
    } else {
        qWarning() << "OnlineGameWidget: NetworkManager pointer is null";
    }

    setFocusPolicy(Qt::StrongFocus);
    setFocus(Qt::OtherFocusReason);
}

OnlineGameWidget::~OnlineGameWidget()
{
    if (m_networkTimer) m_networkTimer->stop();
    if (m_net) disconnect(m_net, &NetworkManager::snapshotReceived, this, &OnlineGameWidget::onSnapshotUpdate);
    clearNetworkEntities();
}

void OnlineGameWidget::setAuthTokenAndId(int roomId,const QString &token,  uint64_t myEntityId)
{
    m_token = token;
    m_roomId = roomId;
    m_myEntityId = myEntityId;
    if (m_networkTimer && !m_networkTimer->isActive()) m_networkTimer->start(16);
    if (m_roomLabel) {
        m_roomLabel->setText(QString("Room %1").arg(roomId));
        m_roomLabel->adjustSize(); // 更新控件大小
        m_roomLabel->show();
    }
    qDebug() << "OnlineGameWidget: auth set token=" << (m_token.isEmpty() ? "<empty>" : "<set>") << " room=" << roomId << " myId=" << myEntityId;
}

void OnlineGameWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;
    m_pressedKeys.insert(event->key());
    QWidget::keyPressEvent(event);
}

void OnlineGameWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;
    m_pressedKeys.remove(event->key());
    QWidget::keyReleaseEvent(event);
}

void OnlineGameWidget::sendInputLoop()
{
    if (m_token.isEmpty()) return;
    uint32_t mask = 0;
    if(m_pressedKeys.contains(Qt::Key_Escape))emit toStart();
    if (m_pressedKeys.contains(Qt::Key_A) || m_pressedKeys.contains(Qt::Key_Left))  mask |= InputBit::LEFT;
    if (m_pressedKeys.contains(Qt::Key_D) || m_pressedKeys.contains(Qt::Key_Right)) mask |= InputBit::RIGHT;
    if (m_pressedKeys.contains(Qt::Key_K) || m_pressedKeys.contains(Qt::Key_Up))    mask |= InputBit::JUMP;
    if (m_pressedKeys.contains(Qt::Key_J) || m_pressedKeys.contains(Qt::Key_Space)) mask |= InputBit::FIRE;
    qDebug()<<"Input"<<mask;
    if (m_net) {
        m_net->sendUdpInput(mask);
    }
}

void OnlineGameWidget::onSnapshotUpdate(const game::ServerSnapshot &snapshot)
{
    static uint32_t lastSeq = 0;
    if (snapshot.snapshot_seq()) {
        uint32_t seq = snapshot.snapshot_seq();
        if (seq <= lastSeq) {
            return; // drop old
        }
        lastSeq = seq;
    }

    if (!m_scene) {
        qWarning() << "OnlineGameWidget: no scene available in onSnapshotUpdate";
        return;
    }

    QSet<uint64_t> activeIds;
    for (int i = 0; i < snapshot.entities_size(); ++i) {
        const auto &st = snapshot.entities(i);
        activeIds.insert(st.id());
        updateOrCreateEntity(st);
        if (st.id() == m_myEntityId) {
            centerOnView(QPointF(st.pos().x(), st.pos().y()));
        }
    }
    cleanMissingEntities(activeIds);
}

void OnlineGameWidget::buildMapIntoScene()
{
    // Similar approach as BlockWindow::initMap but simplified into scene pixmaps.
    // Uses m_map.MapLayer, m_map.tileWidth(), m_map.width/height
    if (m_map.MapLayer.isEmpty()) {
        qWarning() << "OnlineGameWidget: map layers empty";
        m_scene->setSceneRect(0,0,800,600);
        return;
    }

    int tileSize = (m_map.tileWidth() > 0 ? m_map.tileWidth() : 32);
    int mapW = m_map.width();
    int mapH = m_map.height();
    int baseW = mapW * tileSize;
    int baseH = mapH * tileSize;

    // Render each layer to a pixmap and add to scene
    for (int layerIndex = 0; layerIndex < m_map.MapLayer.size(); ++layerIndex) {
        const QVector<QVector<int>>& layer = m_map.MapLayer.at(layerIndex);
        int rows = layer.size();
        int cols = rows ? layer.at(0).size() : 0;

        int imageW = std::max(baseW, cols * tileSize);
        int imageH = std::max(baseH, rows * tileSize);

        QImage image(imageW, imageH, QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::transparent);
        QPainter painter(&image);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, false);

        for (int r = 0; r < rows; ++r) {
            const QVector<int>& row = layer.at(r);
            int colsRow = row.size();
            for (int c = 0; c < colsRow; ++c) {
                int gid = row.at(c);
                if (gid == 0) continue;
                int idx = gid - m_map.Firstgid();
                if (idx < 0 || !m_map.hasTile(idx)) continue;
                tile t = m_map.getTileSafe(idx);
                if (t.Img().isNull()) continue;
                int tw = t.Img().width();
                int th = t.Img().height();
                int cellX = c * tileSize;
                int cellY = r * tileSize;
                int drawX = cellX;
                int drawY = cellY + (tileSize - th); // align bottom inside tile
                painter.drawPixmap(drawX, drawY, t.Img());
            }
        }
        painter.end();

        QPixmap pix = QPixmap::fromImage(image);
        QGraphicsPixmapItem* item = m_scene->addPixmap(pix);
        item->setZValue(layerIndex);
    }

    // set scene rect to map pixel size
    m_scene->setSceneRect(0, 0, baseW, baseH);

    // center view on start
    m_view->setSceneRect(m_scene->sceneRect());
    // avoid automatic scaling, center on start top-left
    m_view->centerOn(0, 0);
}

QPixmap OnlineGameWidget::createBulletPixmap(bool isMine) const
{
    QPixmap pix(8, 4);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setPen(Qt::NoPen);
    p.setBrush(isMine ? Qt::yellow : Qt::red);
    p.drawRect(0, 0, 8, 4);
    return pix;
}

void OnlineGameWidget::updateOrCreateEntity(const game::EntityState &state)
{
    if (!m_scene) return;
    uint64_t id = state.id();

    if (!m_netEntities.contains(id)) {
        QGraphicsPixmapItem* item = new QGraphicsPixmapItem();
        QPixmap pix;
        switch (state.type()) {
        case game::PLAYER:
            pix = QPixmap(":/image/001.png");
            if (pix.isNull()) pix = QPixmap(32,64);
            break;
        case game::ENEMY:
            pix = QPixmap(":/image/201.png");
            if (pix.isNull()) pix = QPixmap(32,32);
            else {
                QTransform tx;
                tx.scale(-1, 1);
                pix=pix.transformed(tx,Qt::SmoothTransformation);
            }
            break;
        case game::BULLET:
            pix = createBulletPixmap(state.owner() == m_myEntityId);
            break;
        default:
            pix = QPixmap(16,16);
            pix.fill(Qt::magenta);
            break;
        }
        item->setPixmap(pix);
        item->setZValue(1000);
        m_scene->addItem(item);
        m_netEntities[id] = item;

        if (state.type() == game::PLAYER || state.type() == game::ENEMY) {
            QGraphicsRectItem* hp = new QGraphicsRectItem(0,0,40,4);
            hp->setBrush(Qt::red);
            hp->setZValue(1100);
            m_scene->addItem(hp);
            m_netHpBars[id] = hp;
        }

        return;
    }

    QGraphicsPixmapItem *item = m_netEntities.value(id, nullptr);
    if(state.type()==game::PLAYER)qDebug()<<state.vel().x();
    if (item) {
        item->setPos(state.pos().x(), state.pos().y());
        if (qAbs(state.vel().x()) > 0.1f) {
            bool lookLeft = (state.vel().x()<0);//(state.vel().x()*last < 0)||last==0;
            //last=state.vel().x();
            //qDebug()<<last<<' '<<state.vel().x();
            if (lookLeft) item->setTransform(QTransform().scale(-1,1).translate(-item->boundingRect().width(),0));
            else item->setTransform(QTransform());
        }
    }

    if (m_netHpBars.contains(id)) {
        QGraphicsRectItem* hp = m_netHpBars[id];
        hp->setPos(state.pos().x(), state.pos().y() - 10);
        float hpRate = qBound(0.0f, (float)state.hp() / 100.0f, 1.0f);
        hp->setRect(0, 0, 40 * hpRate, 4);
    }
}

void OnlineGameWidget::cleanMissingEntities(const QSet<uint64_t> &activeIds)
{
    auto it = m_netEntities.begin();
    while (it != m_netEntities.end()) {
        uint64_t id = it.key();
        if (!activeIds.contains(id)) {
            QGraphicsItem *item = it.value();
            if (m_scene && item) m_scene->removeItem(item);
            delete item;
            it = m_netEntities.erase(it);

            if (m_netHpBars.contains(id)) {
                QGraphicsItem *hp = m_netHpBars[id];
                if (m_scene && hp) m_scene->removeItem(hp);
                delete hp;
                m_netHpBars.remove(id);
            }
        } else ++it;
    }
}

void OnlineGameWidget::centerOnView(const QPointF &pos)
{
    if (!m_view || !m_scene) return;
    QRectF srect = m_scene->sceneRect();
    qreal halfW = m_view->viewport()->width() / 2.0;
    qreal halfH = m_view->viewport()->height() / 2.0;
    qreal cx = pos.x();
    qreal cy = pos.y();
    qreal tx = qBound(srect.left() + halfW, cx, srect.right() - halfW);
    qreal ty = qBound(srect.top() + halfH, cy, srect.bottom() - halfH);
    m_view->centerOn(tx, ty);
}

void OnlineGameWidget::clearNetworkEntities()
{
    for (auto it = m_netEntities.begin(); it != m_netEntities.end(); ++it) {
        QGraphicsItem *item = it.value();
        if (m_scene && item) m_scene->removeItem(item);
        delete item;
    }
    m_netEntities.clear();

    for (auto it = m_netHpBars.begin(); it != m_netHpBars.end(); ++it) {
        QGraphicsItem *hp = it.value();
        if (m_scene && hp) m_scene->removeItem(hp);
        delete hp;
    }
    m_netHpBars.clear();
}
