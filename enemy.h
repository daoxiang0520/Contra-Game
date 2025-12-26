#ifndef ENEMY_H
#define ENEMY_H

#include <QObject>
#include <QPainter>
#include <QPixmap>
#include <QPointF>
#include "GameDefs.h"

// 前置声明
class BlockWindow;
class Bullet;

class Enemy : public QObject
{
    Q_OBJECT
public:
    explicit Enemy(BlockWindow* window, QPointF pos, float moveLeftBound,
                   float moveRightBound, QObject *parent = nullptr);

    int hp() const { return m_hp; }
    int height() const { return m_height; }
    int width() const { return m_width; }
    int maxHp() const { return m_maxHP; }

    // 核心方法
    void update();
    QPixmap currentPixmap();
    QPixmap pixmapNormal(){return m_imgLeft;}
    void draw(QPainter& painter);
    void takeDamage(int damage);  // 扣血
    bool isAlive() const { return m_hp > 0 && m_isAlive; }
    // 新增函数：判断是否需要绘制（存活 或 濒死）
    bool isDrawable() const { return m_isAlive; }
    // 新增：判断是否濒死（仅绘制死亡图片）
    bool isDying() const { return m_isAlive && m_hp <= 0; }
    QRectF rect() const;          // 碰撞盒

private:
    BlockWindow* m_window;        // 游戏窗口
    QPointF m_pos;                // 位置
    const int m_width = ENEMY_WIDTH;  // 宽度
    const int m_height = ENEMY_HEIGHT;// 高度
    int m_hp = ENEMY_MAX_HP;      // 生命值
    int m_maxHP = ENEMY_MAX_HP;    //最大HP
    float m_moveSpeed = ENEMY_SPEED; // 移动速度
    float m_leftBound;            // 左移动边界
    float m_rightBound;           // 右移动边界
    Direction m_dir = Direction::Right; // 移动朝向
    bool m_isAlive = true;        // 是否存活
    int m_deathFrame = 0;         // 死亡动画帧数（消失前显示）

    // 子弹发射相关
    int m_fireCooldown = FIRE_COOLDOWN; // 发射冷却
    int m_currentCooldown = 0;          // 当前冷却
    int m_bulletDamage = ENEMY_BULLET_DAMAGE;

    // 图片
    QPixmap m_imgLeft;          // 向左图片
    QPixmap m_imgRight;         //向右图片
    QPixmap m_imgDeath;          // 死亡图片
    double m_walkingStateNumber=0;

    // 辅助方法
    void move();                  // 往返移动
    void checkPlayerAndFire();    // 检测玩家并发射子弹
    void die();                   // 死亡处理


    void updatreAnimation();
};

#endif // ENEMY_H
