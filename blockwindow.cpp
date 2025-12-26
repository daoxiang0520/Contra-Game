#include "blockwindow.h"
#include "ResultWindow.h"
#include <QPainter>
#include <QKeyEvent>
#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QImage>
#include <QVBoxLayout>
#include <QApplication>
#include <QShowEvent>
#include <QDir>
#include"GameDefs.h"

extern int currentLevel;
extern int unlockedLevel;

BlockWindow::BlockWindow(int level,QWidget *parent)
    : QWidget(parent), m_map(level) // default mapdata ctor will load resource/DB
{
    m_level=level;
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setWindowTitle("Contra");
    setFocusPolicy(Qt::StrongFocus);

    // init map sizes from m_map (读取 Tiled/DB 数据)
    m_mapWidth = m_map.width();
    m_mapHeight = m_map.height();
    if (m_mapWidth <= 0 || m_mapHeight <= 0) {
        qWarning() << "Invalid map size, falling back to defaults";
        m_mapWidth = MAP_WIDTH_TILES;
        m_mapHeight = MAP_HEIGHT_TILES;
    }

    // 先创建 Scene & View（initMap 可能会访问 Scene）
    QRectF rect = sceneRectForMap();
    View = new GameView(this);
    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(0,0,0,0);
    lay->setSpacing(0);
    lay->addWidget(View);
    setLayout(lay);

    Scene = new QGraphicsScene(rect, this);
    View->setScene(Scene);
    View->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    View->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    View->setFocusPolicy(Qt::NoFocus);
    this->setFocusPolicy(Qt::StrongFocus);
    // 防止 View 或其 viewport 抢走焦点
    View->viewport()->setFocusPolicy(Qt::NoFocus);

    // 让 BlockWindow 本身接收焦点
    this->setFocus(Qt::OtherFocusReason);
    this->setFocus();
    qDebug() << "BlockWindow ctor: Scene created =" << Scene << "rect=" << rect;

    // 现在安全地初始化地图（initMap 内可以放心使用 Scene）
    initMap();

    //开始位置
    if(m_level==1)
    {
        PLAYER_START_X=64;
        PLAYER_START_Y=64;
    }
    else if(m_level==2)
    {
        PLAYER_START_X=64;
        PLAYER_START_Y=672;
    }
    else if(m_level==3)
    {
        PLAYER_START_X=64;
        PLAYER_START_Y=64;
    }
    else if(m_level==4)
    {
        PLAYER_START_X=40;
        PLAYER_START_Y=448;
    }
    else if(m_level==5)
    {
        PLAYER_START_X=64;
        PLAYER_START_Y=64;
    }
    else if(m_level==6)
    {
        PLAYER_START_X=64;
        PLAYER_START_Y=672;
    }
    else if(m_level==7)
    {
        PLAYER_START_X=64;
        PLAYER_START_Y=64;
    }
    else if(m_level==8)
    {
        PLAYER_START_X=40;
        PLAYER_START_Y=448;
    }
    else if(m_level==9)
    {
        PLAYER_START_X=64;
        PLAYER_START_Y=64;
    }
    else if(m_level==10)
    {
        PLAYER_START_X=64;
        PLAYER_START_Y=672;
    }
    else if(m_level==11)
    {
        PLAYER_START_X=64;
        PLAYER_START_Y=64;
    }
    else if(m_level==12)
    {
        PLAYER_START_X=40;
        PLAYER_START_Y=448;
    }


    //结束点
    if (level == 1) {
        m_levelEndPos = QPoint(1580, 64);
    }
    else if (level == 2) {
        m_levelEndPos = QPoint(20, 64);
    }
    else if (level == 3) {
        m_levelEndPos = QPoint(2340, 500);
    }
    else if (level == 4) {
        m_levelEndPos = QPoint(0, 64);
    }
    else if (level == 5) {
        m_levelEndPos = QPoint(1580, 64);
    }
    else if (level == 6) {
        m_levelEndPos = QPoint(20, 64);
    }
    else if (level == 7) {
        m_levelEndPos = QPoint(2340, 500);
    }
    else if (level == 8) {
        m_levelEndPos = QPoint(0, 64);
    }
    else if (level == 9) {
        m_levelEndPos = QPoint(1580, 64);
    }
    else if (level == 10) {
        m_levelEndPos = QPoint(20, 64);
    }
    else if (level == 11) {
        m_levelEndPos = QPoint(2340, 500);
    }
    else if (level == 12) {
        m_levelEndPos = QPoint(0, 64);
    }
    else if (level == 13) {
        m_levelEndPos = QPoint(2340, 400);
    }
    else if (level == 14) {
        m_levelEndPos = QPoint(2340, 400);
    }
    else if (level == 15) {
        m_levelEndPos = QPoint(2340, 400);
    }
    else if (level == 16) {
        m_levelEndPos = QPoint(2144, 672);
    }
    // 初始化结果窗口指针为空
    m_resultWindow = nullptr;


    int px = PLAYER_START_X;
    int py = PLAYER_START_Y;
    int tileX = px / m_tileSize;
    int tileY = (py + PLAYER_HEIGHT) / m_tileSize; // 玩家脚所在瓦片
    qDebug() << "Spawn tile coords (tileX,tileY)=" << tileX << tileY
             << " tileValue=" << (m_map.RealMap.isEmpty()? -1 : m_map.RealMap[tileY][tileX]);
    // 创建玩家（parent=this），然后把玩家作为 scene item 添加/同步
    m_player = new Player(this, this);
    if (m_player) {
        // 把玩家逻辑坐标上移一个 tile（使实际坐标与视觉对齐）
        int shift = m_tileSize; // 或 m_window->tileSize()
        m_player->setPosition(m_player->x(), m_player->y() - shift);

        // 如果你已经把玩家作为 scene item 添加了，立即同步 item 的位置
        qDebug() << "Player logical position shifted up by" << shift << " new y =" << m_player->y();
    }
    // 在 Scene 中创建一个 item 用于显示玩家
    QPixmap playerPix = m_player->currentPixmap();
    m_playerItem = nullptr;
    if (Scene) {
        m_playerItem = Scene->addPixmap(playerPix);
        if (m_playerItem) {
            m_playerItem->setZValue(10000); // 放到最上层
            m_playerItem->setPos(m_player->x(), m_player->y());
        }
    }
    if (m_playerItem) {
        m_playerItem->setPos(m_player->x(), m_player->y());
        m_playerTextItem = new QGraphicsTextItem(m_playerItem);
        m_playerTextItem->setDefaultTextColor(Qt::green);
        m_playerTextItem->setFont(QFont("微软雅黑", 10));
        QRectF playerTextRect = m_playerTextItem->boundingRect();
        m_playerTextItem->setPos(
            ( m_player->width() - playerTextRect.width() )/ 2,
            -25
            );
        m_hpBarBgPlayer = new QGraphicsRectItem(0, 0, 50, 5, m_playerItem);
        m_hpBarBgPlayer->setBrush(Qt::gray);

        m_hpBarPlayer = new QGraphicsRectItem(0, 0, 50, 5, m_hpBarBgPlayer);
        m_hpBarPlayer->setBrush(Qt::green);

        m_hpBarBgPlayer->setPos(
            ( m_player->width() - 50 )/ 2,
            -20
            );
    }

    // 初始化敌人
    initEnemies();

    // 游戏定时器（60FPS） —— 先创建但延后启动，防止 init 中的操作与定时器并发
    m_gameTimer = new QTimer(this);
    connect(m_gameTimer, &QTimer::timeout, this, &BlockWindow::updateGame);

    // 将视图中心初始化到玩家起始点
    centerOnPlayer();
    View->show();

    // 最后启动定时器
    m_gameTimer->start(16);

    // qDebug() << "BlockWindow ctor: finished, playerItem=" << m_playerItem;
}
BlockWindow::~BlockWindow()
{
    // m_player 的 parent = this（QObject 管理），不要手动 delete
    // 删除动态分配的非-parent 对象（enemies/bullets）
    for (Enemy* enemy : m_enemies) delete enemy;
    for (Bullet* bullet : m_bullets) delete bullet;
    // m_gameTimer has parent this, no need to delete
}
void BlockWindow::showEvent(QShowEvent *ev)
{
    QWidget::showEvent(ev);
    // qDebug() << "BlockWindow: showEvent called, isVisible()=" << isVisible();
}
void BlockWindow::initMap()
{
    // 清空场景（若重新加载）
    Scene->clear();

    qDebug() << "initMap: map width/height:" << m_mapWidth << m_mapHeight
             << " layers:" << m_map.MapLayer.size();

    // 是否在每个 tile 单元内居中小图（若为 false 则左上对齐）
    const bool centerTiles = true;

    for (int layerIndex = 0; layerIndex < m_map.MapLayer.size(); ++layerIndex) {
        const QVector<QVector<int>>& layer = m_map.MapLayer.at(layerIndex);

        // 先计算本层需要的像素尺寸（以便当某些 tile 原图大于 m_tileSize 时不被裁切）
        int maxPixelW = 0;
        int maxPixelH = 0;
        int rows = std::min<int>(layer.size(), m_mapHeight);
        for (int r = 0; r < rows; ++r) {
            const QVector<int>& row = layer.at(r);
            int cols = std::min<int>(row.size(), m_mapWidth);
            for (int c = 0; c < cols; ++c) {
                int gid = row.at(c);
                if (gid == 0) continue;
                int idx = gid - m_map.Firstgid();
                if (idx < 0 || !m_map.hasTile(idx)) continue;
                tile t = m_map.getTileSafe(idx);
                if (t.Img().isNull()) continue;
                int tileW = t.Img().width();
                int tileH = t.Img().height();
                // tile 放置的左上角为 c*m_tileSize, r*m_tileSize
                int neededW = c * m_tileSize + tileW;
                int neededH = r * m_tileSize + tileH;
                if (neededW > maxPixelW) maxPixelW = neededW;
                if (neededH > maxPixelH) maxPixelH = neededH;
            }
        }

        // 至少覆盖网格的默认像素区域
        int baseW = m_mapWidth * m_tileSize;
        int baseH = m_mapHeight * m_tileSize;
        int imageW = std::max(baseW, maxPixelW);
        int imageH = std::max(baseH, maxPixelH);

        // Create an image to paint the whole layer (use ARGB32 premultiplied like before)
        QImage image(imageW, imageH, QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::transparent);
        QPainter painter(&image);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, false); // 关闭缩放平滑（无缩放时可关闭）

        // 绘制 tiles（不缩放，显示原图）
        for (int r = 0; r < rows; ++r) {
            const QVector<int>& row = layer.at(r);
            int cols = std::min<int>(row.size(), m_mapWidth);
            for (int c = 0; c < cols; ++c) {
                int gid = row.at(c);
                if (gid == 0) continue; // empty
                int idx = gid - m_map.Firstgid();
                if (idx < 0 || !m_map.hasTile(idx)) {
                    qWarning() << "initMap: skip invalid tile gid/idx" << gid << idx << "at" << r << c;
                    continue;
                }
                tile t = m_map.getTileSafe(idx);
                if (t.Img().isNull()) {
                    qWarning() << "initMap: tile image null for idx" << idx << "gid" << gid;
                    continue;
                }

                // 计算绘制位置：格子左上角为 (c*m_tileSize, r*m_tileSize)
                int cellX = c * m_tileSize;
                int cellY = r * m_tileSize;

                int tw = t.Img().width();
                int th = t.Img().height();

                int drawX = cellX;
                int drawY = cellY;
                if (centerTiles) {
                    // 将原图在格子中居中（若原图比格子大，则会以左上为基准重叠）
                    drawX = cellX;
                    drawY = cellY + (m_tileSize - th);
                }

                // 直接绘制原始 pixmap（不缩放）
                // t.Img() 可能是 QPixmap 或 QImage（此处假设返回 QPixmap）
                painter.drawPixmap(drawX, drawY, t.Img());
            }
        }
        painter.end();

        QPixmap pix = QPixmap::fromImage(image);
        QGraphicsPixmapItem* item = Scene->addPixmap(pix);
        item->setZValue(layerIndex);
    }

    // update scene rect（使用所有层的最大像素范围）
    QRectF rect = sceneRectForMap(); // 如果这个函数仍返回 m_mapWidth*m_tileSize 的矩形，可以替换为计算过的物理像素范围
    // 若希望 sceneRect 能容纳上面按原图绘制的溢出部分，设置为 image 尺寸（这里简单取最大）
    int totalW = std::max(m_mapWidth * m_tileSize, /* 或者根据所有层计算的最大宽度 */ m_mapWidth * m_tileSize);
    int totalH = std::max(m_mapHeight * m_tileSize, m_mapHeight * m_tileSize);
    // 如果你想用精确层大小，请把 imageW/imageH 存起来并取所有层的 max
    Scene->setSceneRect(0, 0, totalW, totalH);
}
void BlockWindow::initEnemies()
{
    // keep your original enemy spawn (example positions), you may adapt to map coords
    if(m_level==1)
    {
        m_enemies.append(new Enemy(this, QPointF(944, 224), 944, 1460));
        m_enemies.append(new Enemy(this, QPointF(400, 288), 400, 650));
        m_enemies.append(new Enemy(this, QPointF(650, 288), 400, 650));
    }
    else if(m_level==2)
    {
        m_enemies.append(new Enemy(this, QPointF(536, 672), 536, 1180));
        m_enemies.append(new Enemy(this, QPointF(1536, 288), 1536, 1696));
        m_enemies.append(new Enemy(this, QPointF(1700, 672), 1700, 1924));
        m_enemies.append(new Enemy(this, QPointF(636, 512), 636, 868));
    }
    else if(m_level==3)
    {
        m_enemies.append(new Enemy(this, QPointF(928, 416), 928, 1400));
        m_enemies.append(new Enemy(this, QPointF(1400, 416), 928, 1400));
        m_enemies.append(new Enemy(this, QPointF(1024, 640), 1024, 1820));
        m_enemies.append(new Enemy(this, QPointF(1224, 640), 1224, 1534));
    }
    else if(m_level==4)
    {
        m_enemies.append(new Enemy(this, QPointF(356, 288), 356, 384));
        m_enemies.append(new Enemy(this, QPointF(356, 288), 356, 384));
        m_enemies.append(new Enemy(this, QPointF(356, 288), 356, 384));
        m_enemies.append(new Enemy(this, QPointF(288, 160), 288, 324));
        m_enemies.append(new Enemy(this, QPointF(288, 160), 288, 324));
        m_enemies.append(new Enemy(this, QPointF(288, 160), 288, 324));
        m_enemies.append(new Enemy(this, QPointF(448, 512), 448, 864));
        m_enemies.append(new Enemy(this, QPointF(992, 224), 992, 1404));
        m_enemies.append(new Enemy(this, QPointF(992, 224), 992, 1404));
    }
    else if(m_level==5)
    {
        m_enemies.append(new Enemy(this, QPointF(944, 224), 944, 1460));
        m_enemies.append(new Enemy(this, QPointF(400, 288), 400, 650));
        m_enemies.append(new Enemy(this, QPointF(650, 288), 400, 650));
    }
    else if(m_level==6)
    {
        m_enemies.append(new Enemy(this, QPointF(536, 672), 536, 1180));
        m_enemies.append(new Enemy(this, QPointF(1536, 288), 1536, 1696));
        m_enemies.append(new Enemy(this, QPointF(1700, 672), 1700, 1924));
        m_enemies.append(new Enemy(this, QPointF(636, 512), 636, 868));
    }
    else if(m_level==7)
    {
        m_enemies.append(new Enemy(this, QPointF(928, 416), 928, 1400));
        m_enemies.append(new Enemy(this, QPointF(1400, 416), 928, 1400));
        m_enemies.append(new Enemy(this, QPointF(1024, 640), 1024, 1820));
        m_enemies.append(new Enemy(this, QPointF(1224, 640), 1224, 1534));
    }
    else if(m_level==8)
    {
        m_enemies.append(new Enemy(this, QPointF(356, 288), 356, 384));
        m_enemies.append(new Enemy(this, QPointF(356, 288), 356, 384));
        m_enemies.append(new Enemy(this, QPointF(356, 288), 356, 384));
        m_enemies.append(new Enemy(this, QPointF(288, 160), 288, 324));
        m_enemies.append(new Enemy(this, QPointF(288, 160), 288, 324));
        m_enemies.append(new Enemy(this, QPointF(288, 160), 288, 324));
        m_enemies.append(new Enemy(this, QPointF(448, 512), 448, 864));
        m_enemies.append(new Enemy(this, QPointF(992, 224), 992, 1404));
        m_enemies.append(new Enemy(this, QPointF(992, 224), 992, 1404));
    }
    else if(m_level==9)
    {
        m_enemies.append(new Enemy(this, QPointF(944, 224), 944, 1460));
        m_enemies.append(new Enemy(this, QPointF(400, 288), 400, 650));
        m_enemies.append(new Enemy(this, QPointF(650, 288), 400, 650));
    }
    else if(m_level==10)
    {
        m_enemies.append(new Enemy(this, QPointF(536, 672), 536, 1180));
        m_enemies.append(new Enemy(this, QPointF(1536, 288), 1536, 1696));
        m_enemies.append(new Enemy(this, QPointF(1700, 672), 1700, 1924));
        m_enemies.append(new Enemy(this, QPointF(636, 512), 636, 868));
    }
    else if(m_level==11)
    {
        m_enemies.append(new Enemy(this, QPointF(928, 416), 928, 1400));
        m_enemies.append(new Enemy(this, QPointF(1400, 416), 928, 1400));
        m_enemies.append(new Enemy(this, QPointF(1024, 640), 1024, 1820));
        m_enemies.append(new Enemy(this, QPointF(1224, 640), 1224, 1534));
    }
    else if(m_level==12)
    {
        m_enemies.append(new Enemy(this, QPointF(356, 288), 356, 384));
        m_enemies.append(new Enemy(this, QPointF(356, 288), 356, 384));
        m_enemies.append(new Enemy(this, QPointF(356, 288), 356, 384));
        m_enemies.append(new Enemy(this, QPointF(288, 160), 288, 324));
        m_enemies.append(new Enemy(this, QPointF(288, 160), 288, 324));
        m_enemies.append(new Enemy(this, QPointF(288, 160), 288, 324));
        m_enemies.append(new Enemy(this, QPointF(448, 512), 448, 864));
        m_enemies.append(new Enemy(this, QPointF(992, 224), 992, 1404));
        m_enemies.append(new Enemy(this, QPointF(992, 224), 992, 1404));
    }
    else if(m_level==13)
    {
        m_enemies.append(new Enemy(this, QPointF(800, 160), 800, 960));
        m_enemies.append(new Enemy(this, QPointF(960, 160), 800, 960));
        m_enemies.append(new Enemy(this, QPointF(1440, 640), 1440, 1632));
        m_enemies.append(new Enemy(this, QPointF(1632, 640), 1440, 1632));
        m_enemies.append(new Enemy(this, QPointF(1632, 576), 1632, 1760));
        m_enemies.append(new Enemy(this, QPointF(1440, 64), 1440, 1632));
        m_enemies.append(new Enemy(this, QPointF(1632, 64), 1440, 1632));
    }
    else if(m_level==14)
    {
        m_enemies.append(new Enemy(this, QPointF(800, 160), 800, 960));
        m_enemies.append(new Enemy(this, QPointF(960, 160), 800, 960));
        m_enemies.append(new Enemy(this, QPointF(1440, 640), 1440, 1632));
        m_enemies.append(new Enemy(this, QPointF(1632, 640), 1440, 1632));
        m_enemies.append(new Enemy(this, QPointF(1632, 576), 1632, 1760));
        m_enemies.append(new Enemy(this, QPointF(1440, 64), 1440, 1632));
        m_enemies.append(new Enemy(this, QPointF(1632, 64), 1440, 1632));
    }
    else if(m_level==15)
    {
        m_enemies.append(new Enemy(this, QPointF(800, 160), 800, 960));
        m_enemies.append(new Enemy(this, QPointF(960, 160), 800, 960));
        m_enemies.append(new Enemy(this, QPointF(1440, 640), 1440, 1632));
        m_enemies.append(new Enemy(this, QPointF(1632, 640), 1440, 1632));
        m_enemies.append(new Enemy(this, QPointF(1632, 576), 1632, 1760));
        m_enemies.append(new Enemy(this, QPointF(1440, 64), 1440, 1632));
        m_enemies.append(new Enemy(this, QPointF(1632, 64), 1440, 1632));
    }
    else if(m_level==16)
    {
        m_enemies.append(new Enemy(this, QPointF(800, 96), 480, 864));
        m_enemies.append(new Enemy(this, QPointF(700, 96), 480, 864));
        m_enemies.append(new Enemy(this, QPointF(600, 96), 480, 864));
        m_enemies.append(new Enemy(this, QPointF(500, 96), 480, 864));
    }

    // create items for each enemy
    for (Enemy* e : m_enemies) {
        if (!Scene) { m_enemyItems.append(nullptr); continue; }
        QGraphicsPixmapItem* it = nullptr;
        QGraphicsTextItem* textItem = nullptr;
        QGraphicsRectItem* barBgItem = nullptr;
        QGraphicsRectItem* barItem = nullptr;
        QPixmap p = e->pixmapNormal();

        if (!p.isNull()) {
            it = Scene->addPixmap(p); // returns QGraphicsPixmapItem*, convertible to QGraphicsItem*
        } else {
            //it = Scene->addRect(0, 0, ENEMY_WIDTH, ENEMY_HEIGHT, QPen(Qt::black), QBrush(Qt::red)); // QGraphicsRectItem*
        }
        if (it) {
            it->setZValue(900);
            it->setPos(e->rect().topLeft());

            textItem = new QGraphicsTextItem(it);
            textItem ->setDefaultTextColor(Qt::red);
            textItem ->setFont(QFont("微软雅黑", 10));
            QRectF enemyTextRect = textItem->boundingRect();
            textItem ->setPos(
                (e->width() - enemyTextRect.width() )/ 2,
                -25
                );


            barBgItem = new QGraphicsRectItem(0, 0, 50, 5, textItem);
            barBgItem->setBrush(Qt::gray);

            barItem = new QGraphicsRectItem(0, 0, 50, 5, barBgItem);
            barItem->setBrush(Qt::red);

            barBgItem->setPos(
                ( e->width() - enemyTextRect.width()  )/ 2,
                -5
                );

        }
        m_enemyItems.append(it);
        m_enemyTextItem.append(textItem);
        m_hpBarBgEnemy.append(barBgItem);
        m_hpBarEnemy.append(barItem);
        int num = ENEMY_DYING_FRAME;
        enemyDyingTime.append(num);

        /*
        if (true) {

            m_hpBarBgPlayer = new QGraphicsRectItem(0, 0, 50, 5, m_playerItem);
            m_hpBarBgPlayer->setBrush(Qt::gray);

            m_hpBarPlayer = new QGraphicsRectItem(0, 0, 50, 5, m_hpBarBgPlayer);
            m_hpBarPlayer->setBrush(Qt::red);

            m_hpBarBgPlayer->setPos(
                ( m_player->width() - 50 )/ 2,
                -20
                );
        }*/
    }
}
void BlockWindow::addBullet(Bullet* bullet)
{
    if (!bullet) return;
    m_bullets.append(bullet);

    if (Scene)
    {
        QPixmap pixmapEnemyRight;
        pixmapEnemyRight.load(":/image/stage/firebulletright.png");
        QPixmap pixmapEnemyLeft;
        pixmapEnemyLeft.load(":/image/stage/firebulletleft.png");
        QPixmap pixmapPlayerRight;
        pixmapPlayerRight.load(":/image/stage/bulletright.png");
        QPixmap pixmapPlayerLeft;
        pixmapPlayerLeft.load(":/image/stage/bulletleft.png");
        QPixmap p = (bullet->owner() == BulletOwner::Player
                    ? (bullet->isDirectionRight() ? pixmapPlayerRight : pixmapPlayerLeft)
                    : (bullet->isDirectionRight() ? pixmapEnemyRight : pixmapEnemyLeft)
                    );
        QGraphicsPixmapItem* it = Scene-> addPixmap(p);
        if (it)
        {
            it->setZValue(1000);
            it->setFlag(QGraphicsItem::ItemIsFocusable, false);
            it->setAcceptedMouseButtons(Qt::NoButton);
            if(bullet->owner() == BulletOwner::Player)
            {
                QPointF p = bullet->rect().topLeft();
                it->setPos(p);
                SoundPlayer *sp = new SoundPlayer(this);

                QString playerFireSound = QDir::currentPath()+"/../../bgms/PlayerFire.mp3";
                sp->playSound(playerFireSound);
            }
            else
            {
                QPointF p = bullet->rect().topLeft();
                p.setY(p.y()-20);
                it->setPos(p);
                SoundPlayer *sp = new SoundPlayer(this);
                QString enemyFireSound = QDir::currentPath()+"/../../bgms/EnemyFire.mp3";
                sp->playSound(enemyFireSound);
            }
        }
        m_bulletItems.append(it);
    }

    // restore focus to the window so keyboard continues to work
    this->setFocus(Qt::OtherFocusReason);
}

