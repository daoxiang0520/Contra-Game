#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QPainter>
#include <QKeyEvent>
#include <QPixmap>
#include <QPointF>
#include "GameDefs.h"
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QFile>
#include <QDebug>
#include <QList>
#include <QDir>
#include "soundplayer.h"
#include "globalsound.h"
#include"blockwindow.h"


// 前置声明
class BlockWindow;
extern int currentDifficulty;
class Player : public QObject
{
    Q_OBJECT
public:
    explicit Player(BlockWindow *window, QObject *parent = nullptr);
    // int numDebug=0;
    QPixmap currentPixmap();
    // 核心方法
    int hp() const { return m_hp; }
    int height() const { return m_height; }
    int width() const { return m_width; }
    int maxHp() const { return m_maxHP; }
    void handleKeyPress(QKeyEvent *event);
    void handleKeyRelease(QKeyEvent *event);
    void update(float deltaTime);
    void draw(QPainter &painter);
    void takeDamage(int damage); // 扣血
    bool isAlive() const { return m_hp > 0; }
    QRectF rect() const;         // 碰撞盒
    QPointF pos() const { return QPointF(m_x, m_y); }
    int x() const { return m_x; }
    int y() const { return m_y; }
    int debug1() {return m_currentSpikeCooldown;}
    int debug2() {return m_isOnGround;}
    int debug3() {return m_isOnSpring;}
    QJsonObject toJsonObject() const;
    void setPosition(int x, int y);


private:
    QVector<QPixmap> m_walkRight;
    QVector<QPixmap> m_walkLeft;
    BlockWindow *m_window;       // 关联的窗口
    qreal m_x, m_y;                // 位置
    const int m_width = PLAYER_WIDTH;  // 宽度
    const int m_height = PLAYER_HEIGHT;// 高度
    qreal m_vx;                // 水平速度
    qreal m_vy;                    // 垂直速度

    // 扩展属性
    int PLAYER_MAX_HP;

    int m_hp;    // 生命值

    int m_maxHP;    // 允许的最大生命值
    qreal m_gravity;//重力
    Direction m_dir = Direction::Right; // 朝向
    int m_spikeCooldown = SPIKE_COOLDOWN; // 地刺受伤冷却
    int m_currentSpikeCooldown = 0;   // 当前地刺受伤冷却
    int m_healingCooldown = HEALING_COOLDOWN; // 治疗冷却
    int m_currentHealingCooldown = 0;   // 当前治疗冷却

    //子弹相关
    int m_fireCooldown = FIRE_COOLDOWN; // 发射冷却
    int m_currentFireCooldown = 0;   // 当前发射冷却
    int m_bulletDamage = PLAYER_BULLET_DAMAGE; //当前子弹伤害
    int m_fireBulletMode = 2; //子弹发射模式（0为默认，1为并排子弹，2为五向子弹）

    // 状态
    bool m_isLeftPressed = false;
    bool m_isRightPressed = false;
    bool m_isJumpPressed = false;
    bool m_isFirePressed = false; // 发射子弹按键
    bool m_isFireModeChangedTriggered = false; // 更换子弹模式按键（待维护）
    bool m_isJumping = false;
    bool m_isOnGround = false;
    bool m_isOnSpike = false;
    bool m_isOnHealing = false;
    bool m_isOnSpring = false;
    bool m_willSoonOnGround = false;
    int m_jumpBufferFrames = 0;  // 跳跃缓冲帧
    int m_walkingStateNumber = 0;//帧数字
    int m_curedFrame = 0;
    int m_injuredFrame = 0;


    // 动画
    QPixmap m_imgRight;          // 朝右图片
    QPixmap m_imgLeft;           // 朝左图片
    QPixmap m_cureLeft;
    QPixmap m_cureRight;
    QString  cureLeftPath;
    QString  cureRightPath;


    QPixmap m_injureLeft;
    QPixmap m_injureRight;
    QString injureLeftPath;
    QString injureRightPath;

    int m_currentFrame = 0;
    int m_frameCounter = 0;

    // 辅助方法
    void resolveHorizontalCollision();
    void resolveVerticalCollision();
    void detectSpike();
    void detectHealing();
    void detectSpring();
    bool checkCollision(int x, int y);
    void updateAnimation();
    void fireBullet();           // 发射子弹
    void changeFireMode();      //改变发射方式
};

#endif // PLAYER_H
