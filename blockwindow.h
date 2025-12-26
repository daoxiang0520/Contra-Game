#ifndef BLOCKWINDOW_H
#define BLOCKWINDOW_H

#include <QWidget>
#include <QTimer>
#include <QVector>
#include <QPointF>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include "player.h"
#include "ResultWindow.h"
#include "Enemy.h"
#include "bullet.h"
#include "GameDefs.h"
#include "mapdata.h"
#include "gameview.h"
#include "soundplayer.h"
#include "globalsound.h"

class BlockWindow : public QWidget
{
    Q_OBJECT

public:
    explicit BlockWindow(int level=1,QWidget *parent = nullptr);
    ~BlockWindow() override;
    // int debugNUM2=0;
    // 对外接口
    bool isTileSolid(int tileX, int tileY) const;
    int typeOfTile(int tileX, int tileY) const;//瓦片类型（用于交互）
    int imgOfTile(int tileX, int tileY) const;
    bool checkPlayerCollision(int x, int y, int width, int height) const;
    bool isPlayerOnGround(int x, int y, int width, int height) const;
    bool checkTileCollision(float x, float y, int width, int height) const;
    int playerOnGroundState(int x, int y, int width, int height) const;
    int tileSize() const { return m_tileSize; }
    Player* player() const { return m_player; }
    QVector<Enemy*> enemies() const { return m_enemies; }
    void addBullet(Bullet* bullet);
    int Getwidth(){return m_map.width()*TILE_SIZE;}
    int Getheight(){return m_map.height()*TILE_SIZE;}

    void gamePause();
    void gameContinue();


    int PLAYER_START_X = 64;//初始
    int PLAYER_START_Y = 64;
protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void showEvent(QShowEvent *ev) override;


private slots:
    void updateGame();

private:
    QVector<QGraphicsTextItem*> m_enemyHpItems; // 与 m_enemyItems 索引对应
    QGraphicsTextItem* m_playerHpItem = nullptr;
    // 地图相关
    mapdata m_map; // uses TiledImg.json / DB as implemented
    int m_level;
    int m_mapWidth = 0;
    int m_mapHeight = 0;

    const int m_tileSize = TILE_SIZE;

    // Scene / View
    GameView* View = nullptr;
    QGraphicsScene* Scene = nullptr;

    // 游戏对象
    Player* m_player = nullptr;
    QVector<Enemy*> m_enemies;
    QVector<Bullet*> m_bullets;
    QTimer* m_gameTimer = nullptr;

    //怪物死亡相关
    QVector<int> enemyDyingTime;

    // Scene items 对应
    QVector<QGraphicsPixmapItem*> m_bulletItems;        // 与 m_bullets 索引对应
    QVector<QGraphicsPixmapItem*> m_enemyItems;         // simple rect for enemy placeholders
    QGraphicsPixmapItem* m_playerItem = nullptr;
    QGraphicsTextItem* m_playerTextItem;
    QVector<QGraphicsTextItem*> m_enemyTextItem;
    QGraphicsRectItem* m_hpBarBgPlayer; // 血条底色
    QGraphicsRectItem* m_hpBarPlayer;   // 血条血量（红）
    QVector<QGraphicsRectItem*> m_hpBarBgEnemy; // 血条底色
    QVector<QGraphicsRectItem*> m_hpBarEnemy;   // 血条血量（红）

    //时间相关
    bool m_isPaused = false;
    ResultWindow *m_resultWindow = nullptr; // 结果窗口指针
    QPoint m_levelEndPos;                   // 通关位置（保留之前的）

    // 取消头文件中 inline 的 addBullet 实现（如果原来 header 里有实现，删除该实现）
    // 在 class 声明中声明方法（保持原声明）：
    //void addBullet(Bullet* bullet);

    // 初始化
    void initMap();
    void initEnemies(); // 初始化敌人

    // 清理死亡对象
    void cleanupDeadObjects();
    void clearResultWindow();

    // camera
    void centerOnPlayer();

    // helper
    QRectF sceneRectForMap() const { return QRectF(0, 0, m_mapWidth * m_tileSize, m_mapHeight * m_tileSize); }
    //地图长宽函数


signals:
    void toStart();
    void toLevel();
    void unlock(int a);
};

#endif // BLOCKWINDOW_H