bool BlockWindow::isTileSolid(int tileX, int tileY) const
{
    return (typeOfTile(tileX, tileY) >= 1);
    if (tileX < 0 || tileY < 0 || tileX >= m_mapWidth || tileY >= m_mapHeight) {
        return true;
    }
    // RealMap may be empty; guard access
    if (m_map.RealMap.isEmpty()) return false;
    const QVector<int>& row = m_map.RealMap.at(tileY);
    if (tileX < 0 || tileX >= row.size()) return false;
    return row.at(tileX) >= 1;
}

int BlockWindow::typeOfTile(int tileX, int tileY) const
{
    if (tileX < 0 || tileY < 0 || tileX >= m_mapWidth || tileY >= m_mapHeight) {
        return 2;
    }
    if (m_map.RealMap.isEmpty()) return 0;
    const QVector<int>& row = m_map.RealMap.at(tileY);
    if (tileX < 0 || tileX >= row.size()) return 0;
    // qDebug()<<"x="<<tileX<<";y="<<tileY<<"  is "<<row.at(tileX);
    switch (row.at(tileX)) {
    case 9: return 0;
    case 10: return 1;
    case 11: return 1;
    case 12: return 0;
    case 13: return 2;
    case 14: return 3;
    case 15: return 4;
    case 16: return 4;
    default: return 0;
    }
}

int BlockWindow::imgOfTile(int tileX, int tileY) const
{
    if (tileX < 0 || tileY < 0 || tileX >= m_mapWidth || tileY >= m_mapHeight) {
        return 2;
    }
    if (m_map.RealMap.isEmpty()) return 0;
    const QVector<int>& row = m_map.RealMap.at(tileY);
    if (tileX < 0 || tileX >= row.size()) return 0;
    switch (row.at(tileX)) {
    case 9: return 1;
    case 10: return 1;
    case 11: return 1;
    case 12: return 0;
    case 13: return 2;
    case 14: return 3;
    case 15: return 4;
    case 16: return 4;
    default: return 0;
    }
}

bool BlockWindow::checkPlayerCollision(int x, int y, int width, int height) const
{
    int tileXMin = x / m_tileSize;
    int tileXMax = (x + width - 1) / m_tileSize;
    int tileYMin = y / m_tileSize;
    int tileYMax = (y + height - 1) / m_tileSize;

    tileXMin = std::max(tileXMin, 0);
    tileXMax = std::min(tileXMax, m_mapWidth - 1);
    tileYMin = std::max(tileYMin, 0);
    tileYMax = std::min(tileYMax, m_mapHeight - 1);

    for (int tileY = tileYMin; tileY <= tileYMax; ++tileY) {
        for (int tileX = tileXMin; tileX <= tileXMax; ++tileX) {
            if (isTileSolid(tileX, tileY)) return true;
        }
    }
    return false;
}

bool BlockWindow::isPlayerOnGround(int x, int y, int width, int height) const
{
    int footY = y + height;
    int startTileX = x / m_tileSize;
    int endTileX = (x + width - 1) / m_tileSize;
    int footTileY = footY / m_tileSize;

    for (int nowTileX = startTileX; nowTileX <= endTileX; ++nowTileX) {
        if (isTileSolid(nowTileX, footTileY)) return true;
    }
    return false;
}

int BlockWindow::playerOnGroundState(int x, int y, int width, int height) const
{
    int onGroundState = 0;
    int footY = y + height;
    int startTileX = x / m_tileSize;
    int endTileX = (x + width - 1) / m_tileSize;
    int footTileY = footY / m_tileSize;

    for (int nowTileX = startTileX; nowTileX <= endTileX; ++nowTileX) {
        int t = typeOfTile(nowTileX, footTileY);
        if (onGroundState < t) onGroundState = t;
    }
    if (!isPlayerOnGround(x, y, width, height)) onGroundState = -1;
    return onGroundState;
}

bool BlockWindow::checkTileCollision(float x, float y, int width, int height) const
{
    int pixelX = static_cast<int>(x);
    int pixelY = static_cast<int>(y);
    int tileXMin = pixelX / m_tileSize;
    int tileXMax = (pixelX + width - 1) / m_tileSize;
    int tileYMin = pixelY / m_tileSize;
    int tileYMax = (pixelY + height - 1) / m_tileSize;

    tileXMin = std::max(tileXMin, 0);
    tileXMax = std::min(tileXMax, m_mapWidth - 1);
    tileYMin = std::max(tileYMin, 0);
    tileYMax = std::min(tileYMax, m_mapHeight - 1);

    for (int tileY = tileYMin; tileY <= tileYMax; ++tileY) {
        for (int tileX = tileXMin; tileX <= tileXMax; ++tileX) {
            if (isTileSolid(tileX, tileY)) return true;
        }
    }
    return false;
}

void BlockWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    // background
    painter.setBrush(Qt::lightGray);
    painter.drawRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // draw player/enemies/bullets via their own draw() methods
    m_player->draw(painter);
    for (Enemy* enemy : m_enemies) enemy->draw(painter);
    for (Bullet* b : m_bullets) b->draw(painter);

    // debug texts
    painter.setPen(Qt::green);
    painter.setFont(QFont("Arial", 24, QFont::Bold));
    painter.drawText(10, 35, QString("Debug1: %1").arg(m_player->debug1()));
    painter.drawText(10, 75, QString("Debug2: %1").arg(m_player->debug2()?"true":"false"));
    painter.drawText(10, 105, QString("Debug3: %1").arg(m_player->debug3()?"true":"false"));
}

void BlockWindow::keyPressEvent(QKeyEvent *event)
{
    if (m_resultWindow) return;
    //qDebug() << "keyPressEvent:" << event->key() << " focusWidget=" << QApplication::focusWidget();
    if (m_player) m_player->handleKeyPress(event);
    if (event->key() == Qt::Key_Escape) emit toLevel();
    // 新增Z键暂停/恢复功能
    if (event->key() == Qt::Key_Z) {
        m_isPaused = !m_isPaused;  // 切换暂停状态
        if (m_isPaused) {
            m_gameTimer->stop();   // 暂停时停止定时器
        } else {
            m_gameTimer->start(16); // 恢复时启动定时器
        }
    }
}

void BlockWindow::keyReleaseEvent(QKeyEvent *event)
{
    //qDebug() << "keyReleaseEvent:" << event->key() << " focusWidget=" << QApplication::focusWidget();
    if (m_player) m_player->handleKeyRelease(event);
}

// 新增：清理结果窗口的辅助函数（避免内存泄漏）
void BlockWindow::clearResultWindow()
{
    if (m_resultWindow) {
        m_resultWindow->close();
        delete m_resultWindow;
        m_resultWindow = nullptr;
    }
}

void BlockWindow::updateGame()
{
     if (m_resultWindow) return;
    // qDebug() << "updateGame: end tick, player pos" << m_player->x() << m_player->y();
    // qDebug()<<"before"<<debugNUM2++;
    m_player->update(16.0f / 1000.0f);
    // qDebug()<<"after"<<debugNUM2++;
    if (m_playerItem)
    {
        // 更新 pixmap（如果方向/帧有变化）
        QPixmap newPix = m_player->currentPixmap();
        if (!newPix.isNull()) m_playerItem->setPixmap(newPix);
        // 更新位置（Scene 坐标系）
        m_playerItem->setPos(m_player->x(), m_player->y());
        qDebug()<<m_player->x()<<" "<<m_player->y();
        int currentHp = m_player->hp();
        int maxHp = m_player->maxHp();
        float hpRatio = qMax(0.0f, (float)currentHp / maxHp); // 血量比例（避免负数）

        m_playerTextItem->setPlainText(QString("HP: %1").arg(currentHp));
        // m_playerTextItem->setPlainText(QString("HP:"));
        m_playerTextItem->setDefaultTextColor(Qt::green);
        m_playerTextItem->setFont(QFont("微软雅黑", 10));
        //m_playerTextItem->setPos(
         //   m_player->width() - m_playerTextItem->boundingRect().width()/2, // 水平居中
          //  m_player->height()/2 -
        QRectF playerTextRect = m_playerTextItem->boundingRect();
        m_playerTextItem->setPos(
            ( m_player->width() - playerTextRect.width() )/ 2,
            -20
            );
        m_hpBarBgPlayer->setPos(
            ( m_player->width() - 50 )/ 2,
            -25
            );
        m_hpBarPlayer->setRect(0, 0, 50 * hpRatio, 5);
    }
    for (Enemy* enemy : m_enemies) enemy->update();
    for (Bullet* bullet : m_bullets) bullet->update();
    int nEnemies = std::min(m_enemies.size(), m_enemyItems.size());
    for (int i = 0; i < nEnemies; ++i)
    {
        Enemy* e = m_enemies.at(i);
        int currentHp = e->hp();
        int maxHp = e->maxHp();
        float hpRatio = qMax(0.0f, (float)currentHp / maxHp); // 敌人血量比例（避免负数）
        QGraphicsPixmapItem* it = m_enemyItems.at(i);
        QGraphicsTextItem* textItem = m_enemyTextItem.at(i);
        QGraphicsRectItem* barBgItem = m_hpBarBgEnemy.at(i);
        QGraphicsRectItem* barItem = m_hpBarEnemy.at(i);
        if (!e || !it) continue;
        QPixmap newEnemyPix = e->currentPixmap();
        if (!newEnemyPix.isNull()) it->setPixmap(newEnemyPix);
        QPointF p = e->rect().topLeft();
        p.setY(p.y());
        it->setPos(p);
        textItem ->setDefaultTextColor(Qt::red);
        textItem ->setFont(QFont("微软雅黑", 10));
        textItem ->setPlainText(QString("HP: %1").arg(currentHp));
        QRectF enemyTextRect = textItem->boundingRect();
        textItem ->setPos(
            (e->width() - enemyTextRect.width() )/ 2,
            -25
            );
        barBgItem->setPos(
            (e->width() - enemyTextRect.width()  )/ 2,
            -5
            );
        barItem->setRect(0, 0, 50 * hpRatio, 5);
    }

    // 同步子弹 item
    int nBullets = std::min(m_bullets.size(), m_bulletItems.size());
    for (int i = 0; i < nBullets; ++i) {
        Bullet* b = m_bullets.at(i);
        QGraphicsPixmapItem* it = m_bulletItems.at(i);
        if (!b || !it) continue;
        QPointF p = b->rect().topLeft();

        if(b->owner() == BulletOwner::Player)
        {
            QPointF p = b->rect().topLeft();
            it->setPos(p);
        }
        else
        {
            QPointF p = b->rect().topLeft();
            p.setY(p.y()-20);
            it->setPos(p);
        }
    }


    // 保证焦点回到 BlockWindow（防止某些交互把焦点交给 View）
    this->setFocus(Qt::OtherFocusReason);
    cleanupDeadObjects();

    // keep view following player
    centerOnPlayer();

    update(); // paintEvent for HUD etc
    // 1. 检测玩家死亡
    if (m_player && !m_player->isAlive()) {
        m_gameTimer->stop(); // 停止游戏定时器
        clearResultWindow(); // 清理旧窗口（如有）
        // 创建并显示阵亡窗口
        m_resultWindow = new ResultWindow(ResultType::GameOver, this);
        // 连接返回信号到原有 toStart 信号
        connect(m_resultWindow, &ResultWindow::backToLevel, this, [this]() {
            clearResultWindow();
            if(currentLevel == 17){
                emit toStart();
            }else if(currentLevel <= 16){
                emit toLevel();
            }
            currentLevel = 0;
        });
        m_resultWindow->show();
        return;
    }

    // 2. 检测玩家到达通关位置
    if (m_player) {
        QRectF playerRect = m_player->rect();
        QRectF endZone(m_levelEndPos.x() - 50, m_levelEndPos.y() - 50, 100, 100);
        if (playerRect.intersects(endZone)) {
            m_gameTimer->stop(); // 停止游戏定时器
            if (currentLevel == unlockedLevel && currentLevel < 16)
            {
                unlockedLevel++;
                emit unlock(currentLevel+1);
            }

            clearResultWindow(); // 清理旧窗口（如有）
            // 创建并显示通关窗口
            m_resultWindow = new ResultWindow(ResultType::LevelCleared, this);
            // 连接返回信号
            connect(m_resultWindow, &ResultWindow::backToLevel, this, [this]() {
                clearResultWindow();
                if(currentLevel == 17){
                    emit toStart();
                }else if(currentLevel <= 16){
                    emit toLevel();
                }

                currentLevel = 0;

            });
            m_resultWindow->show();
            return;
        }
    }
}

void BlockWindow::cleanupDeadObjects()
{
    /*for (int i = m_enemies.size() - 1; i >= 0; --i) {
        if (!m_enemies[i]->isAlive()) {
            delete m_enemies.takeAt(i);
        }
    }
    for (int i = m_bullets.size() - 1; i >= 0; --i) {
        if (!m_bullets[i]->isAlive()) {
            delete m_bullets.takeAt(i);
        }
    }*/
    for (int i = m_enemies.size() - 1; i >= 0; --i)
    {
        if (!m_enemies[i]->isAlive())
        {
            if(enemyDyingTime[i] > 0)
            {
                if(enemyDyingTime[i]== ENEMY_DYING_FRAME/2)
                {
                    SoundPlayer *sp = new SoundPlayer(this);
                    QString enemyDeathSound = QDir::currentPath()+"/../../bgms/Death.mp3";
                    sp->playSound(enemyDeathSound);
                }
                enemyDyingTime[i]--;
            }
            else
            {


                delete m_enemies.takeAt(i);
                if (i < m_enemyItems.size())
                {
                    QGraphicsItem* itBar = m_hpBarEnemy.takeAt(i);
                    if (Scene && itBar) Scene->removeItem(itBar);
                    delete itBar;
                    QGraphicsItem* itBarBg = m_hpBarBgEnemy.takeAt(i);
                    if (Scene && itBarBg) Scene->removeItem(itBarBg);
                    delete itBarBg;
                    QGraphicsItem* itText = m_enemyTextItem.takeAt(i);
                    if (Scene && itText) Scene->removeItem(itText);
                    delete itText;
                    QGraphicsItem* it = m_enemyItems.takeAt(i);
                    if (Scene && it) Scene->removeItem(it);
                    delete it;
                }
                enemyDyingTime.takeAt(i);
            }

        }
    }

    // 清理死亡子弹
    for (int i = m_bullets.size() - 1; i >= 0; --i) {
        if (!m_bullets[i]->isAlive()) {
            delete m_bullets.takeAt(i);
            if (i < m_bulletItems.size()) {
                QGraphicsPixmapItem* it = m_bulletItems.takeAt(i);
                if (Scene && it) Scene->removeItem(it);
                delete it;
            }
        }
    }
}

void BlockWindow::centerOnPlayer()
{
    if (!View || !Scene || !m_playerItem) return;

    // center on player's center point
    qreal cx = m_playerItem->pos().x() + m_playerItem->boundingRect().width() / 2.0;
    qreal cy = m_playerItem->pos().y() + m_playerItem->boundingRect().height() / 2.0;

    QRectF srect = Scene->sceneRect();
    qreal halfW = View->viewport()->width() / 2.0;
    qreal halfH = View->viewport()->height() / 2.0;

    qreal minX = halfW;
    qreal maxX = srect.width() - halfW;
    qreal minY = halfH;
    qreal maxY = srect.height() - halfH;

    if (maxX < minX) { minX = maxX = srect.width() / 2.0; }
    if (maxY < minY) { minY = maxY = srect.height() / 2.0; }

    qreal cxClamped = std::clamp(cx, minX, maxX);
    qreal cyClamped = std::clamp(cy, minY, maxY);

    View->centerOn(cxClamped, cyClamped);
}

void BlockWindow::gamePause(){
    if (!m_isPaused) {
        m_gameTimer->stop();   // 暂停时停止定时器
        m_isPaused = 1;
    }
};
void BlockWindow::gameContinue(){
    if (m_isPaused) {
        m_gameTimer->start(16); // 恢复时启动定时器
        m_isPaused = 0;
    }

};
